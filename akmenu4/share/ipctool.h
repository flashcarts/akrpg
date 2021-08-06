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









#ifndef _IPC_TOOL_H_
#define _IPC_TOOL_H_

#define IPC_MSG_NONE                    0
#define IPC_MSG_BOOT_SLOT1                1
#define IPC_MSG_BOOT_SLOT2                2
#define IPC_MSG_ARM7_READY_BOOT            3
#define IPC_MSG_LOAD_HOMEBREW            4
#define IPC_MSG_ARM7_LOAD_DONE            5
#define IPC_MSG_ARM7_REBOOT                6
#define IPC_MSG_GET_BRIGHTNESS            7
#define IPC_MSG_SET_BRIGHTNESS            8
#define IPC_MSG_SUSPEND                 9
#define IPC_MSG_WAKEUP                 10


#define IPC_ARM9    (*(vu32*)0x27ff200)    //message from arm9
#define IPC_ARM9_P1 (*(vu32*)0x27ff204) // parameter 1 from arm9
#define IPC_ARM9_P2 (*(vu32*)0x27ff208) // parameter 2 from arm9
#define IPC_ARM7    (*(vu32*)0x27ff20C)    //message from arm7


#endif//_IPC_TOOL_H_

