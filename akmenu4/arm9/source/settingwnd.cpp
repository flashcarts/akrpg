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









#include "settingwnd.h"
#include "systemfilenames.h"
#include "msgbox.h"
#include "windowmanager.h"
#include "globalsettings.h"
#include "uisettings.h"
#include "language.h"

using namespace akui;

cSettingWnd::cSettingWnd( s32 x, s32 y, u32 w, u32 h, cWindow * parent, const std::string & text )
: cForm( x, y, w, h, parent, text ),
_buttonOK(0,0,46,18,this,"\x01 OK"),
_buttonCancel(0,0,48,18,this,"\x02 Cancel")
{
    s16 buttonY = size().y - _buttonCancel.size().y - 4;

    _buttonCancel.setStyle( cButton::press );
    _buttonCancel.setText( "\x02 " + LANG( "setting window", "cancel" ) );
    _buttonCancel.setTextColor( uis().buttonTextColor );
    _buttonCancel.loadAppearance( SFN_BUTTON3 );
    _buttonCancel.clicked.connect( this, &cSettingWnd::onCancel );
    addChildWindow( &_buttonCancel );

    _buttonOK.setStyle( cButton::press );
    _buttonOK.setText( "\x01 " + LANG( "setting window", "ok" ) );
    _buttonOK.setTextColor( uis().buttonTextColor );
    _buttonOK.loadAppearance( SFN_BUTTON3 );
    _buttonOK.clicked.connect( this, &cSettingWnd::onOK );
    addChildWindow( &_buttonOK );

    s16 nextButtonX = size().x;
    s16 buttonPitch = _buttonCancel.size().x + 8;
    nextButtonX -= buttonPitch;
    _buttonCancel.setRelativePosition( cPoint(nextButtonX, buttonY) );

    buttonPitch = _buttonOK.size().x + 8;
    nextButtonX -= buttonPitch;
    _buttonOK.setRelativePosition( cPoint(nextButtonX, buttonY) );

    loadAppearance( "" );
    arrangeChildren();
}

cSettingWnd::~cSettingWnd()
{
    for( size_t i = 0; i < _labels.size(); ++i ) {
        delete _labels[i];
    }

    for( size_t i = 0; i < _items.size(); ++i ) {
        delete _items[i];
    }
}

void cSettingWnd::draw()
{
    _renderDesc.draw( windowRectangle(), _engine );
    cForm::draw();
}

bool cSettingWnd::process( const akui::cMessage & msg )
{
    bool ret = false;

    ret = cForm::process( msg );

    if( !ret ) {
        if( msg.id() > cMessage::keyMessageStart && msg.id()
            < cMessage::keyMessageEnd )
        {
            ret = processKeyMessage( (cKeyMessage &)msg );
        }

        //if( msg.id() > cMessage::touchMessageStart && msg.id()
        //    < cMessage::touchMessageEnd )
        //{
        //    ret = processTouchMessage( (cTouchMessage &)msg );
        //}
    }
    return ret;
}

void cSettingWnd::onOK()
{
    u32 ret = messageBox( this,
        LANG("setting window","confirm"),
        LANG("setting window","confirm text"), MB_OK | MB_CANCEL );

    if( ID_OK != ret )
        return;

    _modalRet = 1;
}

void cSettingWnd::onCancel()
{
    _modalRet = 0;
}


bool cSettingWnd::processKeyMessage( const cKeyMessage & msg )
{
    bool ret = false;
    if( msg.id() == cMessage::keyDown )
    {
        switch( msg.keyCode() )
        {
        case cKeyMessage::UI_KEY_DOWN:
            onUIKeyDOWN();
            ret = true;
            break;
        case cKeyMessage::UI_KEY_UP:
            onUIKeyUP();
            ret = true;
            break;
        case cKeyMessage::UI_KEY_LEFT:
            onUIKeyLEFT();
            ret = true;
            break;
        case cKeyMessage::UI_KEY_RIGHT:
            onUIKeyRIGHT();
            ret = true;
            break;
        case cKeyMessage::UI_KEY_A:
            onOK();
            ret = true;
            break;
        case cKeyMessage::UI_KEY_B:
            onCancel();
            ret = true;
            break;
        default:
            {}
        };
    }

    return ret;
}

cWindow& cSettingWnd::loadAppearance(const std::string& aFileName )
{
    _renderDesc.loadData( SFN_FORM_TITLE_L, SFN_FORM_TITLE_R, SFN_FORM_TITLE_M );
    _renderDesc.setTitleText( _text );
    return *this;
}

void cSettingWnd::addSettingItem( const std::string & text,
                                    const std::vector< std::string > & itemTexts,
                                    size_t defaultValue )
{
    if( 0 == itemTexts.size() )
        return;
    if( defaultValue >= itemTexts.size() )
        defaultValue = 0;

    if( _maxLabelLength < text.length() )
        _maxLabelLength = text.length();

    // recompute window size and position
    setSize( cSize( _size.x, (_items.size()+1) * 20 + 80 ) );
    _position.x = (SCREEN_WIDTH - _size.x) / 2;
    _position.y = (SCREEN_HEIGHT - _size.y) / 2;

    // insert label, item and set their position
    s32 itemY = _items.size() * 22 + 32;
    s32 itemX = 8;

    cSpinBox * item = new cSpinBox(0,0,108,18,this,"spin");
    for( size_t i = 0; i < itemTexts.size(); ++i ) {
        item->insertItem( itemTexts[i], i );
    }

    CIniFile ini( SFN_UI_SETTINGS );
    u32 spinBoxWidth = ini.GetInt( "setting window", "spinBoxWidth", 108 );
    item->loadAppearance( "" );
    item->setSize( cSize(spinBoxWidth, 18) );
    //item->setTextColor( RGB15(20,14,0) );
    item->setRelativePosition( cPoint( _size.x - spinBoxWidth - 4, itemY ) );
    _items.push_back( item );
    addChildWindow( item );
    item->selectItem( defaultValue );
    item->componentClicked.connect( this, &cSettingWnd::onItemClicked );

    cStaticText * label = new cStaticText( 0, 0, _maxLabelLength * 6, gs().fontHeight, this, text );
    itemY += (item->windowRectangle().height() - label->windowRectangle().height()) / 2;
    label->setRelativePosition( cPoint( itemX, itemY ) );
    label->setTextColor( uis().formTextColor );
    label->setSize( cSize( _size.x / 2 + 8, 12 ) );
    _labels.push_back( label );
    addChildWindow( label );

    // recompute button position
    s16 buttonY = size().y - _buttonCancel.size().y - 4;

    _buttonCancel.setRelativePosition( cPoint(_buttonCancel.relativePosition().x, buttonY) );
    _buttonOK.setRelativePosition( cPoint(_buttonOK.relativePosition().x, buttonY) );

    arrangeChildren();
}

void cSettingWnd::onShow()
{
    if( _items.size() )
        windowManager().setFocusedWindow( _items[0] );
}


void cSettingWnd::onUIKeyUP()
{
    size_t focusItem = focusedItemId();

    if( 0 == focusItem )
        focusItem = _items.size() - 1;
    else
        --focusItem;

    windowManager().setFocusedWindow( _items[focusItem] );
}

void cSettingWnd::onUIKeyDOWN()
{
    size_t focusItem = focusedItemId();

    if( _items.size() - 1 == focusItem )
        focusItem = 0;
    else
        ++focusItem;

    windowManager().setFocusedWindow( _items[focusItem] );
}

void cSettingWnd::onUIKeyLEFT()
{
    size_t focusItem = focusedItemId();
    _items[focusItem]->selectPrev();

}

void cSettingWnd::onUIKeyRIGHT()
{
    size_t focusItem = focusedItemId();
    _items[focusItem]->selectNext();

}

size_t cSettingWnd::getItemSelection( size_t itemId )
{
    if( itemId >= _items.size() )
        return -1;
     return _items[itemId]->selectedItemId();
}

size_t cSettingWnd::focusedItemId()
{
    size_t focusItem = 0;
    for( size_t i = 0; i < _items.size(); ++i )
    {
        if( _items[i]->isFocused() ) {
            focusItem = i;
            break;
        }
    }

    return focusItem;
}

void cSettingWnd::onItemClicked( cSpinBox * item )
{
    windowManager().setFocusedWindow( item );
}
