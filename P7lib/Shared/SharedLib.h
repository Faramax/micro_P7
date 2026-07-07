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
#ifndef ISHARED_LIB_H
#define ISHARED_LIB_H

class CSharedLib
{
protected:
    tINT32 volatile m_lReference;
    CWString        m_cPath;
    tBOOL           m_bState;

public:
    CSharedLib(const tXCHAR *i_pPath)
        : m_lReference(1)
        , m_cPath(i_pPath)
        , m_bState(FALSE)
    {
    }

    virtual void  *GetFunction(const tACHAR *i_pName) = 0;

    tBOOL GetState() 
    {
        return m_bState;
    }

    tINT32 Add_Ref()
    {
        return ATOMIC_INC(&m_lReference);
    }

    tINT32 Release()
    {
        tINT32 l_lResult = ATOMIC_DEC(&m_lReference);
        if ( 0 >= l_lResult )
        {
            delete this;
        }

        return l_lResult;
    }

    virtual ~CSharedLib() {}
};

#endif //ISHARED_LIB_H
