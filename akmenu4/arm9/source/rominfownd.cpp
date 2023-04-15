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









#include <fat.h>
#include "rominfownd.h"
#include "systemfilenames.h"
#include "msgbox.h"
#include "windowmanager.h"
#include "globalsettings.h"
#include "uisettings.h"
#include "language.h"
#include "unicode.h"
#include "settingwnd.h"
#include "romsettingwnd.h"
#include "files.h"
#include "options.h"
#include "gbaoptions.h"
#include "fileicons.h"

#define SLOTSIZE 7

using namespace akui;

cRomInfoWnd::cRomInfoWnd( s32 x, s32 y, u32 w, u32 h, cWindow * parent, const std::string & text )
: cForm( x, y, w, h, parent, text ),
_buttonOK(0,0,46,18,this,"\x01 OK"),
//_buttonSaveType(0,0,64,18,this,"\x04 Save Type"),
_buttonSaveType(0,0,64,18,this,"\x04 Rom Info"),
_buttonShortcut(0,0,64,18,this,"\x03 Shortcut")
{
    s16 buttonY = size().y - _buttonOK.size().y - 4;

    _buttonOK.setStyle( cButton::press );
    _buttonOK.setText( "\x01 " + LANG( "setting window", "ok" ) );
    _buttonOK.setTextColor( uis().buttonTextColor );
    _buttonOK.loadAppearance( SFN_BUTTON3 );
    _buttonOK.clicked.connect( this, &cRomInfoWnd::onOK );
    addChildWindow( &_buttonOK );

    s16 nextButtonX = size().x;

    s16 buttonPitch = _buttonOK.size().x + 3;
    nextButtonX -= buttonPitch;

    _buttonOK.setRelativePosition( cPoint(nextButtonX, buttonY) );

    _buttonSaveType.setStyle( cButton::press );
    //_buttonSaveType.setText( "\x04 " + LANG( "setting window", "savetype" ) );
    _buttonSaveType.setText( "\x04 " + LANG( "setting window", "rominfo" ) );
    _buttonSaveType.setTextColor( uis().buttonTextColor );
    _buttonSaveType.loadAppearance( SFN_BUTTON4 );
    _buttonSaveType.clicked.connect( this, &cRomInfoWnd::showSaveType );
    addChildWindow( &_buttonSaveType );

    buttonPitch = _buttonSaveType.size().x + 3;
    nextButtonX -= buttonPitch;

    _buttonSaveType.setRelativePosition( cPoint(nextButtonX, buttonY) );


    //Shortcut Button
	_buttonShortcut.setStyle( cButton::press );
	_buttonShortcut.setText( "\x03 " + LANG3( "setting window", "shortcut" , "shortcut") );
	_buttonShortcut.setTextColor( uis().buttonTextColor );
	_buttonShortcut.loadAppearance( SFN_BUTTON4 );
	_buttonShortcut.clicked.connect( this, &cRomInfoWnd::setShortcut );
	addChildWindow( &_buttonShortcut );

	buttonPitch = _buttonShortcut.size().x + 3;
	nextButtonX -= buttonPitch;

	_buttonShortcut.setRelativePosition( cPoint(nextButtonX, buttonY) );

    loadAppearance( "" );
    arrangeChildren();
}

cRomInfoWnd::~cRomInfoWnd()
{}


void cRomInfoWnd::draw()
{
    _renderDesc.draw( windowRectangle(), _engine );

    _romInfo.drawDSRomIcon( position().x + 8, position().y + 24, selectedEngine() );

    gdi().setPenColor( uiSettings().formTitleTextColor, selectedEngine() );
    gdi().textOutRect( position().x + 48, position().y + 22, size().x - 40, 40, _romInfoText.c_str(), selectedEngine() );

    gdi().textOutRect( position().x + 8, position().y + 64 , size().x - 8, 40, _filenameText.c_str(), selectedEngine() );
    gdi().textOutRect( position().x + 8, position().y + 64 + 14, size().x - 8, 40, _fileDateText.c_str(), selectedEngine() );
    gdi().textOutRect( position().x + 8, position().y + 64 + 14 + 14, size().x - 8, 40, _fileSizeText.c_str(), selectedEngine() );
    gdi().textOutRect( position().x + 8, position().y + 64 + 14 + 14 + 14, size().x - 8, 40, _saveTypeText.c_str(), selectedEngine() );

	if ( _shortName == "slot2:/" || gs().language == 8 )
    gdi().textOutRect( position().x + 9, position().y + 64 , size().x - 8, 40, _filenameText.c_str(), selectedEngine() );


    cForm::draw();

}

bool cRomInfoWnd::process( const akui::cMessage & msg )
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

bool cRomInfoWnd::processKeyMessage( const cKeyMessage & msg )
{
    bool ret = false;
    if( msg.id() == cMessage::keyDown )
    {
        switch( msg.keyCode() )
        {
        case cKeyMessage::UI_KEY_A:
        case cKeyMessage::UI_KEY_B:
            onOK();
            ret = true;
            break;
        case cKeyMessage::UI_KEY_Y:
            showSaveType();
            ret = true;
            break;
        case cKeyMessage::UI_KEY_X:
			setShortcut();
			ret = true;
			break;
        case cKeyMessage::UI_KEY_UP:
        case cKeyMessage::UI_KEY_DOWN:
        case cKeyMessage::UI_KEY_LEFT:
        case cKeyMessage::UI_KEY_RIGHT:
			ret = true;
			break;
        default:
            {}
        };
    }

    return ret;
}


cWindow& cRomInfoWnd::loadAppearance(const std::string& aFileName )
{
    _renderDesc.loadData( SFN_FORM_TITLE_L, SFN_FORM_TITLE_R, SFN_FORM_TITLE_M );
    _renderDesc.setTitleText( _text );
    return *this;
}

static std::string getFriendlyFileSizeString( u64 size )
{
    std::string fileSize;
    std::string sizeUnit;
    if( size < 1024 ) {
        fileSize = formatString( "%d", size);
        sizeUnit = " Byte";
    }
    else if( size < 1024 * 1024 ) {
        fileSize = formatString( "%4.2f", (float)size / 1024);
        sizeUnit = " KB";
    }
    else if( size < 1024 * 1024 * 1024 ) {
        fileSize = formatString( "%4.2f", (float)size / 1024 / 1024 );
        sizeUnit = " MB";
    }  else {
        fileSize = formatString( "%4.2f", (float)size / 1024 / 1024 / 1024 );
        sizeUnit = " GB";
    }

    return fileSize + sizeUnit;
}

void cRomInfoWnd::setDiskInfo( const std::string & diskName )
{
    u64 total = 0;
    u64 used = 0;
    u64 freeSpace = 0;

    if( !getDiskSpaceInfo( diskName, total, used, freeSpace ) )
        return;

    _filenameText = formatString( LANG("disk info", "total").c_str(), getFriendlyFileSizeString(total).c_str() );
    _fileDateText = formatString( LANG("disk info", "used").c_str(), getFriendlyFileSizeString(used).c_str() );
    _fileSizeText = formatString( LANG("disk info", "free").c_str(), getFriendlyFileSizeString(freeSpace).c_str() );

}


void cRomInfoWnd::setFullPath( const std::string & longName )
{
	    _longName = longName;
}


// shortName is ascii code, it is used for open file
// longName is converted from unicode filename, it is used for show
void cRomInfoWnd::setFileInfo( const std::string & shortName, const std::string & showName )
{
    if( "" == shortName || "" == showName ) {
        dbg_printf("short name %s\nshow name %s\n", shortName.c_str(), showName.c_str() );
        return;
    }

    struct stat st;
    if( -1 == stat( shortName.c_str(), &st ) ) {
		if ( shortName == "shortcut:/" ) {
			stat( _longName.c_str(), &st );
		}
        //return;
    }

    if( "fat0:/" == shortName || "fat1:/" == shortName ) {
        setDiskInfo( shortName );
        return;
    }

    _shortName = shortName;

	if ( shortName == "slot2:/") {
		_buttonShortcut.setText( "\x03 " + LANG3( "slot2", "boot mode" , "Boot Mode") );
		_buttonShortcut.show();
		_filenameText = gs().slot2BootMode ? LANG3("slot2","passme","Boot in NDS mode (PassMe)").c_str() : LANG3("slot2","gbamode","Boot in GBA mode").c_str() ;

        u8 chk = 0;
        for( u32 i = 0xA0; i < 0xBD; ++i ) { chk = chk - *(u8*)(0x8000000+i); }
        chk = (chk- 0x19) & 0xff;

		_fileDateText = GBA_HEADER.title[0] && chk == GBA_HEADER.complement ? GBA_HEADER.title : LANG3("slot2","unknown","Unkown Slot2 Title");

		if ( gs().enable3in1Exp ) {
			_fileSizeText = LANG("slot2","3in1 Expansion Pack");
		}
		else if ( gs().enableEwinExp ) {
			_fileSizeText = LANG("slot2","EWin Expansion Pack");
		}
		return;
	}

	if ( shortName == "shortcut:/") _filenameText = LANG3( "setting window", "shortcut" , "shortcut") ;
	else if( showName.size() > 0 && '/' == showName[showName.size()-1] )
        _filenameText = showName.substr( 0, showName.size() - 1 );
    else
        _filenameText = showName;

    //_shortName = shortName;
	_showName = _filenameText;
	
    //dbg_printf("st.st_mtime %d\n", st.st_mtime );
    //struct tm * filetime = localtime(&st.st_mtime);

    st.st_mtime += 312784909; // what the fuck....why i need this adjustment
    struct tm * filetime = gmtime(&st.st_mtime);

    _fileDateText = formatString( LANG("rom info", "file date").c_str(),
        filetime->tm_year + 1900, filetime->tm_mon+1, filetime->tm_mday,
        filetime->tm_hour, filetime->tm_min, filetime->tm_sec );

    dbg_printf("st.st_mtime %d\n", (u32)st.st_mtime );
    dbg_printf( "%d-%d-%d %02d:%02d:%02d\n",
        filetime->tm_year + 1900, filetime->tm_mon+1, filetime->tm_mday,
        filetime->tm_hour, filetime->tm_min, filetime->tm_sec );

    _fileSizeText = formatString( LANG("rom info", "file size").c_str(), getFriendlyFileSizeString(st.st_size).c_str() );

}

void cRomInfoWnd::setRomInfo( const DSRomInfo & romInfo )
{
    _romInfo = romInfo;

	_slotNum = -1;
	if ( _romInfo._fileIconIdx < 0 ) {
		u8 titleIndex = 1; // en
		if ( gs().language > 0 ) titleIndex = 0; // cj
		if ( gs().language > 3 ) titleIndex = gs().language -2 ; //eu
		if ( gs().language > 7 ) titleIndex = 1; // others to en
		if ( gs().language == 5 ) titleIndex = 4; // it
		else if ( gs().language == 6 ) titleIndex = 3; // de

		_romInfoText = unicode_to_local_string( _romInfo.banner().titles[titleIndex], 128, NULL );
	}
	else {
		_romInfoText = fileicons().getExtName(_romInfo._fileIconIdx);
		_romInfoText += " file";
	}

	_isGBArom = false;
    if( _romInfo.isDSRom() ) {

        const char * stLangStrings[] = { "Unknown", "No Save", "4K", "64K", "512K", "2M", "4M", "8M" };
        if( _romInfo.saveInfo().saveType < sizeof(stLangStrings)/sizeof(char*) ) {
            _saveTypeText = formatString( LANG("rom info", "save type").c_str(),
                LANG("save type", stLangStrings[_romInfo.saveInfo().saveType]).c_str() );
        }
        else
            _saveTypeText = "";

		// by bliss to display gamecode
		char code[5] = {}; memcpy ( code, _romInfo.saveInfo().gameCode, 4);
		_saveTypeText = _saveTypeText + ", " + formatString( LANG("rom info", "game code").c_str(), code );
		if ( _romInfo.isHomebrew() ) _buttonSaveType.hide();
    } 
	else {
		_slotNum = getSaveSlot();
		if ( _slotNum < 0 ) 
			_buttonShortcut.hide();
		else
			_buttonShortcut.setText( "\x03 " + LANG3( "rom info", "copy slot" , "Copy Slot") );

		std::string extName;
		size_t lastDotPos = _longName.find_last_of( '.' );
		if( _longName.npos != lastDotPos )
			extName = _longName.substr( lastDotPos );
		else
			extName = "";

		for( size_t i = 0; i < extName.size(); ++i )
			extName[i] = tolower( extName[i] );

		if ( extName == ".gba" ) 
			_isGBArom = true;
		else 
			_buttonSaveType.hide();
	}
}

const DSRomInfo & cRomInfoWnd::getRomInfo()
{
    return _romInfo;
}

void cRomInfoWnd::onOK()
{
	if ( _modalRet != 2 ) 
		cForm::onOK();
}


void cRomInfoWnd::onShow()
{
    centerScreen();
}

void cRomInfoWnd::showSaveType()
{
    //if( !_romInfo.isDSRom() || _romInfo.isHomebrew() )
    if( (!_romInfo.isDSRom() || _romInfo.isHomebrew()) && !_isGBArom  )
     	return;

    cRomSettingWnd settingWnd( 0,0, 252, 188, this, LANG("rom info", "title_rominfo" ) );
    std::vector< std::string > _values;

	if ( _isGBArom ) {

		sGBAHeader header;
		FILE* gbaFile=fopen(_longName.c_str(),"rb");
		fread(&header,1,sizeof(header),gbaFile);
		fclose(gbaFile);
 
		GBAOPTION gbaOption;
		memset(&gbaOption,0,sizeof(gbaOption));
		memcpy(&gbaOption, header.title, sizeof(GBAOPTION) - 8);
		gbaOptionManager().getRomOption(gbaOption);


		_values.push_back(LANG("gba info", "Normal"));
		_values.push_back(LANG("gba info", "Linkage"));
		settingWnd.addSettingItem( LANG("gba info", "run mode") , _values, gbaOption.gbaMode);

		_values.clear();
		_values.push_back( LANG("switches", "Enable" ) );
		_values.push_back( LANG("switches", "Disable" ) );
		settingWnd.addSettingItem( LANG("gba info", "patch sleep") , _values, gbaOption.patchSleep);
		settingWnd.addSettingItem( LANG("gba info", "patch sram") , _values, gbaOption.patchSram);

		_values.clear();
		_values.push_back( LANG("switches", "Default" ) );
		_values.push_back( LANG("exp window", "auto" ) );
		_values.push_back( LANG("exp window", "always" ) );
		settingWnd.addSettingItem( LANG("exp window", "NOR Mode") , _values, gbaOption.norMode);

		char code[5] = {}; 
		settingWnd.setFileInfo(_longName, code, gbaOption.saveSlots);
		settingWnd.disableCheatMenu();

		u32 ret = settingWnd.doModal();
		if( ID_CANCEL == ret )
			return;

		gbaOption.saveSlots = settingWnd.getSaveSlot();
		gbaOption.gbaMode = settingWnd.getItemSelection(0);
		gbaOption.patchSleep = settingWnd.getItemSelection(1);
		gbaOption.patchSram = settingWnd.getItemSelection(2);
		gbaOption.norMode = settingWnd.getItemSelection(3);
		gbaOptionManager().updateOptionList(gbaOption);

		return;
	}

    _values.push_back( LANG("save type", "Unknown" ) );
    _values.push_back( LANG("save type", "No Save" ) );
    _values.push_back( LANG("save type", "4K" ) );
    _values.push_back( LANG("save type", "64K" ) );
    _values.push_back( LANG("save type", "512K" ) );
    _values.push_back( LANG("save type", "2M" ) );
    _values.push_back( LANG("save type", "4M" ) );
    _values.push_back( LANG("save type", "8M" ) );
    settingWnd.addSettingItem( LANG("save type", "text" ), _values, _romInfo.saveInfo().saveType );

	OPTION romOption;
	memset(&romOption,0,sizeof(romOption));
	memcpy(&romOption, &_romInfo.saveInfo(), 18);
	optionManager().getRomOption(romOption);

	/*
	_values.clear();
    _values.push_back (LANG("switches", "Default" ));
	for( int i = 1; i < SLOTSIZE ; i++ ) {
		std::string slot = LANG("rom info", "Slot") + " ";
		slot += ('0' + i );
		_values.push_back (slot);
	}
    settingWnd.addSettingItem( LANG("rom info", "save slot" ), _values, romOption.saveSlots); // 1
	*/

	_values.clear();
    _values.push_back( LANG("switches", "Default" ) );
    _values.push_back( LANG("switches", "Disable" ) );
    _values.push_back( LANG("switches", "Enable" ) );
    settingWnd.addSettingItem( LANG("patches", "download play" ), _values, romOption.downloadPlayPatch); // 1
    settingWnd.addSettingItem( LANG("patches", "reset in game" ), _values, romOption.resetInGame); // 2
    _values.push_back( LANG3("switches", "auto menu","Auto Menu" ) );
    settingWnd.addSettingItem( LANG("patches", "cheating system" ), _values, romOption.cheatingSystem); // 3

	if ( gs().enable3in1Exp || isRumbleInserted() ) {
		_values.clear();
		_values.push_back( LANG("exp window", "off") );
		_values.push_back( LANG("exp window", "low") );
		_values.push_back( LANG("exp window", "mid") );
		_values.push_back( LANG("exp window", "high") );
		settingWnd.addSettingItem( LANG("exp window", "strength" ), _values, romOption.rumbleStrength); // 4
	}

	char code[5] = {}; memcpy( code, _romInfo.saveInfo().gameCode, 4 );
	settingWnd.setFileInfo(_longName, code, romOption.saveSlots);

    u32 ret = settingWnd.doModal();
	if( ID_CANCEL == ret )
        return;

	romOption.saveSlots = settingWnd.getSaveSlot();
	//romOption.saveSlots = settingWnd.getItemSelection(1);
	romOption.downloadPlayPatch = settingWnd.getItemSelection(1);
	romOption.resetInGame= settingWnd.getItemSelection(2);
	romOption.cheatingSystem = settingWnd.getItemSelection(3);
	if ( gs().enable3in1Exp || isRumbleInserted() ) 
		romOption.rumbleStrength = settingWnd.getItemSelection(4);

	optionManager().updateOptionList(romOption);

    _romInfo.saveInfo().saveType = (SAVE_TYPE)settingWnd.getItemSelection( 0 );

    const char * stLangStrings[] = { "Unknown", "No Save", "4K", "64K", "512K", "2M", "4M", "8M" };
    if( _romInfo.saveInfo().saveType < sizeof(stLangStrings)/sizeof(char*) ) {
        _saveTypeText = formatString( LANG("rom info", "save type").c_str(),
            LANG("save type", stLangStrings[_romInfo.saveInfo().saveType]).c_str() );

		char code[5] = {}; memcpy ( code, _romInfo.saveInfo().gameCode, 4);
		_saveTypeText = _saveTypeText + ", " + formatString( LANG("rom info", "game code").c_str(), code );
    }

    saveManager().updateCustomSaveList( _romInfo.saveInfo() );
}

void cRomInfoWnd::setSlot2BootMode()
{
	gs().slot2BootMode = gs().slot2BootMode ? 0 : 1;
    gs().saveSettings();
	_filenameText = gs().slot2BootMode ? LANG3("slot2","passme","Boot in NDS mode (PassMe)").c_str() : LANG3("slot2","gbamode","Boot in GBA mode").c_str() ;
}

int cRomInfoWnd::getSaveSlot() 
{
	std::string extName;
	size_t lastDotPos = _longName.find_last_of( '.' );
	if( _longName.npos != lastDotPos )
		extName = _longName.substr( lastDotPos );
	else
		extName = "";

	for( size_t i = 0; i < extName.size(); ++i )
		extName[i] = tolower( extName[i] );

	if ( extName != ".sav" ) return -1;

	// get slot num
	const char *slotNum = (_longName.c_str() + lastDotPos - 1);

	int slotnum = *slotNum - '0';
	if ( ( *(slotNum -1) == '.') && slotnum > 0 && slotnum < SLOTSIZE &&
			(*(slotNum -2) == 's'  || *(slotNum -2) == 'S') && 
			(*(slotNum -3) == 'd'  || *(slotNum -3) == 'D') && 
			(*(slotNum -4) == 'n'  || *(slotNum -4) == 'N') ) 
		return slotnum;
	else if ( 
			(*(slotNum -0) == 's'  || *(slotNum -0) == 'S') && 
			(*(slotNum -1) == 'd'  || *(slotNum -1) == 'D') && 
			(*(slotNum -2) == 'n'  || *(slotNum -2) == 'N') ) 
		return 0;
	
	return -1;
}

void cRomInfoWnd::copySaveSlot()
{

	int current_slot = _slotNum;
	if ( current_slot < 0 ) return;

    cSettingWnd settingWnd( 0,0, 252, 188, this, LANG("saveslot", "title_copyslot" ) );

    std::vector< std::string > _values;

    if ( current_slot ) _values.push_back (LANG("saveslot", "Default Slot" ));

	for( int i = 1; i < SLOTSIZE ; i++ ) {
		if ( current_slot != i ) {
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

	int target_slot = settingWnd.getItemSelection( 0 );	
	std::string target = _longName.substr(0, _longName.find_last_of('.'));

	if ( current_slot ) 
		target = target.substr(0,target.find_last_of('.'));

	if ( target_slot < current_slot ) {
		if ( target_slot ) {
			target += '.';
			target += ('0' + target_slot);
		}
	}
	else {
		target += '.';
		target += ('0' + target_slot +1) ;
	}

	target += ".sav";

	struct stat st;
	stat(_longName.c_str(),&st);
	copyFile(_longName, target, false, st.st_size);
	_modalRet = 2;
	onOK();
}

void cRomInfoWnd::setShortcut()
{
	std::string shortPath = _shortName;

	if( shortPath == "slot2:/" ) {
		setSlot2BootMode();
		return;
	}
	if ( _slotNum >= 0 ) {
		copySaveSlot();
	}

    if( !_romInfo.isDSRom() ) return;

	if( shortPath == "shortcut:/"  ) {
		std::string title = LANG3("shortcut", "title_delete", "Shortcut Delete?");
		std::string text = LANG3("shortcut", "text_delete", "Are you sure you want to delete your shortcut?");
		u32 result = messageBox(this , title,text, MB_YES | MB_NO);

		if(result==ID_NO)
			return;
		gs().Shortcut = false;
		gs().ShortcutPath = "none";
		gs().ShortcutStr = "none";
		gs().ShortcutLongPath = "none";
		gs().saveSettings();

		_modalRet = 2;
		onOK();
	}
	else if( "" != shortPath ) {

		std::string title = LANG3("shortcut", "title_overwrite", "Shortcut Overwrite?");
		std::string text = LANG3("shortcut", "text_overwrite", "Are you sure you want to create / overwrite your shortcut?");
		//u32 result = messageBox( NULL, title, text, MB_YES | MB_NO );
		
		//cMessageBox msgBox( 0,0, 240, 188, this, title, text, MB_YES | MB_NO );
		u32 result = messageBox(this , title,text, MB_YES | MB_NO);

		//u32 result = msgBox.doModal();

		if(result==ID_NO)
			return;
		gs().Shortcut = true;
		gs().ShortcutPath = shortPath;
		gs().ShortcutStr = _showName;
		gs().ShortcutLongPath = _longName;
		gs().saveSettings();
		
		onShow();
	}

}

