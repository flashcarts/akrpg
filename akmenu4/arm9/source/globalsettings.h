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









#ifndef _GLOBALSETTINGS_H_
#define _GLOBALSETTINGS_H_

#include <nds.h>
#include <string>
#include "singleton.h"

class cGlobalSettings
{

public:

    cGlobalSettings();

    ~cGlobalSettings();

public:

    void loadSettings();
    void saveSettings();
    void setLanguage( u8 aLanguage );

public:

    u8            fontWidth;
    u8            halfFontWidth;
    u8            fontHeight;
    u8            sdCardSpeed;
    bool        animatedFileIcon;
    u8            keyDelay;
    u8            keyRepeat;
    u8            fileListViewMode;
    u8            brightness;
    u8            language;
    std::string langDirectory;
    std::string uiName;
    u8            akmenuVersion;
    u8            akmenuSubVersion;
    u8            fileListType;
    u8          romTrim;
    bool        showHiddenFiles;
    bool        enterLastDirWhenBoot;
    u8            downloadPlayPatch;
    u8            cheatingSystem;
    u8            resetInGame;

};


typedef t_singleton< cGlobalSettings > globalSettings_s;
inline cGlobalSettings & globalSettings() { return globalSettings_s::instance(); }
inline cGlobalSettings & gs() { return globalSettings_s::instance(); }

#endif//_GLOBALSETTINGS_H_
