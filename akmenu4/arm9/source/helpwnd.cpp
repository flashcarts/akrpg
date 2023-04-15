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









#include <iorpg.h>
#include "helpwnd.h"
#include "msgbox.h"
#include "windowmanager.h"
#include "uisettings.h"
#include "language.h"
#include "version.h"
#include "../../share/ipctool.h"

using namespace akui;

cHelpWnd::cHelpWnd( s32 x, s32 y, u32 w, u32 h, cWindow * parent, const std::string & text )
: cForm( x, y, w, h, parent, text ),
_buttonOK(0,0,46,18,this,"\x01 OK"),
_buttonPowerOff(0,0,24,24,this,"")
{
    s16 buttonY = size().y - _buttonOK.size().y - 4;

    _buttonOK.setStyle( cButton::press );
    _buttonOK.setText( "\x01 " + LANG( "setting window", "ok" ) );
    _buttonOK.setTextColor( uis().buttonTextColor );
    _buttonOK.loadAppearance( SFN_BUTTON3 );
    _buttonOK.clicked.connect( this, &cHelpWnd::onOK );
    addChildWindow( &_buttonOK );

    s16 nextButtonX = size().x;

    s16 buttonPitch = _buttonOK.size().x + 8;
    buttonPitch = _buttonOK.size().x + 8;
    nextButtonX -= buttonPitch;
    _buttonOK.setRelativePosition( cPoint(nextButtonX, buttonY) );

	_buttonPowerOff.loadAppearance(SFN_ICON_POWEROFF);
    _buttonPowerOff.clicked.connect( this, &cHelpWnd::onPowerOff);
    addChildWindow( &_buttonPowerOff);
	_buttonPowerOff.setRelativePosition( cPoint( size().x - 28, 20) );

    loadAppearance( "" );
    arrangeChildren();

    _versionText = formatString( "AKMENU %s.%s ", AKMENU_VRESION_MAIN, AKMENU_VRESION_SUB );
    for( size_t i = 0; i < 11; ++i ) {
        std::string textIndex = formatString( "item%d", i );
		if ( LANG3("help window",textIndex,"") == "" ) break;
        _helpText += LANG( "help window", textIndex );
        _helpText += "\n";
    }
    _helpText = formatString( _helpText.c_str(), 7,1,2,4,3,5,6, "START", "SELECT" );
    u32 getHWVer[2] = { 0xd1000000, 0x00000000 };
    u32 hwVer = 0;
    ioRpgSendCommand( getHWVer, 4, 0, &hwVer );
    hwVer &= 0xff;
    //u8 nandDriverVer = getNandDriverVer();
    _helpText += '\n';
    _helpText += _versionText;
    _helpText += formatString( "HW: %02x", hwVer );


}

cHelpWnd::~cHelpWnd()
{}


void cHelpWnd::draw()
{
    _renderDesc.draw( windowRectangle(), _engine );
    gdi().setPenColor( uiSettings().formTitleTextColor,_engine );
    gdi().textOutRect( position().x + 8, position().y + 20 , size().x - 8, size().y - 32, _helpText.c_str(), _engine );
    cForm::draw();
}

bool cHelpWnd::process( const akui::cMessage & msg )
{
    bool ret = false;

    ret = cForm::process( msg );

    if( !ret ) {
        if( msg.id() > cMessage::keyMessageStart && msg.id()
            < cMessage::keyMessageEnd )
        {
            ret = processKeyMessage( (cKeyMessage &)msg );
        }
    }
    return ret;
}

bool cHelpWnd::processKeyMessage( const cKeyMessage & msg )
{
    bool ret = false;
    if( msg.id() == cMessage::keyDown )
    {
        switch( msg.keyCode() )
        {
        case cKeyMessage::UI_KEY_A:
            onOK();
            ret = true;
            break;
        default:
            {}
        };
    }
    return ret;
}


cWindow& cHelpWnd::loadAppearance(const std::string& aFileName )
{
    _renderDesc.loadData( SFN_FORM_TITLE_L, SFN_FORM_TITLE_R, SFN_FORM_TITLE_M );
    _renderDesc.setTitleText( _text );
    return *this;
}

void cHelpWnd::onOK()
{
    cForm::onOK();
}


void cHelpWnd::onShow()
{
    centerScreen();
}

void cHelpWnd::onPowerOff()
{
	IPC_ARM9 = IPC_MSG_POWEROFF;
}
