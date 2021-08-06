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









#include "bigclock.h"
#include "../../share/stringtool.h"
#include "systemfilenames.h"
#include "windowmanager.h"
#include "inifile.h"
#include "globalsettings.h"

using namespace akui;

cBigClock::cBigClock() : cWindow( NULL, "big clock" )
{
    _size = cSize( 0, 0 );
    _position = cPoint( 8, 80 );
    _engine = GE_SUB;
    _show = false;
}

void cBigClock::init()
{
    loadAppearance( SFN_UI_SETTINGS );
}

cWindow& cBigClock::loadAppearance(const std::string& aFileName )
{
    CIniFile ini( aFileName );
    _position.x = ini.GetInt( "big clock", "x", 8 );
    _position.y = ini.GetInt( "big clock", "y", 80 );
    _show = ini.GetInt( "big clock", "show", _show );

    _numbers = createBMP15FromFile( SFN_CLOCK_NUMBERS );
    _colon = createBMP15FromFile( SFN_CLOCK_COLON );


    return *this;
}

void cBigClock::drawNumber( u8 id, u8 number )
{
    if( number > 10 )
        return;

    u8 x = _position.x + id * (_numbers.width() + 2);
    if( id > 2 ) {// minute number
        x -= 8;
    }
    if( _numbers.valid() ) {
        gdi().maskBlt( _numbers.buffer() + number * 24 * 36 / 2,
            x, _position.y, _numbers.width(), _numbers.height() / 10, selectedEngine() );
    }
}

void cBigClock::drawColon()
{
    u8 x = _position.x + 2 * _numbers.width();
    if( _colon.valid() ) {
        gdi().maskBlt( _colon.buffer(),
            x, _position.y, _colon.width(), _colon.height(), selectedEngine() );
    }
}

void cBigClock::draw()
{
    if( !_show )
        return;

    u8 number1 = datetime().hours() / 10;
    u8 number2 = datetime().hours() % 10;
    u8 number3 = datetime().minutes() / 10;
    u8 number4 = datetime().minutes() % 10;
    //u8 number5 = datetime().seconds() / 10;
    //u8 number6 = datetime().seconds() % 10;

    drawNumber( 0, number1 );
    drawNumber( 1, number2 );
    drawNumber( 3, number3 );
    drawNumber( 4, number4 );
    if( _colonShow)
        drawColon();
}


void cBigClock::blinkColon()
{
    _colonShow ^= 1;
}
