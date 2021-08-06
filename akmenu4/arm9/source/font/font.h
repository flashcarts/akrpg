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









#ifndef _FONT_H_
#define _FONT_H_

#include <string>
#include "singleton.h"
#include "gdi.h"

class cFont
{
public:

    cFont();

    cFont( u8 language, const std::string & filename );

    virtual ~cFont();

public:

    u8 getCharStartPoint( char c ) { return *(_fontAscii + c*24 + 1) & 0x3f; }

    u8 getCharScreenWidth( char c ) { return *(_fontAscii + c*24 + 3) & 0x3f; }

    u32 getStringScreenWidth( const char * str, size_t len );

    std::string breakLine( const std::string & text, u32 maxLineWidth );

    void drawByte( u16 * mem, s16 x, s16 y, u8 byte, u16 color );

    void drawWideChar( u16 * mem, s16 x, s16 y, const u8 aWord[2], u16 color );

    virtual bool isWideChar( const char * aWord ) = 0;

protected:

    virtual u8 * getWideCharPixels( const u8 aWord[2] ) = 0;

    const u8 * _fontAscii;
    u8 * _fontWideChar;
    u8 _language;
};


#endif//_FONT_H_
