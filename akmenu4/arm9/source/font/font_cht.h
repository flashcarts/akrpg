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









#include "font.h"


class cFontCht : public cFont
{

public:

    cFontCht( u8 language, const std::string & filename ) : cFont( language, filename)
    {
    }

    bool isWideChar( const char * aWord )
    {
        return ( NULL != _fontWideChar )
            && ( *aWord >= 0xA1 )
            && ( 0 != *(aWord + 1) )
            && ( ((*(aWord + 1) >= 0x40) && (*(aWord + 1) <= 0x7E))
                || ((*(aWord + 1) >= 0xA1) && (*(aWord + 1) <= 0xFE)) ); // big5ºº×Ö
    }

protected:

    u8 * getWideCharPixels( const u8 aWord[2] )
    {
        //return _fontWideChar + ;
        u8 qu = aWord[0] - 0xA1;    //qu ma
        u16 wei = (aWord[1] <= 0x7E) ? aWord[1] - 0x40 : aWord[1] - 0x40 - (0xA1 - 0x7F);    //wei ma
        u32 fontOffset = qu * 157 + wei;
        u8 * fontPos = _fontWideChar + fontOffset * 13;// + 200 ;
        return fontPos;
///////////////////////////////////////////////////////////////////////////////
        //u32 fontOffset = (aWord[0] - 0x81) * 191 + aWord[1] - 0x40;
    }

};
