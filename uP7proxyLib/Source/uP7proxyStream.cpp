////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                     /
// This library is free software; you can redistribute it and/or modify it under the terms of the provided License.    /
//                                                                                                                     /
// This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even  the  implied/
// warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more/
// details.                                                                                                            /
// You should have received a copy of the the License along with this library.                                         /
//                                                                                                                     /
// 2012-2024 (c) Baical                                                                                                /
//                                                                                                                     /
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "uP7common.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CProxyStream::CProxyStream(CWString      &i_rName, 
                           bool           i_bConvertEndianess,
                           CuP7Fifo      *i_pFifo,
                           uint8_t        i_bId,
                           uint64_t       i_qwFreq,
                           IuP7Time      *i_iTime,
                           IP7_Trace     *i_pP7Trace,
                           IP7_Telemetry *i_pP7Tel,
                           IProxyClient  *i_pClient
                          )
    : m_lReference(1)
    , m_cName(i_rName.Get())
    , m_bConvertEndianess(i_bConvertEndianess)
    , m_pP7Trace(i_pP7Trace)
    , m_hP7Mod(NULL)
    , m_pP7Tel(i_pP7Tel)
    , m_sTelTimeCorrection(P7TELEMETRY_INVALID_ID_V2)
    , m_pFifo(i_pFifo)
    , m_bId(i_bId)
    , m_eState(eStateDescription)
    , m_uSessionId(0)
    , m_uCrc7(0)
    , m_pProxyClient(i_pClient)
    , m_pClient(NULL)
    , m_uClientId(USER_PACKET_CHANNEL_ID_MAX_SIZE)
    , m_cPool(uP7_POOL_DEF_CHUNK_SIZE, uP7_POOL_DEF_MAX_SIZE)
    , m_bClosed(false)

    , m_qwCpuTimeDriftCalibration(0)
    , m_qwCpuProxyCreationTime(0)
    , m_qwCpuStartTime(0)
    , m_qwCpuFreq(i_qwFreq)
    
    , m_qwHostTimeDriftCalibration(0)
    , m_qwHostFreq(GetPerformanceFrequency())

    , m_llTimeCorrection(0)
    , m_uOperationTimeStamp(0)
    , m_iTime(i_iTime)
    , m_bTimeInSync(false)
{
    LOCK_CREATE(m_hLock);

    m_stStatus.bConnected = TRUE;
    m_stStatus.dwResets   = 0;

    if (m_pFifo)
    {
        m_pFifo->Add_Ref();
    }

    if (m_iTime)
    {
        m_iTime->Add_Ref();
        m_qwCpuTimeDriftCalibration  = m_iTime->GetTime();
        m_qwHostTimeDriftCalibration = GetPerformanceCounter();

        m_bTimeInSync = true;
    }

    if (m_pP7Tel)
    {
        m_pP7Tel->Create(TM("Time correction (ms)"), -1000, -1000, 1000, 1000, TRUE, &m_sTelTimeCorrection);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CProxyStream::~CProxyStream()
{
    if (m_pClient)
    {
        m_pProxyClient->ReleaseChannel(m_pClient, m_uClientId);
        m_uClientId = USER_PACKET_CHANNEL_ID_MAX_SIZE;
        m_pClient   = NULL;
    }

    SAFE_RELEASE(m_pFifo);

    SAFE_RELEASE(m_iTime);

    LOCK_DESTROY(m_hLock);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CProxyStream::WaitTimeResponseAsync()
{
    LOCK_ENTER(m_hLock);
    if (eStateError != m_eState)
    {
        m_uOperationTimeStamp = GetTickCount();
        m_eState              = eStateWaitTimeResponse;
    }
    LOCK_EXIT(m_hLock);
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CProxyStream::Maintain()
{
    LOCK_ENTER(m_hLock);

    if (    (eStateWaitTimeResponse == m_eState)
         && (CTicks::Difference(GetTickCount(), m_uOperationTimeStamp) > TIME_SYNC_TIMEOUT_MS)
       )

    {
        uWARNING(TM("[CPU#%d] Synchronization timeout. Timestamps won't be synchronized with HOST!"), (int)m_bId);
        Start();
    }
    else if (eStateReady == m_eState)
    {
        if (m_iTime)
        {
            //time passed for CPU & HOST
            double l_dbCpuDuration  = (double)(m_iTime->GetTime() - m_qwCpuTimeDriftCalibration); 
            double l_dbHostDuration = (double)(GetPerformanceCounter() - m_qwHostTimeDriftCalibration);   

            //converting to CPU timer frequency
            l_dbHostDuration = l_dbHostDuration * (double)m_qwCpuFreq / (double)m_qwHostFreq;


            //Expo. filter: y[k] = y[k-1] + (1-A)*(x[k] - y[k-1]); where A = (1-coef), coef=[0, 0.5, 0.9, 0.95, 0.98]
            //coef=0.95 to smooth adaptation and filer out non RT fluctuations, but with significant delay
            //coef=0.5 fast enough, with small delay, more applicable for small clock drift 
            m_llTimeCorrection = (int64_t)(   (double)m_llTimeCorrection 
                                            + (1.0f-0.5f)*(   ((double)l_dbHostDuration - (double)l_dbCpuDuration) 
                                                             - (double)m_llTimeCorrection
                                                           )
                                          );
            

            if (((int64_t)l_dbCpuDuration + m_llTimeCorrection) <= 0)
               
            {
                uERROR(TM("[CPU#%d] Time correction is wrong!"), (int)m_bId);
                m_llTimeCorrection = 0;
            }

            //uINFO(TM("[CPU#%d] Host duration %f, CPU Duration %f, Host roundtrip %f"), (int)m_bId ,
            //    (double)l_qwHostDuration / (double)m_qwHostFreq,
            //    (double)l_qwCpuDuration / (double)m_qwCpuFreq,
            //    (double)l_qwRoundTrip / (double)m_qwHostFreq);
            //printf("Correction: H%.04f R%.04f C%.03f\n", 
            //       (double)l_dbHostDuration / (double)m_qwHostFreq,
            //       (double)l_dbCpuDuration / (double)m_qwCpuFreq,
            //       ((double)m_llTimeCorrection * 1000.0)/(double)m_qwCpuFreq);

            if (    (m_pP7Tel)
                 && (P7TELEMETRY_INVALID_ID_V2 != m_sTelTimeCorrection)
               )
            {
                m_pP7Tel->Add(m_sTelTimeCorrection, ((double)m_llTimeCorrection * 1000.0)/(double)m_qwCpuFreq);
            }
        }
    }

    LOCK_EXIT(m_hLock);

    return false;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CProxyStream::SendChunks()
{
    bool l_bReturn = true;

    if (!m_pChunksHead)
    {
        return false;
    }

    if (m_stStatus.bConnected)
    {
        //sending first prev. accumulated data
        while (    (m_pChunksHead) 
                && (m_pChunksHead->szChunks)
              )
        {
            if (ECLIENT_STATUS_OK == m_pClient->Sent(m_uClientId, 
                                                     m_pChunksHead->pChunks, 
                                                     (tUINT32)m_pChunksHead->szChunks, 
                                                     (tUINT32)m_pChunksHead->szData)
               )
            {
                ClearChunkHead();
            }
            else
            {
                l_bReturn = false;
                break;
            }
        }

        //it all service data has been sent - trying to send data from CPU
        if (!m_pChunksHead->szChunks)
        {
            CLinearPool::stBuffer *l_pBuf = NULL;
            while ((l_pBuf = m_cPool.PullUsed()))
            {
                sP7C_Data_Chunk l_stChunk = {l_pBuf->pData, (tUINT32)l_pBuf->szData};
                if (ECLIENT_STATUS_OK != m_pClient->Sent(m_uClientId, &l_stChunk, 1, l_stChunk.dwSize))
                {
                    m_cPool.PushFree(l_pBuf);
                    l_bReturn = false;
                    break;
                }

                m_cPool.PushFree(l_pBuf);
            }
        }
    }

    return l_bReturn;
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int32_t CProxyStream::Add_Ref()
{
    return ATOMIC_INC(&m_lReference);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int32_t CProxyStream::Release()
{
    tINT32 l_lResult = ATOMIC_DEC(&m_lReference);
    if ( 0 >= l_lResult )
    {
        delete this;
    }

    return l_lResult;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CProxyStream::On_Init(sP7C_Channel_Info *i_pInfo) 
{
    LOCK_ENTER(m_hLock);
    if (i_pInfo)
    {
        m_uClientId = i_pInfo->dwID;
    }
    LOCK_EXIT(m_hLock);
}


