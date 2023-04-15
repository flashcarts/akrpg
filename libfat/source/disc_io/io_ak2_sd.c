#include <nds.h>
#include <string.h>
#include <ioak2.h>
#include "io_ak2_sd.h"



//---------------------------------------------------------------
// Functions needed for the external interface

bool _AK2_SD_isInserted (void) {
	u32 cmd[2] = { 0xCD000000, 0 };
	u32 data = 0xFFFFFFFF;
	ioAK2SendCommand( cmd, 4, 0, &data );
	return (0x00000fc2 == data );
}


bool _AK2_SD_startUp (void) {
	sysSetBusOwners( BUS_OWNER_ARM9, BUS_OWNER_ARM9 );
	return AK2_sddInitSD();
}

bool _AK2_SD_readSectors (u32 sector, u32 numSectors, void* buffer) 
{
	AK2_sddReadBlocks( sector << 9, numSectors, buffer );
	return true;
}

bool _AK2_SD_writeSectors (u32 sector, u32 numSectors, const void* buffer) 
{
	AK2_sddWriteBlocks( sector << 9, numSectors, buffer );
	return true;
}

bool _AK2_SD_clearStatus (void) 
{
	return true;
}

bool _AK2_SD_shutdown (void) 
{
	return true;
}

const IO_INTERFACE _io_AK2_sd = {
	DEVICE_TYPE_AK2_SD,
	FEATURE_MEDIUM_CANREAD | FEATURE_MEDIUM_CANWRITE | FEATURE_SLOT_NDS,
	(FN_MEDIUM_STARTUP)&_AK2_SD_startUp,
	(FN_MEDIUM_ISINSERTED)&_AK2_SD_isInserted,
	(FN_MEDIUM_READSECTORS)&_AK2_SD_readSectors,
	(FN_MEDIUM_WRITESECTORS)&_AK2_SD_writeSectors,
	(FN_MEDIUM_CLEARSTATUS)&_AK2_SD_clearStatus,
	(FN_MEDIUM_SHUTDOWN)&_AK2_SD_shutdown
};

