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









#include "diskicon.h"
#include "bmp15.h"
#include "inifile.h"
#include "systemfilenames.h"
#include "../../share/memtool.h"
#include "../../share/timetool.h"
#include "globalsettings.h"

using namespace akui;



cDiskIcon::cDiskIcon() : cWindow( NULL, "diskicon" )
{
    _size = cSize( 0, 0 );
    _position = cPoint( 0, 0 );
    _engine = GE_MAIN;
    _lightTime = 0.f;
    _icon.init( 1 );
    _icon.setPosition( 226, 174 );
    _icon.setPriority( 0 );
    _icon.setBufferOffset( 32 );
    _icon.show();

    fillMemory( _icon.buffer(), 32 * 32 * 2, 0x00000000 );
}

void cDiskIcon::draw()
{
    // do nothing
}

cWindow & cDiskIcon::loadAppearance(const std::string& aFileName )
{

    CIniFile ini( SFN_UI_SETTINGS );

    u16 x = ini.GetInt( "disk icon", "x", 238 );
    u16 y = ini.GetInt( "disk icon", "y", 172 );
    _icon.setPosition( x, y );

    cBMP15 icon = createBMP15FromFile( aFileName );

    for( u8 i = 0; i < icon.height(); ++i ) {
        for( u8 j = 0; j < icon.width(); ++j ) {
            ((u16 *)_icon.buffer())[i*32+j] = ((u16 *)icon.buffer())[i * icon.width() + j];
        }
    }
    dbg_printf("cDiskIcon::loadAppearance ok %d\n", icon.valid() );
    return *this;
}

void cDiskIcon::blink( float lightTime )
{
    _lightTime = lightTime;
    if( _icon.visible() )
        _icon.hide();
    else
        _icon.show();
}

float cDiskIcon::lightTime()
{
    return _lightTime;
}

void cDiskIcon::turnOn()
{
    _icon.show();
}

void cDiskIcon::turnOff()
{
    _icon.hide();
}
