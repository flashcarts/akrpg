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
#include <string>
#include "files.h"
//#include "dbgtool.h"
#include <fat.h>
#include <fatdir.h>
#include "msgbox.h"
#include "progresswnd.h"
#include <errno.h>
#include <cstdio>
#include <unistd.h>
#include "language.h"
#include "rpgprotocol.h"
#include "datetime.h"
#include "dsrom.h"
#include <iorpg.h>

using namespace akui;

static SRC_FILE_MODE _srcFileMode = SFM_COPY;
static std::string _srcFilename = "";

bool loadFile( void * buffer, const std::string & filename, size_t offset, size_t & readCount )
{
    if( "" == filename )
        return false;

    if( NULL == buffer ) {
        dbg_printf("invalid buffer pointer\n");
        struct stat st;
        stat( filename.c_str(), &st );
        readCount = st.st_size;
        return false;
    }

    FILE * f = fopen( filename.c_str(), "rb" );
    if( NULL == f ) {
        dbg_printf("file does not exist\n");
        readCount = 0;
        return false;
    }

    fseek( f, 0, SEEK_END );
    int fileSize = ftell( f );

    if( -1 == fileSize ) {
        fclose( f );
        readCount = 0;
        return false;
    }

    fseek( f, offset, SEEK_SET );
    size_t readed = fread( buffer, 1, fileSize, f );
    fclose( f );

    readCount = readed;
    if( readed != (size_t)fileSize-offset ) {
        dbg_printf("fread fail: %d/%d\n", readed, fileSize );
        readCount = 0;
        return false;
    }

    return true;
}

int getFileSize( const std::string & filename )
{
    if( "" == filename )
        return -1;

    struct stat st;
    if( -1 == stat( filename.c_str(), &st ) ) {
        return -1;
    }
    return st.st_size;
}




//---------------------------------------------------------------------------------//
extern "C"
{
    int freadex( void * buffer, int _size, int _n, FILE * f );
    int fwriteex( const void * buffer, int _size, int _n, FILE * f );
}

bool stopCopying = false;
bool copyingFile = false;

#define CONTINUOUS_COPY_SIZE (1024 * 1024)
bool copyFile( const std::string & srcFilename, const std::string & destFilename, bool silently, size_t copyLength )
{
    dbg_printf("copy %s to %s\n", srcFilename.c_str(), destFilename.c_str() );
    struct stat srcSt;
    if( 0 != stat( srcFilename.c_str(), &srcSt ) ) {
        messageBox( NULL, LANG("copy file error","title"), LANG("copy file error","text"), MB_OK );
        return false;
    }

    u64 total = 0;
    u64 used = 0;
    u64 freeSpace = 0;

    std::string destDiskName = destFilename.substr( 0, 6 );
    if( destDiskName != "fat0:/" && destDiskName != "fat1:/" )
        return false;

    if( !getDiskSpaceInfo( destDiskName, total, used, freeSpace ) ) {
        messageBox( NULL, LANG("no free space","title"), LANG("no free space","text"), MB_OK );
        return false;
    }

    if( 0 == copyLength || copyLength > (size_t)srcSt.st_size )
        copyLength = srcSt.st_size;

    if( freeSpace < copyLength ) {
        messageBox( NULL, LANG("no free space","title"), LANG("no free space","text"), MB_OK );
        return false;
    }

    if( !silently ) {
        struct stat destSt;
        if( 0 == stat( destFilename.c_str(), &destSt ) ) {
            if( !( destSt.st_mode & S_IFDIR ) ) {
                u32 ret = messageBox( NULL, LANG("copy file exists","title"),
                    LANG("copy file exists","text"), MB_YES | MB_NO );
                if( ret != ID_YES )
                    return false;
            } else {
                messageBox( NULL, LANG("copy dest is directory","title"),
                    LANG("copy dest is directory","text"), MB_CANCEL );
                return false;
            }
        }
    }

    std::string tempText = LANG("progress window", "processing copy");
    std::string copyTip = formatString( tempText.c_str(), (char)0x02 );
    progressWnd().setTipText( copyTip );
    progressWnd().show();
    progressWnd().setPercent( 0 );
    stopCopying = false;
    copyingFile = true;

    FILE * wf = fopen( destFilename.c_str(), "wb" );
    FILE * rf = fopen( srcFilename.c_str(), "rb" );

    u8 * copyBuffer = new u8[CONTINUOUS_COPY_SIZE];
    u8 percent = 0;

    dbg_printf("start: %s", datetime().getTimeString().c_str() );

    u32 writeCount = copyLength / CONTINUOUS_COPY_SIZE;
    if( copyLength % CONTINUOUS_COPY_SIZE )
        writeCount++;
    dbg_printf("write count %d\n", writeCount );

    u32 remain = copyLength;

    for( size_t i = 0; i < writeCount; ++i ) {
        if( stopCopying ) {
            copyingFile = false;
            u32 ret = messageBox( &progressWnd(), LANG("stop copying file","title"),
                LANG("stop copying file","text"), MB_YES | MB_NO );

            if( ID_YES == ret ) {
                fclose( rf );
                fclose( wf );
                progressWnd().hide();
                copyingFile = false;
                delete[] copyBuffer;
                return false;
            }
            copyingFile = true;
            stopCopying = false;
        }

        u32 toRead = remain > CONTINUOUS_COPY_SIZE ? CONTINUOUS_COPY_SIZE : remain;
        u32 readed = freadex( copyBuffer, 1, toRead, rf );
        u32 written = fwriteex( copyBuffer, 1, (int)readed, wf );
        if( written != readed ) {
            dbg_printf("err %d\n", errno );
            dbg_printf("COPY FILE ERROR! %d/%d\n", readed, written );
            // todo: judge error types in errno
            fclose( rf );
            fclose( wf );
            progressWnd().hide();
            copyingFile = false;
            delete[] copyBuffer;
            messageBox( NULL, LANG("no free space","title"), LANG("no free space","text"), MB_OK );
            return false;
        }
        remain -= written;
        percent = i * 100 / writeCount;
        progressWnd().setPercent( percent );
    }
    fclose( rf );
    fclose( wf );
    progressWnd().hide();
    copyingFile = false;
    delete[] copyBuffer;

    dbg_printf("finish: %s", datetime().getTimeString().c_str());
    return true;
}

bool renameFile( const std::string & oldName, const std::string & newName )
{
    if( "" == oldName || "" == newName )
        return false;

    struct stat destSt;
    if( 0 == stat( newName.c_str(), &destSt ) ) {
        if( !( destSt.st_mode & S_IFDIR ) ) {
            u32 ret = messageBox( NULL, LANG("copy file exists","title"),
                LANG("copy file exists","text"), MB_YES | MB_NO );
            if( ret != ID_YES )
                return false;
        } else {
            messageBox( NULL, LANG("move dest is directory","title"),
                LANG("move dest is directory","text"), MB_CANCEL );
            return false;
        }
    }

    if( 0 != renamex( oldName.c_str(), newName.c_str() ) ) {
        if( EEXIST == errno || EXDEV == errno ) {
            bool success = copyFile( oldName, newName, true );
            if( success ) {
                unlink( oldName.c_str() );
                return true;
            } else {
                unlink( newName.c_str() );
                return false;
            }
        }
        return false;
    }

    return true;
}

bool deleteFile( const std::string & filename )
{
    if( "" == filename )
        return false;

    struct stat destSt;
    if( 0 != stat( filename.c_str(), &destSt ) ) {
        return false;
    }

    std::string confirmText = LANG("confirm delete file","text");
    std::string showname;
    if( '/' == filename[filename.size()-1] )
        showname = filename.substr( 0, filename.size() - 1 );
    else
        showname = filename;

    size_t slashPos = showname.find_last_of( '/' );
    if( showname.npos != slashPos )
        showname = showname.substr( slashPos + 1 );

    confirmText = formatString( confirmText.c_str(), showname.c_str() );
    u32 result = messageBox( NULL, LANG("confirm delete file","title"), confirmText.c_str(), MB_YES | MB_NO );
    if( result != ID_YES ) {
        return false;
    }

    int ret = unlink( filename.c_str() );
    if( 0 != ret ) {
        if( EPERM == errno ) {
            messageBox( NULL, LANG("do not delete directory","title"), LANG("do not delete directory","text"), MB_OK );
        }
        return false;
    }
    return true;
}

void setSrcFile( const std::string & filename, SRC_FILE_MODE mode )
{
    _srcFilename = filename;
    _srcFileMode = mode;
}

bool copyOrMoveFile( const std::string & destDir )
{
    if( "" == _srcFilename )
        return false;

    const char * pPath = _srcFilename.c_str();
    const char * pName = NULL;
    while( pPath < _srcFilename.c_str() + _srcFilename.size() )
    {
        if( '/' == *pPath++ )
            pName = pPath;
    }

    if( 0 == *pName )
        return false;

    std::string destPath = destDir + pName;
    if( destPath == _srcFilename )
        return false;

    if( SFM_COPY == _srcFileMode )
    {
        u32 copyLength = 0;

        if( gs().romTrim )    {
            std::string extName;
            size_t lastDotPos = _srcFilename.find_last_of( '.' );
            if( _srcFilename.npos != lastDotPos )
                extName = _srcFilename.substr( lastDotPos );
            else
                extName = "";
            for( size_t i = 0; i < extName.size(); ++i )
                extName[i] = tolower( extName[i] );

            if( ".nds" == extName ) {
                DSRomInfo info;
                info.loadDSRomInfo( _srcFilename, false );
                if( info.isDSRom() && !info.isHomebrew() ) {
                    FILE * f = fopen( _srcFilename.c_str(), "rb" );
                    fseek( f, 0x80, SEEK_SET );
                    fread( &copyLength, 1, 4, f );
                    fclose(f);
                    copyLength += 0x88;     // to keep RSA signature
                }
            }
        }

        bool copyOK = copyFile( _srcFilename, destPath, false, copyLength );
        if( copyOK ) {
            _srcFilename = "";
        }
        return copyOK;
    }

    if( SFM_CUT == _srcFileMode )
    {
        bool moveOK = renameFile( _srcFilename, destPath );
        if( moveOK ) {
            _srcFilename = "";
        }
        return moveOK;
    }
    return false;
}

bool getDirSize( const std::string & path, bool includeSubdirs, u64 * dirSize )
{
    if( "" == path )
        return false;

    u64 size = 0;

    std::string dirPath = path;
    if( dirPath[dirPath.size()-1] != '/' )
        dirPath += "/";
    if( dirPath.size() > MAX_FILENAME_LENGTH )
        return false;

    DIR_ITER * dir = NULL;
    dir = diropen(dirPath.c_str());
    if (dir == NULL) {
        //dbg_printf("getDirSize couldn't open dir %s", path.c_str() );
        return false;
    }

    size = 0;
    char filename[256];

    struct stat stat_buf;
    while (dirnext(dir, filename, &stat_buf) == 0) {
        if (strcmp(filename, ".") == 0 || strcmp(filename, "..") == 0) {
            continue;
        }

        //dbg_printf("getDirSize dir entry '%s'", path.c_str());
        if (!(stat_buf.st_mode & S_IFDIR)) {
            //dbg_printf("getDirSize add size %d for '%s'", (int)stat_buf.st_size, path.c_str());
            size += (u64)stat_buf.st_size;
        } else if (includeSubdirs) {
            /* calculate the size recursively */
            u64 subDirSize = 0;
            bool succ = getDirSize( dirPath + filename, includeSubdirs, &subDirSize );
            /* ignore failure in subdirs */
            if( succ ) {
                size += subDirSize;
            }
        }
    }

    dirclose(dir);
    *dirSize = size;
    return true;
}

bool getDiskTotalSpace( std::string diskName, u64 * diskSpace )
{
    if( "" == diskName )
        return false;

    if( diskName[diskName.size()-1] != '/' )
        diskName += "/";

    PARTITION * diskPartition = _FAT_partition_getPartitionFromPath( diskName.c_str() );
    if( NULL == diskPartition ) {
        dbg_printf( "partition %s is not ready\n", diskName.c_str() );
        return false;
    }

    //*diskSpace = diskPartition->totalSize - 1024 * 1024 * 2;
    *diskSpace = (u64)diskPartition->numberOfSectors * (u64)diskPartition->bytesPerSector;
    return true;
}

bool getDiskSpaceInfo( const std::string & diskName, u64 & total, u64 & used, u64 & freeSpace ) {
    if( "" == diskName )
        return false;
    if( !getDiskTotalSpace( diskName, &total ) ) {
        cwl();
        return false;
    }
    if( !getDirSize( diskName, true, &used ) ) {
        cwl();
        return false;
    }

    cwl();
    freeSpace = total - used;
    return true;
}
