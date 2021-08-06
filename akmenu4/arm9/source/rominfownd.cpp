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
#include "files.h"

using namespace akui;

cRomInfoWnd::cRomInfoWnd( s32 x, s32 y, u32 w, u32 h, cWindow * parent, const std::string & text )
: cForm( x, y, w, h, parent, text ),
_buttonOK(0,0,46,18,this,"\x01 OK"),
_buttonSaveType(0,0,64,18,this,"\x04 Save Type")
{
    s16 buttonY = size().y - _buttonOK.size().y - 4;

    _buttonOK.setStyle( cButton::press );
    _buttonOK.setText( "\x01 " + LANG( "setting window", "ok" ) );
    _buttonOK.setTextColor( uis().buttonTextColor );
    _buttonOK.loadAppearance( SFN_BUTTON3 );
    _buttonOK.clicked.connect( this, &cRomInfoWnd::onOK );
    addChildWindow( &_buttonOK );

    s16 nextButtonX = size().x;

    s16 buttonPitch = _buttonOK.size().x + 8;
    buttonPitch = _buttonOK.size().x + 8;
    nextButtonX -= buttonPitch;

    _buttonOK.setRelativePosition( cPoint(nextButtonX, buttonY) );

    _buttonSaveType.setStyle( cButton::press );
    _buttonSaveType.setText( "\x04 " + LANG( "setting window", "savetype" ) );
    _buttonSaveType.setTextColor( uis().buttonTextColor );
    _buttonSaveType.loadAppearance( SFN_BUTTON4 );
    _buttonSaveType.clicked.connect( this, &cRomInfoWnd::showSaveType );
    addChildWindow( &_buttonSaveType );

    buttonPitch = _buttonSaveType.size().x + 8;
    nextButtonX -= buttonPitch;

    _buttonSaveType.setRelativePosition( cPoint(nextButtonX, buttonY) );

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
        return;
    }

    if( "fat0:/" == shortName || "fat1:/" == shortName ) {
        setDiskInfo( shortName );
        return;
    }

    if( showName.size() > 0 && '/' == showName[showName.size()-1] )
        _filenameText = showName.substr( 0, showName.size() - 1 );
    else
        _filenameText = showName;

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
    u8 titleIndex = 1;      // default english title

    if( gs().language >= 1 && gs().language <= 3 ) // chinese and japanese
        titleIndex = 0;    // chinese/japanese title

    else if( gs().language >= 4 )
        titleIndex = gs().language - 2; // european title

    _romInfoText = unicode_to_local_string( _romInfo.banner().titles[titleIndex], 128, NULL );

    if( _romInfo.isDSRom() ) {
        const char * stLangStrings[] = { "Unknown", "No Save", "4K", "64K", "512K", "2M", "4M" };
        if( _romInfo.saveInfo().saveType < sizeof(stLangStrings)/sizeof(char*) ) {
            _saveTypeText = formatString( LANG("rom info", "save type").c_str(),
                LANG("save type", stLangStrings[_romInfo.saveInfo().saveType]).c_str() );
        }
        else
            _saveTypeText = "";
    } else {
        _buttonSaveType.hide();
    }
}

const DSRomInfo & cRomInfoWnd::getRomInfo()
{
    return _romInfo;
}

void cRomInfoWnd::onOK()
{
    cForm::onOK();
}


void cRomInfoWnd::onShow()
{
    centerScreen();
}

void cRomInfoWnd::showSaveType()
{
    if( !_romInfo.isDSRom() )
        return;

    cSettingWnd settingWnd( 0,0, 252, 188, this, LANG("save type", "title" ) );
    std::vector< std::string > _values;
    _values.push_back( LANG("save type", "Unknown" ) );
    _values.push_back( LANG("save type", "No Save" ) );
    _values.push_back( LANG("save type", "4K" ) );
    _values.push_back( LANG("save type", "64K" ) );
    _values.push_back( LANG("save type", "512K" ) );
    _values.push_back( LANG("save type", "2M" ) );
    _values.push_back( LANG("save type", "4M" ) );
    settingWnd.addSettingItem( LANG("save type", "text" ), _values, _romInfo.saveInfo().saveType );

    u32 ret = settingWnd.doModal();
    if( ID_CANCEL == ret )
        return;

    _romInfo.saveInfo().saveType = (SAVE_TYPE)settingWnd.getItemSelection( 0 );

    const char * stLangStrings[] = { "Unknown", "No Save", "4K", "64K", "512K", "2M", "4M" };
    if( _romInfo.saveInfo().saveType < sizeof(stLangStrings)/sizeof(char*) ) {
        _saveTypeText = formatString( LANG("rom info", "save type").c_str(),
            LANG("save type", stLangStrings[_romInfo.saveInfo().saveType]).c_str() );
    }

    saveManager().updateCustomSaveList( _romInfo.saveInfo() );
}
