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
#include "dsrom.h"
#include "dbgtool.h"
#include "folder_banner_bin.h"
#include "nds_banner_bin.h"
#include "nds_save_banner_bin.h"
#include "unknown_nds_banner_bin.h"
#include "unknown_banner_bin.h"
#include "fileicons.h"

DSRomInfo & DSRomInfo::operator =( const DSRomInfo & src )
{
    memcpy( this, &src, sizeof(DSRomInfo) );
    return *this;
}


bool DSRomInfo::loadDSRomInfo( const std::string & filename, bool loadBanner )
{
    FILE * f = fopen( filename.c_str(), "rb" );
    if( NULL == f )// 打开文件失败
    {
        return false;
    }


    tNDSHeader header;
    if( 512 != fread( &header, 1, 512, f ) ) // 读文件头失败
    {
        dbg_printf( "read rom header fail\n" );
        memcpy( &_banner, unknown_nds_banner_bin, sizeof(_banner) );
        fclose( f );
        _isDSRom = false;
        return false;
    }

    ///////// ROM Header /////////
    u16 crc = swiCRC16( 0xFFFF, &header, 0x15E );    if( crc != header.headerCRC16 ) // 文件头 CRC 错误，不是nds游戏
    {
        dbg_printf( "%s rom header crc error\n", filename.c_str() );
        _isDSRom = false;
        memcpy( &_banner, unknown_nds_banner_bin, sizeof(_banner) );
        fclose( f );
        return true;
    } else {
        _isDSRom = true;
        if( header.arm7destination >= 0x037F8000 || 0x23232323 == *(u32 *)header.gameCode ) {//23->'#'
            _isHomebrew = true;
        }

    }

    ///////// saveInfo /////////
    memcpy( _saveInfo.gameTitle, header.gameTitle, 12 );
    memcpy( _saveInfo.gameCode, header.gameCode, 4 );
    _saveInfo.gameCRC = header.headerCRC16;
    _saveInfo.saveType = saveManager().getSaveTypeByInfo( _saveInfo );

    //dbg_printf( "save type %d\n", _saveInfo.saveType );

    ///////// banner /////////
    if( header.bannerOffset != 0 ) {
        fseek( f, header.bannerOffset, SEEK_SET );
        tNDSBanner banner;
        u32 readed = fread( &banner, 1, 0x840, f );
        if( sizeof(tNDSBanner) != readed ) {
            memcpy( &_banner, nds_banner_bin, sizeof(_banner) );
        } else {
            crc = swiCRC16( 0xffff, banner.icon, 0x840 - 32 );

            if( crc != banner.crc ) {
                dbg_printf("banner crc error, %04x/%04x\n", banner.crc, crc );
                memcpy( &_banner, nds_banner_bin, sizeof(_banner) );
            } else {
                memcpy( &_banner, &banner, sizeof(_banner) );
            }
        }
    } else {
        //dbg_printf( "%s has no banner\n", filename );
        memcpy( &_banner, nds_banner_bin, sizeof(_banner) );
    }

    fclose( f );
    return true;
}


void DSRomInfo::drawDSRomIcon( u8 x, u8 y, GRAPHICS_ENGINE engine )
{
	if ( _fileIconIdx >= 0 ) {
		fileicons().drawIcon(_fileIconIdx, x,y, engine);
		return;
	}

    for( int tile=0; tile < 16; ++tile )
    {
        for( int pixel=0; pixel < 32; ++pixel )
        {
            u8 a_byte = _banner.icon[(tile<<5)+pixel];

            //int px = (tile & 3) * 8 + (2 * pixel & 7);
            //int py = (tile / 4) * 8 + (2 * pixel / 8);
            int px = ((tile & 3) << 3) + ((pixel<<1) & 7);
            int py = ((tile >> 2) << 3) + (pixel >> 2);


            u8 idx1 = (a_byte & 0xf0) >> 4;
            if( 0 != idx1 ) {
                gdi().setPenColor( _banner.palette[idx1], engine );
                gdi().drawPixel( px+1+x, py+y, engine );
            }

            u8 idx2 = (a_byte & 0x0f);
            if( 0 != idx2 ) {
                gdi().setPenColor( _banner.palette[idx2], engine );
                gdi().drawPixel( px+x, py+y, engine );
            }
        }
    }
}

void DSRomInfo::drawDSRomIconMem( void * mem )
{
	if ( _fileIconIdx >= 0 ) {
		fileicons().drawIconMem(_fileIconIdx, mem);
		return;
	}

    u16 * pmem = (u16 *)mem;
    for( int tile=0; tile < 16; ++tile )
    {
        for( int pixel=0; pixel < 32; ++pixel )
        {
            u8 a_byte = _banner.icon[(tile<<5)+pixel];

            //int px = (tile & 3) * 8 + (2 * pixel & 7);
            //int py = (tile / 4) * 8 + (2 * pixel / 8);
            int px = ((tile & 3) << 3) + ((pixel<<1) & 7);
            int py = ((tile >> 2) << 3) + (pixel >> 2);


            u8 idx1 = (a_byte & 0xf0) >> 4;
            if( 0 != idx1 ) {
                pmem[py*32+px+1] = _banner.palette[idx1] | BIT(15);
                //gdi().setPenColor( _banner.palette[idx1] );
                //gdi().drawPixel( px+1+x, py+y, engine );
            }

            u8 idx2 = (a_byte & 0x0f);
            if( 0 != idx2 ) {
                pmem[py*32+px] = _banner.palette[idx2] | BIT(15);
                //gdi().setPenColor( _banner.palette[idx2] );
                //gdi().drawPixel( px+x, py+y, engine );
            }
        }
    }
}
