#include <nds.h>
#include <string.h>
#include <iorpg.h>
#include "io_rpg_nand.h"

//---------------------------------------------------------------
// Functions needed for the external interface


bool _RPG_NAND_isInserted (void) {
	u32 cmd[2] = { 0xB8000000, 0x00000000 };
	u32 data = 0;
	ioRpgSendCommand( cmd, 4, 0, &data );
	return (0x00000fc2 == data );
}

bool _RPG_NAND_startUp (void) {
	if( !_RPG_NAND_isInserted() )
		return false;
	return ndInitNAND();
}

bool _RPG_NAND_readSectors (u32 sector, u32 numSectors, void* buffer) {
	bool noError = true;
	u8 retry = 4;
	do {
		ndReadPages( sector<<9, numSectors, buffer );
		noError = ndCheckError();
	} while( !noError && --retry );

	return noError;
}

bool _RPG_NAND_writeSectors (u32 sector, u32 numSectors, const void* buffer) {
	ndWritePages( sector<<9, numSectors, buffer );
	return true;
}

bool _RPG_NAND_clearStatus (void) {
	dbg_printf("ndFinishPartialCopy() called from _RPG_NAND_clearStatus()\n");
	ndFinishPartialCopy();
	return true;
}

bool _RPG_NAND_shutdown (void) {
	dbg_printf("ndFinishPartialCopy() called from _RPG_NAND_shutdown()\n");
	ndFinishPartialCopy();
	return true;
}

const IO_INTERFACE _io_rpg_nand = {
	DEVICE_TYPE_RPG_NAND,
	FEATURE_MEDIUM_CANREAD | FEATURE_MEDIUM_CANWRITE | FEATURE_SLOT_NDS,
	(FN_MEDIUM_STARTUP)&_RPG_NAND_startUp,
	(FN_MEDIUM_ISINSERTED)&_RPG_NAND_isInserted,
	(FN_MEDIUM_READSECTORS)&_RPG_NAND_readSectors,
	(FN_MEDIUM_WRITESECTORS)&_RPG_NAND_writeSectors,
	(FN_MEDIUM_CLEARSTATUS)&_RPG_NAND_clearStatus,
	(FN_MEDIUM_SHUTDOWN)&_RPG_NAND_shutdown
} ;
