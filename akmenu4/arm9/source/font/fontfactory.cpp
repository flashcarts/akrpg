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









#include "globalsettings.h"
#include "fontfactory.h"
#include "font_chs.h"
#include "font_cht.h"
#include "font_jpn.h"
#include "font_en_eu.h"
#include "systemfilenames.h"
#include "stringtool.h"

cFontFactory::cFontFactory() : _font( NULL )
{
}

cFontFactory::~cFontFactory()
{
    if( NULL != _font )
        delete _font;
}


void cFontFactory::makeFont( u8 language )
{
    switch( language )
    {
    case 0://en
        _font = new cFontEnEu( language, "" );
        break;
    case 1://cn
        _font = new cFontChs( language, SFN_LOCAL_FONT_FILE );
        break;
    case 2://zh
        _font = new cFontCht( language, SFN_LOCAL_FONT_FILE );
        break;
    case 3://jp
        _font = new cFontJpn( language, SFN_LOCAL_FONT_FILE );
        break;
    case 4://fr
    case 5://de
    case 6://sp
    case 7://it
    default:
        _font = new cFontEnEu( language, "" );
    }
}

