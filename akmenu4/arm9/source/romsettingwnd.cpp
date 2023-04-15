/*
 * 2008/05/18
 * @by bliss (bliss@hanirc.org)
 *
 * Copyright (C) 2008 Bliss. All rights reserved.
 * This code following BSD LICENSE.
 *
 */


#include "systemfilenames.h"
#include "msgbox.h"
#include "windowmanager.h"
#include "globalsettings.h"
#include "uisettings.h"
#include "language.h"
#include "cheatlist.h"
#include "saveslotwnd.h"
#include <sys/stat.h>
#include "romsettingwnd.h"

using namespace akui;

cRomSettingWnd::cRomSettingWnd( s32 x, s32 y, u32 w, u32 h, cWindow * parent, const std::string & text )
: cSettingWnd( x, y, w, h, parent, text ),
_buttonSaveSlot(0,0,46,18,this,"\x03 SaveSlot"),	// call save slot management wnd
_buttonCheatMenu(0,0,46,18,this,"\x04 CheatMenu")	// call cheatmenu wnd
{

    _buttonCheatMenu.setStyle( cButton::press );
    _buttonCheatMenu.setText( "\x04" + LANG( "rom info", "cheat menu" ) );
    _buttonCheatMenu.setTextColor( uis().buttonTextColor );
    _buttonCheatMenu.loadAppearance( SFN_BUTTON4 );
    _buttonCheatMenu.clicked.connect( this, &cRomSettingWnd::onCheatMenu );
    addChildWindow( &_buttonCheatMenu );

    _buttonSaveSlot.setStyle( cButton::press );
    _buttonSaveSlot.setText( "\x03" + LANG( "rom info", "SaveSlot" ) );
    _buttonSaveSlot.setTextColor( uis().buttonTextColor );
    _buttonSaveSlot.loadAppearance( SFN_BUTTON3 );
    _buttonSaveSlot.clicked.connect( this, &cRomSettingWnd::onSaveSlot);
    addChildWindow( &_buttonSaveSlot );

    loadAppearance( "" );

	_saveSlot = 0;
	_useCheatMenu = true;
}

cRomSettingWnd::~cRomSettingWnd() { }

bool cRomSettingWnd::setFileInfo( const std::string & longPath, const char* romid, const u8 saveSlot )
{	
    if( "" == longPath) {
        dbg_printf("long name %s\n", longPath.c_str());
        return false;
    }

    struct stat st;
    if( -1 == stat( longPath.c_str(), &st ) ) {
        return false;
    }

    s16 buttonY = size().y - _buttonCheatMenu.size().y - 4;
    s16 nextButtonX = size().x;
    s16 buttonPitch = _buttonCancel.size().x + 2;
    nextButtonX -= buttonPitch;
    _buttonCancel.setRelativePosition( cPoint(nextButtonX, buttonY) );

    buttonPitch = _buttonOK.size().x + 1;
    nextButtonX -= buttonPitch;
    _buttonOK.setRelativePosition( cPoint(nextButtonX, buttonY) );

    buttonPitch = _buttonCheatMenu.size().x + 2;
    nextButtonX -= buttonPitch;
    _buttonCheatMenu.setRelativePosition( cPoint(nextButtonX, buttonY) );

    buttonPitch = _buttonSaveSlot.size().x + 1;
    nextButtonX -= buttonPitch;
    _buttonSaveSlot.setRelativePosition( cPoint(nextButtonX, buttonY) );

    arrangeChildren();

	_longName = longPath;
	_romId = romid;
	_saveSlot = saveSlot;

	return true;
}


void cRomSettingWnd::onCheatMenu() 
{ 
	CIniFile ini(SFN_UI_SETTINGS);
	u32 w = ini.GetInt( "cheat window", "w", 240 );
	u32 h = ini.GetInt( "cheat window", "h", 168 );
	cCheatListWnd * cheatlistWnd = new cCheatListWnd( (256-w)/2, (192-h)/2, w, h, this, LANG("cheats", "title" ) );
	cheatlistWnd->setFileInfo( _longName, _romId.c_str()) ;
	cheatlistWnd->ReadList();
	cheatlistWnd->doModal();
	delete cheatlistWnd;
}

void cRomSettingWnd::disableCheatMenu() 
{
	_buttonCheatMenu.hide();
	_useCheatMenu = false;
}

void cRomSettingWnd::onSaveSlot()
{
	cSaveSlotWnd * saveslotwnd = new cSaveSlotWnd( 8, (192-132)/2, 240, 132, this, LANG("saveslot", "title" ) );
	saveslotwnd->setFileInfo( _longName , _saveSlot);
	_saveSlot = saveslotwnd->doModal();
	delete saveslotwnd;

}

bool cRomSettingWnd::process( const akui::cMessage & msg )
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

bool cRomSettingWnd::processKeyMessage( const cKeyMessage & msg )
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
        case cKeyMessage::UI_KEY_X: 
			onSaveSlot();
			ret = true;
			break;
        case cKeyMessage::UI_KEY_Y:
			if ( _useCheatMenu ) {
				onCheatMenu();
				ret = true;
				break;
			}
        default:
            {}
        };
    }

    return ret;
}


