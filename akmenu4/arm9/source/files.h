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









#ifndef _FILES_H_
#define _FILES_H_

#include <string>

bool loadFile( void * buffer, const std::string & filename, size_t offset, size_t & readCount );

// 返回文件大小，-1表示文件不存在
int getFileSize( const std::string & filename );

extern bool stopCopying;
extern bool copyingFile;
bool copyFile( const std::string & srcFilename, const std::string & destFilename, bool silently, size_t copyLength = 0 );

bool renameFile( const std::string & oldName, const std::string & newName );

bool deleteFile( const std::string & filename, const std::string &dispName );

enum SRC_FILE_MODE
{
    SFM_COPY = 0,
    SFM_CUT = 1
};

void setSrcFile( const std::string & filename, SRC_FILE_MODE mode );

bool copyOrMoveFile( const std::string & destDir );

bool getDirSize( const std::string & path, bool includeSubdirs, u64 * dirSize );

bool getDiskSpaceInfo( const std::string & diskName, u64 & total, u64 & used, u64 & freeSpace );

#endif//_FILES_H_
