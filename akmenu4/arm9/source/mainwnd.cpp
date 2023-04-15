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
#include <fat.h>

#include "windowmanager.h"
#include "mainwnd.h"
#include "msgbox.h"
#include "dbgtool.h"
#include "systemfilenames.h"
#include "brightnesssetting.h"
#include "timer.h"
#include "../../share/timetool.h"

#include "testcases.h"
#include "datetime.h"

#include "romloader.h"
#include "savemngr.h"
#include "progresswnd.h"
#include "files.h"
#include "inifile.h"
#include "language.h"
#include "testcases.h"
#include "rominfownd.h"
#include "helpwnd.h"

#include "expwnd.h"		// added by bliss for 3in1 exp. 
#include "exptools.h"
#include "ewintools.h"
#include "gbaloader.h"


#include "sdidentify.h"

#include "plugin.h" // bliss
#include "cheatlist.h" //Cheatlist - Normmatt
#include "options.h"
#include "sdspeedwnd.h" 

using namespace akui;

cMainWnd::cMainWnd( s32 x, s32 y, u32 w, u32 h, cWindow * parent, const std::string & text )
: cForm( x, y, w, h, parent, text )
{
    _mainList = NULL;

    _startMenu = NULL;

    _startButton = NULL;

    _brightnessButton = NULL;

    _folderUpButton = NULL;

    _renderDesc = new cBitmapDesc();
}

cMainWnd::~cMainWnd()
{
    if( NULL != _renderDesc )
        delete _renderDesc;
}

void cMainWnd::init()
{
    int x = 0;
    int y = 0;
    int w = 0;
    int h = 0;
    COLOR color = 0;
    std::string file("");
    CIniFile ini( SFN_UI_SETTINGS );
    // self init
    dbg_printf( "mainwnd init() %08x\n", this );
    loadAppearance( SFN_LOWER_SCREEN_BG );
    windowManager().addWindow( this );
    //setAsActiveWindow();

    // init game file list
    //waitMs( 2000 );
    _mainList = new cMainList( 4, 20, 248, 152, this, "main list" );
    _mainList->setRelativePosition( cPoint(4, 20) );
    _mainList->init();
    _mainList->selectChanged.connect( this, &cMainWnd::listSelChange );
    _mainList->selectedRowClicked.connect( this, &cMainWnd::onMainListSelItemClicked );
    _mainList->selectedRowHeadClicked.connect( this, &cMainWnd::onMainListSelItemHeadClicked );
    _mainList->directoryChanged.connect( this, &cMainWnd::onFolderChanged );
    //_mainList->enterDir( "fat0:/" );
    addChildWindow( _mainList );
    dbg_printf( "mainlist %08x\n", _mainList );

    //waitMs( 1000 );

    // init startmenu
    _startMenu = new cStartMenu( 160, 40, 61,108, NULL, "start menu" );
    //_startMenu->setRelativePosition( cPoint(160, 40) );
    _startMenu->init();
    _startMenu->itemClicked.connect( this, &cMainWnd::startMenuItemClicked );
    _startMenu->hide();
    windowManager().addWindow( _startMenu );
    dbg_printf( "startMenu %08x\n", _startMenu );

    // init start button
    x = ini.GetInt( "start button", "x", 0 );
    y = ini.GetInt( "start button", "y", 172 );
    w = ini.GetInt( "start button", "w", 48 );
    h = ini.GetInt( "start button", "h", 10 );
    color = ini.GetInt( "start button", "textColor", 0x7fff );
    file = ini.GetString( "start button", "file", "none" );
    //_startButton = new cButton( 0, 172, 48, 18, this, " Start" );
    //_startButton = new cButton( x, y, w, h, this, "START" );
    bool sbShow = ini.GetInt( "start button", "show", 1 );
	_startButton = new cButton( x, y, w, h, this, sbShow ? "START" :"" );
    _startButton->setRelativePosition( cPoint(x, y) );
    _startButton->loadAppearance( file );
    _startButton->clicked.connect( this, &cMainWnd::startButtonClicked );
    _startButton->setTextColor( color | BIT(15) );
    addChildWindow( _startButton );

    // init brightness button
    x = ini.GetInt( "brightness btn", "x", 240 );
    y = ini.GetInt( "brightness btn", "y", 1 );
    w = ini.GetInt( "brightness btn", "w", 16 );
    h = ini.GetInt( "brightness btn", "h", 16 );
    _brightnessButton = new cButton( x, y, w, h, this, "" );
    _brightnessButton->setRelativePosition( cPoint(x, y) );
    _brightnessButton->loadAppearance( SFN_BRIGHTNESS_BUTTON );
    _brightnessButton->pressed.connect( this, &cMainWnd::brightnessButtonClicked );
    addChildWindow( _brightnessButton );

    x = ini.GetInt( "folderup btn", "x", 0 );
    y = ini.GetInt( "folderup btn", "y", 2 );
    w = ini.GetInt( "folderup btn", "w", 32 );
    h = ini.GetInt( "folderup btn", "h", 16 );
    _folderUpButton = new cButton( x, y, w, h, this, "" );
    _folderUpButton->setRelativePosition( cPoint(x, y) );
    _folderUpButton->loadAppearance( SFN_FOLDERUP_BUTTON );
    _folderUpButton->setSize( cSize(w,h) );
    _folderUpButton->pressed.connect( _mainList, &cMainList::backParentDir );
    addChildWindow( _folderUpButton );


    x = ini.GetInt( "folder text", "x", 20 );
    y = ini.GetInt( "folder text", "y", 2 );
    w = ini.GetInt( "folder text", "w", 160 );
    h = ini.GetInt( "folder text", "h", 16 );
    _folderText    = new cStaticText( x, y, w, h, this, "" );
    _folderText->setRelativePosition( cPoint(x, y) );
    _folderText->setTextColor( ini.GetInt( "folder text", "color", 0 ) );
    addChildWindow( _folderText );


    gdi().setMainEngineLayer( MEL_DOWN );
    gdi().fillRect( 0, 0, 0, 0, 256, 192, selectedEngine() );
    _renderDesc->draw( windowRectangle(), selectedEngine() );
    gdi().present( selectedEngine() );
    gdi().setMainEngineLayer( MEL_UP );

    arrangeChildren();
}

void cMainWnd::draw()
{
    cForm::draw();
    //char fpsText[128];
    //sprintf( fpsText, "fps %.2f\n", timer().getFps() );
    //gdi().setPenColor( 1 );
    //gdi().textOut( 40, 0, fpsText, GE_MAIN );
}

void cMainWnd::listSelChange( u32 i )
{
#ifdef DEBUG
    //dbg_printf( "main list item %d\n", i );
    DSRomInfo info;
    if( _mainList->getRomInfo( i, info ) ) {
        char title[13] = {}; memcpy( title, info.saveInfo().gameTitle, 12 );
        char code[5] = {}; memcpy( code, info.saveInfo().gameCode, 4 );
        u16 crc = swiCRC16( 0xffff, ((unsigned char *)&(info.banner())) + 32, 0x840 - 32);
        dbg_printf( "%s %s %04x %d %04x/%04x\n",
            title, code, info.saveInfo().gameCRC, info.isDSRom(), info.banner().crc,crc );
        //dbg_printf("sizeof banner %08x\n", sizeof( info.banner() ) );
    }
#endif//DEBUG
}

void cMainWnd::startMenuItemClicked( s16 i )
{
    dbg_printf( "start menu item %d\n", i );
    //messageBox( this, "Power Off", "Are you sure you want to turn off ds?", MB_YES | MB_NO );

    if( START_MENU_ITEM_COPY == i ) {
        if( "" == _mainList->getSelectedFullPath() )
            return;
        struct stat st;
        stat( _mainList->getSelectedFullPath().c_str(), &st );
        if( st.st_mode & S_IFDIR ) {
            messageBox( this, LANG("no copy dir", "title"), LANG("no copy dir", "text"), MB_YES | MB_NO );
            return;
        }
        setSrcFile( _mainList->getSelectedFullPath(), SFM_COPY );
    }

    else if( START_MENU_ITEM_CUT == i ) {
        if( "" == _mainList->getSelectedFullPath() )
            return;
        struct stat st;
        stat( _mainList->getSelectedFullPath().c_str(), &st );
        if( st.st_mode & S_IFDIR ) {
            messageBox( this, LANG("no copy dir", "title"), LANG("no copy dir", "text"), MB_YES | MB_NO );
            return;
        }
        setSrcFile( _mainList->getSelectedFullPath(), SFM_CUT );
    }

    else if( START_MENU_ITEM_PASTE == i ) {
        bool ret = copyOrMoveFile( _mainList->getCurrentDir() );
        if( ret ) // refresh current directory
            _mainList->enterDir( _mainList->getCurrentDir() );
    }

    else if( START_MENU_ITEM_DELETE == i ) {
        std::string fullPath = _mainList->getSelectedFullPath();
        std::string shortPath = _mainList->getSelectedShortPath();
        if( "" != fullPath ) {
            bool ret = deleteFile( shortPath, fullPath );
            if( ret )
                _mainList->enterDir( _mainList->getCurrentDir() );
        }
    }

    else if( START_MENU_ITEM_PATCHES == i ) {
        setPatches();
        return;
    }

    else if( START_MENU_ITEM_SETTING == i ) {
        setSystemParam();
    }

    else if( START_MENU_ITEM_INFO == i ) {
        showFileInfo();
    }

    else if( START_MENU_ITEM_HELP == i ) {
        CIniFile ini(SFN_UI_SETTINGS); //(256-)/2,(192-128)/2, 220, 128
        u32 w = 200;
        u32 h = 160;
        w = ini.GetInt( "help window", "w", w );
        h = ini.GetInt( "help window", "h", h );
        cHelpWnd * helpWnd = new cHelpWnd( (256-w)/2, (192-h)/2, w, h, this, LANG("help window", "title" ) );
        helpWnd->doModal();
        delete helpWnd;
    }
			
}

void cMainWnd::startButtonClicked()
{
    if( _startMenu->isVisible() )
        _startMenu->hide();
    else {
        _startMenu->show();
        _startMenu->setAsActiveWindow();
        windowManager().setPopUpWindow( _startMenu );
    }
}

void cMainWnd::brightnessButtonClicked()
{
    u8 currentLevel = getBrightness();
    setBrightness( currentLevel + 1 );
    gs().brightness = currentLevel + 1;
    //gs().saveSettings();
}

cWindow& cMainWnd::loadAppearance(const std::string & aFileName )
{
    _renderDesc->loadData( aFileName );
    _renderDesc->setBltMode( BM_BITBLT );
    return *this;
}

bool cMainWnd::process( const cMessage & msg )
{
    bool ret = false;

    ret = cForm::process( msg );

    if( !ret ) {
        if( msg.id() > cMessage::keyMessageStart && msg.id()
            < cMessage::keyMessageEnd )
        {
            ret = processKeyMessage( (cKeyMessage &)msg );
        }

        if( msg.id() > cMessage::touchMessageStart && msg.id()
            < cMessage::touchMessageEnd )
        {
            ret = processTouchMessage( (cTouchMessage &)msg );
        }
    }
    return ret;
}

bool cMainWnd::processKeyMessage( const cKeyMessage & msg )
{
    bool ret = false;
    if( msg.id() == cMessage::keyDown )
    {
        switch( msg.keyCode() )
        {
        case cKeyMessage::UI_KEY_DOWN:
            _mainList->selectNext();
            ret = true;
            break;
        case cKeyMessage::UI_KEY_UP:
            _mainList->selectPrev();
            ret = true;
            break;

        case cKeyMessage::UI_KEY_LEFT:
            _mainList->selectRow( _mainList->selectedRowId() - _mainList->visibleRowCount() );
            ret = true;
            break;

        case cKeyMessage::UI_KEY_RIGHT:
            _mainList->selectRow( _mainList->selectedRowId() + _mainList->visibleRowCount() );
            ret = true;
            break;
        case cKeyMessage::UI_KEY_A:
            onKeyAPressed();
            ret = true;
            break;
        case cKeyMessage::UI_KEY_B:
            onKeyBPressed();
            ret = true;
            break;
        case cKeyMessage::UI_KEY_Y:
            onKeyYPressed();
            ret = true;
            break;
        case cKeyMessage::UI_KEY_X: {
            const std::string dir =  _mainList->getCurrentDir();
            if( dir.length() < 5 ) {
                _mainList->enterDir( "fat0:/" );
            } else if( dir.substr( 0, 5 ) == "fat0:" ) {
                _mainList->enterDir( "fat1:/" );
            } else {
                _mainList->enterDir( "fat0:/" );
            }
            break;
        }
        case cKeyMessage::UI_KEY_START:
            startButtonClicked();
            ret = true;
            break;
        case cKeyMessage::UI_KEY_SELECT:
            _mainList->setViewMode( (cMainList::VIEW_MODE)(_mainList->getViewMode() ^ 1) );
            ret = true;
            break;
        case cKeyMessage::UI_KEY_L:
            _mainList->backParentDir();
            ret = true;
            break;
        case cKeyMessage::UI_KEY_R:
            brightnessButtonClicked();
#ifdef DEBUG
            gdi().switchSubEngineMode();gdi().present( GE_SUB );
#endif//DEBUG
            ret = true;
            break;
        default:
            {}
        };
    }
    return ret;
}

bool cMainWnd::processTouchMessage( const cTouchMessage & msg )
{
    bool ret = false;

    return ret;
}

void cMainWnd::onKeyYPressed()
{
#ifdef DEBUG
    // hardware software version check
    {
        u32 getHWVer[2] = { 0xd1000000, 0x00000000 };

        u32 hwVer = 0;
        ioRpgSendCommand( getHWVer, 4, 0, &hwVer );
        hwVer &= 0xff;

        u8 nandDriverVer = getNandDriverVer();

        dbg_printf("HW: %02x NDD: %02x\n", hwVer, nandDriverVer );

    }
#endif//#ifdef DEBUG
    showFileInfo();
}

void cMainWnd::onMainListSelItemClicked( u32 index )
{

}

void cMainWnd::onMainListSelItemHeadClicked( u32 index )
{
    onKeyAPressed();
}


void cMainWnd::onKeyAPressed()
{
    launchSelected();
}

void cMainWnd::launchSelected()
{
    std::string shortPath = _mainList->getSelectedShortPath();

    if( shortPath[shortPath.size()-1] == '/' ) {
        _mainList->enterDir( shortPath );
        return;
    }

    std::string fullPath = _mainList->getSelectedFullPath();
	execute(shortPath, fullPath,false);
/*
    DSRomInfo rominfo;
    if( !_mainList->getRomInfo( _mainList->selectedRowId(), rominfo ) )
        return;

    //rominfo.loadDSRomInfo( fullPath, false );

    if( !rominfo.isDSRom() ) {
		dbg_printf("Load GBA\n");
		if ( gs().enable3in1Exp || gs().enableEwinExp ) CGbaLoader(fullPath).Load(); // added by bliss for 3in1 exp.

		dbg_printf("Load PLUGIN\n");
		plugin().loadPlugin(fullPath);
        return;
	}

    dbg_printf("(%s)\n", fullPath.c_str() );
    dbg_printf("%d\n", fullPath[fullPath.size()-1] );

    if( !rominfo.isHomebrew() ) {

        // reading speed setting
        std::string disk = fullPath.substr( 0, 5 );
        if( disk == "fat0:" || disk == "FAT0:" ) { // if we are using internal NAND flash, use fast reading setting
            NDSHeader.cardControl13 = 0x004060d0;    //
        } else if( disk == "fat1:" || disk == "FAT1:" ) { // check sd card, warn user if sd card is not suitable for running offical ds program
            u32 sdSpeed = sdidCheckSDSpeed( 8192 );
            if( sdSpeed >= 0x2000 ) {
                std::string model = sdidGetSDManufacturerName() + " " + sdidGetSDName();
                std::string title = LANG("unsupported sd", "title");
                std::string text = LANG("unsupported sd", "text");
                text = formatString( text.c_str(), model.c_str() );
                messageBox( NULL, title, text, MB_OK );
                return;
            } else {
                NDSHeader.cardControl13 = 0x00406000 | (sdSpeed & 0x1FFF);
            }
        }

        // restore save data only for offical programs
        SAVE_TYPE st = (SAVE_TYPE)rominfo.saveInfo().saveType;
        if( ST_UNKNOWN == st )
            st = ST_AUTO;
        bool ret = saveManager().restoreSaveData( fullPath.c_str(), st );
        if( !ret ) {
            u32 ret = messageBox( this, LANG( "restore save fail", "title" ),
                LANG( "restore save fail", "text" ), MB_YES | MB_NO );
            if( ID_YES != ret )
                return;
        }
		//3in1 support added by bliss
		if ( gs().enable3in1Exp ) {
			 u32 gameCode;
			 memcpy(&gameCode,rominfo.saveInfo().gameCode,sizeof(gameCode)); //because alignment
			 if(gameCode==0x4a524255||gameCode==0x50524255)
			 {
				 cExpansion::Reset();
				 cExpansion::SetRompage(0x300);
				 cExpansion::OpenNorWrite();
				 cExpansion::EnableBrowser();
			 }
		}
		else if ( gs().enableEwinExp ) {
			 u32 gameCode;
			 memcpy(&gameCode,rominfo.saveInfo().gameCode,sizeof(gameCode)); //because alignment
			 if(gameCode==0x4a524255||gameCode==0x50524255)
			 {
				 cEwin::EnableBrowser();
			 }
		}
		
		INPUT & inputs = updateInput(); 
		if ( (inputs.keysHeld&(KEY_L|KEY_A))==(KEY_L|KEY_A) || gs().cheatingSystem == 2 ) { 
			std::string showName =  _mainList->getSelectedLFN();
			showCheatList((const char *)rominfo.saveInfo().gameCode, fullPath, showName,true);
		}
    } else {
        saveManager().saveLastInfo( fullPath );
    }

	//by bliss to autorun
	CIniFile fi( SFN_LAST_SAVEINFO ) ;
	fi.SetString( "Save Info", "shortPathNDS", shortPath );
	fi.SetString( "Save Info", "Plugin", "" );
	fi.SaveIniFile( SFN_LAST_SAVEINFO );
    loadRom( shortPath );
*/

}


void cMainWnd::onKeyBPressed()
{
    _mainList->backParentDir();
}


void cMainWnd::setSystemParam()
{
    cSettingWnd settingWnd( 0,0, 252, 188, this, LANG("system setting", "title" ) );

    u8 currentLang = gs().language;
    u8 currentFileListType = gs().fileListType;
    std::string currentUIStyle = gs().uiName;
    std::vector< std::string > _values;
    u32 uiIndex = 0;


    //// sd speed
    //for( size_t i = 0; i < 4; ++i ) {
    //    std::string itemName = formatString( "item%d", i );
    //    _values.push_back( LANG("sd speed", itemName ) );
    //}
    //settingWnd.addSettingItem( LANG("sd speed", "text" ), _values, gs().sdCardSpeed );

    // user interface style
    _values.clear();
    std::vector< std::string > uiNames;
    DIR_ITER * dir = diropen( SFN_SYSTEM_DIR"ui/" );
    if( NULL != dir ) {
        struct stat st;
        char filename[256]; // to hold a full filename and string terminator
        char longFilename[512];

        //while (dirnextl(dir, filename, longFilename, &st) == 0) {
        while (dirnextl(dir, filename, longFilename, &st) == 0) { 
            std::string fn( filename );
            std::string lfn( longFilename );
            if( 0 == longFilename[0] )
                lfn = fn;
            if( lfn != ".." && lfn != "." && (st.st_mode & S_IFDIR) )
                _values.push_back( lfn );
        }
    } else {
        _values.push_back( gs().uiName );
    }
    std::sort( _values.begin(), _values.end() );
    //std::vector< std::string >::iterator findRet = std::find( _values.begin(), _values.end(), gs().uiName );
    //if( _values.end() != findRet ) {
    //    uiIndex = std::distance( _values.begin(), findRet ); ;
    //}
    for( size_t i = 0; i < _values.size(); ++i ) {
        if( 0 == stricmp( _values[i].c_str(), gs().uiName.c_str() ) )
            uiIndex = i;
    }

    uiNames = _values;
    settingWnd.addSettingItem( LANG("ui style", "text" ), _values, uiIndex );

    // language
    _values.clear();
    //for( size_t i = 0; i < 8; ++i ) {
	for( size_t i = 0; i < 11; ++i ) { // by bliss. 8-ko,9-nl,10-custom
        std::string itemName = formatString( "item%d", i );
		if ( LANG3("language", itemName,"") == "" ) break;
        _values.push_back( LANG("language", itemName ) );
    }
    settingWnd.addSettingItem( LANG("language", "text" ), _values, gs().language );

    // brightness
    _values.clear();
    for( size_t i = 0; i < 4; ++i ) {
        std::string itemName = formatString( "%d", i + 1 );
        _values.push_back( itemName );
    }
    settingWnd.addSettingItem( LANG("brightness", "text" ), _values, gs().brightness );

    // file list type
    _values.clear();
    for( size_t i = 0; i < 3; ++i ) {
        std::string itemName = formatString( "item%d", i );
        _values.push_back( LANG("filelist type", itemName ) );
    }
    settingWnd.addSettingItem( LANG("filelist type", "text" ), _values, gs().fileListType );

    // rom trim
    _values.clear();
    for( size_t i = 0; i < 2; ++i ) {
        std::string itemName = formatString( "item%d", i );
        _values.push_back( LANG("rom trim", itemName ) );
    }
    settingWnd.addSettingItem( LANG("rom trim", "text" ), _values, gs().romTrim );


	// by bliss
	cButton *_button = new cButton(0,0,46,18,&settingWnd,"Advanced");
	_button->setStyle( cButton::press );
	_button->setText( "\x04 " + LANG( "advanced setting", "Advanced" ) );
	_button->setTextColor( uis().buttonTextColor );
	_button->loadAppearance( SFN_BUTTON4 );
	_button->clicked.connect( &settingWnd, &cSettingWnd::onCustom);
	settingWnd.addChildWindow( _button);
	settingWnd._customButton = true;
	_button->setRelativePosition( cPoint(8, settingWnd.size().y - _button->size().y -  4) );
	settingWnd.arrangeChildren();

    u32 ret = settingWnd.doModal();

	// modified by bliss
	delete _button;
	if ( ret == 2 ) {
		int scrollSpeed =0;
		if ( gs().scrollSpeed == SCROLL_FAST ) scrollSpeed = 0;
		else if ( gs().scrollSpeed == SCROLL_MEDIUM ) scrollSpeed = 1;
		else if ( gs().scrollSpeed == SCROLL_SLOW ) scrollSpeed = 2;

		cSettingWnd advancedWnd( 0,0, 252, 188, this, LANG("advanced setting", "title" ) );
		_values.clear();
		_values.push_back( LANG("switches", "Disable" ) );
		_values.push_back( LANG("switches", "Enable" ) );
		advancedWnd.addSettingItem( LANG("advanced setting", "Show Hidden Files" ), _values, gs().showHiddenFiles);
		//advancedWnd.addSettingItem( LANG("advanced setting", "Hide Extension Name" ), _values, gs().hideExtNDS);
		_values.clear();
		_values.push_back( LANG("romnametype", "Normal" ) );
		_values.push_back( LANG("romnametype", "HideExtName" ) );
		_values.push_back( LANG("romnametype", "BannerTitle" ) );
		advancedWnd.addSettingItem( LANG("advanced setting", "Rom Name Type" ), _values, gs().romNameTypeInList);
		_values.clear();
		_values.push_back( LANG("scrolling", "Fast" ) );
		_values.push_back( LANG("scrolling", "Medium" ) );
		_values.push_back( LANG("scrolling", "Slow" ) );
		advancedWnd.addSettingItem( LANG("advanced setting", "Scrolling Speed" ), _values, scrollSpeed);

		u32 fontIndex = 0;
		std::vector< std::string > fontNames;
		if ( gs().language == 8 || gs().language == 3 ) { // select a font

			_values.clear();
			std::string dName = SFN_LANGUAGE_DIRECTORY + gs().langDirectory;
			DIR_ITER * dir = diropen( dName.c_str()  );
			if( NULL != dir ) {
				struct stat st;
				char filename[256]; // to hold a full filename and string terminator
				char longFilename[512];

				while (dirnextl(dir, filename, longFilename, &st) == 0) { 
					std::string fn( filename );
					std::string lfn( longFilename );
					if( 0 == longFilename[0] )
						lfn = fn;
					if( lfn != ".." && lfn != "." && !(st.st_mode & S_IFDIR) ) {
						std::string extName;
			            size_t lastDotPos = lfn.find_last_of( '.' );
						if( lfn.npos != lastDotPos )
							extName = lfn.substr( lastDotPos );
						else
							extName = "";
						for( size_t j = 0; j < extName.size(); ++j )
							extName[j] = tolower( extName[j] );
						if ( extName == ".fon" ) 
							_values.push_back( lfn.substr(0,lastDotPos) );
					}
				}
			} else {
				_values.push_back( "LOCAL" );
			}
			std::sort( _values.begin(), _values.end() );
			for( size_t i = 0; i < _values.size(); ++i ) {
				if( 0 == stricmp( _values[i].c_str(), gs().fontName.c_str() ) )
					fontIndex = i;
			}

			fontNames = _values;
			advancedWnd.addSettingItem( LANG("advanced setting", "font" ), _values, fontIndex );
		}

		// attach sd card speed button
		cButton *_speedbutton = new cButton(0,0,46,18,&settingWnd,"SD Speed");
		_speedbutton->setStyle( cButton::press );
		_speedbutton->setText( "\x04 " + LANG( "advanced setting", "SD Speed" ) );
		_speedbutton->setTextColor( uis().buttonTextColor );
		_speedbutton->loadAppearance( SFN_BUTTON4 );
		_speedbutton->clicked.connect( &advancedWnd, &cSettingWnd::onCustom);
		advancedWnd.addChildWindow( _speedbutton);
		advancedWnd._customButton = true;
		_speedbutton->setRelativePosition( cPoint(8, advancedWnd.size().y - _speedbutton->size().y -  4) );
		advancedWnd.arrangeChildren();

		u32 ret_adv= advancedWnd.doModal();

		delete _speedbutton;
		if ( ret_adv == 2 ) {
			std::string model = sdidGetSDManufacturerName() + " " + sdidGetSDName();
			cSdSpeedWnd sdSpeedWnd( 0,0, 252, 140, this, model );
			sdSpeedWnd.doModal();
			return;
		}

		if ( ret_adv!= ID_CANCEL ) {
			gs().showHiddenFiles = advancedWnd.getItemSelection(0);
			//gs().hideExtNDS = advancedWnd.getItemSelection(1);
			gs().romNameTypeInList = advancedWnd.getItemSelection(1);

			if ( advancedWnd.getItemSelection(2) == 0 )  gs().scrollSpeed = SCROLL_FAST;
			else if ( advancedWnd.getItemSelection(2) == 1 )  gs().scrollSpeed = SCROLL_MEDIUM;
			else if ( advancedWnd.getItemSelection(2) == 2 )  gs().scrollSpeed = SCROLL_SLOW;

			if ( gs().language == 8 || gs().language == 3 ) { // select a font
				u32 fontIndexAfter = advancedWnd.getItemSelection(3);
				if ( fontIndex != fontIndexAfter ) {
					u32 ret = messageBox( this, LANG("ui style changed", "title"), LANG("ui style changed", "text"), MB_YES | MB_NO );
					if( ID_YES == ret ) {
						gs().fontName = fontNames[fontIndexAfter];
						gs().saveSettings();
						loadRom( "fat0:/akmenu4.nds" );
					}
				}
			}

			gs().saveSettings();
			if ( _mainList->getCurrentDir() == "" ) _mainList->enterDir("...");
			else _mainList->enterDir( _mainList->getCurrentDir() );
		}
		//cExpWnd * expWnd = new cExpWnd( (256-w)/2, (192-h)/2, w, h, this, LANG("exp window", "title" ) );
		//expWnd->doModal();
		//delete expWnd;
		return;
	}

    if( ID_CANCEL == ret )
        return;

    //gs().sdCardSpeed = settingWnd.getItemSelection( 0 );
    u32 uiIndexAfter = settingWnd.getItemSelection( 0 );
    gs().brightness = settingWnd.getItemSelection( 2 );
    gs().setLanguage( settingWnd.getItemSelection( 1 ) );
    gs().fileListType = settingWnd.getItemSelection( 3 );
    gs().romTrim = settingWnd.getItemSelection( 4 );

    if( uiIndex != uiIndexAfter ) {
        u32 ret = messageBox( this,
            LANG("ui style changed", "title"),
            LANG("ui style changed", "text"), MB_YES | MB_NO );
        if( ID_YES == ret ) {
            gs().uiName = uiNames[uiIndexAfter];
            gs().saveSettings();
            loadRom( "fat0:/akmenu4.nds" );
        }
    }

    gs().saveSettings();

    if( gs().language != currentLang ) {
		gs().setLanguage( currentLang ); // modified by bliss. fix message box
        u32 ret = messageBox( this,
            LANG("language changed", "title"),
            LANG("language changed", "text"), MB_YES | MB_NO );
        if( ID_YES == ret )
            loadRom( "fat0:/akmenu4.nds" );
    }


    if( gs().fileListType != currentFileListType ) {
        _mainList->enterDir( _mainList->getCurrentDir() );
    }

}

void cMainWnd::showFileInfo()
{
    DSRomInfo rominfo;
    if( !_mainList->getRomInfo( _mainList->selectedRowId(), rominfo ) ) {
        return;
    }

    dbg_printf("show '%s' info\n", _mainList->getSelectedShortPath().c_str() );

    CIniFile ini(SFN_UI_SETTINGS); //(256-)/2,(192-128)/2, 220, 128
    u32 w = 240;
    u32 h = 144;
    w = ini.GetInt( "rom info window", "w", w );
    h = ini.GetInt( "rom info window", "h", h );

    cRomInfoWnd * romInfoWnd = new cRomInfoWnd( (256-w)/2, (192-h)/2, w, h, this, LANG("rom info", "title" ) );
    std::string shortName = _mainList->getSelectedShortPath();
    std::string showName = _mainList->getSelectedLFN();

	//for shortcut
	std::string longname = (shortName == "shortcut:/") ? gs().ShortcutLongPath : _mainList->getSelectedFullPath();
	romInfoWnd->setFullPath( longname );
    romInfoWnd->setRomInfo( rominfo );

    romInfoWnd->setFileInfo( shortName, showName );
    u8 ret = romInfoWnd->doModal();
    rominfo = romInfoWnd->getRomInfo();
    _mainList->setRomInfo( _mainList->selectedRowId(), rominfo );

    delete romInfoWnd;
    if ( ret == 2 ) {
		_mainList->enterDir( _mainList->getCurrentDir() == "" ? "...": _mainList->getCurrentDir() );
	}
}

void cMainWnd::onFolderChanged()
{
    std::string dirShowName = _mainList->getCurrentDir();
    if( dirShowName.substr( 0, 5 ) == "fat0:" )
        dirShowName.replace( 0, 4, "Flash" );
    else if( dirShowName.substr( 0, 5 ) == "fat1:" )
        dirShowName.replace( 0, 4, "SD" );
    else if( "slot2:/" == _mainList->getSelectedShortPath() ) {
        u8 chk = 0;
        for( u32 i = 0xA0; i < 0xBD; ++i ) {
            chk = chk - *(u8*)(0x8000000+i);
        }
        chk = (chk- 0x19) & 0xff;
        if( chk != GBA_HEADER.complement ) {
            dbg_printf("chk %02x header checksum %02x\n", chk, GBA_HEADER.complement );
            std::string title = LANG("no gba card", "title");
            std::string text = LANG("no gba card", "text");
            messageBox( NULL, title, text, MB_OK );
			//comment it otherwise it'll hang up when enterDir() invoke getrominfo at shortcut:/
            //_mainList->enterDir( "..." );
            _mainList->selectRow( 2 );
            return;
        }
		if ( gs().slot2BootMode )  loadRom("slot2:/"); else  CGbaLoader("").LaunchGBA();

	} else if( "shortcut:/" == _mainList->getSelectedShortPath() ) {
		execute(gs().ShortcutPath, gs().ShortcutLongPath,false);
	}
	
    dbg_printf("%s\n", _mainList->getSelectedShortPath().c_str() );
    dbg_printf("%s\n", _mainList->getSelectedFullPath().c_str() );

    _folderText->setText( dirShowName );
}

void cMainWnd::setPatches()
{
    cSettingWnd settingWnd( 0,0, 252, 188, this, LANG("patches", "title" ) );
    std::vector< std::string > _values;
    _values.push_back( LANG("switches", "Disable" ) );
    _values.push_back( LANG("switches", "Enable" ) );

    // download play compatiblity patch
    settingWnd.addSettingItem( LANG("patches", "download play" ), _values, gs().downloadPlayPatch );

    // reset to menu in game
    settingWnd.addSettingItem( LANG("patches", "reset in game" ), _values, gs().resetInGame );

	// enable  autorun - bliss
    settingWnd.addSettingItem( LANG("autorun", "text" ), _values, gs().autorunWithLastRom);

    // cheating system
    _values.push_back( LANG3("switches", "auto menu","Auto Menu" ) );
    settingWnd.addSettingItem( LANG("patches", "cheating system" ), _values, gs().cheatingSystem );

    // cheat XML select - Smiths
    _values.clear();
    u32 cheatindex = 0;
    std::string extName;
    std::vector< std::string > cheatDBs;
    DIR_ITER * dir = diropen( SFN_CHEATS_DIRECTORY );
    if( NULL != dir ) {
        struct stat st;
        char filename[256];
        char longFilename[512];

        while (dirnextl(dir, filename, longFilename, &st) == 0) {
			if ( st.st_mode & S_IFDIR ) continue;

            std::string fn( filename );
            std::string lfn( longFilename );
            if( 0 == longFilename[0] )
                lfn = fn;
            size_t lastDotPos = lfn.find_last_of( '.' );
            if( lfn.npos != lastDotPos )
                extName = lfn.substr( lastDotPos );
            else
                extName = "";
            if( extName == ".XML" || extName == ".xml" || extName == ".DAT" || extName == ".dat" )
                _values.push_back( lfn );
        }
    } else {
        _values.push_back( gs().cheatsDB );
    }
    std::sort( _values.begin(), _values.end() );
    for( size_t i = 0; i < _values.size(); ++i ) {
        _values[i].insert(0, SFN_CHEATS_DIRECTORY );
        if( 0 == stricmp( _values[i].c_str(), gs().cheatsDB.c_str() ) ) { cheatindex = i; }
        _values[i].erase(0,_values[i].find_last_of('/')+1);
    }

    cheatDBs = _values;
	if ( cheatDBs.size() )
		settingWnd.addSettingItem( LANG("patches", "cheatdb select" ), _values, cheatindex );


	// make a 3in1 expansion setting button
	//cButton *_button = new cButton(0,0,46,18,this,"3in1 Exp.");
	cButton *_button = new cButton(0,0,46,18,&settingWnd,"3in1 Exp.");
	if ( gs().enable3in1Exp ) {
		_button->setStyle( cButton::press );
		_button->setText( "\x04 " + LANG( "patches", "3in1" ) );
		_button->setTextColor( uis().buttonTextColor );
		_button->loadAppearance( SFN_BUTTON4 );
		_button->clicked.connect( &settingWnd, &cSettingWnd::onCustom);
		settingWnd._customButton = true;
		settingWnd.addChildWindow( _button);
		_button->setRelativePosition( cPoint(8, settingWnd.size().y - _button->size().y -  4) );
		settingWnd.arrangeChildren();
	}

    u32 ret = settingWnd.doModal();

	// modified by bliss
	delete _button;
	if ( ret == 2 ) {
		u32 w = 250;
		u32 h = 100;
		cExpWnd * expWnd = new cExpWnd( (256-w)/2, (192-h)/2, w, h, this, LANG("exp window", "title" ) );
		expWnd->doModal();
		delete expWnd;
		return;
	}

    if( ID_CANCEL == ret )
        return;

    gs().downloadPlayPatch = settingWnd.getItemSelection( 0 );
    gs().cheatingSystem = settingWnd.getItemSelection( 3 );
    gs().resetInGame = settingWnd.getItemSelection( 1 );
    gs().autorunWithLastRom = settingWnd.getItemSelection( 2 ); // by bliss

	if ( cheatDBs.size() ) {
		std::string selectedDB = _values[settingWnd.getItemSelection( 4 )];
		selectedDB.insert(0, SFN_CHEATS_DIRECTORY);
		gs().cheatsDB = selectedDB ; //cheats XML - Smiths
	}

    gs().saveSettings();
}

void cMainWnd::showCheatList(const char *gamecode, std::string &fullPath, bool cheat_on ) //cheatlist - Normmatt
{
	if (cheat_on && !gs().cheatingSystem) gs().cheatingSystem = 1; // automatic cheating on
    //dbg_printf("show '%s' info\n", _mainList->getSelectedShortPath().c_str() );

    CIniFile ini(SFN_UI_SETTINGS); //(256-)/2,(192-128)/2, 220, 128
    u32 w = 240;
    //u32 h = 144;
    u32 h = 168;
    w = ini.GetInt( "cheat window", "w", w );
    h = ini.GetInt( "cheat window", "h", h );

    cCheatListWnd * cheatlistWnd = new cCheatListWnd( (256-w)/2, (192-h)/2, w, h, this, LANG("cheats", "title" ) );
    //std::string fullPath = shortCut? gs().ShortcutLongPath :_mainList->getSelectedFullPath();
    //std::string showName = shortCut? gs().ShortcutStr : _mainList->getSelectedLFN();

    char code[5] = {}; memcpy( code, gamecode, 4 );
    //if ( cheatlistWnd->setFileInfo( fullPath, showName, code ) ) {
    cheatlistWnd->setFileInfo( fullPath, code ) ;
		cheatlistWnd->ReadList();
		cheatlistWnd->doModal();
	//}

    delete cheatlistWnd;
}

void cMainWnd::execute( std::string & shortPath, std::string &fullPath, bool autorun) 
{

	if ( fullPath == "" || shortPath == "" ) 
		return;

	DSRomInfo rominfo;
	rominfo.loadDSRomInfo( fullPath.c_str(), false);

	if( !rominfo.isDSRom() ) { // check for gba or plugin 
		dbg_printf("Load GBA\n");
		if ( gs().enable3in1Exp || gs().enableEwinExp ) CGbaLoader(fullPath).Load(); 

		dbg_printf("Load PLUGIN\n");
		plugin().loadPlugin(fullPath);
		return;
	}

	if( !rominfo.isHomebrew() ) {
		// reading speed setting
		std::string disk = fullPath.substr( 0, 5 );
		if( disk == "fat0:" || disk == "FAT0:" ) { 
			NDSHeader.cardControl13 = 0x004060d0;  
		} else if( disk == "fat1:" || disk == "FAT1:" ) { 
			u32 sdSpeed = sdidCheckSDSpeed( 8192 );
			if( sdSpeed >= 0x2000 ) {
				std::string model = sdidGetSDManufacturerName() + " " + sdidGetSDName();
				std::string title = LANG("unsupported sd", "title");
				std::string text = LANG("unsupported sd", "text");
				text = formatString( text.c_str(), model.c_str() );
				messageBox( NULL, title, text, MB_OK );
				return;
			} else {
				NDSHeader.cardControl13 = 0x00406000 | (sdSpeed & 0x1FFF);
			}
		} // end of speed check

		OPTION romOption;
		memset(&romOption,0,sizeof(romOption));
		memcpy(&romOption, &rominfo.saveInfo(), 18);
		optionManager().getRomOption(romOption);

		if ( autorun ) { // rebuilding SRAM
			saveManager().rebuildSaveTable();
		}
		else { // restore save data
			SAVE_TYPE st = (SAVE_TYPE)rominfo.saveInfo().saveType;
			if( ST_UNKNOWN == st ) st = ST_AUTO;

			bool ret = saveManager().restoreSaveData( fullPath.c_str(), st ,romOption.saveSlots);
			if( !ret ) {
				u32 ret = messageBox( this, LANG( "restore save fail", "title" ),
				LANG( "restore save fail", "text" ), MB_YES | MB_NO );
				if( ID_YES != ret )
					return;
			}
		}

		// expansion packs
		u32 gameCode;
		memcpy(&gameCode,rominfo.saveInfo().gameCode,sizeof(gameCode)); //because alignment
		if ( gs().enable3in1Exp ) {
			 if(gameCode==0x4a524255||gameCode==0x50524255)
			 {
				 cExpansion::Reset();
				 cExpansion::SetRompage(0x300);
				 cExpansion::OpenNorWrite();
				 cExpansion::EnableBrowser();
			 }
			 // 3in1 rumbleOption
			 if ( romOption.rumbleStrength )
				cExpansion::SetShake(0xEF + romOption.rumbleStrength);
		}
		else if ( gs().enableEwinExp ) {
			 if(gameCode==0x4a524255||gameCode==0x50524255)
			 {
				 cEwin::EnableBrowser();
			 }
		} 
		else if ( isRumbleInserted() ) {
			setRumble(romOption.rumbleStrength);
		}



		if ( romOption.downloadPlayPatch ) gs().downloadPlayPatch = romOption.downloadPlayPatch - 1 ;
		if ( romOption.cheatingSystem ) gs().cheatingSystem = romOption.cheatingSystem - 1;
		if ( romOption.resetInGame ) gs().resetInGame = romOption.resetInGame - 1;

		// call cheat menu
		INPUT & inputs = updateInput(); 
		if ( (inputs.keysHeld&(KEY_L|KEY_A))==(KEY_L|KEY_A) || gs().cheatingSystem == 2 ) { 
			showCheatList((const char *)rominfo.saveInfo().gameCode, fullPath,true); 
		}

	} // end of "is not Homebrew"
	else {
		saveManager().saveLastInfo( fullPath );
	}

	//if ( !autorun ) { // save saveinfo
	CIniFile fi( SFN_LAST_SAVEINFO ) ;
	fi.SetString( "Save Info", "shortPathNDS", shortPath );
	fi.SetString( "Save Info", "Plugin", "" );
	fi.SaveIniFile( SFN_LAST_SAVEINFO );
	//}
	loadRom( shortPath );

}
