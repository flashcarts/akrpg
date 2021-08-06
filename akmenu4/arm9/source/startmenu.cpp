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









#include "startmenu.h"
#include "mainlist.h"
#include "windowmanager.h"
#include "systemfilenames.h"
#include "inifile.h"
#include "language.h"

using namespace akui;

void cStartMenu::init()
{
    addItem( START_MENU_ITEM_COPY, LANG("start menu", "Copy") );
    addItem( START_MENU_ITEM_CUT, LANG("start menu", "Cut") );
    addItem( START_MENU_ITEM_DELETE, LANG("start menu", "Delete") );
    addItem( START_MENU_ITEM_PASTE, LANG("start menu", "Paste") );
    addItem( START_MENU_ITEM_SAVETYPE, LANG("start menu", "Patches") );
    addItem( START_MENU_ITEM_SETTING, LANG("start menu", "Setting")  );
    addItem( START_MENU_ITEM_INFO, LANG("start menu", "Info") );
    addItem( START_MENU_ITEM_HELP, LANG("start menu", "Help") );
    loadAppearance( SFN_UI_SETTINGS );
    dbg_printf("startmenu ok\n");
}

void cStartMenu::onHide()
{
    windowManager().setPopUpWindow( NULL );
    dbg_printf("%s cStartMenu::onHide()\n", _text.c_str() );
}


bool cStartMenu::process( const cMessage & msg )
{
    if( msg.id() == cMessage::keyDown ) {
        cKeyMessage & kmsg = (cKeyMessage &)msg;
        if( kmsg.keyCode() == cKeyMessage::UI_KEY_START ) {
            hide();
            return false;
        }
    }
    return cPopMenu::process( msg );
}


cWindow& cStartMenu::loadAppearance(const std::string& aFileName )
{
    _renderDesc->loadData( SFN_STARTMENU_BG );
    _size = _renderDesc->size();

    CIniFile ini( aFileName );
    //std::string bgFile = ini.GetString( "bg", "file",  );
    int ix = ini.GetInt( "start menu", "itemX", 4 );
    int iy = ini.GetInt( "start menu", "itemY", 12 );
    int x = ini.GetInt( "start menu", "x", 4 );
    int y = ini.GetInt( "start menu", "y", 4 );
    setPosition( cPoint( x, y ) );
    _itemTopLeftPoint = cPoint( ix, iy );
    int ih = ini.GetInt( "start menu", "itemHeight", 16 );
    _itemHeight = ih;
    return *this;
}
