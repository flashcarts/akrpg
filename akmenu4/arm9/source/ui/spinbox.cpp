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
#include "spinbox.h"
#include "fontfactory.h"

//#include "windowmanager.h"

namespace akui
{

cSpinBox::cSpinBox( s32 x, s32 y, u32 w, u32 h, cWindow * parent, const std::string & text )
    :cForm( x, y, w, h, parent, text ),
    _prevButton( 0,0,0,0,this,""),
    _nextButton( 0,0,0,0,this,"" ),
    _itemText(0,0,0,0, this, "spinbox")
{
    _normalColor = uiSettings().spinBoxNormalColor; //RGB15( 0, 0, 31 );
    _focusedColor = uiSettings().spinBoxFocusColor; //RGB15( 0, 31, 0 );
    _frameColor = uiSettings().spinBoxFrameColor;
    _itemText.setTextColor( uiSettings().spinBoxTextColor );

    _prevButton.pressed.connect( this, &cSpinBox::selectPrev );
    _prevButton.pressed.connect( this, &cSpinBox::onCmponentClicked );
    _nextButton.pressed.connect( this, &cSpinBox::selectNext );
    _nextButton.pressed.connect( this, &cSpinBox::onCmponentClicked );

    addChildWindow( &_prevButton );
    addChildWindow( &_nextButton );
    addChildWindow( &_itemText );

    _itemText.setTextColor( RGB15(31,31,31 ) );

    u8 cx = 0;
    _prevButton.setSize( cSize(18, 18) );
    _prevButton.setRelativePosition( cPoint(cx, 0) );

    cx = 0 + _prevButton.windowRectangle().width();
    _itemText.setRelativePosition( cPoint(cx, 0) );
    _itemText.setSize( cSize(w-18*2, 18 ) );

    cx = windowRectangle().width() - _nextButton.windowRectangle().width();
    _nextButton.setSize( cSize(18, 18) );
    _nextButton.setRelativePosition( cPoint(cx, 0 ) );

    selectItem( 0 );
}

cSpinBox::~cSpinBox()
{

}

void cSpinBox::selectItem( u32 id )
{
    // danger !!!! it may cause system halt when cSpinBox destruct
    //windowManager().setFocusedWindow( this );
    _selectedItemId = id;
    if( _selectedItemId >= _items.size() )
        return;

    _itemText.setText( _items[_selectedItemId] );

    //s32 textWidth = _items[_selectedItemId].length() * 6;
    //s32 textHeight = 12;
    //_itemText.setRelativePosition( cPoint((_size.x - textWidth) >> 1, (_size.y - textHeight) >> 1) );

    arrangeButton();
    arrangeText();
    arrangeChildren();
}

void cSpinBox::selectNext()
{
    if( _items.size() - 1 == _selectedItemId )
        return;

    selectItem( _selectedItemId + 1 );
}

void cSpinBox::selectPrev()
{
    if( 0 == _selectedItemId )
        return;

    selectItem( _selectedItemId - 1 );
}

void cSpinBox::insertItem( const std::string & item, u32 position )
{
    if( position > _items.size() )
        return;

    _items.insert( _items.begin() + position, item );
}


void cSpinBox::removeItem( u32 position )
{
    if( position > _items.size() - 1 )
        return;

    _items.erase( _items.begin() + position );
}

void cSpinBox::setTextColor( COLOR color )
{

}

void cSpinBox::draw()
{
    // draw bar
    u16 barColor = _normalColor;
    if( isFocused() ) {
        barColor = _focusedColor;
        _itemText.setTextColor( uiSettings().spinBoxTextHighLightColor );
    } else {
        _itemText.setTextColor( uiSettings().spinBoxTextColor );
    }

    u8 bodyX1 = _prevButton.position().x + _prevButton.size().x;
    u8 fillWidth = windowRectangle().size().x - _nextButton.size().x - _prevButton.size().x;
    gdi().setPenColor( barColor, _engine  );
    gdi().fillRect( barColor, barColor, bodyX1, _position.y, fillWidth, _prevButton.size().y, selectedEngine() );
    gdi().setPenColor( _frameColor, _engine  );
    gdi().frameRect( bodyX1, _position.y, fillWidth, _prevButton.size().y, selectedEngine() );

    // draw previous button
    _prevButton.draw();

    // draw text
    _itemText.draw();


    // draw next button
    _nextButton.draw();
}

cWindow& cSpinBox::loadAppearance(const std::string& aFileName )
{
    _prevButton.loadAppearance( SFN_SPINBUTTON_L );
    _nextButton.loadAppearance( SFN_SPINBUTTON_R );

    return *this;
}

void cSpinBox::onGainedFocus()
{

}

void cSpinBox::onResize()
{
    dbg_printf("spin box on resize\n");
    arrangeButton();
    arrangeText();
    arrangeChildren();
}

void cSpinBox::onMove()
{
    arrangeButton();
    arrangeText();
    arrangeChildren();
}

void cSpinBox::arrangeText()
{
    s32 textWidth = font().getStringScreenWidth( _items[_selectedItemId].c_str(), _items[_selectedItemId].length() );
    s32 textHeight = gs().fontHeight;

    _itemText.setRelativePosition( cPoint((_size.x - textWidth) >> 1, (_size.y - textHeight) >> 1) );
}

void cSpinBox::arrangeButton()
{
    u8 x = 0;
    _prevButton.setSize( cSize(_prevButton.size().x, _size.y) );
    _prevButton.setRelativePosition( cPoint(x, (_size.y - _prevButton.size().y) / 2) );

    x = _prevButton.size().x;
    _itemText.setRelativePosition( cPoint(x, (_size.y - gs().fontHeight) / 2) );

    x = size().x - _nextButton.size().x;
    _nextButton.setSize( cSize(_nextButton.size().x, _size.y) );
    _nextButton.setRelativePosition( cPoint(x, (_size.y - _nextButton.size().y) ) );
}

void cSpinBox::onCmponentClicked()
{
    componentClicked( this );
}


}

