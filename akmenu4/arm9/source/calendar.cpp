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









#include "calendar.h"
#include "systemfilenames.h"
#include "windowmanager.h"
#include "inifile.h"
#include "globalsettings.h"
#include "../../share/stringtool.h"
#include "../../share/memtool.h"

using namespace akui;

cCalendar::cCalendar() : cWindow( NULL, "calendar" )
{
    _size = cSize( 0, 0 );
    _position = cPoint( 134, 34 );
    _engine = GE_SUB;

    _showYear = false;
    _showMonth = false;
    _showDay = false;
}

void cCalendar::init()
{
    loadAppearance( SFN_UI_SETTINGS );
}

cWindow& cCalendar::loadAppearance(const std::string& aFileName )
{
    // load day number
    _dayNumbers = createBMP15FromFile( SFN_DAY_NUMBERS );

    // load year number
    _yearNumbers = createBMP15FromFile( SFN_YEAR_NUMBERS );

    CIniFile ini( aFileName );
    _dayPosition.x = ini.GetInt( "calendar day", "x", 134 );
    _dayPosition.y = ini.GetInt( "calendar day", "y", 34 );
    _daySize.x = ini.GetInt( "calendar day", "dw", 16 );
    _daySize.y = ini.GetInt( "calendar day", "dh", 14 );
    _dayHighlightColor = ini.GetInt( "calendar day", "highlightColor", 0xfc00 );
    _showDay = ini.GetInt( "calendar day", "show", _showDay );

    _monthPosition.x = ini.GetInt( "calendar month", "x", 12 );
    _monthPosition.y = ini.GetInt( "calendar month", "y", 28 );
    _showMonth = ini.GetInt( "calendar month", "show", _showMonth );

    _yearPosition.x = ini.GetInt( "calendar year", "x", 52 );
    _yearPosition.y = ini.GetInt( "calendar year", "y", 28 );
    _showYear = ini.GetInt( "calendar year", "show", _showYear );


    return *this;
}

static u8 daysOfMonth[] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
void cCalendar::drawDayNumber( u8 day )
{
    if( day > 31 )
        return;

    u8 weekDayOfDay = (((day - 1) % 7) + weekDayOfFirstDay()) % 7;
    u8 x = weekDayOfDay * _daySize.x + _dayPosition.x;
    u8 y = ((day - 1 + weekDayOfFirstDay()) / 7 * _daySize.y) + _dayPosition.y;
    u8 w = _dayNumbers.width();
    u8 h = _dayNumbers.height() / 10;
    u8 firstNumber = day / 10;
    u8 secondNumber = day % 10;

    if( day == datetime().day() )
        gdi().fillRect( _dayHighlightColor, _dayHighlightColor,
            x-(_daySize.x/2-w), y-(_daySize.y-h)/2, _daySize.x-1, _daySize.y-1, selectedEngine() );

    if( _dayNumbers.valid() ) {
        gdi().maskBlt( _dayNumbers.buffer() + firstNumber * 24, x, y, w, h, selectedEngine() );
        gdi().maskBlt( _dayNumbers.buffer() + secondNumber * 24, x+w, y, w, h, selectedEngine() );
    }
}

u8 cCalendar::weekDayOfFirstDay()
{
    return (datetime().weekday() + 7 - (( datetime().day() - 1) % 7)) % 7;
}


// positoin:  0 - 3
void cCalendar::drawYearNumber( u8 position, u8 n )
{
    if( !_yearNumbers.valid() )
        return;

    u8 x = _yearPosition.x + position * _yearNumbers.width();
    u8 y = _yearPosition.y;

    gdi().maskBlt( _yearNumbers.buffer() + n * 16 * 28 / 2,
        x, y, _yearNumbers.width(), _yearNumbers.height() / 10, selectedEngine() );
}

void cCalendar::drawYearText( u32 year )
{
    u32 factor = 1000;
    for( u8 i = 0; i < 4; ++i )
    {
        u8 number = year / factor;
        year %= factor;
        factor /= 10;
        drawYearNumber( i, number );
    }
}

void cCalendar::drawMonthNumber( u8 position, u8 n )
{
    if( !_yearNumbers.valid() )
        return;

    u8 x = _monthPosition.x + position * _yearNumbers.width();
    u8 y = _monthPosition.y;

    gdi().maskBlt( _yearNumbers.buffer() + + n * 16 * 28 / 2,
        x, y, _yearNumbers.width(), _yearNumbers.height() / 10, selectedEngine() );
}

void cCalendar::drawMonthText( u8 month )
{
    u32 factor = 10;
    for( u8 i = 0; i < 2; ++i )
    {
        u8 number = month / factor;
        month %= factor;
        factor /= 10;
        drawMonthNumber( i, number );
    }
}

void cCalendar::draw()
{
    if( _showDay ) {
        for( u8 i = 1; i <= daysOfMonth[datetime().month()]; ++i )
        {
            drawDayNumber( i );
        }
    }

    if( _showMonth ) {
        drawMonthText( datetime().month() );
    }
    if( _showYear ) {
        drawYearText( datetime().year() );
    }
}
