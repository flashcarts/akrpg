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
#include "button.h"
#include "fontfactory.h"
#include "window.h"

namespace akui
{

cButton::cButton( s32 x, s32 y, u32 w, u32 h, cWindow * parent, const std::string & text )
:cWindow( parent, text ),
_renderDesc(NULL)
{
    _captured = false;
    _state = up;
    _size = cSize( w, h );
    _position = cPoint( x, y );
    _textColor = uiSettings().buttonTextColor; //RGB15(0,0,0) | BIT(15);
    _style = single;
}


cButton::~cButton()
{
    if( _renderDesc )
        delete _renderDesc;
}

void cButton::draw()
{
    if( NULL == _renderDesc )
        _renderDesc = new cButtonDesc();
    const cPoint topLeft = position();
    const cPoint bottomRight = position() + size();

    cRect rect(topLeft, bottomRight);
    _renderDesc->draw(rect, selectedEngine() );
}


cWindow& cButton::loadAppearance(const std::string& aFileName )
{
    _renderDesc = new cButtonDesc();
    _renderDesc->setButton( this );
    _renderDesc->loadData( aFileName );

    return *this;
}

bool cButton::process( const cMessage & msg )
{
    //dbg_printf("cButton::process %s\n", _text.c_str() );
    bool ret = false;
    if(isVisible() && isEnabled())
    {
        if( msg.id() > cMessage::touchMessageStart
            && msg.id() < cMessage::touchMessageEnd )
        {
            ret = processTouchMessage( (cTouchMessage &)msg );
        }
    }
    return ret;
}

bool cButton::processTouchMessage( const cTouchMessage & msg )
{
    bool ret = false;
    if( msg.id() == cMessage::touchUp ) {
        //cPoint clickedPt( msg.touchPt.x, inputs.touchPt.y );
        cRect myRect( _position.x, _position.y, _position.x + _size.x, _position.y + _size.y );
        if( _captured ) {
            if( myRect.surrounds( msg.position() ) ) {
                onClicked();
                clicked();
            }
            else {
                onReleased();
            }
            _captured = false;
            ret = true;
        }
        _state = up;
    }
    if( msg.id() == cMessage::touchDown ) {
        //cPoint clickedPt( inputs.touchPt.x, inputs.touchPt.y );
        cRect myRect( _position.x, _position.y, _position.x + _size.x, _position.y + _size.y );
        if( myRect.surrounds( msg.position() ) ) {
            onPressed();
            pressed();
            _captured = true;
            _state = down;
            ret = true;
        }
    }
    //    if( inputs.touchDown ) {
    //        cPoint clickedPt( inputs.touchPt.px, inputs.touchPt.py );
    //        cRect myRect( _position.x, _position.y, _position.x + _size.x, _position.y + _size.y );
    ////        dbg_printf("%d %d %d %d, %d %d\n",
    ////            _position.x, _position.y, _position.x + _size.x, _position.y + _size.y, clickedPt.x, clickedPt.y );
    //        if( myRect.surrounds( clickedPt ) ) {
    //            //dbg_printf("in!\n");
    //            _state = down;
    //        } else {
    //            _state = up;
    //        }
    //    } else {
    //        _state = up;
    return ret;
}


void cButton::onPressed()
{
}

void cButton::onReleased()
{
}

void cButton::onClicked()
{
}



///////////////////////////////// desc ////////////////
cButtonDesc::cButtonDesc()
{
    _button = NULL;
    _textColor = RGB15(31,31,31);
}

cButtonDesc::~cButtonDesc()
{
    //if( NULL != _background )
    //    destroyBMP15( _background );
}

void cButtonDesc::draw( const cRect & area, GRAPHICS_ENGINE engine ) const
{
    const u32 * pBuffer = NULL;
    u32 height = 0;
    if( _background.valid() ) {
        pBuffer = _background.buffer();
        height = _background.height();
        if( _button->style() != cButton::single ) {
            height /= 2;
        if( cButton::down == _button->state() )
            pBuffer += _background.width() * _background.height()/4;
        }
    }

    if( NULL != pBuffer ) {
        gdi().maskBlt( pBuffer, area.position().x, area.position().y,
            _background.width(), height, _button->selectedEngine() );
    }

    // �����������
    u32 textPixels = font().getStringScreenWidth( _button->text().c_str(), _button->text().size() );
    u32 textX = area.position().x + (( area.size().x - textPixels ) >> 1);
    u32 textY = area.position().y + (( area.size().y - SYSTEM_FONT_HEIGHT ) >> 1 );

    if( cButton::down == _button->state() ) {
        textX++;
        textY++;
    }
    gdi().setPenColor( _button->textColor(), _button->selectedEngine() );
    gdi().textOutRect( textX, textY, area.size().x, area.size().y,
        _button->text().c_str(), _button->selectedEngine() );
}

void cButtonDesc::loadData( const std::string & filename )
{
    int width = 40;
    int height = 16;
    if( _button ) {
        width = _button->size().x;
        height = _button->size().y;
    }

    if( !_background.valid() ) {
        _background = createBMP15FromFile( filename );
        if( _background.valid() ) {
            if( _button->style() == cButton::single )
                height = _background.height();
            else
                height = _background.height() / 2;
            _button->setSize( cSize( _background.width(), height ) );
        }
    }
}

}
