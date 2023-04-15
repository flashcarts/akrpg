#include <nds.h>
#include "sddriver.h"
#include "rpgprotocol.h"
#include "dbgtool.h"


static u16 SDCardAddr = 0;
static bool _isSD20 = false;
static bool _isSDHC = false;

static void composeResponse1( void * data, void * response ) {
	const u8 * pdata = (u8 *)data;
	u8 * pResponse = (u8 *)response;
	*(u32 *)pResponse = 0;

	u32 bitCount = 39;
	for( int i = 0; i < 4; ++i ) {
		for( int j = 0; j < 8; ++j ) {
			u8 bit = (pdata[bitCount--] & 0x80) ? 1 : 0;
			//dbg_printf( "%s", bit ? "1" : "0" );
			pResponse[i] |= bit << j;
		}
	}
}

static void composeResponse2( void * data, void * response ) {
	const u8 * pdata = (u8 *)data;
	u8 * pResponse = (u8 *)response;
	*(u64 *)response = *(((u64 *)response) + 1) = 0;

	u32 bitCount = 135;
	for( int i = 0; i < 16; ++i ) {
		for( int j = 0; j < 8; ++j ) {
			u8 bit = (pdata[bitCount--] & 0x80) ? 1 : 0;
			pResponse[i] |= bit << j;
		}
	}
}

static void sendSDCommandR0( u32 * cmd )
{
	ioRpgSendCommand( cmd, 0, 80, NULL );
}

static void sendSDCommandR1( u32 * cmd, u8 * response )
{
	ALIGN(4) u8 tempResponse[64];tempResponse[0] = 0;
	ioRpgSendCommand( cmd, 48, 40, tempResponse + 1 );

	composeResponse1( tempResponse, response );
}

static void sendSDCommandR2( u32 * cmd, u8 * response )
{
	ALIGN(4) u8 tempResponse[160];tempResponse[0] = 0;
	ioRpgSendCommand( cmd, 136, 40, tempResponse + 1 );
	composeResponse2( tempResponse, response );
}

static void waitSDState( u8 state )
{
	u32 ready = 0;
	u32 cmd[2] = { 0xC0000000, 0x00000000 };
	do {
		ioRpgSendCommand( cmd, 4, 0, &ready );
		ready &= 0x0f;
	} while( ready != state );

}

//===============================================================//

bool sddInitSD()
{
	// reset sd card
	{
		u32 cmd[2] = { 0xD5000000, 0x00000000 };
		sendSDCommandR0( cmd );
	}

	// CMD8 for SDHC initialization
	{
		ALIGN(4) u8 response[4];
		u32 cmd[2] = { 0xD5010008, 0x000001AA };
		sendSDCommandR1( cmd, response );
		_isSD20 = (*(u32 *)response == cmd[1]);
		dbg_printf("cmd %d response %08x\n", 0x08, *(u32 *)response );
	}


	u32 retry = 0;

	while( retry < 10000 ) // about 1 second
	{
		// ACMD41
		{
			ALIGN(4) u8 response[4];
			u32 cmd[2] = { 0xD5010037, 0x00000000 };
			sendSDCommandR1( cmd, response );
			//dbg_printf("cmd %d response %08x\n", 0x37, *(u32 *)response );
		}

		{
			ALIGN(4) u8 response[4];
			u32 cmd[2] = { 0xD5010029, 0x00ff8000 };
			if( _isSD20 )
				cmd[1] |= 0x40000000;
			sendSDCommandR1( cmd, response );
			//dbg_printf("cmd %d response %08x\n", 0x29, *(u32 *)response );
			if( !(response[3] & 0x80) ) {
				ioRpgDelay( 1666 );
				retry++;
			} else {
				_isSDHC = ( *(u32 *)response & 0x40000000 );
				break;
			}
		}
	}
	dbg_printf("retry %d times\n", retry );

	// CMD2
	{
		ALIGN(4) u8 response[16];
		u32 cmd[2] = { 0xD5010002, 0x00000000 };
		sendSDCommandR2( cmd, response );
		dbg_printf("cmd 02 response %08x\n", *(u32 *)response );
	}

	// CMD3
	{
		ALIGN(4) u8 response[4];
		u32 cmd[2] = { 0xD5010003, 0x00000000 };
		sendSDCommandR1( cmd, response );
		dbg_printf("cmd 03 response %08x\n", *(u32 *)response );
		SDCardAddr = *(u32 *)response >> 16;
	}

	// CMD7
	{
		ALIGN(4) u8 response[4];
		u32 cmd[2] = { 0xD5010007, 0x00000000 | (SDCardAddr << 16) };
		sendSDCommandR1( cmd, response );
		dbg_printf("cmd 07 response %08x\n", *(u32 *)response );
	}

	// ACMD6
	{
		ALIGN(4) u8 response[4];
		u32 cmd[2] = { 0xD5010037, 0x00000000 | (SDCardAddr << 16) };
		sendSDCommandR1( cmd, response );
		dbg_printf("cmd %d response %08x\n", 0x37, *(u32 *)response );
	}

	{
		ALIGN(4) u8 response[4];
		u32 cmd[2] = { 0xD5010006, 0x00000002 };
		sendSDCommandR1( cmd, response );
		dbg_printf("cmd 06 response %08x\n", *(u32 *)response );
	}

	// CMD13
	{
		ALIGN(4) u8 response[4];
		u32 cmd[2] = { 0xD501000D, 0x00000000 | (SDCardAddr << 16) };
		sendSDCommandR1( cmd, response );
		u8 state = (response[1] >> 1);  // response bit 9 - 12
		if( 4 == state ) {
			dbg_printf("sd init ok\n");
			//wait_press_b();
			if( _isSDHC ) {
				u32 isSDHCcmd[2] = { 0xC1010000, 0x00000000 };
				ioRpgSendCommand( isSDHCcmd, 0, 0, NULL );
			}
			return true;
		}
		else {
			dbg_printf("sd init fail\n" );
			//wait_press_b();
			return false;
		}

		dbg_printf(" the last response is\n%08x\n", *(u32 *)response );
	}
}

void sddReadBlocks( u32 addr, u32 blockCount, void * buffer )
{
	u8 * pBuffer = (u8 *)buffer;

	u32 address = _isSDHC ? addr : (addr << 9);
	if( 1 == blockCount ) {
		u32 sdReadSingleBlock[2] = { 0xD5030011, address }; // input read address here
		sendSDCommandR0( sdReadSingleBlock );
		ioRpgWaitCmdBusy( true );

		u32 readSD[2] = { 0xB7000000, 0x00000000 | 0x00130000 }; // address dont care here
		ioRpgSendCommand( readSD, 512, 4, pBuffer );
		ioRpgDelay( 16 * 5 );                                // wait for sd crc auto complete
	} else {
		ALIGN(4) u8 response[4];
		u32 sdReadMultiplyBlock[2] = { 0xD5040012, address };      // input read address here
		sendSDCommandR0( sdReadMultiplyBlock );
		ioRpgWaitCmdBusy( true );

		u32 readSD[2] = { 0xB7000000, 0x00000000 | 0x00130000 };    // address dont care here
		while( blockCount-- ) {
			ioRpgSendCommand( readSD, 512, 4, pBuffer );
			waitSDState( 0x07 );
			pBuffer += 512;
		}

		u32 sdStopTransmission[2] = { 0xD501000C, 0x00000000 };
		sendSDCommandR1( sdStopTransmission, response );
		//dbg_printf("cmd %d response %08x\n", 0x0C, *(u32 *)response );
	}
}

void sddWriteBlocks( u32 addr, u32 blockCount, const void * buffer )
{
	const u8 * pBuffer = (u8 *)buffer;

	u32 address = _isSDHC ? addr : (addr << 9);
	if( 1 == blockCount ) {
		u32 sdWriteBlock[2] = { 0xD5050018, address };
		sendSDCommandR0( sdWriteBlock );
		ioRpgPushData( pBuffer, 512 );
		waitSDState( 0x00 );
		pBuffer += 512;
	} else {
		u32 sdWriteBlocks[2] = { 0xD5060019, address };

		while( blockCount-- ) {
			sendSDCommandR0( sdWriteBlocks );
			ioRpgPushData( pBuffer, 512 );
			waitSDState( 0x0e );
			pBuffer += 512;
		}

		ALIGN(4) u8 response[4];
		u32 sdStopTransmission[2] = { 0xD501000C, 0x00000000 };
		sendSDCommandR1( sdStopTransmission, response );
		//dbg_printf("cmd %d response %08x\n", 0x0C, *(u32 *)response );
		waitSDState( 0x00 );
	}
}

void sddGetSDInfo( u8 info[8] )
{
	// use cmd7 to put sd card in standby mode
	// CMD7
	{
		ALIGN(4) u8 response[4];
		u32 cmd[2] = { 0xD5010007, 0x00000000 };
		sendSDCommandR1( cmd, response );
		dbg_printf("cmd 07 response %08x\n", *(u32 *)response );
	}

	// get sd card info
	// use cmd10 to read CID
	{
		ALIGN(4) u8 response[16];
		u32 cmd[2] = { 0xD501000A, 0x00000000 | (SDCardAddr << 16) };
		sendSDCommandR2( cmd, response  );
		dbg_printf("cmd 10 response %08x\n", *(u32 *)response );

		info[0] = response[15];
		for( u8 i = 0; i < 5; ++i ) {
			info[i + 1] = response[12-i];
		}
		info[6] = info[7] = 0;
	}

	// put sd card back to transfer mode
	// CMD7
	{
		ALIGN(4) u8 response[4];
		u32 cmd[2] = { 0xD5010007, 0x00000000 | (SDCardAddr << 16) };
		sendSDCommandR1( cmd, response );
		dbg_printf("cmd 07 response %08x\n", *(u32 *)response );
	}


}

bool isSDHC()
{
	return _isSDHC;
}
