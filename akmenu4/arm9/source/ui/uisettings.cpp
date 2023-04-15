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









#include "ui.h"
#include "uisettings.h"
#include "inifile.h"

cUISettings::cUISettings()
{
    showCalendar = true;
    formFrameColor = RGB15(23,25,4);
    formBodyColor = RGB15(30,29,22);
    formTextColor = RGB15(17,12,0);
    formTitleTextColor = RGB15(11,11,11);
    buttonTextColor = RGB15(17,12,0);
    spinBoxNormalColor = RGB15(0,0,31);
    spinBoxFocusColor = RGB15(0,31,0);
    spinBoxTextColor = RGB15(31,31,31);
    spinBoxTextHighLightColor = RGB15(31,31,31);
    spinBoxFrameColor = RGB15(11,11,11);
    listViewBarColor1 = RGB15(0,11,19);
    listViewBarColor2 = RGB15(0,5,9);
    listTextColor = 0;
    listTextHighLightColor = 0;
    popMenuTextColor = RGB15(0,0,0);
    popMenuTextHighLightColor = RGB15(31,31,31);
    popMenuBarColor = RGB15(0,11,19);
}

cUISettings::~cUISettings()
{
}

void cUISettings::loadSettings()
{
    CIniFile ini( SFN_UI_SETTINGS );

    showCalendar = ini.GetInt( "global settings", "showCalendar", showCalendar );
    formFrameColor = ini.GetInt( "global settings", "formFrameColor", formFrameColor );
    formBodyColor = ini.GetInt( "global settings", "formBodyColor", formBodyColor );
    formTextColor = ini.GetInt( "global settings", "formTextColor", formTextColor );
    formTitleTextColor = ini.GetInt( "global settings", "formTitleTextColor", formTitleTextColor );
    buttonTextColor = ini.GetInt( "global settings", "buttonTextColor", buttonTextColor );
    spinBoxNormalColor = ini.GetInt( "global settings", "spinBoxNormalColor", spinBoxNormalColor );
    spinBoxFocusColor = ini.GetInt( "global settings", "spinBoxFocusColor", spinBoxFocusColor );
    spinBoxTextColor = ini.GetInt( "global settings", "spinBoxTextColor", spinBoxTextColor );
    spinBoxTextHighLightColor = ini.GetInt( "global settings", "spinBoxTextHighLightColor", spinBoxTextHighLightColor );
    spinBoxFrameColor = ini.GetInt( "global settings", "spinBoxFrameColor", spinBoxFrameColor );
    listViewBarColor1 = ini.GetInt( "global settings", "listViewBarColor1", listViewBarColor1 );
    listViewBarColor2 = ini.GetInt( "global settings", "listViewBarColor2", listViewBarColor2 );
    listTextColor = ini.GetInt( "global settings", "listTextColor", listTextColor );
    listTextHighLightColor = ini.GetInt( "global settings", "listTextHighLightColor", listTextHighLightColor );
    popMenuTextColor = ini.GetInt( "global settings", "popMenuTextColor", popMenuTextColor );
    popMenuTextHighLightColor = ini.GetInt( "global settings", "popMenuTextHighLightColor", popMenuTextHighLightColor );
    popMenuBarColor = ini.GetInt( "global settings", "popMenuBarColor", popMenuBarColor );
}


