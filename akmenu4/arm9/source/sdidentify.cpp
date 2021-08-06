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









#include <iorpg.h>
#include <fat.h>
#include "timer.h"
#include "sdidentify.h"
#include "irqs.h"
#include "inifile.h"
#include "stringtool.h"
#include "systemfilenames.h"
#include "language.h"
#include "progresswnd.h"

std::string sdidGetSDName()
{
    u8 sdinfo[8] = {};
    sddGetSDInfo( sdinfo );
    for( u8 i = 0; i < 5; ++i ) {
        if( ' ' ==sdinfo[i+1] )
            sdinfo[i+1] = '_';
    }
    return (const char *)(sdinfo + 1);

}

std::string sdidGetSDManufacturerID()
{
    u8 sdinfo[8] = {};
    sddGetSDInfo( sdinfo );
    return formatString( "%02x", sdinfo[0] );
}

std::string sdidGetSDManufacturerName()
{
    CIniFile ini( SFN_SDCARD_LIST );
    u8 sdinfo[8] = {};
    sddGetSDInfo( sdinfo );
    std::string manufacturerID = formatString( "%02x", sdinfo[0] );
    std::string manufacturerName = ini.GetString( "SD Card Manufacturers", manufacturerID, manufacturerID );
    return manufacturerName;
}

u32 sdidCheckSDSpeed( u32 readCount )
{
    CIniFile ini( SFN_SDCARD_LIST );

    std::string name = sdidGetSDName();
    std::string manufacturerID = sdidGetSDManufacturerID();

    u32 ret = ini.GetInt( "SD Card Speed", manufacturerID + name, 0 );

    if( ret != 0 )
        return ret;

    PARTITION * sdPartition = _FAT_partition_getPartitionFromPath( "fat1:/" );
    if( NULL == sdPartition ) {
        dbg_printf("no sd card\n");
        return 0;
    }

    u32 totalSectors = sdPartition->numberOfSectors;
    dbg_printf("total sectors %d, max addr %08x\n", totalSectors, (totalSectors << 9) - 0x200);

    // some sd card needs to be readed one time for initialization
    sddReadBlocks( 0, 1, NULL );

    irq().vblankStop();
    double maxAccessTime = 0.f;

    std::string tipText = LANG("progress window", "first use sd");
    progressWnd().setTipText( tipText );
    progressWnd().show();
    progressWnd().setPercent( 0 );

    vu64 tick1 = 0;
    vu64 tick2 = 0;
    for( size_t i = 0; i < readCount; ++i ) {
        u32 randAddr = ((rand() % totalSectors)<<(isSDHC()?0:9) ) & (~(0x200-1));

        u32 sdReadSingleBlock[2] = { 0xD5030011, randAddr }; // input read address here
        ioRpgSendCommand( sdReadSingleBlock, 0, 80, NULL );
        timer().initTimer();
        tick1 = timer().getTick();
        ioRpgWaitCmdBusy( true );
        tick2 = timer().getTick();

        u32 readSD[2] = { 0xB7000000, 0x00000000 | 0x00130000 }; // address dont care here
        ioRpgSendCommand( readSD, 512, 4, NULL );

        if( tick2 < tick1 ) {
            tick2 += 65536;
        }
        tick1 = tick2 - tick1;
        double timeCostUs = timer().tickToUs( tick1 );
        if( timeCostUs > maxAccessTime ) {
            maxAccessTime = timeCostUs;
        }

        u32 percent = i * 100 / readCount;
        if( (i & 0x1ff) == 0x1ff ) {
            dbg_printf( "%02d ", percent );
            progressWnd().setPercent( percent );
        }
    }
    progressWnd().hide();
    irq().vblankStart();


    maxAccessTime = (((u32)(maxAccessTime / 100)) + 1) * 100;

    ret = ((u32)(maxAccessTime * 1000 / 150));

    dbg_printf("max access time: max %4.2fus\n", maxAccessTime );
    if( ret > 0x1f00 ) {
        ret = 0x2000;
        dbg_printf("max %4.2fus ret=%08x\n", maxAccessTime, ret );
    }

    ini.SetInt( "SD Card Speed", manufacturerID + name, ret );
    ini.SaveIniFile( SFN_SDCARD_LIST );

    return ret;
}

