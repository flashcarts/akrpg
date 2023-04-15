/*
 * 2008/03/10
 * @by bliss
 *
 * Copyright (C) 2008 Bliss. All rights reserved.
 * This code following BSD LICENSE.
 *
 */


#include <string.h>
#include <fat.h>
#include "gdi.h"
#include "fileicons.h"
#include "systemfilenames.h"
#include "globalsettings.h"
#include "bmp15.h"

cFileIcons::cFileIcons() 
{
}

cFileIcons::~cFileIcons() 
{
}

void cFileIcons::_update(const char *extName, std::string & fullPath ) {
    size_t i = 0;
    for( i = 0; i < _file_icons.size(); ++i )
    {
        if( strncasecmp( _file_icons[i].extname, extName, 6 ) == 0 ) {
			// already exist
			return;
        }
    }
	FILE_ICONS fi;
	strncpy(fi.extname, extName , 6);
	fi.image = createBMP15FromFile(fullPath);
	if ( fi.image.height() <= 32  && fi.image.width() <= 32 ) {
		_file_icons.push_back(fi);
	}
	else {
		u32 *buf = fi.image.buffer();
		delete buf;
	}
}

void cFileIcons::_load(const char *directory )
{
    DIR_ITER * dir = diropen( directory );
	if ( dir != NULL ) {
		struct stat st;
		char filename[256];
		char longFilename[512];
		while(!dirnextl(dir, filename,longFilename, &st)) {
			if ( st.st_mode & S_IFDIR ) continue;

			if ( longFilename[0] == 0 ) {
				strcpy( longFilename, filename);
			}
			strupr(longFilename);

			char *extName = strrchr(longFilename,'.');
			if ( extName && !strcmp(extName, ".BMP") ) {
				*extName = '\0';
				std::string fullPath = directory;
				fullPath += filename;
				_update(longFilename, fullPath);
			}

		} // end of while
	}
}

void cFileIcons::loadIcons()
{
	std::string dirName = SFN_UI_ICONS_DIRECTORY;
	_load( dirName.c_str() );
	_load( SFN_ICONS_DIRECTORY );
}

bool cFileIcons::isExist(const char *extName) {
    size_t i = 0;
	
	extName = strchr( extName, '.') ;
	if ( !extName  || *(extName +1) == 0) return false;

    for( i = 0; i < _file_icons.size(); ++i )
    {
        if( !strncasecmp( _file_icons[i].extname, extName + 1, 6 ) ) {
			_index = i;
			return true;
        }
    }

	return false;
}

int cFileIcons::getIndex(void) {
	return _index;
}

const char * cFileIcons::getExtName(int idx) {
	if( idx < (int )_file_icons.size() ) {
		return _file_icons[idx].extname;
	}

	return NULL;
}

void cFileIcons::drawIcon(int idx, u8 x, u8 y, GRAPHICS_ENGINE engine) {
	if( idx < (int )_file_icons.size() ) {
		// draw Icon
		if ( _file_icons[idx].image.valid() ) {
			gdi().maskBlt( _file_icons[idx].image.buffer(), x, y,
				_file_icons[idx].image.width(), _file_icons[idx].image.height(), engine );
		}
	}
}

void cFileIcons::drawIconMem(int idx, void *mem) {
	if( idx < (int )_file_icons.size() ) {
		// draw Icon to memory
		if ( _file_icons[idx].image.valid() ) {
			//memcpy ( mem, _file_icons[idx].image.buffer(), sizeof ( mem ) );
			maskBlt( _file_icons[idx].image.buffer(), 
				_file_icons[idx].image.width(), _file_icons[idx].image.height(), mem);
		}
	}
}

void cFileIcons::maskBlt( const void * src, u16 destW, u16 destH, void *mem)
{
    u16 * pSrc = (u16 *)src;
    u16 * pDest = (u16 *)mem;

    u16 pitch = (destW + (destW & 1));
    u16 destInc = 32 - pitch;
    u16 halfPitch = pitch >> 1;

    for( u32 i = 0; i < destH; ++i ) {
        for( u32 j = 0; j < halfPitch; ++j ) {
            if( ((*(u32 *)pSrc) & 0x80008000) == 0x80008000 ) {
                *(u32 *)pDest = *(u32 *)pSrc;
                pSrc += 2; pDest += 2;
            }
            else {
                if( *pSrc & 0x8000 )
                    *pDest = *pSrc;
                pSrc++; pDest++;
                if( *pSrc & 0x8000 )
                    *pDest = *pSrc;
                pSrc++; pDest++;
            }
        }
        pDest += destInc;
    }
}


