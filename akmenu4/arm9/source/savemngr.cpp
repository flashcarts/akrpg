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









#include "savemngr.h"
#include "inifile.h"
#include "dsrom.h"
#include "dbgtool.h"
#include "systemfilenames.h"
#include "progresswnd.h"
#include "rpgprotocol.h"
#include "nanddriver.h"
#include "fatfile_ex.h"
#include "language.h"
#include "datetime.h"
#include "ui.h"

using namespace akui;

cSaveManager::cSaveManager()
{
    lockSave();
    for( size_t i = 0; i < 64; ++i ) {
        _saveBlockTable[i] = 0xFFFFFFFF;
    }
}

cSaveManager::~cSaveManager()
{
}

//EEP 指令
//D2 00 00 00 00 00 00 00
//
//bit 51 - 48
//48    eep enable
//49    eep to nand enable
//53 - 50 0001=auto 0000=95040 1000=95640 0100=95512 0010=25pe80

void selectSaveChip( CHIP_TYPE type, bool nandON )
{
    u8 byte2 = 0;
    switch( type )
    {
    case CT_NONE:
        byte2 = 0x00;
        break;
    case CT_4K:
        byte2 = 0x03; // 0000 0011
        break;
    case CT_512K:
        byte2 = 0x13; // 0001 0011
        break;
    case CT_8M:
        byte2 = 0x0b; // 0000 1011
        break;
    case CT_AUTO: // auto detect
        byte2 = 0x07; // 0000 0111
        break;
    default:
        byte2 = 0x00;
    };
    if( !nandON )
        byte2 &= 0xFD; // 1111 1101

    u32 cmd[2] = { 0xd2000000 | (byte2 <<16 ), 0x00000000 };
    ioRpgSendCommand( cmd, 0, 0, NULL );
}


void cSaveManager::lockChips()
{
    selectSaveChip( CT_NONE, false );
}

bool cSaveManager::unlockChip( CHIP_TYPE chipType, bool writeToDisk )
{
    selectSaveChip( chipType, writeToDisk );

    if( writeToDisk || CT_AUTO == chipType ) {
        clearSaveBlocks();
        u32 nandAddress[64];
        if( !assignSaveBlocks( chipType, nandAddress ) )
            return false;
    }

    return true;
}

void cSaveManager::lockSave()
{
    lockChips();
}

bool cSaveManager::unlockSave( SAVE_TYPE saveType, bool writeToDisk )
{
    return unlockChip( chipTypeFromSaveType( saveType ), writeToDisk );
}

static bool loadSaveList( const std::string & filename, std::vector< SAVE_INFO > & buffer )
{
    FILE * f = fopen( filename.c_str(), "rb" );
    if( NULL == f )
    {
        dbg_printf("fopen %s fail\n", filename.c_str() );
        return false;
    }
    fseek( f, 0, SEEK_END );
    u32 filesize = ftell( f );
    if( 0 == filesize || 0 != (filesize % 19) )
    {
        dbg_printf("%s size error %d\n", filename.c_str(), filesize );
        fclose( f );
        f = NULL;
        return false;
    }

    u32 saveCount = filesize / 19;
    buffer.resize( saveCount );

    fseek( f, 0, SEEK_SET );
    u32 readed = fread( &buffer[0], 1, filesize, f );
    if( filesize != readed )
    {
        dbg_printf("%s read length error %d\n", filename.c_str(), readed );
        fclose( f );
        f = NULL;
        return false;
    }

    fclose( f );
    return true;
}

bool cSaveManager::importSaveList( const std::string & officialFile, const std::string & customFile )
{
    bool ret = loadSaveList( officialFile, _saveList );
    loadSaveList( customFile, _customSaveList );
    return ret;
}

bool cSaveManager::exportCustomSaveList( const std::string & filename )
{
    FILE * f = fopen( filename.c_str(), "wb" );
    if( NULL == f )
    {
        dbg_printf("fopen %s fail\n", filename.c_str() );
        return false;
    }
    u32 filesize = _customSaveList.size() * 19;

    u32 written = fwrite( &_customSaveList[0], 1, filesize, f );
    if( filesize != written )
    {
        dbg_printf("%s write length error %d\n", filename.c_str(), written );
        fclose( f );
        f = NULL;
        return false;
    }
    fclose( f );
    f = NULL;

    return true;
}

void cSaveManager::updateCustomSaveList( const SAVE_INFO & aSaveInfo )
{
    size_t i = 0;
    for( i = 0; i < _customSaveList.size(); ++i )
    {
        if( 0 == memcmp( &_customSaveList[i], &aSaveInfo, sizeof(SAVE_INFO) - 1 ) ) {
            _customSaveList[i].saveType = aSaveInfo.saveType;
            break;
        }
    }
    if( i == _customSaveList.size() )
        _customSaveList.push_back( aSaveInfo );

    exportCustomSaveList( SFN_CUSTOM_SAVELIST );
}

bool cSaveManager::saveLastInfo( const std::string & romFilename )
{
    CIniFile f;
    f.SetString( "Save Info", "lastLoaded", romFilename );
    if( !f.SaveIniFile( SFN_LAST_SAVEINFO ) )
        return false;

    return true;
}

bool cSaveManager::loadLastInfo( std::string & lastLoadedFilename )
{
    CIniFile f;
    if( !f.LoadIniFile( SFN_LAST_SAVEINFO ) ) {
        lastLoadedFilename = "";
        return false;
    }

    lastLoadedFilename = f.GetString( "Save Info", "lastLoaded", "" );
    if( "" == lastLoadedFilename )
        return false;

    return true;
}

bool cSaveManager::clearLastInfo()
{
    return saveLastInfo( "" );
}

SAVE_BLOCK_INFO cSaveManager::getBlockInfo( CHIP_TYPE chipType )
{
    SAVE_BLOCK_INFO ret;
    switch( chipType )
    {
    case CT_4K:
        ret.eepPageSize = 16;
        ret.nandBlockCount = 32;
        ret.nandPageSize = 16;
        ret.saveSize = 512;
        ret.validPageCount = 1;
        break;
    case CT_512K:
        ret.eepPageSize = 128;
        ret.nandBlockCount = 32;
        ret.nandPageSize = 2048;
        ret.saveSize = 65536;
        ret.validPageCount = 1;
        break;
    case CT_8M:
        ret.eepPageSize = 256;
        ret.nandBlockCount = 64;
        ret.nandPageSize = 256;
        ret.saveSize = 1048576;
        ret.validPageCount = 64;
        break;
    case CT_NONE:
    default:
        ret.eepPageSize = 0;
        ret.nandBlockCount = 0;
        ret.nandPageSize = 0;
        ret.saveSize = 0;
        ret.validPageCount = 0;
    }
    return ret;
}

bool cSaveManager::backupSaveData()
{
    dbg_printf( "SaveManager::backupSaveData()\n" );

    CHIP_TYPE autoCT = CT_NONE;
    bool buildOK = buildSaveBlockTable( &autoCT );
    if( !buildOK || CT_NONE == autoCT ) {
        clearLastInfo();
        clearSaveBlocks();
        return true;
    }

    std::string loadLoadedFile;
    if( !loadLastInfo( loadLoadedFile ) ) {
        loadLoadedFile = "fat0:/unknown_savedata_" + datetime().getTimeStampString() + ".nds";
    }

    // update custom savlist if needed
    DSRomInfo info;
    if( info.loadDSRomInfo( loadLoadedFile, false ) ) {
        if( ST_UNKNOWN == info._saveInfo.saveType ) {
            info._saveInfo.saveType = saveTypeFromChipType( autoCT );
            updateCustomSaveList( info._saveInfo );
        }
    }

////////////////
    loadLoadedFile += ".sav";
    FILE * f = fopen( loadLoadedFile.c_str(), "wb" );
    if( NULL == f ) {
        if( (0 == memcmp( "fat1:/", loadLoadedFile.c_str(), 6 ) ) ) { // no sd card, backup to nand flash, and warn user
            size_t lastSlashPos = loadLoadedFile.find_last_of( '/' );
            loadLoadedFile.replace( 0, lastSlashPos + 1, "fat0:/" );
            loadLoadedFile.insert( loadLoadedFile.size() - 4, "." + datetime().getTimeStampString() );
            f = fopen( loadLoadedFile.c_str(), "wb" );
            if( NULL == f )
                return false;

            std::string warningText = formatString( LANG("no sd for save", "text").c_str(), loadLoadedFile.c_str() );
            messageBox( NULL, LANG("no sd for save", "title"), warningText, MB_OK );
        } else {
            return false;
        }
    }


    progressWnd().setTipText( LANG("progress window", "processing save" ) );
    progressWnd().show();
    progressWnd().setPercent(0);
    SAVE_BLOCK_INFO bi = getBlockInfo( autoCT );

#define SAVEDATA_BUFFER_SIZE (128 * 1024)
    u8 * saveDataBuffer = new u8[SAVEDATA_BUFFER_SIZE];
    static ALIGN(4) u8 readNandBuffer[2112];
    memset( saveDataBuffer, 0x5a, SAVEDATA_BUFFER_SIZE );
    u8 * pSaveDataBuffer = saveDataBuffer;

    u32 written = 0;
    for( u32 i = 0; i < bi.nandBlockCount; ++i ) {
        u8 percent = i * 100 / bi.nandBlockCount;
        if( !(percent & 0x03) ) {
            progressWnd().setPercent( percent );
        }
        if( _saveBlockTable[i] != 0xFFFFFFFF ) {
            for( u32 j = 0; j < bi.validPageCount; ++j )
            {
                ioRpgReadNand( _saveBlockTable[i] + j * 2048, readNandBuffer, 2048 );
                memcpy( pSaveDataBuffer, readNandBuffer, bi.nandPageSize );
                pSaveDataBuffer += bi.nandPageSize;
                if( pSaveDataBuffer >= saveDataBuffer + SAVEDATA_BUFFER_SIZE ) {
                    written = fwrite( saveDataBuffer, 1, SAVEDATA_BUFFER_SIZE, f );
                    pSaveDataBuffer = saveDataBuffer;
                    dbg_printf("written %d\n", written);
                }
            }
        }
    }
    written = fwrite( saveDataBuffer, 1, pSaveDataBuffer - saveDataBuffer, f );
    dbg_printf("written %d\n", written);
    progressWnd().setPercent( 100 );

    progressWnd().hide();
    fclose( f );
    clearLastInfo();
    clearSaveBlocks();

    delete[] saveDataBuffer;

    return true;
}

static void checkRedund()
{
    ALIGN(4) u8 cfgPage[528];
    dbg_printf( "checkRedund cfgPage\n");
    ioRpgReadNand( 0x00000000, cfgPage, 528 );

    // read zones count
    u32 totalZones = 1 << (cfgPage[0x0c] - 10);
    //wait_press_b();

    u32 saveBlockTable[64];
    for( size_t i = 0; i < 64; ++i )
        saveBlockTable[i] = 0xFFFFFFFF;

    ALIGN(4) u8 redundData[16];
    u8 ctByte = 0x00;
    u32 saveBlockCountCheck = 0;
    for( size_t i = 0; i < totalZones * 1024; ++i ) {
        ioRpgReadNandRedundant( (i << 17) + 512 * 3, redundData );
        if( redundData[0] != 0xff && redundData[0] != 0x00 )
            dbg_printf("(%02x%02x%02x)", redundData[1],redundData[2],redundData[3] );
        if( redundData[0] >= 0x80 && redundData[0] < 0x80 + 64 ) {
            saveBlockCountCheck++;
            u8 saveIndex = redundData[3] - 0x80;
            if( saveBlockTable[saveIndex] != 0xFFFFFFFF ) {
                dbg_printf("checkRedund() FATAL ERROR: Multi save block assign found\n");
                dbg_printf("saveTable[%d] 1=%08x 2=%08x\n", saveIndex, saveBlockTable[saveIndex], (i << 17) );
                wait_press_b();
            }
            saveBlockTable[saveIndex] = (i << 17);
            if( redundData[2] >= 0xc0 && redundData[2] <= 0xc4 && 0 == ctByte ) {
                ctByte = redundData[2];
            }
        }
    }

    if( saveBlockCountCheck != 0 && saveBlockCountCheck != 32 && saveBlockCountCheck != 64 )
    {
        dbg_printf("checkRedund() FATAL ERROR: saveblock COUNT ERROR %d\n", saveBlockCountCheck );
        dbg_printf("totalZones %d\n", totalZones );
        wait_press_b();
    } else {
        dbg_printf("checkRedund() OK OK OK OK OK OK OK OK OK OK OK %d\n", saveBlockCountCheck );
    }


}

static void clearSaveSram()
{
    progressWnd().setTipText( LANG("progress window", "processing save") );
    progressWnd().show();
    progressWnd().setPercent( 0 );

    // clear sram, fill with 0xff, to keep consist with empty save blocks
    u32 fillData = 0xFFFFFFFF;
    for( size_t i = 0;i < 1024 * 1024; i += 4 ) {
        ioRpgWriteSram( SRAM_SAVEDATA_START + i, &fillData, 4 );
        if( !(i % (1024 * 128)) ) {
            dbg_printf("clear sram %d%s\n", i*100/(1024*1024), "%" );
            progressWnd().setPercent( i*100 / (1024 * 1024) );
        }
    }
    for( size_t i = 0;i < 2048; i += 4 ) {
        ioRpgWriteSram( SRAM_EEP_BUFFER_START + i, &fillData, 4 );
    }
    progressWnd().setPercent(100);
}

bool cSaveManager::restoreSaveData( const std::string & romFilename, SAVE_TYPE saveType )
{
    dbg_printf( "SaveManager::restoreSaveData()\n" );

    CHIP_TYPE chipType = chipTypeFromSaveType( saveType );
    if( CT_NONE == chipType ) {
        lockChips();
        return true;
    }

    unlockSave( saveType, true );
    checkRedund();

    if( ST_UNKNOWN == saveType || ST_AUTO == saveType ) {
        clearSaveSram();
        saveLastInfo( romFilename );
        return true;
    }

    u32 saveSize = saveSizeFromSaveType( saveType );
    std::string saveFilename(romFilename);
    saveFilename += ".sav";

    //
    FILE * f = fopen( saveFilename.c_str(), "rb" );
    if( NULL == f ) { // try to create a new savefile and test if can allocate disk space (not fully allocate now, only write a string)
        //if( !createSaveFile( saveFilename.c_str(), saveType ) ) {
        //    return false;
        //} else {
            clearSaveSram();
            saveLastInfo( romFilename );
            return true;
        //}
    }

    // clear FLASH
    if( saveType >= ST_2M && saveType <= ST_8M ) {
        saveChipFlashErase( true );
    }

    const u32 blockSize = 512;
    static ALIGN(4) u8 buffer[blockSize];
    u32 readCount = 0;

    progressWnd().setTipText( LANG("progress window", "processing save") );
    progressWnd().show();
    progressWnd().setPercent( 0 );
    for( u32 i = 0; i < saveSize; i += blockSize )
    {
        u8 percent = i * 100 / saveSize;
        if( !(percent & 0x07) )
            progressWnd().setPercent( percent );

        u32 readed = fread( buffer, 1, blockSize, f );
        readCount += readed;
        //if( blockSize != readed ) {
        //    break;
        //}
        if( 0 == readed )
            break;
        saveChipWrite( i, buffer, readed, chipType );
    }
    progressWnd().setPercent( 100 );
    progressWnd().hide();
    fclose( f );

    saveLastInfo( romFilename );

    return true;
}

static ALIGN(4) u8 cfgPage[528];
bool cSaveManager::buildSaveBlockTable( CHIP_TYPE * ct )
{
    // read cfg page
    // 53 4d 54 44 4d 47 20 00 0e 00 01 06 0d (14) 03 02
    dbg_printf( "cfgPage\n");
    ioRpgReadNand( 0x00000000, cfgPage, 528 );
    for( u32 i = 0; i < 16; ++i ) {
        dbg_printf("%02x", cfgPage[i] );
    }
    // read zones count
    u32 totalZones = 1 << (cfgPage[0x0c] - 10);

    for( size_t i = 0; i < 64; ++i )
        _saveBlockTable[i] = 0xFFFFFFFF;

    // if user turn power off during writting save data(in game or in menu),
    // the content of save blocks is undefined. Don't write save data to .sav
    // file if this function returns false
    ALIGN(4) u8 redundData[16];
    u8 ctByte = 0x00;
    u32 saveBlockCount = 0;
    for( size_t i = 0; i < totalZones * 1024; ++i ) {
        ioRpgReadNandRedundant( (i << 17) + 512 * 3, redundData );
        if( redundData[0] != 0xff && redundData[0] != 0x00 && redundData[0] != 0xf0 )
            dbg_printf("(%02x%02x%02x)", redundData[1],redundData[2],redundData[3] );

        if( redundData[0] >= 0x80 && redundData[0] < 0x80 + 64 ) {
            u8 saveIndex = redundData[3] - 0x80;
            if( saveIndex < 64 ) {
                saveBlockCount++;
                if( _saveBlockTable[saveIndex] != 0xFFFFFFFF ) {
                    dbg_printf("FATAL ERROR: Multi save block assign found\n");
                    dbg_printf("saveTable[%d] 1=%08x 2=%08x\n", saveIndex, _saveBlockTable[saveIndex], (i << 17) );
                    wait_press_b();
                    return false;
                }
                _saveBlockTable[saveIndex] = (i << 17);
                if( redundData[2] >= 0xc0 && redundData[2] <= 0xc4 && 0 == ctByte ) {
                    ctByte = redundData[2];
                }
            } else {
                dbg_printf("FATAL ERROR: idx err, shouldn't have a %d\n", saveIndex );
                dbg_printf("redunt0-4:(%02x%02x%02x%02x%02x)\n", redundData[0], redundData[1],
                            redundData[2],redundData[3], redundData[4] );
                dbg_printf("at block: %d\n", i );
                wait_press_b();
                return false;
            }
        }
    }
    dbg_printf( "saveBlockCount %d\n", saveBlockCount );
    if( 0 != saveBlockCount && saveBlockCount != 32 && saveBlockCount != 64 ) {
        dbg_printf("FATAL ERROR: Multi save block assign found, saveBlockCount %d\n", saveBlockCount );
        wait_press_b();
        return false;
    }

    *ct = CT_NONE;
    switch( ctByte ) {
        case 0xc0: // 40
            *ct = CT_4K;
            break;
        case 0xc1: // 80
            *ct = CT_8M;
            break;
        case 0xc2: // 512
            *ct = CT_512K;
            break;
        case 0xc4: // 640
            *ct = CT_512K;
            break;
        default:
            *ct = CT_NONE;
    }
    dbg_printf("save mark %02x, type %d\n", ctByte, *ct );
    //wait_press_b();

    return true;
}

bool cSaveManager::clearSaveBlocks()
{
    ioRpgReadNand( 0x00000000, cfgPage, 528 );
    for( u32 i = 0; i < 16; ++i ) {
        dbg_printf("%02x", cfgPage[i] );
    }
    // read zones count
    u32 totalZones = 1 << (cfgPage[0x0c] - 10);

    dbg_printf("clear save: \n");
    ALIGN(4) u8 redundData[16];
    for( size_t i = 0; i < totalZones * 1024; ++i ) {
        ioRpgReadNandRedundant( (i << 17) + 512 * 3, redundData );
        if( redundData[0] >= 0x80 && redundData[0] < 0x80 + 64 ) {
            ioRpgEraseNand( i << 17 );
        }
    }
    dbg_printf( "end\n" );

    // 这个地方是否需要重新建表？

    return true;
}

bool cSaveManager::assignSaveBlocks( CHIP_TYPE chipType, u32 nandAddress[64] )
{
    if( CT_NONE == chipType ) {
        lockChips();
        return true;
    }

    memset( nandAddress, 0xFF, 64 * 4 );

    ioRpgReadNand( 0x00000000, cfgPage, 528 );
    for( u32 i = 0; i < 16; ++i ) {
        dbg_printf("%02x", cfgPage[i] );
    }
    // read zones count
    u32 totalZones = 1 << (cfgPage[0x0c] - 10);

    // 获取存档要分配的block的信息
    SAVE_BLOCK_INFO bi = getBlockInfo( chipType );

    //u32 saveBlockPerZone = bi.nandBlockCount / totalZones;
    u32 saveBlockPerZone = 32 / totalZones;      // always assign 32 blocks (4M max), change this value to 64 will enlarge save space to 8M

    static ALIGN(4) u8 markData[528];
    memset( markData, 0xFF, 528 );
    markData[512] = 0xBF;  ///// these two bytes must be 0xbf
    markData[513] = 0xBF;
    for( u8 zone = 0; zone < totalZones; ++zone ) {
        for( u8 b = 0; b < saveBlockPerZone; ++b ) {
            u32 blockAddr = 0x0400;
            blockAddr = ndSearchFreeBlock( zone, true );
            if( 0x0400 == blockAddr ) {
                dbg_printf("ERROR, ZONE %d HAS NO FREE BLOCK FOR SAVE\n", zone);
                wait_press_b();
                return false;
            }
            blockAddr = (zone * 1024 + blockAddr) << 17;

            for( size_t i = 0; i < 32; ++i ) {
                if( nandAddress[i] == blockAddr ) {
                    dbg_printf("ERROR, multi assign, i=%d addr=%08x\n", i, blockAddr );
                    wait_press_b();
                }
            }

            nandAddress[ zone * saveBlockPerZone + b ] = blockAddr;

            //ioRpgWriteSram528( SRAM_DISKBUFFER_START, markData );
            //ioRpgPageCopySramToNand( SRAM_DISKBUFFER_START, blockAddr + 512 * 3 );
            markData[515] = 0x80 + (zone * saveBlockPerZone + b);
            ioRpgWriteNand( blockAddr + 512 * 3, markData, 528 );
        }
    }
    memcpy( nandAddress + 32, nandAddress, 4 * 32 ); // assign the second 32 blocks to the first 32

    ioRpgWriteSram( SRAM_SAVETABLE_START, nandAddress, 4 * 64 );

    for( size_t i = 0; i < 16; ++i ) { // print first 16 blocks for debug
        dbg_printf("savblk(%08x)", nandAddress[i] );
    }
    ioRpgSetMapTableAddress( MTN_SAVE_TABLE, SRAM_SAVETABLE_START );

    /* IMPORTANT */
    ndBuildLUT(); // this is very necessary or lookup table system will be messed up

    dbg_printf("CT: %d\n", chipType );
    dbg_printf("(bi.saveSize %d)", bi.saveSize );
    dbg_printf("(bi.nandBlockCount %d)", bi.nandBlockCount );
    dbg_printf("(bi.validPageCount %d)", bi.validPageCount );
    dbg_printf("(bi.eepPageSize %d)", bi.eepPageSize );
    dbg_printf("(bi.nandPageSize %d)", bi.nandPageSize );

    //wait_press_b();

    return true;
}

// this function should make sure that disk have enough space for save file,
// but it doesnt do that for now.
bool cSaveManager::createSaveFile( const std::string & saveFilename, SAVE_TYPE saveType )
{
    FILE * f = fopen( saveFilename.c_str(), "wb" );
    if( NULL == f )
        return false;

    //u32 writeCount = (saveSizeFromSaveType( saveType )>>2);
    const char * dummy = "empty save file";
    const u32 len = strlen(dummy);
    u32 written = fwrite( &dummy, 1, len, f );
    if( len != written ) {
        fclose( f );
        return false;
    }
    fclose( f );
    return true;
}


CHIP_TYPE cSaveManager::chipTypeFromSaveType( SAVE_TYPE saveType )
{
    if( saveType <= ST_4K )
        return CT_4K;
    if( saveType <= ST_512K )
        return CT_512K;
    if( saveType <= ST_8M )
        return CT_8M;
    if( ST_AUTO == saveType )
        return CT_AUTO;

    return CT_NONE;
}

SAVE_TYPE cSaveManager::saveTypeFromChipType( CHIP_TYPE chipType )
{
    switch( chipType )
    {
    case CT_4K:
        return ST_4K;
    case CT_512K:
        return ST_512K;
    case CT_8M:
        return ST_4M;
    default:
        return ST_UNKNOWN;
    }
    return ST_UNKNOWN;
}


u32 cSaveManager::saveSizeFromSaveType( SAVE_TYPE saveType )
{
    switch( saveType )
    {
    case ST_UNKNOWN:
        return 0;
    case ST_NOSAVE:
        return 0;
    case ST_4K:
        return 512;
    case ST_64K:
        return 8192;
    case ST_512K:
        return 65536;
    case ST_2M:
        return 262144;
    case ST_4M:
        return 524288;
    case ST_8M:
        return 1048576;
    case ST_NEW:
        return 2097152;
    default:
        return 0;
    };
}


SAVE_TYPE cSaveManager::getSaveTypeByFile( const std::string & romFilename )
{
    return ST_UNKNOWN;
}

SAVE_TYPE cSaveManager::getSaveTypeByInfo( const SAVE_INFO & gameInfo )
{
    // 先从用户自定义存档列表里面查找
    size_t saveCount = _customSaveList.size();
    for( size_t i = 0; i < saveCount; ++i )
    {
        if( 0 == memcmp( &gameInfo, &_customSaveList[i], 18 ) )
        {
            //dbg_printf( "custom savetype %d\n", _customSaveList[i].saveType );
            return (SAVE_TYPE)_customSaveList[i].saveType;
        }
    }

    // 再从官方存档列表里面查找
    saveCount = _saveList.size();
    for( size_t i = 0; i < saveCount; ++i )
    {
        if( 0 == memcmp( &gameInfo, &_saveList[i], 18 ) )
        {
            //dbg_printf( "system savetype %d\n", _saveList[i].saveType );
            return (SAVE_TYPE)_saveList[i].saveType;
        }
    }

    return ST_UNKNOWN;
}
