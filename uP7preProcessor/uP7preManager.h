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
#ifndef UP7_MANAGER_H
#define UP7_MANAGER_H

class CpreFile; //forward definition

class CpreManager
{
    class CStrList: public CBList<tXCHAR*>
    {
    protected:
        virtual tBOOL Data_Release(tXCHAR* i_pData)
        {
            if (NULL == i_pData)
            {
                return FALSE;
            }

            PStrFreeDub(i_pData);
            return TRUE;
        }//CListBase::Data_Release

    };

    struct stTraceId
    {
        CFuncTrace *pFunc;
        bool        bUsed;
    };


    CFunctionsList       m_cFunctions;
    CStrList             m_cSrcDirs;
    tXCHAR              *m_pOutDir;
    tXCHAR              *m_pName;
    CBList<CpreFile*>    m_cFiles;
    CBList<CFuncTrace*>  m_cTraces;
    CBList<CFuncModule*> m_cModules;
    CBList<CFuncCounter*>m_cCounters;
    eErrorCodes          m_eError;
    size_t               m_szTargetCpuBits;
    size_t               m_szWcharBits;
    bool                 m_bIDsHeader;
    bool                 m_bConsecutiveId;
    bool                 m_bVerbose;

public:
    CpreManager();
    virtual ~CpreManager();
    CFunctionsList& GetFunctions() {return m_cFunctions;}
    stFuncDesc     *GetFuncDesc(stFuncDesc::eType i_eType);
    void            AddSourcesDir(const tXCHAR *i_pDir);
    void            SetOutputDir(const tXCHAR *i_pDir);
    void            SetName(const tXCHAR *i_pName);
    const tXCHAR*   GetName() { return m_pName; }
    void            SetTargetCpuBitsCount(size_t i_szBits);
    size_t          GetTargetCpuBitsCount() { return m_szTargetCpuBits; }
    void            SetTargetCpuWCharBitsCount(size_t i_szBits);
    size_t          GetTargetCpuWCharBitsCount() { return m_szWcharBits; }
    void            EnableIDsHeader();
    void            EnableConsecutiveId();
    void            EnableVerbose();

    eErrorCodes     AddFile(const tXCHAR *i_pPath);
    eErrorCodes     AddFile(const tXCHAR *i_pPath, Cfg::INode *i_pNode);
    tBOOL           CheckFileHash(const tXCHAR *i_pName, const tXCHAR *i_pHash);
    tBOOL           SetReadOnlyFile(const tXCHAR *i_pName);
    tBOOL           SetExcludedFile(const tXCHAR *i_pName);

    int             Process(Cfg::INode *i_pFiles);
    eErrorCodes     ScanFunctions();
    tBOOL           SaveHashes(Cfg::INode *i_pFiles);
    const tXCHAR   *GetRelativePath(const tXCHAR *i_pFilePath);
private:
    void            SortFilesByNames();

    eErrorCodes     CreateDescriptionHeaderFile(tUINT32  i_uSession, 
                                                tUINT8   i_uSessionCrc7,  
                                                tUINT64  i_qwEpochTime
                                               );
    eErrorCodes     CreateIDsHeaderFile(tUINT64 i_qwEpochTime);
    eErrorCodes     ScanHash(const tXCHAR *i_pTxtHash, tUINT8 o_pSessionHash[CKeccak::EBITS_256 / 8]);
    eErrorCodes     CheckDuplicates();
    eErrorCodes     GenerateDefineName(const char *i_pName, char *o_pDefine, size_t i_szDefineMax);


    inline void     PrintHash(const tUINT8 i_pHashBin[CKeccak::EBITS_256 / 8], tXCHAR *o_pHashTxt)
    {
        const size_t l_szHashSize = CKeccak::EBITS_256 / 8;

        for (size_t l_szI = 0; l_szI < l_szHashSize; l_szI++)
        {
            PSPrint(o_pHashTxt + l_szI * 2, 3, TM("%02X"), i_pHashBin[l_szI]);
        }
    }
};

#endif //UP7_MANAGER_H
