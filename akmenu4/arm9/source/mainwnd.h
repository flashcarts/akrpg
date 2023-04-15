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









#ifndef _MAINWND_H_
#define _MAINWND_H_

#include "form.h"
#include "mainlist.h"
#include "button.h"
#include "keymessage.h"
#include "touchmessage.h"
#include "spinbox.h"
#include "settingwnd.h"

class cMainWnd : public akui::cForm
{
public:

    cMainWnd( s32 x, s32 y, u32 w, u32 h, cWindow * parent, const std::string & text );

    ~cMainWnd();

public:

    bool process( const akui::cMessage & msg );

    cWindow& loadAppearance(const std::string& aFileName );

    void init();

    void draw();

    cMainList * _mainList;

protected:

    void onMainListSelItemClicked( u32 index );

    void onMainListSelItemHeadClicked( u32 index );

    void onKeyAPressed();

    void onKeyBPressed();

    void onKeyXPressed();

    void onKeyYPressed();

    void listSelChange( u32 i );

    void startMenuItemClicked( s16 i );

    void startButtonClicked();

    void brightnessButtonClicked();

    bool processKeyMessage( const akui::cKeyMessage & msg );

    bool processTouchMessage( const akui::cTouchMessage & msg );

    void setSystemParam();

    void setPatches();

    void onFolderChanged();

    void showFileInfo();

	void showCheatList(const char *gamecode, std::string &fullPath, bool cheat_on);

    void launchSelected();

    cStartMenu * _startMenu;

    akui::cButton * _startButton;

    akui::cButton * _brightnessButton;

    akui::cButton * _folderUpButton;

    akui::cBitmapDesc * _renderDesc;

    akui::cSpinBox * _testSpinBox;

    akui::cStaticText * _folderText;

    cSettingWnd * _settingWnd;

public:
	void execute( std::string & shortPath, std::string &fullPath, bool autorun) ;

};



#endif//_MAINWND_H_
