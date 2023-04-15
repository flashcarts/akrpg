/*
 * 2008/06/18
 * @by bliss (bliss@hanirc.org)
 *
 * Copyright (C) 2008 Bliss. All rights reserved.
 * This code is following BSD LICENSE.
 *
 */



#include "sdspeedwnd.h"
#include "sdidentify.h"
#include "systemfilenames.h"
#include "msgbox.h"
#include "windowmanager.h"
#include "globalsettings.h"
#include "uisettings.h"
#include "language.h"
#include <stdlib.h>
#include <math.h>

using namespace akui;

cSdSpeedWnd::cSdSpeedWnd( s32 x, s32 y, u32 w, u32 h, cWindow * parent, const std::string & text )
: cForm( x, y, w, h, parent, text ),
_buttonOK(0,0,46,18,this,"\x01 OK"),
_buttonCancel(0,0,48,18,this,"\x02 Cancel"),
_buttonAuto(0,0,46,18,this,"\x04 Auto"),
_buttonReset(0,0,46,18,this,"\x03 Reset"),
_buttonB4(0,0,46,18,this,"1000"),
_buttonB3(0,0,46,18,this,"0100"),
_buttonB2(0,0,46,18,this,"0010"),
_buttonB1(0,0,46,18,this,"0001"),
_buttonIncrease(0,0,46,18,this,"+"),
_buttonDecrease(0,0,46,18,this,"-")
{
	centerScreen();
    s16 buttonY = size().y - _buttonCancel.size().y - 4;

    _buttonCancel.setStyle( cButton::press );
    _buttonCancel.setText( "\x02 " + LANG( "setting window", "cancel" ) );
    _buttonCancel.setTextColor( uis().buttonTextColor );
    _buttonCancel.loadAppearance( SFN_BUTTON3 );
    _buttonCancel.clicked.connect( this, &cSdSpeedWnd::onCancel );
    addChildWindow( &_buttonCancel );

    _buttonOK.setStyle( cButton::press );
    _buttonOK.setText( "\x01 " + LANG( "setting window", "ok" ) );
    _buttonOK.setTextColor( uis().buttonTextColor );
    _buttonOK.loadAppearance( SFN_BUTTON3 );
    _buttonOK.clicked.connect( this, &cSdSpeedWnd::onOK );
    addChildWindow( &_buttonOK );

    _buttonAuto.setStyle( cButton::press );
    _buttonAuto.setText( "\x04 " + LANG( "advanced setting", "Auto" ) );
    _buttonAuto.setTextColor( uis().buttonTextColor );
    _buttonAuto.loadAppearance( SFN_BUTTON3 );
    _buttonAuto.clicked.connect( this, &cSdSpeedWnd::onAuto);
    addChildWindow( &_buttonAuto);

    _buttonReset.setStyle( cButton::press );
    _buttonReset.setText( "\x03 " + LANG( "advanced setting", "Reset" ) );
    _buttonReset.setTextColor( uis().buttonTextColor );
    _buttonReset.loadAppearance( SFN_BUTTON3 );
    _buttonReset.clicked.connect( this, &cSdSpeedWnd::onReset);
    addChildWindow( &_buttonReset );

	// Base Select & Increase/Decrease
    _buttonB1.setStyle( cButton::press );
    _buttonB1.setTextColor( uis().buttonTextColor );
    _buttonB1.loadAppearance( SFN_BUTTON2 );
    _buttonB1.clicked.connect( this, &cSdSpeedWnd::onBase1 );
    addChildWindow( &_buttonB1);
    _buttonB2.setStyle( cButton::press );
    _buttonB2.setTextColor( uis().buttonTextColor );
    _buttonB2.loadAppearance( SFN_BUTTON2 );
    _buttonB2.clicked.connect( this, &cSdSpeedWnd::onBase2 );
    addChildWindow( &_buttonB2);
    _buttonB3.setStyle( cButton::press );
    _buttonB3.setTextColor( uis().buttonTextColor );
    _buttonB3.loadAppearance( SFN_BUTTON2 );
    _buttonB3.clicked.connect( this, &cSdSpeedWnd::onBase3 );
    addChildWindow( &_buttonB3);
    _buttonB4.setStyle( cButton::press );
    _buttonB4.setTextColor( uis().buttonTextColor );
    _buttonB4.loadAppearance( SFN_BUTTON2 );
    _buttonB4.clicked.connect( this, &cSdSpeedWnd::onBase4 );
    addChildWindow( &_buttonB4);
    _buttonIncrease.setStyle( cButton::press );
    _buttonIncrease.setTextColor( uis().buttonTextColor );
    _buttonIncrease.loadAppearance( SFN_BUTTON2 );
    _buttonIncrease.clicked.connect( this, &cSdSpeedWnd::onUIKeyUP );
    addChildWindow( &_buttonIncrease);
    _buttonDecrease.setStyle( cButton::press );
    _buttonDecrease.setTextColor( uis().buttonTextColor );
    _buttonDecrease.loadAppearance( SFN_BUTTON2 );
    _buttonDecrease.clicked.connect( this, &cSdSpeedWnd::onUIKeyDOWN );
    addChildWindow( &_buttonDecrease);


    s16 nextButtonX = size().x;
    s16 buttonPitch = _buttonCancel.size().x + 4;
    nextButtonX -= buttonPitch;
    _buttonCancel.setRelativePosition( cPoint(nextButtonX, buttonY) );

    buttonPitch = _buttonOK.size().x + 4;
    nextButtonX -= buttonPitch;
    _buttonOK.setRelativePosition( cPoint(nextButtonX, buttonY) );

    buttonPitch = _buttonAuto.size().x + 12;
    nextButtonX -= buttonPitch;
    _buttonAuto.setRelativePosition( cPoint(nextButtonX, buttonY) );

    buttonPitch = _buttonReset.size().x + 4;
    nextButtonX -= buttonPitch;
    _buttonReset.setRelativePosition( cPoint(nextButtonX, buttonY) );

	buttonY = 28;
	buttonPitch = _buttonB1.size().x + 4;
	nextButtonX = (256 - buttonPitch * 4)/2 -1;
	_buttonIncrease.setRelativePosition( cPoint( nextButtonX , 62 ) );
	_buttonDecrease.setRelativePosition( cPoint( nextButtonX , 62 + _buttonIncrease.size().y + 4 ) );

	_buttonB4.setRelativePosition( cPoint( nextButtonX , buttonY) );
	buttonPitch = _buttonB2.size().x + 4;
    nextButtonX += buttonPitch;
	_buttonB3.setRelativePosition( cPoint( nextButtonX , buttonY) );
	buttonPitch = _buttonB3.size().x + 4;
    nextButtonX += buttonPitch;
	_buttonB2.setRelativePosition( cPoint( nextButtonX , buttonY) );
	buttonPitch = _buttonB4.size().x + 4;
    nextButtonX += buttonPitch;
	_buttonB1.setRelativePosition( cPoint( nextButtonX , buttonY) );

    loadAppearance( "" );
    arrangeChildren();

	_sdSpeed = (int) (sdidCheckSDSpeed(8192));

    _numbers = createBMP15FromFile( SFN_YEAR_NUMBERS );
	_basePosition = 0;
}

cSdSpeedWnd::~cSdSpeedWnd()
{
}

void cSdSpeedWnd::drawNumber( u8 _position, u8 n )
{
    if( !_numbers.valid() )
        return;

    u8 x = ( 256 - _numbers.width() * 4 ) / 2 + _position * _numbers.width();
    u8 y = position().y + 62;

	u8 w = _numbers.width();
    u8 h = _numbers.height() / 10;

    gdi().maskBlt( _numbers.buffer() + n * w * h / 2, x, y, w, h, selectedEngine() );
	if ( _position == _basePosition ) {
		u16 _penColor = uiSettings().spinBoxFocusColor;
		gdi().fillRect( _penColor, _penColor, x,y+h,w,4, selectedEngine());
	}
}

void cSdSpeedWnd::drawNumberText( u32 n)
{
    u32 factor = 1000;
    for( u8 i = 0; i < 4; ++i )
    {
        u8 number = n / factor;
        n %= factor;
        factor /= 10;
        drawNumber( i, number );
    }
}


void cSdSpeedWnd::draw()
{
    _renderDesc.draw( windowRectangle(), _engine );
	drawNumberText(_sdSpeed);
    cForm::draw();
}

bool cSdSpeedWnd::process( const akui::cMessage & msg )
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

void cSdSpeedWnd::onOK()
{
	u32 ret = messageBox( this,
		LANG("setting window","confirm"),
		LANG("setting window","confirm text"), MB_OK | MB_CANCEL );

	if( ID_OK != ret ) return;

    CIniFile ini( SFN_SDCARD_LIST );
    std::string name = sdidGetSDName();
    std::string manufacturerID = sdidGetSDManufacturerID();
    ini.SetInt( "SD Card Speed", manufacturerID + name, _sdSpeed);
    ini.SaveIniFile( SFN_SDCARD_LIST );
    _modalRet = 1;
}

void cSdSpeedWnd::onAuto()
{
    CIniFile ini( SFN_SDCARD_LIST );
    std::string name = sdidGetSDName();
    std::string manufacturerID = sdidGetSDManufacturerID();
    ini.SetInt( "SD Card Speed", manufacturerID + name, 0);
    ini.SaveIniFile( SFN_SDCARD_LIST );
	_sdSpeed = (int) (sdidCheckSDSpeed(8192));
	windowManager().setFocusedWindow( this );
}


void cSdSpeedWnd::onReset()
{
	_sdSpeed = (int) (sdidCheckSDSpeed(8192));
}

void cSdSpeedWnd::onCancel()
{
    _modalRet = 0;
}

bool cSdSpeedWnd::processKeyMessage( const cKeyMessage & msg )
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
        case cKeyMessage::UI_KEY_Y: 
			onAuto();
            ret = true;
            break;
        case cKeyMessage::UI_KEY_X: 
			onReset();
            ret = true;
            break;
        default:
            {}
        };
    }

    return ret;
}

cWindow& cSdSpeedWnd::loadAppearance(const std::string& aFileName )
{
    _renderDesc.loadData( SFN_FORM_TITLE_L, SFN_FORM_TITLE_R, SFN_FORM_TITLE_M );
    _renderDesc.setTitleText( _text );
    return *this;
}

void cSdSpeedWnd::onUIKeyUP()
{
	_sdSpeed += (int )(pow( 10, 3 - _basePosition));
	if ( _sdSpeed > 0x1f00 ) _sdSpeed = 0x2000;
}

void cSdSpeedWnd::onUIKeyDOWN()
{
	_sdSpeed -= (int)(pow( 10, 3 - _basePosition));
	if ( _sdSpeed < 0 ) _sdSpeed = 0;
}

void cSdSpeedWnd::onUIKeyLEFT()
{
	if ( _basePosition == 0 ) _basePosition = 3;
	else _basePosition -= 1;
}

void cSdSpeedWnd::onUIKeyRIGHT()
{
	_basePosition += 1;
	if ( _basePosition > 3 ) _basePosition = 0;
}

void cSdSpeedWnd::onShow()
{
	    centerScreen();
}

