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
#include "popmenu.h"
//#include "windowmanager.h"

namespace akui
{

cPopMenu::cPopMenu( s32 x, s32 y, u32 w, u32 h, cWindow * parent, const std::string & text )
: cWindow( parent, text )
{
    _size = cSize( w, h );
    _position = cPoint( x, y );

    _selectedItemIndex = 0;
    _itemHeight = 0;

    _textColor = uiSettings().popMenuTextColor;
    _textHighLightColor = uiSettings().popMenuTextHighLightColor;
    _barColor = uiSettings().popMenuBarColor;

    _renderDesc = new cBitmapDesc();
    _renderDesc->setBltMode( BM_MASKBLT );
}

cPopMenu::~cPopMenu()
{
    if( NULL != _renderDesc )
        delete _renderDesc;
}

void cPopMenu::popup()
{
    show();
    windowManager().setPopUpWindow( this );
    return;
}

void cPopMenu::addItem( size_t index, const std::string & itemText )
{
    if( index > _items.size() )
        index = _items.size();
    _items.insert( _items.begin() + index, itemText );
}

void cPopMenu::removeItem( size_t index )
{
    if( index > _items.size() - 1 )
        index = _items.size() - 1;
    _items.erase( _items.begin() + index );
}

size_t cPopMenu::itemCount()
{
    return _items.size();
}

void cPopMenu::clearItem()
{
    _items.clear();
}

void cPopMenu::draw()
{
    _renderDesc->draw( windowRectangle(), selectedEngine() );
    drawItems();
}

void cPopMenu::drawItems()
{
    // 循环绘制item文字，遇见 selected 文字就先绘制选择条
    for( size_t i = 0; i < _items.size(); ++i )
    {
        s16 itemX = _position.x + _itemTopLeftPoint.x;
        s16 itemY = _position.y + i * _itemHeight + _itemTopLeftPoint.y;
        if( _selectedItemIndex == (s16)i ) {
            s16 barX = _position.x + 2;
            s16 barY = itemY - 2;
            gdi().setPenColor( _barColor, _engine );
            gdi().fillRect( _barColor, _barColor, barX, barY, _size.x - 4, _itemHeight, _engine );
            gdi().setPenColor( _textHighLightColor, _engine );
        }
        else {
            gdi().setPenColor( _textColor, _engine );
        }
        gdi().textOut( itemX, itemY, _items[i].c_str(), _engine );
    }
}

bool cPopMenu::process( const cMessage & msg )
{
    bool ret = false;
    if(isVisible() && isEnabled())
    {
        //ret = cForm::process( msg );
        if( msg.id() > cMessage::keyMessageStart
            && msg.id() < cMessage::keyMessageEnd )
        {
            ret = processKeyMessage( (cKeyMessage &)msg );
        }
        else if( msg.id() > cMessage::touchMessageStart
            && msg.id() < cMessage::touchMessageEnd )
        {
            ret = processTouchMessage( (cTouchMessage &)msg );
        }
    }

    // cPopMenu process all KEY messages while it is showing
    // derived classes can override this feature
    return ret;
}

bool cPopMenu::processKeyMessage( const cKeyMessage & msg )
{
    bool ret = false;
    switch( msg.keyCode() )
    {
    case cKeyMessage::UI_KEY_DOWN :
        _selectedItemIndex += 1;
        if( _selectedItemIndex > (s16)_items.size() - 1 )
            _selectedItemIndex = 0;
        ret = true;
        break;
    case cKeyMessage::UI_KEY_UP:
        _selectedItemIndex -= 1;
        if( _selectedItemIndex < 0 )
            _selectedItemIndex = (s16)_items.size() - 1;
        ret = true;
        break;
    case cKeyMessage::UI_KEY_A:
        // do something by ( _selectedItemIndex )
        hide();
        itemClicked( _selectedItemIndex );
        ret = true;
        break;
    case cKeyMessage::UI_KEY_B:
        hide();
        ret = true;
        break;
    };

    return ret;
}

bool cPopMenu::processTouchMessage( const cTouchMessage & msg )
{
    bool ret = false;
    if( msg.id() == cMessage::touchUp ) {
        if( windowBelow( cPoint( msg.position().x, msg.position().y ) ) ) {
            hide();
            itemClicked( _selectedItemIndex );
        } else
            hide();

        ret = true;
    }
    if( msg.id() == cMessage::touchMove || msg.id() == cMessage::touchDown ) {
        const INPUT & input = getInput();
        size_t item = itemBelowPoint( cPoint( input.touchPt.px, input.touchPt.py ) );
        if( (size_t)-1 != item )
            _selectedItemIndex = item;
        ret = true;
    }

    return ret;
}

u32 cPopMenu::itemBelowPoint( const cPoint & pt )
{
    cRect rect(position(), position() + size());

    if( rect.surrounds( pt ) ) {
        u32 item = (pt.y - _position.y - _itemTopLeftPoint.y  ) / _itemHeight;
        if( item > _items.size() - 1)
            item = _items.size() - 1;
        return item;
    }
    return (u32)-1;
}

void cPopMenu::onShow()
{
    _selectedItemIndex = 0;
}

} // namespace akui
