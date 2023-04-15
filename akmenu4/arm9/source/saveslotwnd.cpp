/*
 * 2008/05/18
 * @by bliss (bliss@hanirc.org)
 *
 * Copyright (C) 2008 Bliss. All rights reserved.
 * This code following BSD LICENSE.
 *
 */

#include <fat.h>
#include <string.h>
#include "systemfilenames.h"
#include "msgbox.h"
#include "windowmanager.h"
#include "globalsettings.h"
#include "uisettings.h"
#include "language.h"
#include "unicode.h"
#include "settingwnd.h"
#include "files.h"
#include "saveslotwnd.h"
#include <unistd.h>

#define MAXSLOTS 7

using namespace akui;

cSaveSlotWnd::cSaveSlotWnd( s32 x, s32 y, u32 w, u32 h, cWindow * parent, const std::string & text )
: cForm( x, y, w, h, parent, text ),
_buttonOK(0,0,46,18,this,"\x01 OK"),
_buttonSetSlot(0,0,46,18,this,"Set Sot"),
_buttonDelete(0,0,46,18,this,"\x03 Delete"),
_buttonCopy(0,0,46,18,this,"\04 Copy")
{
    s16 buttonY = size().y - _buttonOK.size().y - 4;

	// init buttons
    _buttonOK.setStyle( cButton::press );
    _buttonOK.setText( "\x01 " + LANG( "setting window", "ok" ) );
    _buttonOK.setTextColor( uis().buttonTextColor );
    _buttonOK.loadAppearance( SFN_BUTTON3 );
    _buttonOK.clicked.connect( this, &cSaveSlotWnd::onOK);
    addChildWindow( &_buttonOK);

    _buttonSetSlot.setStyle( cButton::press );
    _buttonSetSlot.setText( LANG( "saveslot", "Set Slot" ) );
    _buttonSetSlot.setTextColor( uis().buttonTextColor );
    _buttonSetSlot.loadAppearance( SFN_BUTTON3 );
    _buttonSetSlot.clicked.connect( this, &cSaveSlotWnd::onSetDefault);
    addChildWindow( &_buttonSetSlot);

    _buttonDelete.setStyle( cButton::press );
    _buttonDelete.setText( "\x03 " +  LANG( "saveslot", "Delete" ) );
    _buttonDelete.setTextColor( uis().buttonTextColor );
    _buttonDelete.loadAppearance( SFN_BUTTON3 );
    _buttonDelete.clicked.connect( this, &cSaveSlotWnd::onDelete);
    addChildWindow( &_buttonDelete);

    _buttonCopy.setStyle( cButton::press );
    _buttonCopy.setText(  "\x04 " + LANG("saveslot", "Copy" ) );
    _buttonCopy.setTextColor( uis().buttonTextColor );
    _buttonCopy.loadAppearance( SFN_BUTTON3 );
    _buttonCopy.clicked.connect( this, &cSaveSlotWnd::onCopy);
    addChildWindow( &_buttonCopy);

	// arrange button position
    s16 nextButtonX = size().x;
    s16 buttonPitch = _buttonOK.size().x + 4;
    nextButtonX -= buttonPitch;
    _buttonOK.setRelativePosition( cPoint(nextButtonX, buttonY) );

    buttonPitch = _buttonSetSlot.size().x + 2;
    nextButtonX -= buttonPitch;
    _buttonSetSlot.setRelativePosition( cPoint(nextButtonX, buttonY) );

	nextButtonX = 4;
	_buttonDelete.setRelativePosition( cPoint(nextButtonX, buttonY) );

    nextButtonX += _buttonDelete.size().x + 2;
	_buttonCopy.setRelativePosition( cPoint(nextButtonX, buttonY) );

    loadAppearance( "" );
    arrangeChildren();
	
	_size = cSize( w, h );
    _position = cPoint( x, y );
    _rowHeight = 12;
    _selectedRowId = 0;
	_touchMovedAfterTouchDown = false;

    _textColor = uiSettings().listTextColor; //RGB15(24,24,24);
    _textColorHilight = uiSettings().listTextHighLightColor; //RGB15(31,31,31);
	_selectionBarColor1 = BIT(15)|uiSettings().listViewBarColor1; //RGB15(0,0,31);
    _selectionBarColor2 = BIT(15)|uiSettings().listViewBarColor2; //RGB15(0,0,31);

	initSlots();
}

cSaveSlotWnd::~cSaveSlotWnd()
{
	cRows.clear();
}

void cSaveSlotWnd::initSlots()
{
	for( u8 i = 0; i < MAXSLOTS ;i += 1 ) {
		insertRow(i);
	}
}

void cSaveSlotWnd::insertRow( u8 index )
{
	cRow row;
	row.index = index ;
	row.isdefault= false;
	row.isdefault= false;

	cRows.push_back(row);
}
	
bool cSaveSlotWnd::isExist( const u8 index )
{
	std::string savePath = _longName;
	if ( index ) {
		savePath += ".";
		savePath += ('0'+index);
	}
	savePath += ".sav";

	return isExist(savePath);
}
	
bool cSaveSlotWnd::isExist( const std::string &savePath)
{
    struct stat st;
    if( -1 == stat( savePath.c_str(), &st ) ) {
        return false;
    }
	return true;
}

void cSaveSlotWnd::draw()
{
    _renderDesc.draw( windowRectangle(), _engine );
    gdi().setPenColor( uiSettings().formTitleTextColor, selectedEngine() );

	drawSelectionBar();

	u32 px = position().x + 25;
	u32 py = position().y + 22;
	u32 width = size().x - 8;
	for( u8 i = 0; i < MAXSLOTS ; i += 1 ) {
		std::string slot = LANG("saveslot", "Slot") + " ";
		if ( i ) slot += ('0' + i );
		else slot = LANG("saveslot","Default Slot");

		if ( cRows[i].isexist )
			gdi().textOutRect( px - 6, py + i*12, width, 40, "+", selectedEngine() ); 

		gdi().textOutRect( px ,py + i*12, width , 40, slot.c_str() , selectedEngine() ); 
		if ( cRows[i].isdefault ) 
			gdi().textOutRect( px +1 ,py + i*12, width , 40, slot.c_str() , selectedEngine() ); 
	}

    cForm::draw();
}

void cSaveSlotWnd::drawSelectionBar()
{
	s16 x = _position.x + 1;
    s16 y = _position.y + _rowHeight*2 + _selectedRowId * _rowHeight - 2;
    s16 w = _size.x - 2;
    s16 h = _rowHeight;// - 1;

    for( u8 i = 0; i < h; ++i ) {
            gdi().fillRect( _selectionBarColor1, _selectionBarColor2, x, y+i, w, 1, _engine );
    }
}

bool cSaveSlotWnd::process( const akui::cMessage & msg )
{
    bool ret = false;

    ret = cForm::process( msg );

    if( !ret ) {
        if( msg.id() > cMessage::keyMessageStart && msg.id()
            < cMessage::keyMessageEnd )
        {
            ret = processKeyMessage( (cKeyMessage &)msg );
        }
        else if( msg.id() > cMessage::touchMessageStart
            && msg.id() < cMessage::touchMessageEnd )
        {
            ret = processTouchMessage( (cTouchMessage &)msg );
        }
    }
    return ret;
}

bool cSaveSlotWnd::processKeyMessage( const cKeyMessage & msg )
{
    bool ret = false;
    if( msg.id() == cMessage::keyDown )
    {
        switch( msg.keyCode() )
        {
		case cKeyMessage::UI_KEY_UP:
			if ( _selectedRowId ) _selectedRowId -= 1;
            ret = true;
            break;
		case cKeyMessage::UI_KEY_DOWN:
			if ( _selectedRowId < MAXSLOTS - 1 ) _selectedRowId += 1;
            ret = true;
            break;
		case cKeyMessage::UI_KEY_LEFT:
			if ( _selectedRowId ) {
				u8 cnt = 4;
				while( cnt-- ) 
					if ( _selectedRowId ) _selectedRowId -= 1;
			}
			ret = true;
            break;
		case cKeyMessage::UI_KEY_RIGHT:
			if ( _selectedRowId < MAXSLOTS - 1 ) {
				u8 cnt = 4;
				while( cnt-- ) 
					if ( _selectedRowId < MAXSLOTS - 1 ) _selectedRowId += 1;
			}
			ret = true;
            break;
        case cKeyMessage::UI_KEY_X:
            onDelete();
            ret = true;
            break;
        case cKeyMessage::UI_KEY_Y:
			onCopy();
            ret = true;
            break;
		case cKeyMessage::UI_KEY_SELECT:
            onSetDefault();
            ret = true;
            break;
        case cKeyMessage::UI_KEY_B:
            onCancel();
            ret = true;
            break;
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
bool cSaveSlotWnd::processTouchMessage( const akui::cTouchMessage & msg )
{
    bool ret = false;
    static int sumOfMoveY = 0;

    if( msg.id() == cMessage::touchMove ) {
        if( abs(msg.position().y) > 0 ) {
            sumOfMoveY += msg.position().y;
        }
        if( sumOfMoveY > gs().scrollSpeed ) {
			if ( _selectedRowId < MAXSLOTS - 1 ) _selectedRowId += 1;
            sumOfMoveY = 0;
            _touchMovedAfterTouchDown = true;
        } else if( sumOfMoveY < -gs().scrollSpeed ) {
			if ( _selectedRowId ) _selectedRowId -= 1;
            sumOfMoveY = 0;
            _touchMovedAfterTouchDown = true;
        }
        ret = true;
    }

    if( msg.id() == cMessage::touchUp ) {
        sumOfMoveY = 0;
        _touchMovedAfterTouchDown = false;
        ret = true;
    }

    if( msg.id() == cMessage::touchDown ) {
        _touchMovedAfterTouchDown = false;

        u32 rbp = rowBelowPoint( cPoint(msg.position().x,msg.position().y) );
        if( rbp != (u32)-1 ) {
            if( _selectedRowId == (u32) rbp ) { 
				// set to default
				for(u8 i = 0; i < MAXSLOTS; i += 1 )
					cRows[i].isdefault = (i == _selectedRowId) ?true:false;
			}
			_selectedRowId = (u32) rbp;
        }
        ret = true;
    }

	return ret;
}

u32 cSaveSlotWnd::rowBelowPoint( const akui::cPoint & pt ) 
{
	s16 sx = _position.x + 1;
	s16 sy = _position.y + _rowHeight*2 - 2;
	s16 ex = sx + _size.x - 2;
	s16 ey = sy + _rowHeight * MAXSLOTS ;

    //if( windowRectangle().surrounds( pt ) ) {
	if ( pt.x > sx && pt.x < ex && pt.y > sy && pt.y < ey ) {
        u32 row = (pt.y - sy) / _rowHeight;
		return row;
	}
    return (u32)-1;
}

cWindow& cSaveSlotWnd::loadAppearance(const std::string& aFileName )
{
    _renderDesc.loadData( SFN_FORM_TITLE_L, SFN_FORM_TITLE_R, SFN_FORM_TITLE_M );
    _renderDesc.setTitleText( _text );
    return *this;
}

void cSaveSlotWnd::setFileInfo( const std::string & fullPath, const u8 saveSlot)
{	
    if( "" == fullPath ) {
        return;
    }

    struct stat st;
    if( -1 == stat( fullPath.c_str(), &st ) ) {
        return;
    }

	_longName = fullPath;

	_oldSlot = saveSlot;
	_selectedRowId = saveSlot;
	cRows[saveSlot].isdefault = true;
	for(u8 i = 0; i < MAXSLOTS; i += 1 )
		cRows[i].isexist = isExist(i);
}

void cSaveSlotWnd::onSetDefault()
{
	for(u8 i = 0; i < MAXSLOTS; i += 1 )
		cRows[i].isdefault = (i == _selectedRowId) ?true:false;
}

void cSaveSlotWnd::onDelete()
{
	std::string savePath = _longName;
	if ( _selectedRowId) {
		savePath += ".";
		savePath += ('0'+_selectedRowId);
	}
	savePath += ".sav";

	if ( isExist(savePath) ) {
		std::string confirmText = LANG("confirm delete file","text");
		std::string slot = LANG("saveslot", "Slot") + " ";
		if ( _selectedRowId  ) slot += ('0' + _selectedRowId );
		else slot = LANG("saveslot","Default Slot");

		confirmText = formatString( confirmText.c_str(), slot.c_str() );
		u32 ret = messageBox( this,LANG("confirm delete file","title"),confirmText.c_str(), MB_YES | MB_NO );
		if ( ret == ID_YES ) {
			unlink( savePath.c_str() );
			cRows[_selectedRowId].isexist = false;
		}
	}
}

void cSaveSlotWnd::onCopy()
{
	std::string savePath = _longName;
	if ( _selectedRowId) {
		savePath += ".";
		savePath += ('0'+_selectedRowId);
	}
	savePath += ".sav";
    struct stat st;
    if( -1 == stat( savePath.c_str(), &st ) ) return;

	cSettingWnd settingWnd( 0,0, 252, 188, this, LANG("saveslot", "title_copyslot" ) );

	std::vector< std::string > _values;

	if ( _selectedRowId ) _values.push_back (LANG("saveslot", "Default Slot" ));

	for( u8 i = 1; i < MAXSLOTS; i++ ) {
		if ( _selectedRowId != i ) {
			std::string slot  = LANG("saveslot", "Slot") + " ";
			slot += ('0' + i );
			_values.push_back (slot);
		}
	}
	settingWnd.addSettingItem( LANG("saveslot", "target slot" ), _values, 0);
	settingWnd.setNoConfirm();

	u32 ret = settingWnd.doModal();

	if ( ret == ID_OK ) {
		ret = messageBox( this,
			LANG("saveslot","title_copyslot"),
			LANG("saveslot","confirm_copyslot"), MB_OK | MB_CANCEL );
	}

	if ( ret == ID_CANCEL ) 
		return;

	u8 target_slot = settingWnd.getItemSelection( 0 );	
	std::string targetPath = _longName;

	if ( target_slot < _selectedRowId ) {
		if ( target_slot ) {
			targetPath += '.';
			targetPath += ('0' + target_slot);
		}
	}
	else {
		targetPath += '.';
		targetPath += ('0' + target_slot +1) ;
		target_slot += 1;
	}
	targetPath += ".sav";

	bool ok = copyFile(savePath, targetPath, false, st.st_size);
	if ( ok ) cRows[target_slot].isexist = true;
}

void cSaveSlotWnd::onOK()
{
	_modalRet = _selectedRowId;
}

void cSaveSlotWnd::onCancel()
{
	_modalRet = _oldSlot;
}

void cSaveSlotWnd::onShow()
{
    centerScreen();
}


