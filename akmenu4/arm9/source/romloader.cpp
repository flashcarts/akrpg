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









#include <fat.h>
#include <iorpg.h>
#include "romloader.h"
#include "dbgtool.h"
#include "akloader_arm7_bin.h"
#include "akloader_arm9_bin.h"
#include "savechip.h"
#include "savemngr.h"
#include "rpgprotocol.h"
#include "../../share/ipctool.h"
#include "../../share/timetool.h"
#include "globalsettings.h"



static void resetAndLoop()
{
    // Interrupt
    REG_IME = 0;
    REG_IE = 0;
    REG_IF = ~0;

    IPC_ARM9 = IPC_MSG_ARM7_REBOOT;

    while( IPC_ARM7 != IPC_MSG_ARM7_READY_BOOT ) {
        swiDelay(0);
    }
    // make arm9 loop code
    *((vu32*)0x027FFE08) = (u32)0xE59FF014;  // ldr pc, 0x027FFE24
    *((vu32*)0x027FFE24) = (u32)0x027FFE08;  // Set ARM9 Loop address

    DC_FlushAll();
    DC_InvalidateAll();
    swiSoftReset();
}

bool loadRom( const std::string & filename )
{
    // copy filename to sram
    ALIGN(4) u8 filenameBuffer[512];
    memset( filenameBuffer, 0, 512 );
    memcpy( filenameBuffer, filename.c_str(), filename.length() );
    filenameBuffer[510] = 0;
    filenameBuffer[511] = 0;
    filenameBuffer[511] |= gs().downloadPlayPatch ? BIT(0) : 0;
    filenameBuffer[511] |= gs().cheatingSystem ? BIT(1) : 0;
    filenameBuffer[511] |= gs().resetInGame ? BIT(2) : 0;

    ioRpgWriteSram( SRAM_LOADING_FILENAME_START, filenameBuffer, 512 );

    dbg_printf( "load %s\n", filename.c_str() );

    // copy loader's arm7 code
    memcpy( (void *)0x023FC000, akloader_arm7_bin, akloader_arm7_bin_size );
    NDSHeader.arm7executeAddress = 0x023FC000;

    // copy loader's arm9 code
    memcpy( (void *)0x023b0000, akloader_arm9_bin, akloader_arm9_bin_size );
    NDSHeader.arm9executeAddress = 0x023b0000;

    dbg_printf( "load done\n" );

    fatUnmount( (PARTITION_INTERFACE)0 );

    resetAndLoop();
    return true;
}
