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









#include <nds.h>
#include <string.h>
#include <nds/arm7/clock.h>
#include "../../share/timetool.h"
#include "../../share/ipctool.h"
#include "../../share/memtool.h"

void VblankHandler(void) {}

// arm9 can not access memory space 0x37f8000 - 0x380FFFF
// load those arm7 code which can not be loaded by arm9 here
// this function can not load official roms because their arm7 code are big
static void loadHomebrewArm7Code()
{
    // load_arm7
    u8 * arm7_dest_mem = (u8 *)NDSHeader.arm7destination;
    u32 arm7_code = IPC_ARM9_P2;
    u32 arm7_size = NDSHeader.arm7binarySize;
    memcpy( arm7_dest_mem, (u8 *)arm7_code, arm7_size );
    IPC_ARM7 = IPC_MSG_ARM7_LOAD_DONE;
}

static void prepairReset()
{
    powerON(POWER_SOUND);

    //reset DMA
    zeroMemory( (void *)0x40000B0, 0x30 );

    REG_IME = IME_DISABLE;
    REG_IE = 0;
    REG_IF = ~0;

    IPC_ARM7 = IPC_MSG_ARM7_READY_BOOT;
}

int main(int argc, char ** argv)
{
    IPC_ARM9 = IPC_MSG_NONE;
    IPC_ARM7 = IPC_MSG_NONE;

    // Reset the clock if needed
    rtcReset();

    irqInit();
    irqSet(IRQ_VBLANK, VblankHandler);
    irqEnable(IRQ_VBLANK);

    // Keep the ARM7 out of main RAM
    while ( true ) {
        swiWaitForVBlank();
        vu32 ipc_msg = IPC_ARM9;
        switch( ipc_msg )
        {
        case IPC_MSG_LOAD_HOMEBREW:
            loadHomebrewArm7Code();
            IPC_ARM9 = IPC_MSG_NONE;
            break;
        case IPC_MSG_BOOT_SLOT1:
            {
                vu32 var_27ffe08 = *(vu32 *)0x027FFE08;
                vu32 arm9Dest = *(vu32 *)0x027FFE24;
                prepairReset();
                waitMs( 100 ); // wait ARM9 Reset

                // clear IPC memory
                zeroMemory( (void *)0x027ff000, 0x400 );

                *(vu32 *)0x027FFE24 = arm9Dest;        // let arm9 go
                waitMs( 10 );                        // !!!! wait ARM9 code ldr pc, #0x027ffe24 run !!!!
                *(vu32 *)0x027FFE08 = var_27ffe08;    // !!!! this will overwrite ARM9 code ldr pc, #0x027ffe24 !!!!
                swiSoftReset();
            }
            break;
        case IPC_MSG_BOOT_SLOT2:
            prepairReset();
            swiSoftReset();
            break;
        default:
            {}
        };
    }
}
