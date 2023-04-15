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









//#define NDS_BOOT
//#define DEBUG
#include <string.h>
#include <nds.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fat.h>
#include <iorpg.h>
#include <ctype.h>

#include "memcleaner.h"
#include "loader.h"
#include "msgdisplay.h"
#include "special_effect.h"
#include "rpgprotocol.h"
#include "rpgmaps.h"
#include "dlditool.h"
#include "patches.h"

#include "../../share/ipctool.h"
#include "../../share/memtool.h"
//#include "../../share/timetool.h"


static void resetAndLoop()
{
    IPC_ARM9 = IPC_MSG_BOOT_SLOT1;

    while( IPC_ARM7 != IPC_MSG_ARM7_READY_BOOT ) {
        swiDelay(0);
    }
    // make arm9 loop code
    *((vu32*)0x027FFE08) = (u32)0xE59FF014;  // ldr pc, 0x027FFE24
    *((vu32*)0x027FFE24) = (u32)0x027FFE08;  // Set ARM9 Loop address

    IC_InvalidateAll();
    DC_FlushAll();
    DC_InvalidateAll();
    swiSoftReset();
}

static void resetToSlot2()
{
    // make arm9 loop code
    *((vu32*)0x027FFE08) = (u32)0xE59FF014;  // ldr pc, 0x027FFE24
    *((vu32*)0x027FFE24) = (u32)0x027FFE08;  // Set ARM9 Loop address
    *((vu32*)0x027FFE34) = (u32)0x080000c0;

    sysSetCartOwner(BUS_OWNER_ARM7);  // ARM7 has access to GBA cart

    IPC_ARM9 = IPC_MSG_BOOT_SLOT2;

    while( IPC_ARM7 != IPC_MSG_ARM7_READY_BOOT ) {
        swiDelay(0);
    }

    DC_FlushAll();
    DC_InvalidateAll();
    swiSoftReset();
}

int main(void)
{
    irqInit();
    irqEnable( IRQ_VBLANK );

    sysSetBusOwners( BUS_OWNER_ARM9, BUS_OWNER_ARM9 );

    powerON( POWER_ALL_2D );
    lcdSwap();

    videoSetModeSub(MODE_0_2D | DISPLAY_BG0_ACTIVE );    //sub bg 2 will be used to print text
    vramSetBankC( VRAM_C_SUB_BG_0x06200000 );    // 128k
    vramSetBankD( VRAM_D_MAIN_BG_0x06040000 );    // allocate this bank(128K vram) for DLDI patching buffer

    // init console
    BG_PALETTE_SUB[255] = RGB15(31,31,31);    //by default font will be rendered with color 255
    SUB_BG0_CR = BG_TILE_BASE(0) | BG_MAP_BASE(8) | BG_PRIORITY_0;
    consoleInitDefault((u16*)SCREEN_BASE_BLOCK_SUB(8), (u16*)CHAR_BASE_BLOCK_SUB(0), 16);

    initMsg();
    cleanMsg();
    setMsg( MSGID_LOADING, (192 - 24) / 2 );
    showMsg();

#ifdef NDS_BOOT
    bool succ = fatInitRPG();
#else
    bool succ = fatInitRPG();
#endif

    //bool succ = fatInitDefault();
    dbg_printf( "init fat %s\n", succ ? "succ" : "fail" );
    if( !succ ) {
        showMsg( MSGID_FAT_ERROR, (192 - 24) / 2 );
        return 1;
    }

#ifdef NDS_BOOT
    zeroMemory( (void *)0x2000000, 0x204000 );    // boot nds
#else
    zeroMemory( (void *)0x2000000, 0x3b0000 );  // normal loader
    DC_FlushAll();
#endif//NDS_BOOT

#ifdef NDS_BOOT
    const char * filename = "fat0:/akmenu4.nds";  // boot nds
#else
    ALIGN(4) char filename[512];          // normal loader
    memset( filename, 0, 512 );
    ioRpgReadSram( SRAM_LOADING_FILENAME_START, filename, 512 );
#endif//NDS_BOOT

    strlwr( (char *)filename );
    dbg_printf("%s\n", filename );

    if( 0 == strncmp( "slot2:/", filename, 7 ) ){
        resetARM9Memory();
        resetToSlot2();
    }

    u32 originSpeed = 0;
    u32 changedSpeed = 0;
    bool hasRSA = false;
    bool loadSucc = loadFile( (char *)filename, originSpeed, changedSpeed, hasRSA );
    if( loadSucc ) {
        dbg_printf("%08x\n", NDSHeader.cardControl13 );
        if( (NDSHeader.arm7destination >= 0x037F8000 || 0x23232323 == *(u32 *)NDSHeader.gameCode) ) { // 0x23232323 = "####", homebrew programs
#ifndef NDS_BOOT
            if( NULL == strstr((char *)filename,"/akmenu4.nds") ) {
                // DLDI patch
                static const char * sdPatch = "fat0:/__rpg/rpg_sd.dldi";
                static const char * nandPatch = "fat0:/__rpg/rpg_nand.dldi";
                const char * patch = NULL;
                if( 0 == memcmp( filename, "fat0:/", 6 ) || 0 == memcmp( filename, "FAT0:/", 6 ) ) {
                    patch = nandPatch;
                } else if( 0 == memcmp( filename, "fat1:/", 6 ) || 0 == memcmp( filename, "FAT1:/", 6 ) ) {
                    patch = sdPatch;
                }
                if( NULL != patch ) {
                    dldiPatch( patch, (data_t *)NDSHeader.arm9destination, NDSHeader.arm9binarySize );
                }
            }
#endif
        } else {// official programs
            u8 patchParam = filename[511];
            if( patchParam & BIT(0) ) {
                if( !(hasRSA && NDSHeader.cardControl13 != originSpeed) ) {
                    patchParam &= ~(BIT(0));
                }
            }

            patchRom( filename, 0x2000010, 0x23fe000, patchParam );

            // this must be put in the end of all disk operations
            // build some reading mappings
            rpgBuildMaps( (char *)filename );
        }

        dbg_printf("%08x\n", NDSHeader.cardControl13 );
        dbg_printf("%d %d %d\n", IPC->time.rtc.hours, IPC->time.rtc.minutes, IPC->time.rtc.seconds );
        //fatUnmount( (PARTITION_INTERFACE)0 );

        resetARM9Memory();
        resetAndLoop();

    }
    else {
        dbg_printf("load (%s)fail\n", filename );
#ifdef NDS_BOOT
        showMsg( MSGID_SYS_FILE_MISSING, (192 - 24) / 2 ); //boot nds
#endif//NDS_BOOT
        return 1;
    }

    return 0;
}
