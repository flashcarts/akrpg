#include <cstdio>
#include "rpgprotocol.h"
#include "dbgtool.h"

#define PAGESIZE_0			(0<<24)
#define PAGESIZE_4			(7<<24)
#define PAGESIZE_512		(1<<24)
#define PAGESIZE_1K			(2<<24)
#define PAGESIZE_2K			(3<<24)
#define PAGESIZE_4K			(4<<24)

void ioRpgSendCommand( u32 command[2], u32 pageSize, u32 latency, void * buffer )
{
	CARD_CR1H = CARD_CR1_ENABLE | CARD_CR1_IRQ;

	for( u32 i=0; i<2; ++i )
	{
		CARD_COMMAND[i*4+0] = command[i]>>24;
		CARD_COMMAND[i*4+1] = command[i]>>16;
		CARD_COMMAND[i*4+2] = command[i]>>8;
		CARD_COMMAND[i*4+3] = command[i]>>0;
	}

	pageSize -= pageSize & 3; // align to 4 byte
	u32 pageParam = PAGESIZE_4K;
	u32 transferLength = 4096;
	// make zero read and 4 byte read a little special for timing optimization(and 512 too)
	if( 0 == pageSize ) {
		transferLength = 0;
		pageParam = PAGESIZE_0;
	} else if( 4 == pageSize ) {
		transferLength = 4;
		pageParam = PAGESIZE_4;
	} else if( 512 == pageSize ) {
		transferLength = 512;
		pageParam = PAGESIZE_512;
	}

	// go
	CARD_CR2 = 0;
	CARD_CR2 = KEY_PARAM | CARD_ACTIVATE | CARD_nRESET | pageParam | latency;

	u8 * pbuf = (u8 *)buffer;
	u32 * pbuf32 = (u32 * )buffer;
	bool useBuf = ( NULL != pbuf );
	bool useBuf32 = (useBuf && (0 == (3 & ((unsigned int)buffer))));

	u32 count = 0;
	u32 cardCtrl = CARD_CR2;
	while( (cardCtrl & CARD_BUSY) && count < pageSize) 
	{
		cardCtrl = CARD_CR2;
		if( cardCtrl & CARD_DATA_READY  ) {
			u32 data = CARD_DATA_RD;
			if( useBuf32 && count < pageSize) {
				*pbuf32++ = data;
			}
			else if ( useBuf && count < pageSize ) {
				pbuf[0] = (unsigned char) (data >>  0);
				pbuf[1] = (unsigned char) (data >>  8);
				pbuf[2] = (unsigned char) (data >> 16);
				pbuf[3] = (unsigned char) (data >> 24);
				pbuf += sizeof (unsigned int);
			}
			count += 4;
		}
	}
	
	// if read is not finished, ds will not pull ROM CS to high, we pull it high manually
	if( count != transferLength ) {
		// MUST wait for next data ready, 
		// if ds pull ROM CS to high during 4 byte data transfer, something will mess up
		// so we have to wait next data ready
		do { cardCtrl = CARD_CR2; } while(!(cardCtrl & CARD_DATA_READY));
		// and this tiny delay is necessary
		swiDelay(16);
		// pull ROM CS high
		CARD_CR2 = 0;
		CARD_CR2 = KEY_PARAM | CARD_ACTIVATE | CARD_nRESET | 0 | 0x0000;
	}
	// wait rom cs high
	do { cardCtrl = CARD_CR2; } while( cardCtrl & CARD_BUSY );
}

bool ioRpgWaitCmdBusy( bool forceWait )
{
	bool timeout = true;
	while( timeout && forceWait ) {
		CARD_CR1H = CARD_CR1_ENABLE | CARD_CR1_IRQ;
		for( u32 i=0; i<8; ++i )
			CARD_COMMAND[i] = 0xB8;
		// go
		CARD_CR2 = 0;
		CARD_CR2 = KEY_PARAM | CARD_ACTIVATE | CARD_nRESET | (6<<24) | 0;

		u32 count = 0;
		u32 transferLength = 256 << 6;
		u32 data = 0;
		u32 cardCtrl = CARD_CR2;

		while( cardCtrl & CARD_BUSY ) 
		{
			cardCtrl = CARD_CR2;
			if( cardCtrl & CARD_DATA_READY  ) {
				data = CARD_DATA_RD;
				count += 4;
				if( 0x00000fc2 == data ) {
					timeout = false;
					break;
				}
			}
			swiDelay( 16 ); // 1us
		}

		if( count != transferLength ) {// if read is not finished, ds will not pull ROM CS to high, we pull it high manually
			do { cardCtrl = CARD_CR2; } while(!(cardCtrl & CARD_DATA_READY));
			swiDelay( 16 );
			CARD_CR2 = 0;
			CARD_CR2 = KEY_PARAM | CARD_ACTIVATE | CARD_nRESET | 0 | 0x0000;
		}

		// wait rom cs high
		do { cardCtrl = CARD_CR2; } while( cardCtrl & CARD_BUSY );

		if( timeout ) {
			//dbg_printf("time out! ");
			//dbg_printf("(%08x)\n", data );
			//wait_press_b();
		}
	}
	return !timeout;
}

void ioRpgSetDeviceStatus( u8 deviceSelection, u8 tables, u32 table0BlockSize, u32 table1BlockSize )
{
	u8 byte6 = deviceSelection | (tables<<2);
	u32 cmd[2] = { 0xD7000000, 
					(byte6<<16)|(table1BlockSize<<8)|table0BlockSize };
	ioRpgSendCommand( cmd, 0, 0, NULL );
}

void ioRpgSetMapTableAddress( u32 tableName, u32 tableInRamAddress )
{
	tableName &= 0x0F;
	u32 cmd[2] = { 0xD0000000 | (tableInRamAddress >> 8), 
					((tableInRamAddress & 0xff) << 24) | ((u8)tableName << 16) };

	ioRpgSendCommand( cmd, 0, 0, NULL );
}

void ioRpgReadSram( u32 address, void * buffer, u32 length )
{
	length &= ~(0x03);
	u32 cmd[2] = { 0xB7000000 | (address >> 8), (address & 0xff) << 24 | 0x00120000 };
	ioRpgSendCommand( cmd, length, 2, buffer );
}

static inline void ioRpgWriteSram4( u32 address, u32 data )
{
	// 这个协议比较奇怪，0 表示 1，3表示4，以此类推
	if( address >= 0x0a0000 && address <= 0x0a0200 ) {
		dbg_printf( "ioRpgWriteSram4 try to write %06x", address );
		wait_press_b();
	}
	u32 cmd[2] = { 0xD9000000 | (3 << 22) | (address & 0x001fffff), data };
	ioRpgSendCommand( cmd, 4, 0, NULL );
}

void ioRpgWriteSram( u32 address, const void * data, u32 length )
{
	length &= ~(0x03);
	u32 * pbuffer = (u32 *)data;
	for( u32 i = 0; i < length / 4; ++i )
	{
		ioRpgWriteSram4( address, *(pbuffer+i) );
		address += 4;
	}
}

void ioRpgReadNand( u32 address, void * buffer, u32 length )
{
	u32 cmd[2] = { 0xB7000000 | (address >> 8), ((address & 0xff) << 24) | 0x00110000 };
	ioRpgSendCommand( cmd, length, 0xd0, buffer );
}

void ioRpgReadNandRedundant( u32 address, void * buffer )
{
	u32 cmd[2] = { 0xB7000000 | (address >> 8), ((address & 0xff) << 24)  | 0x00910000 };
	ioRpgSendCommand( cmd, 16, 0xd0, buffer );
}

void ioRpgWriteNand( u32 address, const void * data, u32 length )
{	
	u32 startWrite[2] = { 0xDF000000 | ((length / 8)+1), address };
	ioRpgSendCommand( startWrite, 0, 0, NULL );

	u32 writeNand[2] = { 0x80000000, 0x00000000 };
	ioRpgSendCommand( writeNand, 0, 0, NULL );

	ioRpgPushData( (u64 *)data, length );

	u32 finishWrite[2] = { 0x10000000, 0x00000000 };
	ioRpgSendCommand( finishWrite, 0, 0, NULL );

	ioRpgWaitCmdBusy( true );
}

//void ioRpgWriteNand( u32 address, const void * data, u32 length )
//{
//	if( length > 528 ) length = 528;
//	address &= ~0x1ff;
//	ioRpgWriteSram( SRAM_DISKBUFFER_START, data, length );
//	ioRpgPageCopySramToNand( SRAM_DISKBUFFER_START, address );
//}

void ioRpgPageCopySramToNand( u32 src, u32 dest )
{
	// to protect boot/cfg blocks, avoid write to 0x60000
	if( dest < 0x60000 ) {
		dbg_printf("WARNING: ioRpgPageCopySramToNand() try to write boot block %08x\n", dest );
		wait_press_b();
		return;
	}
	ALIGN(4) u8 buffer[16];
	ioRpgReadNandRedundant( dest, buffer );
	if( swiCRC16( 0x0000, buffer, 16 ) != 0x7040 ) {
		dbg_printf("ERROR: try to write a non-free page %08x\n", dest );
		for( size_t i = 0; i < 16; ++i ) {
			dbg_printf( "%02x", buffer[i] );
		}
		wait_press_b();
		return;
	}
	u32 cmd[2] = { 0xDA000000 | (src & 0x00ffffff), dest };
	ioRpgSendCommand( cmd, 0, 0, NULL );
	ioRpgWaitCmdBusy( true );
}

void ioRpgPageCopyNandToSram( u32 src, u32 dest )
{
	u32 cmd[2] = { 0xDB000000 | (dest & 0x00ffffff), src };
	ioRpgSendCommand( cmd, 0, 0, NULL );
	ioRpgWaitCmdBusy( true );
}

void ioRpgEraseNand( u32 blockAddr )
{
	if( blockAddr < 0x60000 ) {
		dbg_printf("WARNING: ioRpgEraseNand try to erase boot block %08x\n", blockAddr );
		wait_press_b();
		return;
	}
	u32 cmd[2] = { 0xD6030000, blockAddr };
	ioRpgSendCommand( cmd, 0, 0, NULL );
	ioRpgWaitCmdBusy( true );
}

// this is safe for all capacity chips
void ioRpgPageCopyNandToNand( u32 src, u32 dest )
{
	if( dest < 0x60000 ) {
		dbg_printf("WARNING: ioRpgPageCopyNandToNand() try to write boot block %08x\n", dest );
		wait_press_b();
		return;
	}
	
	for( u32 i = 0; i < 4; ++i ) {
			ioRpgPageCopyNandToSram( src + i * 512, SRAM_DISKBUFFER_START );
			ioRpgPageCopySramToNand( SRAM_DISKBUFFER_START, dest + i * 512 );
	}
}

// this is ONLY used on chips which capacity smaller than 4Gbit, use it carefully
// (if this function is used on chips which larger than 4Gbit, two plane address issue must be taken care by caller )
// (this func can NOT work with samsung chips)
void ioRpgPageCopyNandInternal( u32 src, u32 dest )
{ 
	if( dest < 0x60000 ) {
		dbg_printf("WARNING: ioRpgPageCopyNandInternal() try to write boot block %08x\n", dest );
		wait_press_b();
		return;
	}
	u32 cmd[2] = { 0xDE000000 | (src>>11), (dest>>11) };
	ioRpgSendCommand( cmd, 0, 0, NULL );
	ioRpgWaitCmdBusy( true );
}

void ioRpgReadSDBlock( u32 address, void * buffer, u32 blockCount )
{
	//// poke data out
	//u32 cmd[2] = { 0xD4000000 | (address >> 8), ((address & 0xff) << 24) | 0x00130000 };
	//ioRpgSendCommand( cmd, 0, 0, NULL );

	////// wait
	//u32 retry = 1024;
	//bool timeout = true;
	//while( retry-- && timeout)
	//	timeout = !ioRpgWaitCmdBusy( false );

	//cmd[0] = 0xB7000000 | (address >> 8);
	//ioRpgSendCommand( cmd, 512, 10, buffer );

	// direct read
	u32 cmd[2] = { 0xB7000000 | (address >> 8), ((address & 0xff) << 24) | 0x00130000 };
	ioRpgSendCommand( cmd, 512, 0x450, buffer );
}

void ioRpgWriteSDBlock( u32 address, const void * data )
{
	ioRpgWriteSram( SRAM_DISKBUFFER_START, data, 512 );
	ioRpgPageCopySramToSD( SRAM_DISKBUFFER_START, address );
}


void ioRpgPageCopySramToSD( u32 src, u32 dest )
{
	u32 cmd[2] = { 0xDC000000 | (src & 0x00ffffff), dest };
	ioRpgSendCommand( cmd, 0, 0, NULL );
	ioRpgWaitCmdBusy( true );
}

void ioRpgPageCopySDToSram( u32 src, u32 dest )
{
	u32 cmd[2] = { 0xDD000000 | (dest & 0x00ffffff), src };
	ioRpgSendCommand( cmd, 0, 0, NULL );
	ioRpgWaitCmdBusy( true );
}

void ioRpgAsignSaveBlocks( u32 tableAddress, u32 nandAddress[64] )
{
	ioRpgSetMapTableAddress( MTN_SAVE_TABLE, tableAddress );
	for( size_t i = 0; i < 64; ++i ) {
		if( nandAddress[i] < 0x60000 ) {
			dbg_printf("WARNING: ioRpgAsignSaveBlocks() try to ASSIGN boot block %08x\n", nandAddress[i] );
			wait_press_b();
			return;
		}
	}
	ioRpgWriteSram( tableAddress, nandAddress, 64 * 4 );
}
