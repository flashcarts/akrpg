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









#include "ui.h"
#include "formdesc.h"

//#include "globalsettings.h"


namespace akui
{

// ±ß¿òÑÕÉ«£ºb5c71f
//    23, 25, 4
// ¿òÄÚ±³¾°É«£ºeeebae
// 30, 29, 22

cFormDesc::cFormDesc()
{
    _bodyColor = uiSettings().formBodyColor; //RGB15(30,29,22);
    _frameColor = uiSettings().formFrameColor;//RGB15(23,25,4);
}

cFormDesc::~cFormDesc()
{

}

void cFormDesc::draw( const cRect & area, GRAPHICS_ENGINE engine ) const
{
    if( _topleft.valid() ) {
        gdi().maskBlt(
            _topleft.buffer(), area.position().x, area.position().y, _topleft.width(), _topleft.height(), engine );
    }

    if( _middle.valid() ) {
        for( u32 i = 0; i < _middle.height(); ++i )
        {
            COLOR lineColor = _middle.buffer()[i] & 0xFFFF;
            gdi().setPenColor( lineColor, engine );
            gdi().fillRect( lineColor, lineColor, area.position().x + _topleft.width(),
                area.position().y + i,
                area.size().x - _topleft.width() - _topright.width(),
                1, engine );
        }
    }

    if( _topright.valid() ) {
        gdi().maskBlt( _topright.buffer(),
            area.position().x + area.size().x - _topright.width(), area.position().y,
            _topright.width(), _topright.height(), engine );
    }

    if( _titleText != "" ) {
        gdi().setPenColor( uiSettings().formTitleTextColor, engine );
        gdi().textOut( area.position().x + 8,
            area.position().y + (((_topleft.height() - gs().fontHeight)) >> 1) + 1,
            _titleText.c_str(), engine );
    }

    gdi().setPenColor( _bodyColor, engine );
    gdi().fillRect( _bodyColor, _bodyColor, area.topLeft().x, area.topLeft().y + _topleft.height(),
        area.width(), area.height() - _topleft.height(), engine );

    gdi().setPenColor( _frameColor, engine );
    gdi().frameRect( area.topLeft().x, area.topLeft().y + _topleft.height(),
        area.width(), area.height() - _topleft.height(), engine );

}

void cFormDesc::loadData( const std::string & topleftBmpFile,
                         const std::string & toprightBmpFile,
                         const std::string & middleBmpFile )
{
    _topleft = createBMP15FromFile( topleftBmpFile );
    _topright = createBMP15FromFile( toprightBmpFile );
    _middle = createBMP15FromFile( middleBmpFile );
}

void cFormDesc::setTitleText( const std::string & text )
{
    _titleText = text;
}

}
