/*---------------------------------------------------------------------------------


Copyright (C) 2007 Acekard, www.acekard.com

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.


---------------------------------------------------------------------------------*/









#ifndef _MSGDISPLAY_H_
#define _MSGDISPLAY_H_

#include <nds.h>

enum MSG_IDS
{
    MSGID_DONT_TURN_OFF = 0,
    MSGID_FAT_ERROR,
    MSGID_LAUNCHING_RPG,
    MSGID_LOADING,
    MSGID_LOADING_PROGRAM,
    MSGID_PROC_SAVE_DATA,
    MSGID_SYS_FILE_MISSING
};

void initMsg();
void cleanMsg();
void setMsg( u32 msgID, u16 line );
void showMsg();
void showMsg( u32 msgID, u16 line );

#endif//_MSGDISPLAY_H_
