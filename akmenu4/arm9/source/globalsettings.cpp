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









#include "globalsettings.h"
#include "inifile.h"
#include "systemfilenames.h"
#include "brightnesssetting.h"


cGlobalSettings::cGlobalSettings()
{
    fontWidth                = 11;
    halfFontWidth            = 6;
    fontHeight                = 12;
    sdCardSpeed                = 16;
    animatedFileIcon        = true;
    keyDelay                = 30;
    keyRepeat                = 1;
    fileListViewMode        = 0;
    brightness                = 1;
    language                = 0;
    langDirectory            = "lang_cn/";
    uiName                    = "zelda";
    akmenuVersion            = 4;
    akmenuSubVersion        = 0;
    fileListType            = 0;
    romTrim                 = 0;
    showHiddenFiles         = true;
    enterLastDirWhenBoot    = true;
    downloadPlayPatch        = 0;
    cheatingSystem            = 0;
    resetInGame                = 0;
}

void cGlobalSettings::loadSettings()
{
    CIniFile ini( SFN_GLOBAL_SETTINGS );
    fontWidth = ini.GetInt( "system", "fontWidth", fontWidth );
    halfFontWidth = ini.GetInt( "system", "halfFontWidth", halfFontWidth );
    fontHeight = ini.GetInt( "system", "fontHeight", fontHeight );
    sdCardSpeed = ini.GetInt( "system", "sdCardSpeed", sdCardSpeed );
    animatedFileIcon = 0  != ini.GetInt( "system", "animatedFileIcon", animatedFileIcon );
    keyDelay = ini.GetInt( "system", "keyDelay", keyDelay );
    keyRepeat = ini.GetInt( "system", "keyRepeat", keyRepeat );
    fileListViewMode = ini.GetInt( "system", "fileListViewMode", fileListViewMode );
    brightness = ini.GetInt( "system", "brightness", brightness );
    language = ini.GetInt( "system", "language", language );
    langDirectory = ini.GetString( "system", "langDirectory", langDirectory );
    if( *langDirectory.rbegin() != '/' )
        langDirectory += '/';
    uiName = ini.GetString( "system", "uiName", uiName );
    akmenuVersion = ini.GetInt( "system", "akmenuVersion", akmenuVersion );
    akmenuSubVersion = ini.GetInt( "system", "akmenuSubVersion", akmenuSubVersion );
    fileListType = ini.GetInt( "system", "fileListType", fileListType );
    romTrim = ini.GetInt( "system", "romTrim", romTrim );
    showHiddenFiles = ini.GetInt( "system", "showHiddenFiles", showHiddenFiles );
    enterLastDirWhenBoot = ini.GetInt( "system", "enterLastDirWhenBoot", enterLastDirWhenBoot );
    downloadPlayPatch = ini.GetInt( "system", "downloadPlayPatch", downloadPlayPatch );
    cheatingSystem = ini.GetInt( "system", "cheatingSystem", cheatingSystem );
    resetInGame = ini.GetInt( "system", "resetInGame", resetInGame );

    // apply settings
    // brightness
    setBrightness( brightness );
    // sd speed
    if( 0 == sdCardSpeed ) {
        NDSHeader.cardControl13 = 0x00406300;
    } else if ( 1 == sdCardSpeed ) {
        NDSHeader.cardControl13 = 0x00406700;
    } else if ( 2 == sdCardSpeed ) {
        NDSHeader.cardControl13 = 0x00406B00;
    } else if ( 3 == sdCardSpeed ) {
        NDSHeader.cardControl13 = 0x00406E00;
    }
    NDSHeader.cardControl13 += 0x180;
    NDSHeader.headerCRC16 = swiCRC16( 0xFFFF, &NDSHeader, 0x15E );
}

void cGlobalSettings::saveSettings()
{
    setBrightness( brightness );
    // sd speed
    if( 0 == sdCardSpeed ) {
        NDSHeader.cardControl13 = 0x00406300;
    } else if ( 1 == sdCardSpeed ) {
        NDSHeader.cardControl13 = 0x00406700;
    } else if ( 2 == sdCardSpeed ) {
        NDSHeader.cardControl13 = 0x00406B00;
    } else if ( 3 == sdCardSpeed ) {
        NDSHeader.cardControl13 = 0x00406E00;
    }
    NDSHeader.cardControl13 += 0x180;
    NDSHeader.headerCRC16 = swiCRC16( 0xFFFF, &NDSHeader, 0x15E );

    // the commented code means those parameters are not allow to change in menu
    CIniFile ini( SFN_GLOBAL_SETTINGS );
    //ini.SetInt( "system", "fontWidth", fontWidth );
    //ini.SetInt( "system", "halfFontWidth", halfFontWidth );
    //ini.SetInt( "system", "fontHeight", fontHeight );
    ini.SetString( "system", "uiName", uiName );
    //ini.SetInt( "system", "animatedFileIcon", animatedFileIcon ? 1 : 0 );
    //ini.SetInt( "system", "keyDelay", keyDelay );
    //ini.SetInt( "system", "keyRepeat", keyRepeat );
    ini.SetInt( "system", "fileListViewMode", fileListViewMode );
    ini.SetInt( "system", "brightness", brightness );
    ini.SetInt( "system", "language", language );
    ini.SetString( "system", "langDirectory", langDirectory );
    //ini.SetInt( "system", "akmenuVersion", akmenuVersion );
    //ini.SetInt( "system", "akmenuSubVersion", akmenuSubVersion );
    ini.SetInt( "system", "fileListType", fileListType );
    ini.SetInt( "system", "romTrim", romTrim );

    ini.SetInt( "system", "downloadPlayPatch", downloadPlayPatch );
    ini.SetInt( "system", "cheatingSystem", cheatingSystem );
    ini.SetInt( "system", "resetInGame", resetInGame );

    ini.SaveIniFile( SFN_GLOBAL_SETTINGS );
}

void cGlobalSettings::setLanguage( u8 aLanguage )
{
    language = aLanguage;
    switch( language )
    {
    case 0:
        langDirectory = "lang_en";
        break;
    case 1:
        langDirectory = "lang_cn";
        break;
    case 2:
        langDirectory = "lang_zh";
        break;
    case 3:
        langDirectory = "lang_jp";
        break;
    case 4:
        langDirectory = "lang_fr";
        break;
    case 5:
        langDirectory = "lang_it";
        break;
    case 6:
        langDirectory = "lang_de";
        break;
    case 7:
        langDirectory = "lang_es";
        break;
    default:
        ;
    };
}
