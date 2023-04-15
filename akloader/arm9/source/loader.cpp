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









#include <malloc.h>
#include <fat.h>
#include "loader.h"
#include "../../share/ipctool.h"
#include <iorpg.h>


static void loadHomebrewArm7Code( u8 * temp_arm7_code_mem )
{
    sysSetBusOwners( BUS_OWNER_ARM7, BUS_OWNER_ARM7 );

    IPC_ARM7 = IPC_MSG_NONE;
    //IPC_ARM9_P1 = g_sys_info.get_sd_speed();
    IPC_ARM9_P2 = (u32)temp_arm7_code_mem;
    IPC_ARM9 = IPC_MSG_LOAD_HOMEBREW;

    while( IPC_ARM7 != IPC_MSG_ARM7_LOAD_DONE ) {
        swiDelay(0);
    }
    dbg_printf("arm7 load homebrew done\n");

    sysSetBusOwners( BUS_OWNER_ARM9, BUS_OWNER_ARM9 );
}

bool loadFile( const char * filename, u32 & originSpeed, u32 & changedSpeed, bool & hasRSA )
{
    FILE * f = fopen( filename, "rb" );
    if( NULL == f )
        return false;

    // load_header
    fseek( f, 0, SEEK_SET );
    // save card control info
    changedSpeed = NDSHeader.cardControl13;
    fread( (u8 *)&NDSHeader, 1, 512, f );
    originSpeed = NDSHeader.cardControl13;

    // load_arm9
    u8 * arm9_dest_mem = (u8 *)NDSHeader.arm9destination;
    u32 arm9_code = NDSHeader.arm9romSource;
    dbg_printf( "arm9 code offset %08x\n", arm9_code );
    u32 arm9_size = NDSHeader.arm9binarySize;

    fseek( f, arm9_code, SEEK_SET );

    u32 readed = fread( arm9_dest_mem, 1, arm9_size, f );

    dbg_printf( "arm9 code offset %08x\n", arm9_code );
    dbg_printf( "arm9 start memory %08x\n", (u32)arm9_dest_mem );
    dbg_printf( "arm9 entry code:\n");
    for( size_t i = 0; i<4;++i) {
        dbg_printf("%08x", *(vu32 *)(NDSHeader.arm9executeAddress+i*4) );
    }

    // load_arm7
    if( NDSHeader.arm7destination >= 0x037F8000 || 0x23232323 == *(u32 *)NDSHeader.gameCode ) //23->'#'
    {
        u32 arm7_code = NDSHeader.arm7romSource;
        u32 arm7_size = NDSHeader.arm7binarySize;
        u8 * arm7_temp_code_mem = (u8 *)malloc(arm7_size);
        fseek( f, arm7_code, SEEK_SET );
        u32 loaded_arm7_size = fread( arm7_temp_code_mem, 1, arm7_size, f );
		DC_FlushRange(arm7_temp_code_mem,arm7_size); // by gelu
        loadHomebrewArm7Code( arm7_temp_code_mem );
        free(arm7_temp_code_mem);
        dbg_printf( "arm7 temp code memory %08x\n", (u32)arm7_temp_code_mem );
        dbg_printf( "arm7 entry code %08x\n", *(u32 *)arm7_temp_code_mem );
        dbg_printf( "arm7 size: %08x/%08x\n", arm7_size, loaded_arm7_size );
    }
    else
    {
        u8 * arm7_dest_mem = (u8 *)NDSHeader.arm7destination;
        u32 arm7_code = NDSHeader.arm7romSource;
        u32 arm7_size = NDSHeader.arm7binarySize;
        fseek( f, arm7_code, SEEK_SET );
        u32 loaded_arm7_size = fread( arm7_dest_mem, 1, arm7_size, f );
        dbg_printf( "arm7 start memory %08x\n", (u32)arm7_dest_mem );
        dbg_printf( "arm7 size: %08x/%08x\n", arm7_size, loaded_arm7_size );
    }


    fseek( f, NDSHeader.romSize, SEEK_SET );
    u32 rsaSign = 0;
    fread( &rsaSign, 1, 4, f );
    if( 0x00016361 == rsaSign ) { // if this program has a rsa signature, try to do not change header
        hasRSA = true;
    }
    fclose( f );

    // overwrite card control info only if setting speed slower than origin speed
    // keeping origin header can get better compatibility and make most download play program to run
    if( (changedSpeed & 0x1fff) > (NDSHeader.cardControl13 & 0x1fff) ) {
        NDSHeader.cardControl13 = changedSpeed;
        *(u32 *)0x027fff60 = changedSpeed;
        *(u32 *)0x027fff64 = originSpeed;
    }

	if ( *(u32 *)NDSHeader.gameCode != 0x4b384759 ) // to correct language in 2215
    NDSHeader.reserved1[8] = 0; // change this for run iQue game on U/E/J machines
    //NDSHeader.headerCRC16 = swiCRC16( 0xFFFF, &NDSHeader, 0x15E );

    cwl();
    wait_press_b();

    return true;
}
