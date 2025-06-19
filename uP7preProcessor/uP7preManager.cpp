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
#include "uP7preCommon.h"
#include "crc7.h"
#include <vector>


static const tXCHAR *g_puP7files[] = 
{
    TM("uP7.c"),
    TM("uP7.h"),
    TM("uP7context.h"),
    TM("uP7hash.h"),
    TM("uP7helpers.h"),
    TM("uP7protocol.h"),
    TM("uP7version.h"),
    TM("uP7platform.h"),
    TM("uP7preprocessed.h"),
    TM("uP7IDs.h")
};

#define uP7_PREPREOCESSED_H  TM("/uP7preprocessed.h")
#define uP7_IDS_H            TM("/uP7IDs.h")
#define uP7_SESSION_TXT      "uint32_t g_uSessionId = "
#define uP7_CRC7_TXT         "uint8_t  g_bCrc7 = "


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CpreManager::CpreManager()
    : m_pOutDir(NULL)
    , m_pName(NULL)
    , m_eError(eErrorNo)
    , m_szTargetCpuBits(32)
    , m_szWcharBits(16)
    , m_bIDsHeader(false)
    , m_bConsecutiveId(false)
    , m_bVerbose(false)
{

}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CpreManager::~CpreManager()
{
    m_cTraces.Clear(FALSE);
    m_cModules.Clear(FALSE);
    m_cCounters.Clear(FALSE);

    m_cFunctions.Clear(TRUE);
    m_cFiles.Clear(TRUE);
    m_cSrcDirs.Clear(TRUE);

    if (m_pOutDir)
    {
        PStrFreeDub(m_pOutDir);
        m_pOutDir = NULL;
    }

    if (m_pName)
    {
        PStrFreeDub(m_pName);
        m_pName = NULL;
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CpreManager::AddSourcesDir(const tXCHAR* i_pDir)
{
    CWString l_cDir(i_pDir);
    //WARNING: sometimes directories can be provided without '/' at the end.
    //in such cases in function GetRelativePath in specific condition wrong match can be found
    //Let's say we have 2 added directories 
    // /home/user/p1/libMyLib
    // /home/user/p1/libMyLibHal
    //And then function is called GetRelativePath("/home/user/p1/libMyLibHal/myFile.cpp") will return
    // Hal/myFile.cpp because it is first match.
    //To avoid such error - need to put / at the end of the directory
    l_cDir.Append(1, TM("/"));
    m_cSrcDirs.Push_Last(NormalizePath(l_cDir.Get()));
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CpreManager::SetOutputDir(const tXCHAR* i_pDir)
{
    if (m_pOutDir)
    {
        PStrFreeDub(m_pOutDir);
    }

    m_pOutDir = NormalizePath(i_pDir);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CpreManager::SetTargetCpuBitsCount(size_t i_szBits)
{
    m_szTargetCpuBits = i_szBits;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CpreManager::SetName(const tXCHAR *i_pName)
{
    if (m_pName)
    {
        PStrFreeDub(m_pName);
    }

    m_pName = NormalizePath(i_pName);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CpreManager::SetTargetCpuWCharBitsCount(size_t i_szBits)
{
    m_szWcharBits = i_szBits;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CpreManager::EnableIDsHeader()
{
    m_bIDsHeader = true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CpreManager::EnableConsecutiveId()
{
    m_bConsecutiveId = true;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CpreManager::EnableVerbose()
{
    m_bVerbose = true;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
eErrorCodes CpreManager::AddFile(const tXCHAR *i_pPath)
{
    size_t l_szNewFile = PStrLen(i_pPath);
    bool   l_bAdd      = true;

    for (size_t l_szI = 0; l_szI < LENGTH(g_puP7files); l_szI++)
    {
        size_t l_szuP7File = PStrLen(g_puP7files[l_szI]);
        if (    (l_szNewFile > l_szuP7File)
             && (0 == PStrCmp(i_pPath + l_szNewFile - l_szuP7File, g_puP7files[l_szI]))
           )
        {
            //skip file - it is part of up7, we do need to scan it
            //OSPRINT(TM("INFO: Skip uP7 file {%s}\n"), i_pPath);
            l_bAdd = false;
        }
    }

    if (l_bAdd)
    {
        m_cFiles.Push_Last(new CpreFile(this, i_pPath, m_eError));
    }

    return m_eError;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
eErrorCodes CpreManager::AddFile(const tXCHAR *i_pPath, Cfg::INode *i_pNode)
{
    if (i_pNode)
    {
        m_cFiles.Push_Last(new CpreFile(this, i_pPath, i_pNode, m_eError));
    }

    return m_eError;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
tBOOL CpreManager::CheckFileHash(const tXCHAR *i_pName, const tXCHAR *i_pHash)
{
    const size_t l_szHashSize = CKeccak::EBITS_256 / 8;
    tUINT8       l_pHash[l_szHashSize];
    size_t       l_szHashLen  = PStrLen(i_pHash);
    CWString     l_cPath;
    int          l_iFound = 0;
    CWString     l_cTestPath;

    if (l_szHashLen != l_szHashSize * 2)
    {
        printf("Error: wrong hash size\n");
        return FALSE;
    }

    pAList_Cell l_pDirEl = NULL;
    while ((l_pDirEl = m_cSrcDirs.Get_Next(l_pDirEl)))
    {
        const tXCHAR *l_pDir = m_cSrcDirs.Get_Data(l_pDirEl);
        if (l_pDir)
        {
            l_cTestPath.Set(l_pDir);
            l_cTestPath.Append(2, TM("/"), i_pName);
            if (CFSYS::File_Exists(l_cTestPath.Get()))
            {
                l_cPath.Set(l_cTestPath.Get());
                l_iFound++;
            }
        }
    }

    if (0 >= l_iFound)
    {
        return TRUE; //not found, no hash to check!
    }

    if (1 < l_iFound)
    {
        OSPRINT(TM("WARNING: File {%s} exist in few input directories!\n"), i_pName);
    }

    tXCHAR *l_pPath = NormalizePath(l_cPath.Get());

    if (!l_pPath)
    {
        printf("Error: memory allocation error\n");
        return FALSE;
    }


    for (size_t l_szI = 0; l_szI < l_szHashSize; l_szI++)
    {
        CWString::Str2Hex(i_pHash + l_szI * 2, 2, l_pHash[l_szI]);
    }

    pAList_Cell l_pEl = NULL;

    while ((l_pEl = m_cFiles.Get_Next(l_pEl)))
    {
        CpreFile *l_pFile = m_cFiles.Get_Data(l_pEl);

        const tXCHAR *l_pFilePath  = l_pFile->GetOsPath();
        if (0 == PStrICmp(l_pFilePath, l_pPath))
        {
            if (0 == memcmp(l_pFile->GetHash(), l_pHash, l_szHashSize))
            {
                if (l_pFile->GetModification() != CpreFile::eReadOnly)
                {
                    l_pFile->SetModification(CpreFile::eNotModified);
                }
            }
            else if (m_bVerbose)
            {
                OSPRINT(TM("INFO: File {%s} has been modified since last generation\n"), i_pName);
            }
            break;
        }
    }

    free(l_pPath);
    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
tBOOL CpreManager::SetReadOnlyFile(const tXCHAR *i_pName)
{
    tXCHAR *l_pReadOnlyPath = NormalizePath(i_pName);
    if (!l_pReadOnlyPath)
    {
        printf("Error: memory allocation error\n");
        return FALSE;
    }

    pAList_Cell l_pEl            = NULL;
    size_t      l_szReadOnlyPath = PStrLen(l_pReadOnlyPath);

    while ((l_pEl = m_cFiles.Get_Next(l_pEl)))
    {
        CpreFile *l_pFile = m_cFiles.Get_Data(l_pEl);

        const tXCHAR *l_pFilePath  = l_pFile->GetOsPath();
        size_t        l_szFilePath = PStrLen(l_pFilePath);
        if (    (l_szFilePath >= l_szReadOnlyPath)
             && (0 == PStrICmp(l_pFilePath + l_szFilePath - l_szReadOnlyPath, l_pReadOnlyPath))
           )
        {
            l_pFile->SetModification(CpreFile::eReadOnly);
        }
    }

    free(l_pReadOnlyPath);
    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
tBOOL CpreManager::SetExcludedFile(const tXCHAR *i_pName)
{
    tXCHAR *l_pReadOnlyPath = NormalizePath(i_pName);
    if (!l_pReadOnlyPath)
    {
        printf("Error: memory allocation error\n");
        return FALSE;
    }

    pAList_Cell l_pEl            = NULL;
    size_t      l_szReadOnlyPath = PStrLen(l_pReadOnlyPath);

    while ((l_pEl = m_cFiles.Get_Next(l_pEl)))
    {
        CpreFile *l_pFile = m_cFiles.Get_Data(l_pEl);

        const tXCHAR *l_pFilePath  = l_pFile->GetOsPath();
        size_t        l_szFilePath = PStrLen(l_pFilePath);
        if (    (l_szFilePath >= l_szReadOnlyPath)
             && (0 == PStrICmp(l_pFilePath + l_szFilePath - l_szReadOnlyPath, l_pReadOnlyPath))
           )
        {
            m_cFiles.Del(l_pEl, TRUE);
            break;
        }
    }

    free(l_pReadOnlyPath);
    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int CpreManager::Process(Cfg::INode *i_pFiles)
{
    struct stSessionHeader
    {
        tUINT32  uSessionId;
        tUINT8   uCrc7;
        tUINT64  qwEpochTime;
        tUINT8   pSessionHash[CKeccak::EBITS_256 / 8];
        bool     bValid;
    };

    std::uniform_int_distribution<tUINT32> l_cRandSession(0u, 0xFFFFFFFFu);
    std::random_device l_cRd;

    pAList_Cell        l_pFileEl       = NULL;
    eErrorCodes        l_eError        = eErrorNo;
    tUINT32            l_uSessionId    = l_cRandSession(l_cRd);  
    tUINT64            l_qwEpochTime   = GetEpochTime();
    tUINT8             l_uSessionCrc7  = 0;
    CSessionFile      *l_pNewSession   = nullptr;
    stSessionHeader    l_stSessionH;
    CWString           l_cBinFilePath(m_pOutDir);

    while (uP7_PROTOCOL_SESSION_UNK == l_uSessionId)
    {
        l_uSessionId = l_cRandSession(l_cRd);
    }

    l_uSessionCrc7 = GetCrc7((const uint8_t *)&l_uSessionId, sizeof(l_uSessionId));


    l_cBinFilePath.Append(3, TM("/"), m_pName, TM(".") uP7_FILES_EXT);

    memset(&l_stSessionH, 0, sizeof(l_stSessionH));

    if (i_pFiles)
    {
        tXCHAR *l_pSfsHash = nullptr;

        uint32_t l_uCrc7 = 0;
        if (    (Cfg::eResult::eOk == i_pFiles->GetAttrUint32(XML_NARG_OPTIONS_FILES_CRC7, &l_uCrc7))
             && (Cfg::eResult::eOk == i_pFiles->GetAttrUint32(XML_NARG_OPTIONS_FILES_SESSION_ID, &l_stSessionH.uSessionId))
             && (Cfg::eResult::eOk == i_pFiles->GetAttrUint64(XML_NARG_OPTIONS_FILES_TIME, &l_stSessionH.qwEpochTime))
             && (Cfg::eResult::eOk == i_pFiles->GetAttrText(XML_NARG_OPTIONS_FILES_SFILE_HASH, &l_pSfsHash))
            )
        {
            l_stSessionH.uCrc7 = (tUINT8)l_uCrc7;
            if (eErrorNo == ScanHash(l_pSfsHash, l_stSessionH.pSessionHash))
            {
                l_stSessionH.bValid = TRUE;
            }
        }

    }

    while ((l_pFileEl = m_cFiles.Get_Next(l_pFileEl)))
    {
        CpreFile *l_pFile = m_cFiles.Get_Data(l_pFileEl);
        if (    (l_pFile)
             && (eErrorNo != (l_eError = l_pFile->GetError()))
           )
        {
            l_pFile->PrintErrors();
        }
    }

    if (eErrorNo != l_eError)
    {
        return l_eError;
    }

    //sorting files [read only .>. not modified .>. modified] to limit modifications only in recently modified files
    l_pFileEl = NULL;
    while ((l_pFileEl = m_cFiles.Get_Next(l_pFileEl)))
    {
        pAList_Cell l_pMin = l_pFileEl;
        pAList_Cell l_pCur = l_pFileEl;

        while ((l_pCur = m_cFiles.Get_Next(l_pCur)))
        {
            CpreFile *l_pFileM = m_cFiles.Get_Data(l_pMin);
            CpreFile *l_pFileI = m_cFiles.Get_Data(l_pCur);

            if (l_pFileM->GetModification() > l_pFileI->GetModification())
            {
                l_pMin = l_pCur;
            }
        }

        if (l_pMin != l_pFileEl)
        {
            m_cFiles.Extract(l_pMin);
            m_cFiles.Put_After(m_cFiles.Get_Prev(l_pFileEl), l_pMin);
            l_pFileEl = l_pMin;
        }
    } //while ((l_pFileEl = m_cFiles.Get_Next(l_pFileEl)))


    //printing files
    //l_pFileEl = NULL;
    //while ((l_pFileEl = m_cFiles.Get_Next(l_pFileEl)))
    //{
    //    CpreFile *l_pFileM = m_cFiles.Get_Data(l_pFileEl);
    //    printf("[%d] %s\n", l_pFileM->GetModification(), l_pFileM->GetPath());
    //} //while ((l_pFileEl = m_cFiles.Get_Next(l_pFileEl)))



    l_eError = CheckDuplicates();
    if (eErrorNo != l_eError)
    {
        return l_eError;
    }


    l_eError = ScanFunctions();
    if (eErrorNo != l_eError)
    {
        return l_eError;
    }


    //set ID
    if (eErrorNo == l_eError)
    {
        tUINT16     l_wId = 0;
        pAList_Cell l_pEl = NULL;

        while ((l_pEl = m_cCounters.Get_Next(l_pEl)))
        {
            m_cCounters.Get_Data(l_pEl)->SetId(l_wId++);
        }

        l_pEl = 0;
        l_wId = 0;
        while ((l_pEl = m_cModules.Get_Next(l_pEl)))
        {
            m_cModules.Get_Data(l_pEl)->SetId(l_wId++);
        }
    }


    if (eErrorNo == l_eError)
    {
        l_pFileEl = NULL;
        while (    ((l_pFileEl = m_cFiles.Get_Next(l_pFileEl)))
                && (eErrorNo == l_eError)
              )
        {
            CpreFile *l_pFile = m_cFiles.Get_Data(l_pFileEl);
            if (l_pFile)
            {
                l_pFile->Save();
            }
        }

        //Sort files to have predicable Session file
        SortFilesByNames();

        l_pNewSession = new CSessionFile(&m_cFiles, l_uSessionId, l_uSessionCrc7, l_qwEpochTime);
        l_eError = l_pNewSession->GetError();
    }

    if (eErrorNo == l_eError)
    {
        //Checking session file HASH and stored in XML
        if (    (l_stSessionH.bValid)
             && (0 == memcmp(l_stSessionH.pSessionHash, l_pNewSession->GetHash(), CKeccak::EBITS_256 / 8))
           )
        {
            if (m_bVerbose) OSPRINT(TM("INFO: Session file {%s} has not been modified (HASH==XML)!\n"), l_cBinFilePath.Get());

            l_pNewSession->SetSession(l_stSessionH.uSessionId, l_stSessionH.uCrc7, l_stSessionH.qwEpochTime);

            //check that session file is already generated and has the same parameters
            CSessionFile *l_pOldSession = new CSessionFile(l_cBinFilePath.Get());
            bool l_bNeedToSaveSession = true;
            if (l_pOldSession->GetError() == eErrorNo)
            {
                if (    (0 == memcmp(l_pOldSession->GetHash(), l_pNewSession->GetHash(), CKeccak::EBITS_256 / 8))
                     && (l_pOldSession->GetHeader()->qwTimeStamp == l_pNewSession->GetHeader()->qwTimeStamp)
                     && (l_pOldSession->GetHeader()->uCrc7 == l_pNewSession->GetHeader()->uCrc7)
                     && (l_pOldSession->GetHeader()->uSessionId == l_pNewSession->GetHeader()->uSessionId)
                   )
                {
                    //No need to regenerate
                    l_bNeedToSaveSession = false;
                }
                else
                {
                    if (m_bVerbose) OSPRINT(TM("WARNING: Stored on HDD File {%s} has unexpected data!\n"), l_cBinFilePath.Get());
                }
            }

            delete l_pOldSession;
            l_pOldSession = nullptr;

            if (l_bNeedToSaveSession)
            {
                if (m_bVerbose) OSPRINT(TM("INFO: Restore file {%s} !\n"), l_cBinFilePath.Get());
                l_eError = l_pNewSession->Save(l_cBinFilePath.Get());
            }
        }
        else
        {
            if (m_bVerbose) OSPRINT(TM("INFO: Generate file {%s} !\n"), l_cBinFilePath.Get());
            l_eError = l_pNewSession->Save(l_cBinFilePath.Get());
        }
    }


    if (eErrorNo == l_eError)
    {
        l_eError = CreateDescriptionHeaderFile(l_pNewSession->GetHeader()->uSessionId, 
                                               l_pNewSession->GetHeader()->uCrc7,
                                               l_pNewSession->GetHeader()->qwTimeStamp
                                              );
    }

    if (    (eErrorNo == l_eError)
         && (m_bIDsHeader)
       )
    {
        l_eError = CreateIDsHeaderFile(l_pNewSession->GetHeader()->qwTimeStamp);
    }

    //save to XML sessions parameters: id, crc, timestamp, hashes for different files
    if (    (eErrorNo == l_eError)
         && (i_pFiles)
       )
    {
        const size_t l_szHashSize = (CKeccak::EBITS_256 / 8)* 2 + 1;
        tXCHAR l_pHashSf[l_szHashSize];
        PrintHash(l_pNewSession->GetHash(), l_pHashSf);


        if (    (Cfg::eResult::eOk != i_pFiles->SetAttrUint32(XML_NARG_OPTIONS_FILES_CRC7, l_pNewSession->GetHeader()->uCrc7))
             || (Cfg::eResult::eOk != i_pFiles->SetAttrUint32(XML_NARG_OPTIONS_FILES_SESSION_ID, l_pNewSession->GetHeader()->uSessionId))
             || (Cfg::eResult::eOk != i_pFiles->SetAttrUint64(XML_NARG_OPTIONS_FILES_TIME, l_pNewSession->GetHeader()->qwTimeStamp))
             || (Cfg::eResult::eOk != i_pFiles->SetAttrText(XML_NARG_OPTIONS_FILES_SFILE_HASH, l_pHashSf))
           )
        {
            l_eError = eErrorXmlParsing;
        }
    }

    if (l_pNewSession)
    {
        delete l_pNewSession;
        l_pNewSession = nullptr;
    }

    return (int)l_eError;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
eErrorCodes CpreManager::ScanFunctions()
{
    std::uniform_int_distribution<tUINT16> l_cRandTraceId(0u, MAXUINT16);
    std::random_device l_cRd;

    pAList_Cell        l_pFileEl       = NULL;
    eErrorCodes        l_eError        = eErrorNo;
    stTraceId         *l_pTraceIdMap   = NULL;
    size_t             l_szTraceIt     = 0;
    size_t             l_szTraceMax    = 0;
    size_t             l_szTraceIdUsed = 0;


    //create traces ID table to store information about used ID
    l_pTraceIdMap = (stTraceId *)malloc(MAXUINT16 * sizeof(stTraceId));
    if (!l_pTraceIdMap)
    {
        printf("ERROR: can't allocate memory\n");
        return eErrorMemAlloc;
    }

    memset(l_pTraceIdMap, 0, MAXUINT16 * sizeof(stTraceId));


    //generate IDs & save binary data
    l_pFileEl = NULL;
    while ((l_pFileEl = m_cFiles.Get_Next(l_pFileEl)))
    {
        CpreFile *l_pFile = m_cFiles.Get_Data(l_pFileEl);
        if (l_pFile)
        {
            CBList<CFuncRoot*> &l_rFunctions = l_pFile->GetFunctions();
            pAList_Cell         l_pFuncEl    = NULL;

            while ((l_pFuncEl = l_rFunctions.Get_Next(l_pFuncEl)))
            {
                CFuncRoot *l_pFunc = l_rFunctions.Get_Data(l_pFuncEl);

                if (stFuncDesc::eType::eTrace == l_pFunc->GetType())
                {
                    CFuncTrace *l_pTrace = static_cast<CFuncTrace*>(l_pFunc);
                               
                    stTraceId *l_pTraceId = &l_pTraceIdMap[l_pTrace->GetId()];
                    if (!l_pTraceId->bUsed)
                    {
                        if (l_pTrace->GetId() > l_szTraceMax) l_szTraceMax = l_pTrace->GetId();

                        l_pTraceId->bUsed = true;
                        l_pTraceId->pFunc = l_pTrace;
                        l_szTraceIdUsed ++;
                    }
                    else
                    {
                        if (m_bVerbose)
                        {
                            OSPRINT(TM("INFO: Trace ID collision {%s}:%d vs {%s}:%d\n"), 
                                    GetRelativePath(l_pFile->GetOsPath()), 
                                    l_pFunc->GetLine(),
                                    GetRelativePath(l_pTraceId->pFunc->GetFile()->GetOsPath()),
                                    l_pTraceId->pFunc->GetLine()
                                    );
                        }

                        if (    (m_bConsecutiveId)
                                //if only 10% of free ID space is available - random selection isn't efficient any more 
                                //and will consume CPU cycles for nothing - better to switch to continuous search
                             || (l_szTraceIdUsed >= (MAXUINT16/10)) 
                           )
                        {
                            while (l_szTraceIt < MAXUINT16)
                            {
                                l_pTraceId = &l_pTraceIdMap[l_szTraceIt];
                                if (!l_pTraceId->bUsed)
                                {
                                    if (l_szTraceIt > l_szTraceMax) l_szTraceMax = l_szTraceIt;

                                    l_pTraceId->bUsed = true;
                                    l_pTraceId->pFunc = l_pTrace;
                                    l_pTrace->SetId((tUINT16)l_szTraceIt);

                                    if (m_bVerbose)
                                    {
                                        OSPRINT(TM("INFO: Update trace ID in file {%s}:%d\n"), 
                                                GetRelativePath(l_pFile->GetOsPath()), 
                                                l_pFunc->GetLine());
                                    }

                                    l_szTraceIdUsed++;
                                    l_szTraceIt++;
                                    break;
                                }
                                else
                                {
                                    l_szTraceIt++;
                                }
                            }
                        }
                        else
                        {
                            while (l_szTraceIdUsed < MAXUINT16)
                            {
                                l_szTraceIt = l_cRandTraceId(l_cRd);
                                l_pTraceId = &l_pTraceIdMap[l_szTraceIt];
                                if (!l_pTraceId->bUsed)
                                {
                                    l_pTraceId->bUsed = true;
                                    l_pTraceId->pFunc = l_pTrace;
                                    l_pTrace->SetId((tUINT16)l_szTraceIt);
                                    l_szTraceIdUsed++;

                                    if (m_bVerbose)
                                    {
                                        OSPRINT(TM("INFO: Update trace ID in file {%s}:%d\n"), 
                                                GetRelativePath(l_pFile->GetOsPath()), 
                                                l_pFunc->GetLine());
                                    }

                                    break;
                                }
                            }
                        }

                        if (l_szTraceIdUsed >= MAXUINT16)
                        {
                            l_eError = eErrorTraceIdOverFlow;

                            OSPRINT(TM("ERROR: in file {%s}\n"), GetRelativePath(l_pFile->GetOsPath()));
                            printf(" * Line:%d, %s\n", l_pFunc->GetLine(), g_pErrorsText[l_eError]);
                        }
                    }

                    pAList_Cell l_pTraceEl = NULL;
                    bool        l_bAdded   = false;
                    while ((l_pTraceEl = m_cTraces.Get_Next(l_pTraceEl)))
                    {
                        if (m_cTraces.Get_Data(l_pTraceEl)->GetId() >= l_pTrace->GetId())
                        {
                            l_bAdded   = true; 
                            l_pTraceEl = m_cTraces.Get_Prev(l_pTraceEl);
                            m_cTraces.Add_After(l_pTraceEl, l_pTrace);
                            break;
                        }
                    }

                    if (!l_bAdded)
                    {
                        m_cTraces.Add_After(m_cTraces.Get_Last(), l_pTrace);
                    }
                }
                else if (stFuncDesc::eType::eCreateCounter == l_pFunc->GetType())
                {
                    CFuncCounter *l_pCounter = static_cast<CFuncCounter*>(l_pFunc);

                    pAList_Cell   l_pCounterEl = NULL;
                    bool          l_bAdded     = false;
                    while ((l_pCounterEl = m_cCounters.Get_Next(l_pCounterEl)))
                    {
                        if (0 < strcmp(m_cCounters.Get_Data(l_pCounterEl)->GetName(), l_pCounter->GetName()))
                        {
                            l_bAdded   = true; 
                            l_pCounterEl = m_cCounters.Get_Prev(l_pCounterEl);
                            m_cCounters.Add_After(l_pCounterEl, l_pCounter);
                            break;
                        }
                    }

                    if (!l_bAdded)
                    {
                        m_cCounters.Add_After(m_cCounters.Get_Last(), l_pCounter);
                    }

                }
                else if (stFuncDesc::eType::eRegisterModule == l_pFunc->GetType())
                {
                    CFuncModule *l_pModule = static_cast<CFuncModule*>(l_pFunc);
                    pAList_Cell  l_pModEl  = NULL;
                    bool         l_bAdded  = false;

                    while ((l_pModEl = m_cModules.Get_Next(l_pModEl)))
                    {
                        if (0 < strcmp(m_cModules.Get_Data(l_pModEl)->GetName(), l_pModule->GetName()))
                        {
                            l_bAdded   = true; 
                            l_pModEl = m_cModules.Get_Prev(l_pModEl);
                            m_cModules.Add_After(l_pModEl, l_pModule);
                            break;
                        }
                    }

                    if (!l_bAdded)
                    {
                        m_cModules.Add_After(m_cModules.Get_Last(), l_pModule);
                    }
                    
                }
            }
        }
    }

    if (m_bConsecutiveId)
    {
        for (size_t l_szI = 0; l_szI < l_szTraceMax; l_szI++)    
        {
            stTraceId *l_pFreeId = &l_pTraceIdMap[l_szI];
            if (!l_pFreeId->bUsed)
            {
                while ((l_szTraceMax != l_szI) && (!l_pTraceIdMap[l_szTraceMax].bUsed))
                {
                    l_szTraceMax--;
                }

                if (l_szI < l_szTraceMax)
                {
                    l_pFreeId->bUsed = true; 
                    l_pFreeId->pFunc = l_pTraceIdMap[l_szTraceMax].pFunc;

                    l_pFreeId->pFunc->SetId((tUINT16)l_szI);

                    if (m_bVerbose)
                    {
                        OSPRINT(TM("INFO: Trace ID {%s}:%d relocation %zu->%zu\n"), 
                                GetRelativePath(l_pFreeId->pFunc->GetFile()->GetOsPath()), 
                                l_pFreeId->pFunc->GetLine(),
                                l_szTraceMax,
                                l_szI
                                );
                    }

                    l_pTraceIdMap[l_szTraceMax].pFunc = nullptr;
                    l_pTraceIdMap[l_szTraceMax].bUsed = false;
                    l_szTraceMax--;
                }
                else
                {
                    break;
                }
            }
        }
    }


    free(l_pTraceIdMap);
    l_pTraceIdMap = NULL;

    return l_eError;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
tBOOL CpreManager::SaveHashes(Cfg::INode *i_pFiles)
{
    const size_t  l_szHashSize = CKeccak::EBITS_256 / 8;
    pAList_Cell   l_pFileEl = NULL;
    tXCHAR        l_pTxtHash[l_szHashSize * 2 + 1];
    tBOOL         l_bReturn = TRUE;

    //Sort files to have predicable XML output
    SortFilesByNames();

    while ((l_pFileEl = m_cFiles.Get_Next(l_pFileEl)))
    {
        CpreFile *l_pFile = m_cFiles.Get_Data(l_pFileEl);
        if (    (l_pFile)
             && (!l_pFile->IsVirtual())
             && (l_pFile->GetModification() != CpreFile::eReadOnly)
           )
        {
            const tUINT8 *l_pHash = l_pFile->GetHash();

            for (size_t l_szI = 0; l_szI < l_szHashSize; l_szI++)
            {
                PSPrint(l_pTxtHash + l_szI * 2, 3, TM("%02X"), l_pHash[l_szI]);
            }

            Cfg::INode *l_pFileNode = NULL;

            i_pFiles->AddChildEmpty(XML_NODE_FILES_ITEM, &l_pFileNode);
            if (l_pFileNode)
            {
                l_pFileNode->SetAttrText(XML_NARG_FILES_ITEM_RELATIVE_PATH, GetRelativePath(l_pFile->GetOsPath()));
                l_pFileNode->SetAttrText(XML_NARG_FILES_ITEM_HASH, l_pTxtHash);
                l_pFileNode->Release();
            }
            else
            {
                l_bReturn = FALSE;
                break;
            }
        }
    }

    return l_bReturn;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CpreManager::SortFilesByNames()
{
    pAList_Cell l_pFileEl = NULL;

    //sorting to do not change XML all the time
    while ((l_pFileEl = m_cFiles.Get_Next(l_pFileEl)))
    {
        pAList_Cell l_pMax = l_pFileEl;
        pAList_Cell l_pCur = l_pFileEl;

        while ((l_pCur = m_cFiles.Get_Next(l_pCur)))
        {
            CpreFile *l_pFileM = m_cFiles.Get_Data(l_pMax);
            CpreFile *l_pFileI = m_cFiles.Get_Data(l_pCur);

            if (0 > PStrICmp(GetRelativePath(l_pFileI->GetOsPath()), GetRelativePath(l_pFileM->GetOsPath())))
            {
                l_pMax = l_pCur;
            }
        }

        if (l_pMax != l_pFileEl)
        {
            m_cFiles.Extract(l_pMax);
            m_cFiles.Put_After(m_cFiles.Get_Prev(l_pFileEl), l_pMax);
            l_pFileEl = l_pMax;
        }
    } //while ((l_pFileEl = m_cFiles.Get_Next(l_pFileEl)))
}



static const char *g_pVargs[]
{
    "euP7_arg_unk"      ,  //P7TRACE_ARG_TYPE_UNK    = 0x00,
    "euP7_arg_int8"     ,  //P7TRACE_ARG_TYPE_CHAR/P7TRACE_ARG_TYPE_INT8 = 0x01,
    "euP7_arg_char16"   ,  //P7TRACE_ARG_TYPE_CHAR16  ,//(0x02)
    "euP7_arg_int16"    ,  //P7TRACE_ARG_TYPE_INT16   ,//(0x03)
    "euP7_arg_int32"    ,  //P7TRACE_ARG_TYPE_INT32   ,//(0x04)
    "euP7_arg_int64"    ,  //P7TRACE_ARG_TYPE_INT64   ,//(0x05)
    "euP7_arg_double"   ,  //P7TRACE_ARG_TYPE_DOUBLE  ,//(0x06)
    "euP7_arg_pvoid"    ,  //P7TRACE_ARG_TYPE_PVOID   ,//(0x07)
    "euP7_arg_str_utf16",  //P7TRACE_ARG_TYPE_USTR16  ,//(0x08) //un
    "euP7_arg_str_ansi" ,  //P7TRACE_ARG_TYPE_STRA    ,//(0x09) //An
    "euP7_arg_str_utf8" ,  //P7TRACE_ARG_TYPE_USTR8   ,//(0x0A) //un
    "euP7_arg_str_utf32",  //P7TRACE_ARG_TYPE_USTR32  ,//(0x0B) //un
    "euP7_arg_char32"   ,  //P7TRACE_ARG_TYPE_CHAR32  ,//(0x0C)
    "euP7_arg_intmax"      //P7TRACE_ARG_TYPE_INTMAX  ,//(0x0D)
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
eErrorCodes CpreManager::CreateDescriptionHeaderFile(tUINT32 i_uSession, 
                                                     tUINT8  i_uSessionCrc7, 
                                                     tUINT64 i_qwEpochTime
                                                    )
{
    CWString             l_cHdrFilePath(m_pOutDir);
    CPFile               l_cuP7Hdrfile;
    std::vector<char>    l_cFileBody;
    char                *l_pBuffer   = NULL;
    size_t               l_szBuffer  = 16384;
    eErrorCodes          l_eError    = eErrorNo;
    size_t               l_szText    = 0;
    const char          *l_pPreamble = 
        "////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////\n"
        "//                                                     WARNING!                                                       //\n"
        "//                                       this header was automatically generated                                      //\n"
        "//                                                 DO NOT MODIFY IT                                                   //\n"
        "//                                   WE DO NOT RECOMMEND TO COMMIT IT (svn, hq, etc.)                                 //\n"
        "//                                  >>>INSTEAD PLEASE COMMIT XML CONFIGURATION FILE<<<                                //\n"
        "//                                           Generated: %04u.%02u.%02u %02u:%02u:%02u                                           //\n"
        "////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////\n"
        "#ifndef UP7_TARGET_CPU_H\n"
        "#define UP7_TARGET_CPU_H\n\n"
        "%s"
        uP7_SESSION_TXT "%u;\n"
        uP7_CRC7_TXT "%u;\n";
    

    l_cHdrFilePath.Append(1, uP7_PREPREOCESSED_H);

    l_pBuffer = (char*)malloc(l_szBuffer);
    if (!l_pBuffer)
    {
        OSPRINT(TM("ERROR: Can't allocate memory %zu\n"), l_szBuffer);
        return eErrorMemAlloc;
    }


    if (eErrorNo == l_eError)
    {
        tUINT32 l_uYear         = 0;
        tUINT32 l_uMonth        = 0;
        tUINT32 l_uDay          = 0;
        tUINT32 l_uHour         = 0;
        tUINT32 l_uMinutes      = 0;
        tUINT32 l_uSeconds      = 0;
        tUINT32 l_uMilliseconds = 0;
        tUINT32 l_uMicroseconds = 0;
        tUINT32 l_uNanoseconds  = 0;

        UnpackLocalTime(i_qwEpochTime,
                        l_uYear,
                        l_uMonth,
                        l_uDay,
                        l_uHour,
                        l_uMinutes,
                        l_uSeconds,
                        l_uMilliseconds,
                        l_uMicroseconds,
                        l_uNanoseconds
                       );

        l_szText = (size_t)sprintf(l_pBuffer, 
                                   l_pPreamble, 
                                   l_uYear,
                                   l_uMonth,
                                   l_uDay,
                                   l_uHour,
                                   l_uMinutes,
                                   l_uSeconds,
                                   (m_bConsecutiveId) ? "#define UP7_CONSECUTIVE_TRACE_ID\n\n" : "",
                                   i_uSession,
                                   (tUINT32)i_uSessionCrc7
                                  );

        l_cFileBody.insert(l_cFileBody.end(), l_pBuffer, l_pBuffer+l_szText);
    }

    if (eErrorNo == l_eError)
    {
        if (m_cModules.Count())
        {
            l_szText = (size_t)sprintf(l_pBuffer, 
                                       "\n\nsize_t g_szModules = %u;\n"
                                       "struct stuP7Module g_pModules[] = \n{", 
                                       m_cModules.Count());
            l_cFileBody.insert(l_cFileBody.end(), l_pBuffer, l_pBuffer+l_szText);
        }
        else
        {
            l_szText = (size_t)sprintf(l_pBuffer, "\n\nsize_t g_szModules = 0;\nstruct stuP7Module *g_pModules = NULL;\n\n");
            l_cFileBody.insert(l_cFileBody.end(), l_pBuffer, l_pBuffer+l_szText);
        }
    }

    if (    (eErrorNo == l_eError)
         && (m_cModules.Count())
       )
    {
        //generate modules description
        pAList_Cell l_pModEl = NULL;
        bool        l_bFirst  = true;

        while (    ((l_pModEl = m_cModules.Get_Next(l_pModEl)))
                && (eErrorNo == l_eError)
              )
        {
            CFuncModule *l_pModule = m_cModules.Get_Data(l_pModEl);
            if (l_pModule)
            {
                l_szText = (size_t)sprintf(l_pBuffer, 
                                            "%s\n    {\"%s\", %s, %u, %u}", 
                                            l_bFirst ? "" : ",",
                                            l_pModule->GetName(), 
                                            l_pModule->GetVerbosity(), 
                                            l_pModule->GetId(), 
                                            l_pModule->GetHash());

                l_bFirst = false;

                l_cFileBody.insert(l_cFileBody.end(), l_pBuffer, l_pBuffer+l_szText);
            }
        }

        if (eErrorNo == l_eError)
        {
            l_szText = (size_t)sprintf(l_pBuffer, "\n};\n\n"); 
            l_cFileBody.insert(l_cFileBody.end(), l_pBuffer, l_pBuffer+l_szText);
        }
    }

    if (eErrorNo == l_eError)
    {
        if (m_cCounters.Count())
        {
            l_szText = (size_t)sprintf(l_pBuffer, 
                                       "size_t g_szTelemetry = %u;\n"
                                       "struct stuP7telemetry g_pTelemetry[] = \n{",
                                       m_cCounters.Count()
                                      ); 
            l_cFileBody.insert(l_cFileBody.end(), l_pBuffer, l_pBuffer+l_szText);
        }
        else
        {
            l_szText = (size_t)sprintf(l_pBuffer, "size_t g_szTelemetry = 0;\nstruct stuP7telemetry *g_pTelemetry = NULL;\n\n");
            l_cFileBody.insert(l_cFileBody.end(), l_pBuffer, l_pBuffer+l_szText);
        }
    }


    if (    (eErrorNo == l_eError)
         && (m_cCounters.Count())
       )
    {
        //generate modules description
        pAList_Cell l_pCntEl = NULL;
        bool        l_bFirst  = true;

        while (    ((l_pCntEl = m_cCounters.Get_Next(l_pCntEl)))
                && (eErrorNo == l_eError)
              )
        {
            CFuncCounter *l_pCounter = m_cCounters.Get_Data(l_pCntEl);
            if (l_pCounter)
            {
                l_szText = (size_t)sprintf(l_pBuffer, 
                                            "%s\n    {\"%s\", %u, %s, %u}", 
                                            l_bFirst ? "" : ",",
                                            l_pCounter->GetName(), 
                                            l_pCounter->GetHash(), 
                                            l_pCounter->GetEnabled() ? "true" : "false",
                                            l_pCounter->GetId()
                    );

                l_bFirst = false;
                l_cFileBody.insert(l_cFileBody.end(), l_pBuffer, l_pBuffer+l_szText);
            }
        }

        if (eErrorNo == l_eError)
        {
            l_szText = (size_t)sprintf(l_pBuffer, "\n};\n\n"); 
            l_cFileBody.insert(l_cFileBody.end(), l_pBuffer, l_pBuffer+l_szText);
        }
    }

    if (    (eErrorNo == l_eError)
         && (m_cTraces.Count())
       )
    {
        //generate traces description
        pAList_Cell l_pTraceEl = NULL;

        while (    ((l_pTraceEl = m_cTraces.Get_Next(l_pTraceEl)))
                && (eErrorNo == l_eError)
              )
        {
            CFuncTrace         *l_pTrace = m_cTraces.Get_Data(l_pTraceEl);
            const sP7Trace_Arg *l_pVa    = NULL;
            size_t              l_szVA   = l_pTrace->GetVA(l_pVa);

            if (l_szVA)
            {
                l_szText = (size_t)sprintf(l_pBuffer, 
                                           "static const struct stuP7arg g_pArgsId%d[] = { ", 
                                           l_pTrace->GetId()
                                           );

                l_cFileBody.insert(l_cFileBody.end(), l_pBuffer, l_pBuffer+l_szText);



                for (size_t l_szI = 0; l_szI < l_szVA; l_szI++)
                {
                    if (l_pVa[l_szI].bType < LENGTH(g_pVargs))
                    {
                        l_szText = (size_t)sprintf(l_pBuffer, 
                                                    "{(uint8_t)%s, %u}%s", 
                                                    g_pVargs[l_pVa[l_szI].bType], 
                                                    l_pVa[l_szI].bSize,
                                                    ((l_szI + 1) == l_szVA) ? " };\n" : ", " 
                                                    );

                        l_cFileBody.insert(l_cFileBody.end(), l_pBuffer, l_pBuffer+l_szText);
                    }
                    else
                    {
                        printf("ERROR: trace format error parsing {%s} at {%s}:%d\n", 
                                l_pTrace->GetFormat(),
                                l_pTrace->GetFilePath(),
                                l_pTrace->GetLine()
                                );
                        l_eError = eErrorTraceFormat;
                        break;
                    }
                }
            }
        }
    }

    if (eErrorNo == l_eError)
    {
        if (m_cTraces.Count())
        {
            l_szText = (size_t)sprintf(l_pBuffer, 
                                       "\nsize_t g_szTraces = %u;\n"
                                       "struct stuP7Trace g_pTraces[] = \n{",
                                       m_cTraces.Count()
                                      ); 
            l_cFileBody.insert(l_cFileBody.end(), l_pBuffer, l_pBuffer+l_szText);
        }
        else
        {
            l_szText = (size_t)sprintf(l_pBuffer, "size_t g_szTraces = 0;\nstruct stuP7Trace *g_pTraces = NULL;\n");
            l_cFileBody.insert(l_cFileBody.end(), l_pBuffer, l_pBuffer+l_szText);
        }
    }

    if (    (eErrorNo == l_eError)
         && (m_cTraces.Count())
       )
    {
        //generate modules description
        bool        l_bFirst  = true;

        //generate traces description
        pAList_Cell l_pTraceEl = NULL;

        while (    ((l_pTraceEl = m_cTraces.Get_Next(l_pTraceEl)))
                && (eErrorNo == l_eError)
              )
        {
            CFuncTrace         *l_pTrace = m_cTraces.Get_Data(l_pTraceEl);
            const sP7Trace_Arg *l_pVa    = NULL;
            size_t              l_szVA   = l_pTrace->GetVA(l_pVa);

            if (l_szVA)
            {
                l_szText = (size_t)sprintf(l_pBuffer, 
                                            "%s\n    {%u, sizeof(g_pArgsId%u)/sizeof(struct stuP7arg), g_pArgsId%u}", 
                                            l_bFirst ? "" : ",",
                                            l_pTrace->GetId(),
                                            l_pTrace->GetId(),
                                            l_pTrace->GetId()
                                            );
            }
            else
            {
                l_szText = (size_t)sprintf(l_pBuffer, 
                                            "%s\n    {%u, 0, NULL}", 
                                            l_bFirst ? "" : ",",
                                            l_pTrace->GetId()
                                            );
            }

            l_bFirst = false;
            l_cFileBody.insert(l_cFileBody.end(), l_pBuffer, l_pBuffer+l_szText);
        }

        if (eErrorNo == l_eError)
        {
            l_szText = (size_t)sprintf(l_pBuffer, "\n};"); 
            l_cFileBody.insert(l_cFileBody.end(), l_pBuffer, l_pBuffer+l_szText);
        }
    }

    if (eErrorNo == l_eError)
    {
        l_szText = (size_t)sprintf(l_pBuffer, "\n#endif"); 
        l_cFileBody.insert(l_cFileBody.end(), l_pBuffer, l_pBuffer+l_szText);
    }


    //calculating hash of existing file
    CKeccak l_cHash;
    tUINT8 l_pMemHash[CKeccak::EBITS_256 / 8];
    tUINT8 l_pHddHash[CKeccak::EBITS_256 / 8];
    memset(l_pHddHash, 0xA, CKeccak::EBITS_256 / 8);
    memset(l_pMemHash, 0xB, CKeccak::EBITS_256 / 8);


    l_cHash.UpdateB((const tUINT8*)l_cFileBody.data(), l_cFileBody.size());
    l_cHash.Get_HashB(l_pMemHash, CKeccak::EBITS_256 / 8);

    if (CFSYS::File_Exists(l_cHdrFilePath.Get()))
    {
        if (l_cuP7Hdrfile.Open(l_cHdrFilePath.Get(), IFile::EOPEN | IFile::EACCESS_READ))
        {
            uint64_t l_qwSize = l_cuP7Hdrfile.Get_Size();
            char *l_pFileData = (char*)malloc((size_t)l_qwSize);
            if (l_pFileData)
            {
                if (l_qwSize == l_cuP7Hdrfile.Read((tUINT8*)l_pFileData, (size_t)l_qwSize))
                {
                    l_cHash.UpdateB((const tUINT8*)l_pFileData, (size_t)l_qwSize);
                    l_cHash.Get_HashB(l_pHddHash, CKeccak::EBITS_256 / 8);
                }

                free(l_pFileData);
            }

            l_cuP7Hdrfile.Close(FALSE);
        }
    }

    if (0 != memcmp(l_pMemHash, l_pHddHash, CKeccak::EBITS_256 / 8))
    {
        if (m_bVerbose) OSPRINT(TM("INFO: File {%s} has been changed, regenerating!\n"), l_cHdrFilePath.Get());
       
        if (l_cuP7Hdrfile.Open(l_cHdrFilePath.Get(), IFile::ECREATE | IFile::ESHARE_READ | IFile::EACCESS_WRITE))
        {
            if (l_cFileBody.size() != l_cuP7Hdrfile.Write((const tUINT8*)l_cFileBody.data(), l_cFileBody.size(), FALSE))
            {
                OSPRINT(TM("ERROR: Can't write file {%s}\n"), l_cHdrFilePath.Get());
                l_eError = eErrorFileWrite;
            }
            l_cuP7Hdrfile.Close(TRUE);
        }
        else
        {
            OSPRINT(TM("ERROR: Can't open file {%s}\n"), l_cHdrFilePath.Get());
            l_eError = eErrorFileWrite;
        }
    }
    else
    {
        if (m_bVerbose) OSPRINT(TM("INFO: The file {%s} has not been modified!\n"), l_cHdrFilePath.Get());
    }

    free(l_pBuffer);
    l_pBuffer = NULL;

    return l_eError;
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
eErrorCodes CpreManager::CreateIDsHeaderFile(tUINT64 i_qwEpochTime)
{
    CWString             l_cHdrFilePath(m_pOutDir);
    CPFile               l_cuP7Hdrfile;
    char                *l_pBuffer   = NULL;
    size_t               l_szBuffer  = 16384;
    eErrorCodes          l_eError    = eErrorNo;
    size_t               l_szText    = 0;
    std::vector<char>    l_cFileBody;
    const size_t         l_szDefine  = 1024;
    char                 l_pDefine[l_szDefine];
    const char          *l_pPreamble = 
        "////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////\n"
        "//                                                     WARNING!                                                       //\n"
        "//                                       this header was automatically generated                                      //\n"
        "//                                                 DO NOT MODIFY IT                                                   //\n"
        "//                                   WE DO NOT RECOMMEND TO COMMIT IT (svn, hq, etc.)                                 //\n"
        "//                                  >>>INSTEAD PLEASE COMMIT XML CONFIGURATION FILE<<<                                //\n"
        "////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////\n"
        "#ifndef UP7_IDS_H\n"
        "#define UP7_IDS_H\n\n";
    

    l_cHdrFilePath.Append(1, uP7_IDS_H);

    l_pBuffer = (char*)malloc(l_szBuffer);
    if (!l_pBuffer)
    {
        OSPRINT(TM("ERROR: Can't allocate memory %zu\n"), l_szBuffer);
        return eErrorMemAlloc;
    }


    if (eErrorNo == l_eError)
    {
        tUINT32 l_uYear         = 0;
        tUINT32 l_uMonth        = 0;
        tUINT32 l_uDay          = 0;
        tUINT32 l_uHour         = 0;
        tUINT32 l_uMinutes      = 0;
        tUINT32 l_uSeconds      = 0;
        tUINT32 l_uMilliseconds = 0;
        tUINT32 l_uMicroseconds = 0;
        tUINT32 l_uNanoseconds  = 0;

        UnpackLocalTime(i_qwEpochTime,
                        l_uYear,
                        l_uMonth,
                        l_uDay,
                        l_uHour,
                        l_uMinutes,
                        l_uSeconds,
                        l_uMilliseconds,
                        l_uMicroseconds,
                        l_uNanoseconds
                       );

        //l_szText = (size_t)sprintf(l_pBuffer, l_pPreamble);

        l_cFileBody.insert(l_cFileBody.end(), l_pPreamble, l_pPreamble + strlen(l_pPreamble));
    }

    if (    (eErrorNo == l_eError)
         && (m_cModules.Count())
       )
    {
        //generate modules description
        pAList_Cell l_pModEl = NULL;

        while ((l_pModEl = m_cModules.Get_Next(l_pModEl)))
        {
            CFuncModule *l_pModule = m_cModules.Get_Data(l_pModEl);
            if (l_pModule)
            {
                if (eErrorNo == GenerateDefineName(l_pModule->GetName(), l_pDefine, l_szDefine))
                {
                    l_szText = (size_t)sprintf(l_pBuffer, 
                                               "#define uP7M_%s %du%s", 
                                               l_pDefine, 
                                               l_pModule->GetId(),
                                               (l_pModEl == m_cModules.Get_Last()) ? "\n\n" : "\n"
                                              );

                    l_cFileBody.insert(l_cFileBody.end(), l_pBuffer, l_pBuffer+l_szText);
                }
                else
                {
                    OSPRINT(TM("ERROR: Can't generate define name for file {%s}\n"), l_cHdrFilePath.Get());
                    l_eError = eErrorArguments;
                    break;
                }
            }
        }
    }


    if (    (eErrorNo == l_eError)
         && (m_cCounters.Count())
       )
    {
        //generate modules description
        pAList_Cell l_pCntEl = NULL;

        while ((l_pCntEl = m_cCounters.Get_Next(l_pCntEl)))
        {
            CFuncCounter *l_pCounter = m_cCounters.Get_Data(l_pCntEl);
            if (l_pCounter)
            {
                if (eErrorNo == GenerateDefineName(l_pCounter->GetName(), l_pDefine, l_szDefine))
                {
                    l_szText = (size_t)sprintf(l_pBuffer, 
                                               "#define uP7T_%s %du%s", 
                                               l_pDefine, 
                                               l_pCounter->GetId(),
                                               (l_pCntEl == m_cModules.Get_Last()) ? "\n\n" : "\n"
                                              );

                    l_cFileBody.insert(l_cFileBody.end(), l_pBuffer, l_pBuffer+l_szText);
                }
                else
                {
                    OSPRINT(TM("ERROR: Can't generate define name for file {%s}\n"), l_cHdrFilePath.Get());
                    l_eError = eErrorArguments;
                    break;
                }
            }
        }
    }

    if (eErrorNo == l_eError)
    {
        l_szText = (size_t)sprintf(l_pBuffer, "\n#endif //UP7_IDS_H"); 
        l_cFileBody.insert(l_cFileBody.end(), l_pBuffer, l_pBuffer+l_szText);
    }

    //calculating hash of existing file
    CKeccak l_cHash;
    tUINT8 l_pMemHash[CKeccak::EBITS_256 / 8];
    tUINT8 l_pHddHash[CKeccak::EBITS_256 / 8];
    memset(l_pHddHash, 0xA, CKeccak::EBITS_256 / 8);
    memset(l_pMemHash, 0xB, CKeccak::EBITS_256 / 8);


    l_cHash.UpdateB((const tUINT8*)l_cFileBody.data(), l_cFileBody.size());
    l_cHash.Get_HashB(l_pMemHash, CKeccak::EBITS_256 / 8);

    if (CFSYS::File_Exists(l_cHdrFilePath.Get()))
    {
        if (l_cuP7Hdrfile.Open(l_cHdrFilePath.Get(), IFile::EOPEN | IFile::EACCESS_READ | IFile::ESHARE_READ))
        {
            uint64_t l_qwSize = l_cuP7Hdrfile.Get_Size();
            char *l_pFileData = (char*)malloc((size_t)l_qwSize);
            if (l_pFileData)
            {
                if (l_qwSize == l_cuP7Hdrfile.Read((tUINT8*)l_pFileData, (size_t)l_qwSize))
                {
                    l_cHash.UpdateB((const tUINT8*)l_pFileData, (size_t)l_qwSize);
                    l_cHash.Get_HashB(l_pHddHash, CKeccak::EBITS_256 / 8);
                }

                free(l_pFileData);
            }

            l_cuP7Hdrfile.Close(FALSE);
        }
    }

    if (0 != memcmp(l_pMemHash, l_pHddHash, CKeccak::EBITS_256 / 8))
    {
        if (m_bVerbose) OSPRINT(TM("INFO: File {%s} has been changed, regenerating!\n"), l_cHdrFilePath.Get());

        if (l_cuP7Hdrfile.Open(l_cHdrFilePath.Get(), IFile::ECREATE | IFile::ESHARE_READ | IFile::EACCESS_WRITE))
        {
            if (l_cFileBody.size() != l_cuP7Hdrfile.Write((const tUINT8*)l_cFileBody.data(), l_cFileBody.size(), FALSE))
            {
                OSPRINT(TM("ERROR: Can't write file {%s}\n"), l_cHdrFilePath.Get());
                l_eError = eErrorFileWrite;
            }
            l_cuP7Hdrfile.Close(TRUE);
        }
        else
        {
            OSPRINT(TM("ERROR: Can't open file {%s}\n"), l_cHdrFilePath.Get());
            l_eError = eErrorFileWrite;
        }
    }
    else
    {
        if (m_bVerbose) OSPRINT(TM("INFO: The file {%s} has not been modified!\n"), l_cHdrFilePath.Get());
    }

    free(l_pBuffer);
    l_pBuffer = NULL;

    return l_eError;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
eErrorCodes CpreManager::ScanHash(const tXCHAR *i_pTxtHash, tUINT8 o_pSessionHash[CKeccak::EBITS_256 / 8])
{
    eErrorCodes l_eError = eErrorNo;
    const size_t  l_szHashSize = CKeccak::EBITS_256 / 8;

    for (size_t l_szI = 0; l_szI < l_szHashSize; l_szI++)
    {
        uint32_t l_uVal = 0;
        
        if (PStrScan(i_pTxtHash + l_szI * 2, TM("%02X"), &l_uVal))
        {
            o_pSessionHash[l_szI] = (uint8_t)l_uVal;
        }
        else
        {
            l_eError = eErrorFileRead;
            break;
        }
    }

    return l_eError;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
eErrorCodes CpreManager::CheckDuplicates()
{
    eErrorCodes          l_eError = eErrorNo;
    CBList<CFuncModule*> l_cModules;
    CBList<CFuncCounter*>l_cCounters;

    //create lists of modules, traces, telemetry
    pAList_Cell l_pFileEl = NULL;

    while ((l_pFileEl = m_cFiles.Get_Next(l_pFileEl)))
    {
        CpreFile *l_pFile = m_cFiles.Get_Data(l_pFileEl);
        if (l_pFile)
        {
            CBList<CFuncRoot*> &l_rFunctions = l_pFile->GetFunctions();

            pAList_Cell l_pFuncEl = NULL;
            while ((l_pFuncEl = l_rFunctions.Get_Next(l_pFuncEl)))
            {
                CFuncRoot *l_pFunc = l_rFunctions.Get_Data(l_pFuncEl);

                if (stFuncDesc::eType::eRegisterModule == l_pFunc->GetType())
                {
                    CFuncModule *l_pMod   = static_cast<CFuncModule*>(l_pFunc);
                    pAList_Cell  l_pModEl = NULL;
                    bool         l_bDupl  = false;

                    while ((l_pModEl = l_cModules.Get_Next(l_pModEl)))
                    {
                        if (0 == strcmp(l_cModules.Get_Data(l_pModEl)->GetName(), l_pMod->GetName()))
                        {
                            if (m_bVerbose)
                            {
                                OSPRINT(TM("WARNING: at file {%s}\n"), GetRelativePath(l_pFile->GetOsPath()));
                                printf(" * Line:%d, register module call duplicate\n", l_pMod->GetLine());
                                OSPRINT(TM(" * Conflicted with {%s}:%d\n"), 
                                        GetRelativePath(l_cModules.Get_Data(l_pModEl)->GetFile()->GetOsPath()), 
                                        l_cModules.Get_Data(l_pModEl)->GetLine());
                            }
                            

                            pAList_Cell l_pFuncPrevEl = l_rFunctions.Get_Prev(l_pFuncEl);
                            l_rFunctions.Del(l_pFuncEl, TRUE);
                            l_pFuncEl = l_pFuncPrevEl;

                            l_bDupl = true;
                            break;
                        }
                    }

                    if (!l_bDupl)
                    {
                        l_cModules.Add_After(l_cModules.Get_Last(), l_pMod);
                    }
                }
                else if (stFuncDesc::eType::eCreateCounter == l_pFunc->GetType())
                {
                    CFuncCounter *l_pCounter   = static_cast<CFuncCounter*>(l_pFunc);
                    pAList_Cell   l_pCounterEl = NULL;
                    bool          l_bDupl      = false;

                    while ((l_pCounterEl = l_cCounters.Get_Next(l_pCounterEl)))
                    {
                        if (0 == strcmp(l_cCounters.Get_Data(l_pCounterEl)->GetName(), l_pCounter->GetName()))
                        {
                            l_bDupl  = true;
                            if (!l_pCounter->IsEqual(l_cCounters.Get_Data(l_pCounterEl)))
                            {
                                l_eError = eErrorNameDuplicated;

                                OSPRINT(TM("ERROR: at file {%s}\n"), GetRelativePath(l_pFile->GetOsPath()));
                                printf(" * Line:%d, %s\n", l_pCounter->GetLine(), g_pErrorsText[l_eError]);
                                OSPRINT(TM(" * Conflicted with {%s}:%d\n"), 
                                        GetRelativePath(l_cCounters.Get_Data(l_pCounterEl)->GetFile()->GetOsPath()), 
                                        l_cCounters.Get_Data(l_pCounterEl)->GetLine());

                                break;
                            }
                            else
                            {
                                pAList_Cell l_pFuncPrevEl = l_rFunctions.Get_Prev(l_pFuncEl);
                                l_rFunctions.Del(l_pFuncEl, TRUE);
                                l_pFuncEl = l_pFuncPrevEl;

                                break;
                            }
                        }
                    }

                    if (!l_bDupl)
                    {
                        l_cCounters.Add_After(l_cCounters.Get_Last(), l_pCounter);
                    }
                }
            }
        }

        if (eErrorNo != l_eError)
        {
            break;
        }
    }

    l_cModules.Clear(FALSE);
    l_cCounters.Clear(FALSE);

    return l_eError;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
eErrorCodes CpreManager::GenerateDefineName(const char *i_pName, char *o_pDefine, size_t i_szDefineMax)
{
    static const char  l_pAllowed[]    = "_abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    static const char  l_pReplaceSrc[] = "abcdefghijklmnopqrstuvwxyz";
    static const char  l_pReplaceDst[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    size_t             l_szReplace     = LENGTH(l_pReplaceSrc);
    
    if (!i_pName)
    {
        return eErrorArguments;
    }

    char *l_pDefgineIt = o_pDefine;

    while (    (*i_pName)
            && (i_szDefineMax > 1)
          )
    {
        const char *l_pAllowedIt = l_pAllowed;
        bool        l_bReplace   = true;

        while (*l_pAllowedIt)
        {
            if (*l_pAllowedIt == *i_pName)
            {
                l_bReplace = false;
                break;
            }
            l_pAllowedIt++;
        }
        
        *l_pDefgineIt = l_bReplace ? '_' : *i_pName;

        i_pName++;
        l_pDefgineIt++;
        i_szDefineMax--;
    }

    if (*i_pName)
    {
        return eErrorArguments;
    }

    *l_pDefgineIt = 0;

    l_pDefgineIt = o_pDefine;

    while (*l_pDefgineIt)
    {
        for (size_t szI = 0; szI < l_szReplace; szI++)
        {
            if (*l_pDefgineIt == l_pReplaceSrc[szI])
            {
                *l_pDefgineIt = l_pReplaceDst[szI];
                break;
            }
        }

        l_pDefgineIt++;
    }


    return eErrorNo;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const tXCHAR *CpreManager::GetRelativePath(const tXCHAR *i_pFilePath)
{
    const tXCHAR *l_pReturn = TM("ERROR");
    pAList_Cell l_pDirEl = NULL;

    while ((l_pDirEl = m_cSrcDirs.Get_Next(l_pDirEl)))
    {
        const tXCHAR *l_pDir = m_cSrcDirs.Get_Data(l_pDirEl);
        if (l_pDir)
        {
            size_t l_szDirLen = PStrLen(l_pDir);
            if (0 == PStrNiCmp(l_pDir, i_pFilePath, l_szDirLen))
            {
                l_pReturn = i_pFilePath + l_szDirLen;
                break;
            }
        }
    }

    while (*l_pReturn == TM('/')) l_pReturn++;

    return l_pReturn;
}
