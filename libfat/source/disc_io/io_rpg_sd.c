#include <nds.h>
#include <string.h>
#include <iorpg.h>
#include "io_rpg_sd.h"



//---------------------------------------------------------------
// Functions needed for the external interface

bool _RPG_SD_isInserted (void) {
	u32 cmd[2] = { 0xCD000000, 0 };
	u32 data = 0xFFFFFFFF;
	ioRpgSendCommand( cmd, 4, 0, &data );
	return (0x00000fc2 == data );
}


bool _RPG_SD_startUp (void) {
	sysSetBusOwners( BUS_OWNER_ARM9, BUS_OWNER_ARM9 );
	return sddInitSD();
}

bool _RPG_SD_readSectors (u32 sector, u32 numSectors, void* buffer) 
{
	sddReadBlocks( sector, numSectors, buffer );
	return true;
}

bool _RPG_SD_writeSectors (u32 sector, u32 numSectors, const void* buffer) 
{
	sddWriteBlocks( sector, numSectors, buffer );
	return true;
}

bool _RPG_SD_clearStatus (void) 
{
	return true;
}

bool _RPG_SD_shutdown (void) 
{
	return true;
}

const IO_INTERFACE _io_rpg_sd = {
	DEVICE_TYPE_RPG_SD,
	FEATURE_MEDIUM_CANREAD | FEATURE_MEDIUM_CANWRITE | FEATURE_SLOT_NDS,
	(FN_MEDIUM_STARTUP)&_RPG_SD_startUp,
	(FN_MEDIUM_ISINSERTED)&_RPG_SD_isInserted,
	(FN_MEDIUM_READSECTORS)&_RPG_SD_readSectors,
	(FN_MEDIUM_WRITESECTORS)&_RPG_SD_writeSectors,
	(FN_MEDIUM_CLEARSTATUS)&_RPG_SD_clearStatus,
	(FN_MEDIUM_SHUTDOWN)&_RPG_SD_shutdown
};

