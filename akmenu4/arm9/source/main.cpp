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









#include <nds.h>
#include <nds/arm9/console.h> //basic print funcionality
#include <cstdio>
#include <vector>
#include <map>
#include <list>
#include <fat.h>
#include "unicode.h"
#include "gdi.h"
#include "ui.h"
#include "dbgtool.h"
#include "ipctool.h"

#include "systemfilenames.h"
#include "mainlist.h"
#include "startmenu.h"
#include "mainwnd.h"
#include "timer.h"

#include "datetime.h"
#include "calendarwnd.h"
#include "bigclock.h"
#include "calendar.h"

#include "inifile.h"
#include "irqs.h"

#include "diskicon.h"
#include "progresswnd.h"
#include "language.h"
#include "fontfactory.h"

#include "userwnd.h"


using namespace akui;

int main(void)
{
    irq().init();

    windowManager();

    // init basic system
    sysSetBusOwners( BUS_OWNER_ARM9, BUS_OWNER_ARM9 );

    // init tick timer/fps counter
    timer().initTimer();

    // init inputs
    initInput();

    // init graphics
    gdi().init();
#ifdef DEBUG
    gdi().switchSubEngineMode();
#endif//DEBUG
    dbg_printf( "gdi ok\n" );

    //wait_press_b();
    // init fat
    //bool succ = fatInitDefault();
    bool succ = fatInitRPG();
    if( !succ )
        dbg_printf( "init fat %d\n", succ );

    //wait_press_b();

    // setting scripts
    globalSettings().loadSettings();

    // init unicode
    if( initUnicode() )
        _FAT_unicode_init( unicodeL2UTable, unicodeU2LTable, unicodeAnkTable );
    cwl();


    fontFactory().makeFont( gs().language ); // load font file
    cwl();
    lang(); // load language file
    uiSettings().loadSettings();


    bool saveListOK = saveManager().importSaveList( SFN_OFFICIAL_SAVELIST, SFN_CUSTOM_SAVELIST );
    if( !saveListOK ) {
        //messageBox( NULL,
        //    LANG("no savelist","title"),
        //    LANG("no savelist", "text"), MB_OK );
        dbg_printf("WARNING: savelist.bin missed\n");
    }


    cMainWnd * wnd = new cMainWnd( 0, 0, 256, 192, NULL, "main window" );
    wnd->init();
    wnd->setAsActiveWindow();

    progressWnd().init();

    diskIcon().loadAppearance( SFN_CARD_ICON_BLUE );
    diskIcon().show();

    windowManager().update();
    timer().updateFps();

    calendarWnd().init();
    calendarWnd().draw();
    calendar().init();
    calendar().draw();
    bigClock().init();
    bigClock().draw();

    userWindow().draw();

    gdi().present( GE_MAIN );
    gdi().present( GE_SUB );

    dbg_printf( "loop start\n" );

    irq().vblankStart();

    // enter last directory

    std::string lastDirectory = "...";
    if( gs().enterLastDirWhenBoot )
    {
        CIniFile f;
        if( !f.LoadIniFile( SFN_LAST_SAVEINFO ) ) {
            lastDirectory = "...";
        }

        lastDirectory = f.GetString( "Save Info", "lastLoaded", "" );
        if( "" == lastDirectory ) {
            lastDirectory = "...";
        } else {
            size_t slashPos = lastDirectory.find_last_of( '/' );
            if( lastDirectory.npos == slashPos )
                lastDirectory = "...";
            else
                lastDirectory = lastDirectory.substr( 0, slashPos + 1 );
        }
    }
    saveManager().backupSaveData();

    dbg_printf("lastDirectory '%s'\n", lastDirectory.c_str() );
    if( !wnd->_mainList->enterDir( lastDirectory ) ) { // parent of fat0 and fat1
        wnd->_mainList->enterDir( "..." );
    }

    while( true )
    {
        if( IPC_MSG_SUSPEND != IPC_ARM9 ) {
            timer().updateFps();

            INPUT & inputs = updateInput();

            processInput( inputs );

            //swiWaitForVBlank();

            windowManager().update();

            gdi().present( GE_MAIN );
        } else {
            INPUT & inputs = updateInput();
            processInput( inputs );
            //swiWaitForIRQ();
        }
    }

    return 0;
}
