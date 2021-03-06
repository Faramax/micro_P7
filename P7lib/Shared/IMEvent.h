////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                     /
// This library is free software; you can redistribute it and/or modify it under the terms of the  GNU  Lesser  General/
// Public License as published by the Free Software Foundation; either version 3.0 of the License, or (at your  option)/
// any later version.                                                                                                  /
// This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even  the  implied/
// warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more/
// details.                                                                                                            /
// You should have received a copy of the GNU Lesser General Public License along with this library.                   /
//                                                                                                                     /
// 2012-2021 (c) Baical                                                                                                /
//                                                                                                                     /
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef IMEVENT_H
#define IMEVENT_H


////////////////////////////////////////////////////////////////////////////////
enum eMEvent_Type
{
    EMEVENT_SINGLE_AUTO         = 0,
    EMEVENT_SINGLE_MANUAL          ,
    EMEVENT_MULTI
};


#define MEVENT_TIME_OUT        (0xFFFFFFF)
#define MEVENT_SIGNAL_0        (0)


////////////////////////////////////////////////////////////////////////////////
class IMEvent
{
public:
    //For example 
    //Init(3, EMEVENT_SINGLE_AUTO, EMEVENT_SINGLE_MANUAL, EMEVENT_MULTI);  
    virtual tBOOL   Init(tUINT8 i_bCount, ...)                              = 0;
    virtual tBOOL   Set(tUINT32 i_dwID)                                     = 0;
    virtual tBOOL   Clr(tUINT32 i_dwID)                                     = 0;
    virtual tUINT32 Wait()                                                  = 0;
    virtual tUINT32 Wait(tUINT32 i_dwMSec)                                  = 0;
};

#endif //IMEVENT_H
