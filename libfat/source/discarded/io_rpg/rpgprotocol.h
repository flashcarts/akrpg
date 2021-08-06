#ifndef _RPGPROTOCOL_H_
#define _RPGPROTOCOL_H_

#include <nds.h>
#ifdef __cplusplus
extern "C" {
#endif

enum MAP_TABLE_NAME
{
	MTN_MAP_A			= 0,
	MTN_MAP_B			= 1,
	MTN_NAND_OFFSET1	= 2,
	MTN_SAVE_TABLE		= 3
};

enum DEVICE_NAME
{
	DN_DEFAULT	= 0,
	DN_NAND		= 1,
	DN_SRAM		= 2,
	DN_SD		= 3
};

enum SRAM_ALLOC {
	SRAM_NANDLUT_START = 0x000000,
	SRAM_SAVETABLE_START = 0x010000,
	SRAM_FAT_START = 0x020000,
	SRAM_DISKBUFFER_START = 0x0FF000,
	SRAM_SAVEDATA_START = 0x100000,
};

#define KEY_PARAM           0x00406000
//#define KEY_PARAM           0x00000000


void ioRpgSendCommand( u32 command[2], u32 pageSize, u32 latency, void * buffer );

bool ioRpgWaitCmdBusy( bool forceWait );

void ioRpgSetDeviceStatus( u8 deviceSelection, u8 tables, u32 table0BlockSize, u32 table1BlockSize );

void ioRpgSetMapTableAddress( u32 tableName, u32 tableInRamAddress );

void ioRpgReadSram( u32 address, void * buffer, u32 length );

void ioRpgWriteSram( u32 address, const void * data, u32 length );

void ioRpgReadNand( u32 address, void * buffer, u32 length );

void ioRpgReadNandRedundant( u32 address, void * buffer );

void ioRpgWriteNand( u32 address, const void * data, u32 length );

void ioRpgPageCopySramToNand( u32 src, u32 dest );

void ioRpgPageCopyNandToSram( u32 src, u32 dest );

void ioRpgEraseNand( u32 blockAddr );

void ioRpgPageCopyNandToNand( u32 src, u32 dest );

void ioRpgPageCopyNandInternal( u32 src, u32 dest );

void ioRpgReadSDBlock( u32 address, void * buffer, u32 blockCount );

void ioRpgWriteSDBlock( u32 address, const void * data );

void ioRpgPageCopySramToSD( u32 src, u32 dest );

void ioRpgPageCopySDToSram( u32 src, u32 dest );

void ioRpgAsignSaveBlocks( u32 tableAddress, u32 nandAddress[64] );

static inline void ioRpgMemCopy64( void * dest, const void * src, u32 length ) {
	u64 * pSrc = (u64 *)src;
	u64 * pDest = (u64 *)dest;
	length >>= 3;

	while( length-- > 0 ) {
		*pDest++ = *pSrc++;
	}
}

#define CARD_COMMAND64   ((vuint64*)0x040001A8)
static inline void ioRpgPushData( const u64 * data, u16 length )
{
	while( length ) {
		*CARD_COMMAND64 = *data++;
		length -= 8;
		CARD_CR2 = KEY_PARAM | CARD_ACTIVATE | CARD_nRESET;
		while( CARD_CR2 & CARD_BUSY ) {}
	}
}

#ifdef __cplusplus
}
#endif

#endif//_RPGPROTOCOL_H_
