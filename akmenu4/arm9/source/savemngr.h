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









#ifndef _SAVEMNGR_H_
#define _SAVEMNGR_H_

#include <nds.h>
#include <vector>
#include <string>
#include "singleton.h"
#include "savechip.h"

typedef struct _SAVE_INFO_T
{
    u8    gameTitle[12];
    u8    gameCode[4];
    PACKED u16 gameCRC;
    u8    saveType;
} SAVE_INFO;


enum SAVE_TYPE
{
    ST_UNKNOWN = 0,
    ST_NOSAVE,
    ST_4K,
    ST_64K,
    ST_512K,
    ST_2M,
    ST_4M,
    ST_8M,
    ST_NEW,
    ST_AUTO
};

typedef struct _SAVE_BLOCK_INFO_T
{
    u32 saveSize;
    u32 eepPageSize;
    u32 nandPageSize;
    u32 validPageCount;
    u32 nandBlockCount;
} SAVE_BLOCK_INFO;

class cSaveManager
{
public:

    cSaveManager();

    ~cSaveManager();

public:

    bool importSaveList( const std::string & officialFile, const std::string & customFile );

    bool exportCustomSaveList( const std::string & filename );

    void updateCustomSaveList( const SAVE_INFO & aSaveInfo );

    bool saveLastInfo( const std::string & romFilename );

    bool loadLastInfo( std::string & lastLoadedFilename );

    bool clearLastInfo();

    bool backupSaveData();

    bool restoreSaveData( const std::string & romFilename, SAVE_TYPE saveType );

    bool createSaveFile( const std::string & saveFilename, SAVE_TYPE saveType );

    SAVE_TYPE getSaveTypeByFile( const std::string & romFilename );

    SAVE_TYPE getSaveTypeByInfo( const SAVE_INFO & gameInfo );

    void lockSave();

    bool unlockSave( SAVE_TYPE st, bool writeToDisk );

    bool clearSaveBlocks();

    bool buildSaveBlockTable( CHIP_TYPE * ct );

protected:

    void lockChips();

    bool unlockChip( CHIP_TYPE chipType, bool writeToDisk );

    static u32 saveSizeFromSaveType( SAVE_TYPE saveType );

    static CHIP_TYPE chipTypeFromSaveType( SAVE_TYPE saveType );

    static SAVE_TYPE saveTypeFromChipType( CHIP_TYPE chipType );

    static SAVE_BLOCK_INFO getBlockInfo( CHIP_TYPE chipType );

    bool assignSaveBlocks( CHIP_TYPE chipType, u32 nandAddress[64] );

    u32 _saveBlockTable[64];

    std::vector< SAVE_INFO > _saveList;

    std::vector< SAVE_INFO > _customSaveList;

};

typedef t_singleton< cSaveManager > SaveManager_s;
inline cSaveManager & saveManager() { return SaveManager_s::instance(); }

#endif//_SAVEMNGR_H_
