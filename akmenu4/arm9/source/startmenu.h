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









#ifndef _STARTMENU_H_
#define _STARTMENU_H_

#include "popmenu.h"

#define START_MENU_ITEM_COPY         0
#define START_MENU_ITEM_CUT          1
#define START_MENU_ITEM_DELETE       2
#define START_MENU_ITEM_PASTE        3
#define START_MENU_ITEM_SAVETYPE     4
#define START_MENU_ITEM_PATCHES      4
#define START_MENU_ITEM_SETTING      5
#define START_MENU_ITEM_INFO         6
#define START_MENU_ITEM_HELP         7

class cStartMenu : public akui::cPopMenu
{
public:

    cStartMenu( s32 x, s32 y, u32 w, u32 h, cWindow * parent, const std::string & text ) :
      cPopMenu( x, y, w, h, parent, text )
    {}

    ~cStartMenu() {}

    void init();

    bool process( const akui::cMessage & msg );

    cWindow& loadAppearance(const std::string& aFileName );

protected:

    void onHide();

};

//typedef t_singleton< cStartMenu > cStartMenu_s;
//inline cStartMenu & startMenu() { return cStartMenu_s::instance(); }


#endif//_STARTMENU_H_
