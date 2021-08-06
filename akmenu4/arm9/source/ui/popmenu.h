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









#ifndef _POPMENU_H_
#define _POPMENU_H_

#include <nds.h>
#include <vector>
#include "point.h"
#include "window.h"
#include "form.h"
#include "sigslot.h"

namespace akui {

class cPopMenu : public cWindow
{
public:

    cPopMenu( s32 x, s32 y, u32 w, u32 h, cWindow * parent, const std::string & text );

    ~cPopMenu();

public:

    void draw();

    bool process( const cMessage & msg );

    // 返回选中的项
    void popup();

    void addItem( size_t index, const std::string & itemText );

    void removeItem( size_t index );

    size_t itemCount();

    void clearItem();

    Signal1< s16 > itemClicked;

protected:

    void onShow();

    bool processKeyMessage( const cKeyMessage & msg );

    bool processTouchMessage( const cTouchMessage & msg );

    size_t itemBelowPoint( const cPoint & pt );

    void drawItems();

    cPoint _itemTopLeftPoint;

    std::vector< std::string > _items;

    s16 _selectedItemIndex;

    s16 _itemHeight;

    COLOR _textColor;
    COLOR _textHighLightColor;
    COLOR _barColor;

    cBitmapDesc * _renderDesc;
};

}
#endif//_POPMENU_H_
