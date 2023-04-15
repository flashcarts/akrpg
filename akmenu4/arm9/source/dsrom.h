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









#ifndef _DSROM_H_
#define _DSROM_H_

#include <nds.h>
#include "savemngr.h"
#include "gdi.h"
#include "folder_banner_bin.h"
#include "unknown_banner_bin.h"

class DSRomInfo
{
public:
    DSRomInfo() : _isDSRom(false), _isHomebrew(false)
    {
        //memcpy( &_banner, unknown_banner_bin, unknown_banner_bin_size );
        memset( &_banner, 0, sizeof(_banner) );
        memset( &_saveInfo, 0, sizeof(_saveInfo) );
    }

public:

    bool loadDSRomInfo( const std::string & filename, bool loadBanner );

    void drawDSRomIcon( u8 x, u8 y, GRAPHICS_ENGINE engine );

    void drawDSRomIconMem( void * mem );

    tNDSBanner & banner() { return _banner; }

    SAVE_INFO & saveInfo() { return _saveInfo; }

    bool isDSRom() { return _isDSRom; }

    bool isHomebrew() { return _isHomebrew; }

    DSRomInfo & operator =( const DSRomInfo & src );

public:

    tNDSBanner _banner;
    SAVE_INFO _saveInfo;
    bool _isDSRom;
    bool _isHomebrew;
	int _fileIconIdx;
};



#endif//_DSROM_H_

