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









#include <sys/dir.h>
#include <fat.h>
#include "mainlist.h"
#include "files.h"
#include "dbgtool.h"
#include "startmenu.h"
#include "systemfilenames.h"
#include "romloader.h"
#include "windowmanager.h"
#include "../../share/ipctool.h"
#include "../../share/timetool.h"
#include "../../share/memtool.h"
#include "inifile.h"
#include "unknown_banner_bin.h"
#include "nds_save_banner_bin.h"
#include "nand_banner_bin.h"
#include "microsd_banner_bin.h"
#include "gba_banner_bin.h"
#include "nds_banner_bin.h"
#include "gbarom_banner_bin.h"
//#include "dsm_banner_bin.h"
#include "progresswnd.h"
#include "language.h"
#include "plugin.h" // bliss
#include "unicode.h"
#include "fileicons.h"

using namespace akui;

cMainList::cMainList( s32 x, s32 y, u32 w, u32 h, cWindow * parent, const std::string & text )
: cListView( x, y, w, h, parent, text )
{
    _viewMode = VM_LIST;
    _activeIconScale = 1;
    _activeIcon.hide();
    _activeIcon.update();
    animationManager().addAnimation( &_activeIcon );
    dbg_printf("_activeIcon.init\n");
}

cMainList::~cMainList()
{}

int cMainList::init()
{
    CIniFile ini( SFN_UI_SETTINGS);
    _textColor = ini.GetInt( "main list", "textColor", RGB15(7,7,7) );
    _textColorHilight = ini.GetInt( "main list", "textColorHilight", RGB15(31,0,31) );
    _selectionBarColor1 = ini.GetInt( "main list", "selectionBarColor1", RGB15(16,20,24) );
    _selectionBarColor2 = ini.GetInt( "main list", "selectionBarColor2", RGB15(20,25,0) );
    _selectionBarOpacity = ini.GetInt("main list", "selectionBarOpacity", 100); //

    //selectedRowClicked.connect( this, &cMainList::executeSelected );

    //insertColumn( 0, "index", 5 * 6 ); // 6 pixels per number and 5 number total
    insertColumn( INDEX_COLUMN, "index", 6 * 5 );
    insertColumn( ICON_COLUMN, "icon", 6 * 6 ); // 6 pixels per number and 5 number total
    //insertColumn( SHOWNAME_COLUMN, "showName", 218 );
    insertColumn( SHOWNAME_COLUMN, "showName", 0);
    insertColumn( REALNAME_COLUMN, "realName", 0 ); // hidden column for contain real filename
    insertColumn( DISPNAME_COLUMN, "dispName", 218 ); 
    insertColumn( SAVETYPE_COLUMN, "saveType", 0 );
    insertColumn( FILESIZE_COLUMN, "fileSize", 0 );

    setViewMode( VM_ICON );

    _activeIcon.hide();

    return 1;
}

static bool itemSortComp( const cListView::itemVector & item1, const cListView::itemVector & item2 ) {
    const std::string & fn1 = item1[cMainList::SHOWNAME_COLUMN].text;
    const std::string & fn2 = item2[cMainList::SHOWNAME_COLUMN].text;
    if( "../" == fn1 )
        return true;
    if( "../" == fn2 )
        return false;
    if( '/' == fn1[fn1.size()-1] && '/' == fn2[fn2.size()-1] )
        return fn1 < fn2;
    if( '/' == fn1[fn1.size()-1] )
        return true;
    if( '/' == fn2[fn2.size()-1] )
        return false;

    return fn1 < fn2;
}

static bool extnameFilter( const std::vector<std::string> & extNames, std::string extName )
{
    if( 0 == extNames.size() )
        return true;

    for( size_t i = 0; i < extName.size(); ++i )
        extName[i] = tolower( extName[i] );

    for( size_t i = 0; i < extNames.size(); ++i ) {
        if( extName == extNames[i] ) {
            return true;
        }
    }
    return false;
}

// added by bliss to hidden files
static bool sysnameFilter( const std::vector<std::string> & sysNames, std::string fileName )
{
    if( fileName[0] == '.' ) return false;

    if( 0 == sysNames.size() ) return true;


    for( size_t i = 0; i < fileName.size(); ++i ) fileName[i] = tolower( fileName[i] );

    for( size_t i = 0; i < sysNames.size(); ++i ) {
		if( fileName == sysNames[i] ) {
			return false;
		}
    }
    return true;
}

static void Split(const std::string& str, const std::string& delim, std::vector<std::string>& output)
{
	unsigned int offset = 0;
	unsigned int delimIndex = 0;
			    
	delimIndex = str.find(delim, offset);

	while (delimIndex != std::string::npos)
	{
		output.push_back(str.substr(offset, delimIndex - offset));
		offset += delimIndex - offset + delim.length();
		delimIndex = str.find(delim, offset);
	}

	output.push_back(str.substr(offset));
}

bool cMainList::enterDir( const std::string & dirName )
{

	u8 titleIndex = 1; // en
	if ( gs().language > 0 ) titleIndex = 0; // cj
	if ( gs().language > 3 ) titleIndex = gs().language -2 ; //eu
	if ( gs().language > 7 ) titleIndex = 1; // others to en
	if ( gs().language == 5 ) titleIndex = 4; // it
	else if ( gs().language == 6 ) titleIndex = 3; // de


    if( "..." == dirName ) // select RPG or SD card
    {
		size_t itemcount = 3;

		//has short cut
		if(gs().Shortcut == 1)
		{
			itemcount = 4;
			struct stat st;

			if( (stat(gs().ShortcutPath.c_str(),&st) == -1) || (st.st_size == 0) ) 
				itemcount = 3;
		}
		else
			itemcount = 3;
			
		if(strcmp(gs().ShortcutPath.c_str(),"none") == 0)//default value of shortcutpath
			itemcount = 3;

        removeAllRows();
        _romInfoList.clear();
        for( size_t i = 0; i < itemcount ; ++i ) {
            std::vector< std::string > a_row;
            a_row.push_back( formatString("%03d", i ) );
            a_row.push_back( "" ); // make a space for icon
            DSRomInfo rominfo;
            rominfo._isDSRom = false;
			rominfo._fileIconIdx = -1;
            if( 0 == i ) {
                a_row.push_back( "Flash Memory" );
                a_row.push_back( "fat0:/" );
                a_row.push_back( "Flash Memory" ); // dispName
                memcpy( &rominfo.banner(), nand_banner_bin, sizeof(tNDSBanner) );
            }
            else if( 1 == i ) {
                a_row.push_back( "microSD Card" );
                a_row.push_back( "fat1:/" );
                a_row.push_back( "microSD Card" );	// dispName
                memcpy( &rominfo.banner(), microsd_banner_bin, sizeof(tNDSBanner) );
            } else if( 2 == i ) {
                a_row.push_back( "Slot2 Card" );
                a_row.push_back( "slot2:/" );
                a_row.push_back( "Slot2 Card" ); // dispName
                memcpy( &rominfo.banner(), gba_banner_bin, sizeof(tNDSBanner) );
            } else if( 3 == i ) {
                //It'll cause hang up when return from slot2 can't be loaded.
                //We do not investigate it but know it.
                rominfo.loadDSRomInfo(gs().ShortcutLongPath.c_str(),true);
                a_row.push_back( gs().ShortcutStr.c_str() );
                a_row.push_back( "shortcut:/" );
				std::string dispName = gs().ShortcutStr; 

				if ( (gs().fileListType == 0) && gs().romNameTypeInList == 1) {
					dispName =  dispName.substr(0,dispName.find_last_of( '.' ));
				}
				else if ( gs().romNameTypeInList == 2 ) {
					u16 *_bt = (u16*)malloc( sizeof(u16) * 128 );
					memcpy(_bt, rominfo.banner().titles[titleIndex],sizeof(u16) * 128);
					dispName = unicode_to_local_string( _bt, 128, NULL ).c_str();
					free(_bt);
					dispName = dispName.substr(dispName.find_first_not_of(' '));
					dispName =dispName.substr(0,dispName.find_last_of('\n'));

					int idx = 0;
					while( (idx=dispName.find_first_of('\n')) >= 0 ) { dispName.replace( idx, 1, " " ); }
				}

                a_row.push_back( dispName );
            }


            insertRow( i, a_row );
            _romInfoList.push_back( rominfo );
        }
        _currentDir = "";
        directoryChanged();
        return true;
    }

    if( "slot2:/" == dirName || ( "shortcut:/" == dirName )) {
        _currentDir = "";
        directoryChanged();
        return true;
    }

    DIR_ITER * dir = diropen( dirName.c_str() );

    if (dir == NULL) {
        if( "fat1:/" == dirName ) {
            std::string title = LANG( "sd card error", "title" );
            std::string sdError = LANG( "sd card error", "text" );
            messageBox( NULL, title, sdError, MB_OK );
        }
        dbg_printf ("Unable to open directory<%s>.\n", dirName.c_str() );
        return false;
    }

    removeAllRows();
    _romInfoList.clear();


    std::vector< std::string > extNames;
	std::string delim = ","; // by bliss
	if( plugin().extNames != "" ) { Split(plugin().extNames, delim, extNames); }

    extNames.push_back( ".nds" );
    extNames.push_back( ".ids" );
	if( gs().enable3in1Exp || gs().enableEwinExp ) extNames.push_back( ".gba" ); // by bliss

    if( gs().fileListType > 0 )
        extNames.push_back( ".sav" );
    if( gs().fileListType > 1 )
        extNames.clear();

	// filtering System directories or files. modified by bliss
	std::vector< std::string > sysNames;
	Split(gs().hiddenFileNames, delim, sysNames);

    // insert һ???ļ?, ��?У?һ????Ϊ??ʾ??һ????Ϊ??ʵ?ļ???
    struct stat st;
    char filename[256]; // to hold a full filename and string terminator
    char longFilename[512];
    std::string extName;
	//u8 attr; // modified by bliss with gelu's attribute patch

    // list dir
    {
        cwl();
        while (dirnextl(dir, filename, longFilename, &st) == 0) { 
		//while (dirnextl(dir, filename, longFilename, &st, &attr) == 0) { // by bliss
            std::string fn( filename );
            std::string lfn( longFilename );
            if( 0 == longFilename[0] )
                lfn = fn;

            // st.st_mode & S_IFDIR indicates a directory
            size_t lastDotPos = lfn.find_last_of( '.' );
            if( lfn.npos != lastDotPos )
                extName = lfn.substr( lastDotPos );
            else
                extName = "";

            dbg_printf( "%s: %s %s\n", (st.st_mode & S_IFDIR ? " DIR" : "FILE"), filename, extName.c_str() );
            //bool showThisFile = !(st.st_mode & S_IFDIR ) && extnameFilter( extNames, extName );
            //showThisFile = showThisFile && (gs().showHiddenFiles && !(st.st_mode&ATTR_HIDDEN) ); // it's not correct at all
			//
			
            //bool showThisFile = !(st.st_mode & S_IFDIR ) && extnameFilter( extNames, extName ) || ((st.st_mode & S_IFDIR ) && filename[0] != '.' );

            //first filter hidden files and folders then filter specific files.
            //and do not show "./" and "../".
            //bool showThisFile = (gs().showHiddenFiles || !st.st_spare1) && (!(st.st_mode & S_IFDIR ) && extnameFilter( extNames, extName ))
            //                        || (((st.st_mode & S_IFDIR ) && (filename[0] != '.') && (gs().showHiddenFiles || !st.st_spare1)));
            

			// st.st_spare1 == hidden attribute
            bool showThisFile=(st.st_mode & S_IFDIR)?filename[0] != '.' : extnameFilter( extNames, extName );
			if ( showThisFile &&
					(gs().showHiddenFiles || (!st.st_spare1 && sysnameFilter(sysNames,lfn))  )) { // modified by bliss

                u32 row_count = getRowCount();
                char indexText[16] = {};
                sprintf( indexText, "%d", (int)row_count );
                std::vector< std::string > a_row;
                a_row.push_back( indexText );
                a_row.push_back( "" ); // make a space for icon
                if( 0 != longFilename[0] )
                    a_row.push_back( longFilename );
                else
                    a_row.push_back( filename );

                std::string shortName = fn;
                //realName += indexText;
                a_row.push_back( shortName );
                if( st.st_mode & S_IFDIR ) {
                    a_row[SHOWNAME_COLUMN] += "/";
                    a_row[REALNAME_COLUMN] += "/";
                }

				// dispName
                a_row.push_back( a_row[SHOWNAME_COLUMN].c_str() );

                size_t insertPos( row_count );
                insertRow( insertPos, a_row );
                DSRomInfo rominfo;
                _romInfoList.push_back( rominfo );

            }
        }
        std::sort( _rows.begin(), _rows.end(), itemSortComp );

        for( size_t i = 0; i < _rows.size(); ++i )
        {
            ////_romInfoList.push_back( rominfo );

            // ???δ????????𿽱??ļ????ɺ???ͼ????ʾ????ȷ????Ϊͼ???????ݻ?û?б????룬??ȥ??????active icon??????
            //u8 percent = i * 100 / _rows.size();
            //if( !(percent & 0x07) )
            //    progressWnd().setPercent( percent );


            _rows[i][INDEX_COLUMN].text = formatString( "%03d", i );
            DSRomInfo & rominfo = _romInfoList[i];
            std::string filename = (dirName + _rows[i][REALNAME_COLUMN].text);
            size_t lastDotPos = filename.find_last_of( '.' );
            if( filename.npos != lastDotPos )
                extName = filename.substr( lastDotPos );
            else
                extName = "";
            for( size_t j = 0; j < extName.size(); ++j )
                extName[j] = tolower( extName[j] );


			// load banner(file icon)
			rominfo._fileIconIdx = -1;
            if( '/' == filename[filename.size()-1] ) { // directory
                rominfo._isDSRom = false;
                memcpy( &rominfo.banner(), folder_banner_bin, sizeof(tNDSBanner) );
			}
			else if ( extName == ".nds" || extName == ".ids") { // nds rom
                rominfo.loadDSRomInfo( filename.c_str(), true );
			}
			else {
				if ( fileicons().isExist(extName.c_str()) ) { 
					rominfo._fileIconIdx = fileicons().getIndex();
				} else if( ".sav" == extName ) {
					rominfo._isDSRom = false;
					memcpy( &rominfo.banner(), nds_save_banner_bin, sizeof(tNDSBanner) );
				} else if ( ".gba" == extName ) { // by bliss
					rominfo._isDSRom = false;
					memcpy( &rominfo.banner(), gbarom_banner_bin, sizeof(tNDSBanner) );
				} else {
					rominfo._isDSRom = false;
					memcpy( &rominfo.banner(), unknown_banner_bin, sizeof(tNDSBanner) );
				}
			}
/*
            if( '/' == filename[filename.size()-1] ) {
                rominfo._isDSRom = false;
                memcpy( &rominfo.banner(), folder_banner_bin, sizeof(tNDSBanner) );
            } else if( ".sav" == extName ) {
                rominfo._isDSRom = false;
                memcpy( &rominfo.banner(), nds_save_banner_bin, sizeof(tNDSBanner) );
			}else if ( ".gba" == extName ) { // by bliss
                rominfo._isDSRom = false;
                memcpy( &rominfo.banner(), gbarom_banner_bin, sizeof(tNDSBanner) );
			}else if ( ".dsm" == extName ) { // by bliss
                rominfo._isDSRom = false;
                memcpy( &rominfo.banner(), dsm_banner_bin, sizeof(tNDSBanner) );
            } else if( ".nds" != extName && ".ids" != extName ) {
                rominfo._isDSRom = false;
                memcpy( &rominfo.banner(), unknown_banner_bin, sizeof(tNDSBanner) );
            } else {
                rominfo.loadDSRomInfo( filename.c_str(), true );
				//dbg_printf("load: %x\n",*rominfo.banner().titles[1]);
            }
*/
			if ( gs().romNameTypeInList && ( extName == ".nds" || extName == ".gba" || extName == ".ids" ) ) {
				std::string dispName;
				if ( (gs().fileListType == 0) && gs().romNameTypeInList == 1) {
					dispName = _rows[i][DISPNAME_COLUMN].text;
					_rows[i][DISPNAME_COLUMN].text = dispName.substr(0,dispName.find_last_of( '.' ));
				}
				else if ( rominfo._isDSRom  && gs().romNameTypeInList == 2 ) {
					u16 *_bt = (u16*)malloc( sizeof(u16) * 128 );
					memcpy(_bt, rominfo.banner().titles[titleIndex],sizeof(u16) * 128);
					dispName = unicode_to_local_string( _bt, 128, NULL ).c_str();
					free(_bt);
					dispName = dispName.substr(dispName.find_first_not_of(' '));
					dispName =dispName.substr(0,dispName.find_last_of('\n'));

					int idx = 0;
					while( (idx=dispName.find_first_of('\n')) >= 0 ) { dispName.replace( idx, 1, " " ); }
					_rows[i][DISPNAME_COLUMN].text = dispName;
				}
			}
        }
        _currentDir = dirName;
        dirclose( dir );
    }

    directoryChanged();

    return true;
}

void cMainList::onSelectChanged( u32 index )
{
    dbg_printf( "%s\n", _rows[index][3].text.c_str() );
}

void cMainList::onSelectedRowClicked( u32 index )
{
    const INPUT & input = getInput();
    //dbg_printf("%d %d", input.touchPt.px, _position.x );
    if( input.touchPt.px > _position.x && input.touchPt.px < _position.x + 32 )
        selectedRowHeadClicked( index );
}

void cMainList::onScrolled( u32 index )
{
    _activeIconScale = 1;
    //updateActiveIcon( CONTENT );
}

void cMainList::backParentDir()
{
    if( "..." == _currentDir )
        return;

    if( "fat0:/" == _currentDir || "fat1:/" == _currentDir || "/" == _currentDir ) {
        enterDir( "..." );
        return;
    }

    size_t pos = _currentDir.rfind( "/", _currentDir.size() - 2 );
    std::string parentDir = _currentDir.substr( 0, pos + 1 );
    dbg_printf( "%s->%s\n", _currentDir.c_str(), parentDir.c_str() );

    std::string oldCurrentDir = _currentDir;

    if( enterDir( parentDir ) ) { // select last entered director
        for( size_t i = 0; i < _rows.size(); ++i ){
            if( parentDir + _rows[i][REALNAME_COLUMN].text == oldCurrentDir ) {
                selectRow( i );
            }
        }
    }
}

std::string cMainList::getSelectedFullPath()
{
    if( !_rows.size() )
        return std::string("");
    const std::string & filename = _rows[_selectedRowId][SHOWNAME_COLUMN].text;
    return _currentDir + filename;
}

std::string cMainList::getSelectedShortPath()
{
    if( !_rows.size() )
        return std::string("");
    const std::string & shortname = _rows[_selectedRowId][REALNAME_COLUMN].text;
    return _currentDir + shortname;
}

std::string cMainList::getSelectedLFN()
{
    return _rows[_selectedRowId][SHOWNAME_COLUMN].text;
}

bool cMainList::getRomInfo( u32 rowIndex, DSRomInfo & info ) const
{
    if( rowIndex < _romInfoList.size() ) {
        info = _romInfoList[rowIndex];
        return true;
    }
    else {
        return false;
    }
}

void cMainList::setRomInfo( u32 rowIndex, const DSRomInfo & info )
{
    if( !_romInfoList[rowIndex].isDSRom() )
        return;

    if( rowIndex < _romInfoList.size() ) {
        _romInfoList[rowIndex] = info;
    }
}

void cMainList::draw()
{
    cListView::draw();
    updateActiveIcon( POSITION );
    drawIcons();
}

void cMainList::drawIcons()         // ֱ?ӻ????㷨?? icons
{
    if( VM_ICON == _viewMode ) {
        size_t total = _visibleRowCount;
        if(  total > _rows.size() - _firstVisibleRowId )
            total = _rows.size() - _firstVisibleRowId;

        for( size_t i = 0; i < total; ++i ) {
            // ????ͼ?????ֱ??????? MAIN buffer ??תҪ?磬???Ի???һ??
            // ???����????? gdi().present ????ͳһ???ַ?ת
            if( _firstVisibleRowId + i == _selectedRowId ) {
                if( _activeIcon.visible() ) {
                    continue;
                }
            }
            s32 itemX = _position.x;
            s32 itemY = _position.y + i * _rowHeight + ((_rowHeight-32)>>1)  - 1;
            _romInfoList[_firstVisibleRowId+i].drawDSRomIcon( itemX, itemY, _engine );
        }
    }
}

void cMainList::setViewMode( VIEW_MODE mode )
{
    if( !_columns.size() ) return;

    _viewMode = mode;
    if( VM_ICON == _viewMode ) {
        _columns[INDEX_COLUMN].width = 0;             // hide index column
        _columns[ICON_COLUMN].width = 36;            // show icon column
        arangeColumnsSize();
        setRowHeight( 38 );
        //if( 0 == _romInfoList.size() )     // if there is no rom info, refresh
        //    enterDir( _currentDir );
    } else if( VM_LIST == _viewMode ) {
        _columns[INDEX_COLUMN].width = 30;                // show index column
        _columns[ICON_COLUMN].width = 0;                // hide icon column
        arangeColumnsSize();
        setRowHeight( 15 );
    }
    scrollTo( _selectedRowId - _visibleRowCount + 1 );
    //updateActiveIcon( CONTENT );
}

void cMainList::updateActiveIcon( bool updateContent )
{
    const INPUT & temp =  getInput();

    //do not show active icon when hold key to list files. Otherwise the icon will not show correctly.
    if( getInputIdleMs() > 1000 && VM_ICON == _viewMode && !windowManager().popUpWindow() && _romInfoList.size() && 0 == temp.keysHeld )
    {
        if( !_activeIcon.visible()  ) {
            u8 backBuffer[32*32*2];
            zeroMemory( backBuffer, 32 * 32 * 2 );
            _romInfoList[_selectedRowId].drawDSRomIconMem( backBuffer );
            memcpy( _activeIcon.buffer(), backBuffer, 32 * 32 * 2 );
            _activeIcon.setBufferChanged();

            s32 itemX = _position.x;
            s32 itemY = _position.y + (_selectedRowId - _firstVisibleRowId) * _rowHeight + ((_rowHeight-32)>>1) - 1;
            _activeIcon.setPosition( itemX, itemY );
            _activeIcon.show();
            dbg_printf("sel %d ac ico x %d y %d\n", _selectedRowId, itemX, itemY );
            for( u8 i = 0; i < 8; ++i )
                dbg_printf("%02x", backBuffer[i] );
            dbg_printf("\n");
        }
    } else {
        if( _activeIcon.visible() ) {
            _activeIcon.hide();
            cwl();
        }
    }
}

std::string cMainList::getCurrentDir()
{
    return _currentDir;
}
