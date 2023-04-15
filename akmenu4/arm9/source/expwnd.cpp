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
#include "expwnd.h"
#include "msgbox.h"
#include "windowmanager.h"
#include "uisettings.h"
#include "language.h"
#include "exptools.h"
#include "datetime.h"
#include "progresswnd.h"

using namespace akui;

cExpWnd::cExpWnd( s32 x, s32 y, u32 w, u32 h, cWindow * parent, const std::string & text )
: cForm( x, y, w, h, parent, text ),
_buttonOK(0,0,46,18,this,"\x01 OK"),
_buttonCancel(0,0,48,18,this,"\x02 Cancel"),
_buttonRAM(0,0,46,18,this,"\x03 RAM"),
_buttonSRAM(0,0,46,18,this,"\x04 SRAM"),
_Rumble(0,0,108,18,this,"spin"),
_Label( 0, 0, 20 * 6, gs().fontHeight, this, "rumble strength" ),
_NorMode(0,0,108,18,this,"spin"),
_NorLabel( 0, 0, 20 * 6, gs().fontHeight, this, "NOR Mode" )
{
    s16 buttonY = size().y - _buttonCancel.size().y - 4;

    _buttonCancel.setStyle( cButton::press );
    _buttonCancel.setText( "\x02 " + LANG( "setting window", "cancel" ) );
    _buttonCancel.setTextColor( uis().buttonTextColor );
    _buttonCancel.loadAppearance( SFN_BUTTON3 );
    _buttonCancel.clicked.connect( this, &cExpWnd::onCancel );
    addChildWindow( &_buttonCancel );

    _buttonOK.setStyle( cButton::press );
    _buttonOK.setText( "\x01 " + LANG( "setting window", "ok" ) );
    _buttonOK.setTextColor( uis().buttonTextColor );
    _buttonOK.loadAppearance( SFN_BUTTON3 );
    _buttonOK.clicked.connect( this, &cExpWnd::onOK );
    addChildWindow( &_buttonOK );

    _buttonRAM.setStyle( cButton::press );
    _buttonRAM.setText( "\x03 " + LANG( "exp window", "ram" ) );
    _buttonRAM.setTextColor( uis().buttonTextColor );
    _buttonRAM.loadAppearance( SFN_BUTTON3 );
    _buttonRAM.clicked.connect( this, &cExpWnd::onRAM );
    addChildWindow( &_buttonRAM );

    _buttonSRAM.setStyle( cButton::press );
    _buttonSRAM.setText( "\x04 " + LANG( "exp window", "sram" ) );
    _buttonSRAM.setTextColor( uis().buttonTextColor );
    _buttonSRAM.loadAppearance( SFN_BUTTON3 );
    _buttonSRAM.clicked.connect( this, &cExpWnd::onSRAM );
    addChildWindow( &_buttonSRAM );

    s16 nextButtonX = size().x;
    s16 buttonPitch = _buttonCancel.size().x + 4;
    nextButtonX -= buttonPitch;
    _buttonCancel.setRelativePosition( cPoint(nextButtonX, buttonY) );

    buttonPitch = _buttonOK.size().x + 4;
    nextButtonX -= buttonPitch;
    _buttonOK.setRelativePosition( cPoint(nextButtonX, buttonY) );

    buttonPitch = _buttonRAM.size().x + 4;
    nextButtonX -= buttonPitch;
    _buttonRAM.setRelativePosition( cPoint(nextButtonX, buttonY) );

    buttonPitch = _buttonSRAM.size().x + 4;
    nextButtonX -= buttonPitch;
    _buttonSRAM.setRelativePosition( cPoint(nextButtonX, buttonY) );

    s32 itemY = 32;
    s32 itemX = 8;
    char* rumbleLang[]={"off","low","mid","high"};
    for( size_t i = 0; i < 4; ++i ) {
        _Rumble.insertItem( LANG( "exp window", rumbleLang[i] ) , i );
    }
    CIniFile ini( SFN_UI_SETTINGS );
    u32 spinBoxWidth = ini.GetInt( "setting window", "spinBoxWidth", 108 );
    _Rumble.loadAppearance( "" );
    _Rumble.setSize( cSize(spinBoxWidth, 18) );

    _Rumble.setRelativePosition( cPoint( _size.x - spinBoxWidth - 4, itemY ) );
    addChildWindow( &_Rumble );

    _Rumble.selectItem(gs().rumbleStrength);

    _Rumble.componentClicked.connect( this, &cExpWnd::onSpinBoxClicked );

    itemY += (_Rumble.windowRectangle().height() - _Label.windowRectangle().height()) / 2;
    _Label.setText(LANG( "exp window","strength"));
    _Label.setRelativePosition( cPoint( itemX, itemY ) );
    _Label.setTextColor( uis().formTextColor );
    _Label.setSize( cSize( _size.x / 2 + 8, 12 ) );
    addChildWindow( &_Label );


    itemY = 52;
	_NorMode.insertItem( LANG("exp window", "auto"), 0);
	_NorMode.insertItem( LANG("exp window", "always"), 1);

    _NorMode.loadAppearance( "" );
    _NorMode.setSize( cSize(spinBoxWidth, 18) );

    _NorMode.setRelativePosition( cPoint( _size.x - spinBoxWidth - 4, itemY ) );
    addChildWindow( &_NorMode);

    _NorMode.selectItem(gs().norMode3in1Exp);

    _NorMode.componentClicked.connect( this, &cExpWnd::onSpinBoxClicked );

    itemY += (_NorMode.windowRectangle().height() - _NorLabel.windowRectangle().height()) / 2;
    _NorLabel.setText(LANG( "exp window","NOR Mode"));
    _NorLabel.setRelativePosition( cPoint( itemX, itemY ) );
    _NorLabel.setTextColor( uis().formTextColor );
    _NorLabel.setSize( cSize( _size.x / 2 + 8, 12 ) );
    addChildWindow( &_NorLabel );



    loadAppearance( "" );
    arrangeChildren();

	_spinbox = &_Rumble;
    windowManager().setFocusedWindow( _spinbox );
}

cExpWnd::~cExpWnd()
{}


void cExpWnd::draw()
{
    _renderDesc.draw( windowRectangle(), _engine );
    gdi().setPenColor( uiSettings().formTitleTextColor,_engine );
    //
    cForm::draw();
}

bool cExpWnd::process( const akui::cMessage & msg )
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

void cExpWnd::onUIKeyUP()
{
	if ( _spinbox == &_NorMode) _spinbox = &_Rumble;
    windowManager().setFocusedWindow( _spinbox );
}

void cExpWnd::onUIKeyDOWN()
{

	if ( _spinbox == &_Rumble ) _spinbox = &_NorMode;
    windowManager().setFocusedWindow( _spinbox );
}


bool cExpWnd::processKeyMessage( const cKeyMessage & msg )
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
        case cKeyMessage::UI_KEY_B:
            onCancel();
            ret = true;
            break;
        case cKeyMessage::UI_KEY_X:
            onRAM();
            ret = true;
            break;
        case cKeyMessage::UI_KEY_Y:
            onSRAM();
            ret = true;
            break;
        case cKeyMessage::UI_KEY_LEFT:
           // _Rumble.selectPrev();
		    _spinbox->selectPrev();
            ret = true;
            break;
        case cKeyMessage::UI_KEY_RIGHT:
            //_Rumble.selectNext();
		    _spinbox->selectNext();
            ret = true;
            break;
        case cKeyMessage::UI_KEY_DOWN:
            onUIKeyDOWN();
            ret = true;
            break;
        case cKeyMessage::UI_KEY_UP:
            onUIKeyUP();
            ret = true;
            break;
 
        default:
            {}
        };
    }
    return ret;
}


cWindow& cExpWnd::loadAppearance(const std::string& aFileName )
{
    _renderDesc.loadData( SFN_FORM_TITLE_L, SFN_FORM_TITLE_R, SFN_FORM_TITLE_M );
    _renderDesc.setTitleText( _text );
    return *this;
}

void cExpWnd::onOK()
{
    cExpansion::Reset();
    if(_Rumble.selectedItemId()>0) cExpansion::SetShake(0xEF+_Rumble.selectedItemId());
	gs().rumbleStrength = _Rumble.selectedItemId();
	gs().norMode3in1Exp= _NorMode.selectedItemId();
    gs().saveSettings();
    cForm::onOK();
}

void cExpWnd::onCancel()
{
    cForm::onCancel();
}

void cExpWnd::onRAM()
{
    cExpansion::Reset();
    cExpansion::SetRompage(0x300);
    cExpansion::OpenNorWrite();
    cExpansion::EnableBrowser();
    cForm::onOK();
}

void cExpWnd::onSRAM()
{
    char saveName[256];
    const u32 size=4096*128,page=4096,pages=128;
    sprintf(saveName,"fat0:/sram-%04d-%02d-%02d-%02d-%02d-%02d.sav",datetime().year(),datetime().month(),datetime().day(),datetime().hours(),datetime().minutes(),datetime().seconds());
    FILE* saveFile=fopen(saveName,"wb");
    if(saveFile)
    {
      u8* buf=(u8*)malloc(size);
      if(buf)
      {
        progressWnd().setTipText(LANG("progress window","gba save store" ));
        progressWnd().show();
        progressWnd().setPercent(0);
        for(u32 ii=0;ii<pages;ii++)
        {
          cExpansion::SetRampage(ii);
          cExpansion::ReadSram(0x0A000000,buf+ii*page,page);
          progressWnd().setPercent(ii*page*90/size);
        }
        cExpansion::SetRampage(16);
        fwrite(buf,size,1,saveFile);
        progressWnd().setPercent(100);
        progressWnd().hide();
        free(buf);
      }
      fclose(saveFile);
    }
    cForm::onOK();
}

void cExpWnd::onSpinBoxClicked( cSpinBox * item )
{
    windowManager().setFocusedWindow( item );
}

void cExpWnd::onShow()
{
    centerScreen();
}
