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

#include <fat.h>
#include "datetime.h"
#include "exptools.h" 
#include "ewintools.h" 
#include "dbgtool.h" 

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
    langDirectory            = "lang_en/";
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

	scrollSpeed = SCROLL_FAST;

	Shortcut    = false;
	ShortcutStr = "none";
	ShortcutPath = "none";
	ShortcutLongPath = "none";


	hiddenFileNames = "__rpg,__ak2,akmenu4.nds,moonshl,svsip"; // added by bliss
	romNameTypeInList = 0;

	autorunWithLastRom = false;
	enable3in1Exp = false;
	enableEwinExp = false;
	norMode3in1Exp = 0;
	rumbleStrength = 0;
	slot2BootMode = 0;
	cheatsDB = "fat0:/__rpg/cheats/cheats.dat";

	gbaSleepHack = true; 
	gbaSramPatch = true; 
	gbaAutoSave = false;

	fontName = "LOCAL";
	uiRandom = false;
}

void cGlobalSettings::loadSettings()
{
	std::string temp;
    CIniFile ini( SFN_GLOBAL_SETTINGS );

	hiddenFileNames = ini.GetString("system", "hiddenFileNames", hiddenFileNames); // added by bliss
	romNameTypeInList = ini.GetInt("system", "romNameTypeInList", romNameTypeInList);
	/*
	hideExtNDS = ini.GetInt("system", "hideExtNDS", hideExtNDS);
	showBannerName = ini.GetInt("system", "showBannerName", showBannerName);
	*/
	autorunWithLastRom = ini.GetInt("system","autorunWithLastRom", autorunWithLastRom);
	//rumbleStrength = ini.GetInt("system", "rumbleStrength", rumbleStrength);
	rumbleStrength = 0;
	norMode3in1Exp = ini.GetInt("system", "norMode3in1Exp", norMode3in1Exp);
	slot2BootMode = ini.GetInt( "system", "slot2BootMode", slot2BootMode);
	cheatsDB = ini.GetString( "system", "cheatsDB", cheatsDB );
	//gbaSleepHack=ini.GetInt("system","gbaSleepHack",gbaSleepHack);
	gbaAutoSave=ini.GetInt("system","gbaAutoSave",gbaAutoSave);
	fontName = ini.GetString("system","fontName", fontName);

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
    uiRandom = ini.GetInt( "system", "uiRandom", uiRandom);
	if ( uiRandom ) {
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
					uiNames.push_back( lfn );
			}
		} else
			uiNames.push_back( uiName );
		u16 random = datetime().seconds() % uiNames.size();
		uiName = uiNames[random];
	}

    akmenuVersion = ini.GetInt( "system", "akmenuVersion", akmenuVersion );
    akmenuSubVersion = ini.GetInt( "system", "akmenuSubVersion", akmenuSubVersion );
    fileListType = ini.GetInt( "system", "fileListType", fileListType );
    romTrim = ini.GetInt( "system", "romTrim", romTrim );
    showHiddenFiles = ini.GetInt( "system", "showHiddenFiles", showHiddenFiles );
    enterLastDirWhenBoot = ini.GetInt( "system", "enterLastDirWhenBoot", enterLastDirWhenBoot );
    downloadPlayPatch = ini.GetInt( "system", "downloadPlayPatch", downloadPlayPatch );
    cheatingSystem = ini.GetInt( "system", "cheatingSystem", cheatingSystem );
    resetInGame = ini.GetInt( "system", "resetInGame", resetInGame );

	Shortcut = ini.GetInt( "system", "Shortcut", Shortcut );
	ShortcutStr = ini.GetString( "system", "ShortcutStr", ShortcutStr );
	ShortcutPath = ini.GetString( "system", "ShortcutPath", ShortcutPath );
	ShortcutLongPath = ini.GetString( "system", "ShortcutLongPath", ShortcutLongPath );

	temp = ini.GetString( "system", "scrollSpeed", "fast" );
	if(strcmp(temp.c_str(),"slow") == 0)
		scrollSpeed = SCROLL_SLOW;
	else if(strcmp(temp.c_str(),"medium") == 0)
		scrollSpeed = SCROLL_MEDIUM;
	else if(strcmp(temp.c_str(),"fast")==0)
		scrollSpeed = SCROLL_FAST;


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

	// added by bliss for checking 3in1
	cExpansion::Reset();
	cExpansion::OpenNorWrite();
	uint32 id = cExpansion::ReadNorFlashID();
	cExpansion::ChipReset();
	cExpansion::CloseNorWrite();

	if ( id == 0x227E2218 || id == 0x227E2202 ) {
		//if ( rumbleStrength ) cExpansion::SetShake(0xEF + rumbleStrength);

		dbg_printf("DETECTED 3in1!!\n");
		enable3in1Exp = true;
	}
	else if( cEwin::Initialize() ){
		dbg_printf("DETECTED EWIN!!\n");
		enableEwinExp = true;
	}

    //we need save all non-exist items to config file so that users can get all the items if the file or item is missing.
	//no..it's too slow when entering menu.
	//ini.SaveIniFile( SFN_GLOBAL_SETTINGS );
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


	ini.SetInt( "system", "Shortcut", Shortcut);
	ini.SetString( "system", "ShortcutPath", ShortcutPath );
	ini.SetString( "system", "ShortcutStr", ShortcutStr );
	ini.SetString( "system", "ShortcutLongPath", ShortcutLongPath );

    if(scrollSpeed==SCROLL_SLOW) ini.SetString( "system", "scrollSpeed", "slow" );
	else if(scrollSpeed==SCROLL_MEDIUM) ini.SetString( "system", "scrollSpeed", "medium" );
	else if(scrollSpeed==SCROLL_FAST) ini.SetString( "system", "scrollSpeed", "fast" );


    ini.SetInt( "system", "norMode3in1Exp", norMode3in1Exp);  // bliss
    ini.SetInt( "system", "rumbleStrength", rumbleStrength);  // bliss
    ini.SetInt( "system", "autorunWithLastRom", autorunWithLastRom);  // bliss
    //ini.SetInt( "system", "hideExtNDS", hideExtNDS);  // bliss
    ini.SetInt( "system", "romNameTypeInList", romNameTypeInList);  // bliss
    ini.SetInt( "system", "showHiddenFiles", showHiddenFiles);  // bliss
    ini.SetInt( "system", "slot2BootMode", slot2BootMode);  // bliss
	ini.SetString( "system", "cheatsDB", cheatsDB );
    //ini.SetInt("system","gbaSleepHack",gbaSleepHack);
	ini.SetInt("system","gbaAutoSave",gbaAutoSave);

	if ( language == 8 || language == 3) ini.SetString( "system", "fontName", fontName);

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
	// modified by bliss for hangul
	case 8:
		langDirectory = "lang_ko";
		break;
	case 9:
		langDirectory = "lang_nl";
		break;
    default:
		langDirectory = "lang_custom";
		break;
        ;
    };
}
