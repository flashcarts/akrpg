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









#include "zoomingicon.h"
#include "gdi.h"



cZoomingIcon::cZoomingIcon()
{
    _x = 0;
    _y = 0;
    _scale = 1.f;
    _needUpdateBuffer = false;

    _sprite.init( 0 );
    _sprite.setPriority( 1 );
    _sprite.setAlpha( 15 );
    _sprite.show();
}

cZoomingIcon::~cZoomingIcon()
{}

void cZoomingIcon::setScale( float scale )
{
    _scale = scale;
}

void cZoomingIcon::setPosition( u8 x, u8 y )
{
    _x = x;
    _y = y;
}

void cZoomingIcon::setBufferChanged()
{
    _needUpdateBuffer = true;
}

void cZoomingIcon::update()
{
    static float scaleFactor = 0.015;
    if( _visible ) {
        _scale += scaleFactor;
        if( _scale > 1.2 || _scale < 0.9  )
            scaleFactor *= -1;
        _sprite.setScale( _scale, _scale );
        if( !_sprite.visible() )
            _sprite.show();
    } else {
        _scale = 1.0;
        scaleFactor = 0.015;
        _sprite.setScale( 1.f, 1.f );
        if( _sprite.visible() )
            _sprite.hide();
    }

    _sprite.setScale( _scale, _scale );
    _sprite.setPosition( _x, _y );

    if( _needUpdateBuffer ) {
        dmaCopy( _buffer, _sprite.buffer(), 32 * 32 * 2 );
        _needUpdateBuffer = false;
    }
}

void cZoomingIcon::show()
{
    if( !_visible ) {
        _visible = true;
        gdi().maskBlt( _buffer, _x, _y, 32, 32, GE_MAIN ); // sprite only available on main engine
    }
}

void cZoomingIcon::hide()
{
    if( _visible ) {
        _visible = false;
        gdi().maskBlt( _buffer, _x, _y, 32, 32, GE_MAIN ); // sprite only available on main engine
    }
}

void cZoomingIcon::reset()
{
    _scale = 1.f;
}

