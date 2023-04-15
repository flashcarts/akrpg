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









#ifndef _RPGPROTOCOL_H_
#define _RPGPROTOCOL_H_

#include <nds.h>
#ifdef __cplusplus
extern "C" {
#endif

enum MAP_TABLE_NAME
{
    MTN_MAP_A            = 0,
    MTN_MAP_B            = 1,
    MTN_NAND_OFFSET1    = 2,
    MTN_SAVE_TABLE        = 3
};

enum DEVICE_NAME
{
    DN_DEFAULT    = 0,
    DN_NAND        = 1,
    DN_SRAM        = 2,
    DN_SD        = 3
};

enum SRAM_ALLOC {
    SRAM_NANDLUT_START = 0x000000,             // nand LBA lookup table
    SRAM_NANDLUT_END = 0x010000 - 1,
    SRAM_SAVETABLE_START = 0x010000,           // saveblock map table
    SRAM_SAVETABLE_END = 0x020000 - 1,
    SRAM_FAT_START = 0x020000,                 // fat clusters map table
    SRAM_FAT_END = 0x0A0000 - 1,
    SRAM_LOADING_FILENAME_START = 0x0A0000,    // filename to load by akloader
    SRAM_LOADING_FILENAME_END = 0x0FE800 - 1,
    SRAM_EEP_BUFFER_START = 0x0FE800,          // a 2K buffer for eeprom simulation
    SRAM_EEP_BUFFER_END = 0x0FF000 - 1,
    SRAM_DISKBUFFER_START = 0x0FF000,          // reserved
    SRAM_DISKBUFFER_END = 0x100000 - 1,
    SRAM_SAVEDATA_START = 0x100000,            // save data
    SRAM_SAVEDATA_END = 0x200000 - 1
};
#define KEY_PARAM           0x00406000
//#define KEY_PARAM           0x00000000

void ioRpgDelay( u32 us ) ;

void ioRpgSendCommand( u32 command[2], u32 pageSize, u32 latency, void * buffer );

bool ioRpgWaitCmdBusy( bool forceWait );

void ioRpgSetDeviceStatus( u8 deviceSelection, u8 tables, u32 table0BlockSize, u32 table1BlockSize, bool table1RemainderLSR );

void ioRpgSetMapTableAddress( u32 tableName, u32 tableInRamAddress );

void ioRpgReadSram( u32 address, void * buffer, u32 length );

void ioRpgWriteSram( u32 address, const void * data, u32 length );

void ioRpgReadNand( u32 address, void * buffer, u32 length );

void ioRpgReadNandRedundant( u32 address, void * buffer );

void ioRpgWriteNand( u32 address, const void * data, u32 length );

void ioRpgEraseNand( u32 blockAddr );

void ioRpgPageCopyNandToNand( u32 src, u32 dest );

void ioRpgPageCopyNandInternal( u32 src, u32 dest );

void ioRpgReadSDBlock( u32 address, void * buffer, u32 blockCount );

void ioRpgWriteSDBlock( u32 address, const void * data );

void ioRpgAsignSaveBlocks( u32 tableAddress, u32 nandAddress[64] );

static inline void ioRpgMemCopy( void * dest, const void * src, u32 length )
{
    if( 0 == ((u32)dest & 0x03) && 0 == ((u32)src & 0x03) ) {
        u32 * pSrc32 = (u32 *)src;
        u32 * pDest32 = (u32 *)dest;
        length >>= 2;

        while( length-- > 0 ) {
            *pDest32++ = *pSrc32++;
        }
    } else {
        u8 * pSrc8 = (u8 *)src;
        u8 * pDest8 = (u8 *)dest;
        while( length-- > 0 ) {
            *pDest8++ = *pSrc8++;
        }
    }
}

#define CARD_COMMAND32   ((vuint32*)0x040001A8)
static inline void ioRpgPushData( const void * data, u16 length )
{
    if( 0 == ((u32)data & 0x03) ) {
        u32 * pData32 = (u32 *)data;
        while( length ) {
            CARD_COMMAND32[0] = *pData32++;
            CARD_COMMAND32[1] = *pData32++;
            length -= 8;
            CARD_CR2 = KEY_PARAM | CARD_ACTIVATE | CARD_nRESET;
            while( CARD_CR2 & CARD_BUSY ) {}
        }
    } else {
        u8 * pData8 = (u8 *)data;
        while( length ) {
            CARD_COMMAND[0] = *pData8++;
            CARD_COMMAND[1] = *pData8++;
            CARD_COMMAND[2] = *pData8++;
            CARD_COMMAND[3] = *pData8++;
            CARD_COMMAND[4] = *pData8++;
            CARD_COMMAND[5] = *pData8++;
            CARD_COMMAND[6] = *pData8++;
            CARD_COMMAND[7] = *pData8++;
            length -= 8;
            CARD_CR2 = KEY_PARAM | CARD_ACTIVATE | CARD_nRESET;
            while( CARD_CR2 & CARD_BUSY ) {}
        }
    }
}

#ifdef __cplusplus
}
#endif

#endif//_RPGPROTOCOL_H_
