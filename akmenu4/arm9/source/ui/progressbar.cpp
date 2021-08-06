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









#include "progressbar.h"


namespace akui
{


cProgressBar::cProgressBar( s32 x, s32 y, u32 w, u32 h, cWindow * parent, const std::string & text )
:cWindow( parent, text )
{
    setSize( cSize( w, h ) );
    setPosition( cPoint( x, y ) );
}

cProgressBar::~cProgressBar()
{

}

void cProgressBar::draw()
{
    //draw frame

    //draw left

    //draw right

    //draw bar body
    u8 width = _percent * size().x / 100;
    if( _barBmp.valid() ) {
        gdi().maskBlt( _barBmp.buffer(),
            _barBmp.width(), _barBmp.height(),
            _position.x, _position.y, width, _barBmp.height(), _engine );
    } else {
        u16 color1 = 0xfc00;
        u16 color2 = 0x800f;
        for( u8 i = 0; i < size().y; ++i ) {
            if( i & 1 )
                gdi().fillRect(
                    color1, color2, position().x, position().y + i, width, 1, _engine );
            else
                gdi().fillRect(
                    color2, color1, position().x, position().y + i, width, 1, _engine );
        }
    }

}

cWindow& cProgressBar::loadAppearance(const std::string& aFileName )
{
    _barBmp = createBMP15FromFile( aFileName );
    setSize( cSize(_barBmp.width(), _barBmp.height() ) );
    return *this;
}

void cProgressBar::setPercent( u8 percent )
{
    _percent = percent;
}


}

