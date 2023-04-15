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









#ifndef _UISETTINGS_H_
#define _UISETTINGS_H_

#include <nds.h>
#include "gdi.h"
#include "singleton.h"

class cUISettings
{
public:

    cUISettings();

    ~cUISettings();

public:

    void loadSettings();

public:

    bool showCalendar;

    COLOR formFrameColor;
    COLOR formBodyColor;
    COLOR formTextColor;
    COLOR formTitleTextColor;
    COLOR buttonTextColor;
    COLOR spinBoxNormalColor;
    COLOR spinBoxFocusColor;
    COLOR spinBoxTextColor;
    COLOR spinBoxTextHighLightColor;
    COLOR spinBoxFrameColor;
    COLOR listViewBarColor1;
    COLOR listViewBarColor2;
    COLOR listTextColor;
    COLOR listTextHighLightColor;
    COLOR popMenuTextColor;
    COLOR popMenuTextHighLightColor;
    COLOR popMenuBarColor;
};

typedef t_singleton< cUISettings > uiSettings_s;
inline cUISettings & uiSettings() { return uiSettings_s::instance(); }
inline cUISettings & uis() { return uiSettings_s::instance(); }

#endif//_UISETTINGS_H_
