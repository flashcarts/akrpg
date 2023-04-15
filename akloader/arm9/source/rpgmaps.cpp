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









#include <nds.h>
#include <string.h>
#include <stdio.h>
#include <fat.h>
#include <iorpg.h>
#include "rpgmaps.h"
#include "dbgtool.h"
#include "../../share/memtool.h"
#include "dirtools.h"

static u32 iterateFileCluster( PARTITION * partition, u32 firstCluster, u32 * fatMap, bool isSDHC )
{
    u32 cluster = firstCluster;
    u32 mapSize = 1;   // a file takes at least 1 cluster
    u32 * pfatMap = fatMap;
    bool needMap = false;

    while( cluster != CLUSTER_EOF )
    {
        u32 clusterStartSector = _FAT_fat_clusterToSector( partition, cluster );
        if( NULL != pfatMap )
            *pfatMap++ = isSDHC ? clusterStartSector : (clusterStartSector << 9);
        u32 lastCluster = cluster;
        cluster = _FAT_fat_nextCluster( partition, cluster );
        if( cluster != lastCluster + 1 && cluster != CLUSTER_EOF )
        {
            needMap = true;
            dbg_printf("need FAT map:\n%08x->%08x\n", lastCluster, cluster );
        }
        ++mapSize;
    }
    if( needMap )
        return mapSize;
    else
        return mapSize;
}

/*
static void dirFromFullPath( const char * path, char * dir )
{
    char * p = strrchr( path, '/' );
    memcpy( dir, path, p - path + 1 );
    dir[p-path+1]=0;
}

static char * filenameFromFullPath( const char * path )
{
    char * p = strrchr( path, '/' );
    return p+1;
}
*/

static bool rpgBuildFATMap( const char * path, u32 & clusterSize )
{
    char dir[512];
    char * pdir = dir;
    dirFromFullPath( path, dir );
    dbg_printf("dir: %s\n", dir );
    char * filename = filenameFromFullPath( path );
    dbg_printf("file: %s\n", filename );

    /////////////////////////////////////////
    PARTITION* partition = NULL;

    // Get the partition this directory is on
    partition = _FAT_partition_getPartitionFromPath (pdir);

    if (partition == NULL) {
        //r->_errno = ENODEV;
        dbg_printf("ENODEV\n");
        return false;
    }

    // Move the path pointer to the start of the actual path
    if (strchr (pdir, ':') != NULL) {
        pdir = strchr (pdir, ':') + 1;
    }
    if (strchr (pdir, ':') != NULL) {
        //r->_errno = EINVAL;
        dbg_printf("EINVAL\n");
        return false;
    }

    //// Set the default device to match this one
    //if (!_FAT_partition_setDefaultPartition (partition)) {
    //    //r->_errno = ENOENT;
    //    dbg_printf("ENOENT\n");
    //    return false;
    //}

    // Try changing directory
    if (_FAT_directory_chdir (partition, pdir)) {
        // Successful
        dbg_printf("SUCCESS fuck\n");
        //return 1;
    } else {
        // Failed
        //r->_errno = ENOTDIR;
        dbg_printf("ENOTDIR\n");
        return false;
    }

    /////////////////////////////////////////

    DIR_ENTRY entry;
    _FAT_directory_entryFromPath( partition, &entry, filename, NULL );

    u32 fileFirstCluster = _FAT_directory_entryGetCluster( entry.entryData );
    if( CLUSTER_FREE == fileFirstCluster ) {
        dbg_printf("first cluster fail\n");
        return false;
    }
    dbg_printf( "%s startClus %08x\n", filename, fileFirstCluster );

    // compute memory requirement for fat map
    u32 cluster = fileFirstCluster;
    u32 mapSize = iterateFileCluster( partition, cluster, NULL, false );
#ifdef DEBUG
    bool needMap = ( 0 != mapSize );

    //dbg_printf( "%s ClusSize %08x\n", filename, fatMap.size() );
    dbg_printf( "%s mapSize %d\n", filename, mapSize );
    dbg_printf( "need FAT map %d\n", needMap );
#endif
    u32 * fatMap = (u32 *)0x2280000;
    //fatMap = (u32 *)malloc(mapSize*sizeof(u32));

    // build fat map
    bool sdhc = ((memcmp( path, "fat1:", 5 ) == 0) || (memcmp( path, "FAT1:", 5 ) == 0) ) && isSDHC();
    iterateFileCluster( partition, cluster, fatMap, sdhc );

    u32 fatMapAddress = SRAM_FAT_START;
    ioRpgSetMapTableAddress( MTN_NAND_OFFSET1, 0 );

    //for( size_t i = 0; i < fatMap.size(); ++i )
    /* u32 clusterZeroAtSector = */ _FAT_fat_clusterToSector( partition, 0 );
    for( size_t i = 0; i < mapSize; ++i )
    {
        ioRpgWriteSram( fatMapAddress + i * 4, &fatMap[i], 4 );
        if( i < 16 ) {
            dbg_printf("%08x", fatMap[i] );
        }
    }
    dbg_printf("isSDHC %d\n", isSDHC() );
    dbg_printf("fat map build done.\n" );

    clusterSize = partition->bytesPerCluster;
    //u8 offset = 0;
    //while( clusterSize > 1 )
    //{
    //    clusterSize >>= 1;
    //    ++offset;
    //}
    //offset -= 8;
    //ioRpgSetDeviceStatus( 0x01, 0x03, 0/*auto set to 128K*/, offset );
    dbg_printf("clusterZeroAtSector A %08x\n", clusterZeroAtSector );
    //dbg_printf("clusterZeroAtSector B %08x\n", _FAT_fat_clusterToSector( partition, 1752 ) );

    zeroMemory( (void *)fatMap, 0x100000 );  // normal loader
    DC_FlushAll();
    DC_InvalidateAll();
    //if( NULL != fatMap )
    //    free( fatMap );

    return true;
}

extern "C" u32 ndLog2Phy( u32 logicAddress, u32 * oldPhyAddress );
static ALIGN(4) u8 cfgPage[528];
bool rpgBuildNANDMap()
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
    //wait_press_b();
    u32 totalLBAs = totalZones * 1024;   // map all blocks
    for( u32 i = 0; i < totalLBAs; ++i ) {
        u32 phyAddress = ndLog2Phy( i << 17, NULL );
        ioRpgWriteSram( i * 4, &phyAddress, 4 );
        if( i < 16 ) {
            dbg_printf("%08x", phyAddress );
        }
    }
    return true;
}

void rpgBuildMaps( const char * path )
{
    rpgBuildNANDMap();
    u32 clusterSize = 0;
    rpgBuildFATMap( path, clusterSize );
    u32 clusterSizeShift = 0;
    while( clusterSize > 512 ) {
        clusterSize >>= 1;
        clusterSizeShift ++;
    }

    ioRpgSetMapTableAddress( MTN_NAND_OFFSET1, 0 );
    ioRpgSetMapTableAddress( MTN_MAP_A, SRAM_NANDLUT_START );
    ioRpgSetMapTableAddress( MTN_SAVE_TABLE, SRAM_SAVETABLE_START );
    ioRpgSetMapTableAddress( MTN_MAP_B, SRAM_FAT_START );


    if( memcmp( path, "fat0:", 5 ) == 0 ) {
        ioRpgSetDeviceStatus( 0x01, 0x03, clusterSizeShift, clusterSizeShift, false );
        dbg_printf("use nand cluster size %d\n", clusterSizeShift);
    }
    else {
        ioRpgSetDeviceStatus( 0x03, 0x02, clusterSizeShift, clusterSizeShift, isSDHC() );
        dbg_printf("use sd cluster size %d, isSDHC %d\n", clusterSizeShift, isSDHC()  );
    }

}
