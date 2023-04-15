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









#ifndef _SPINBOX_H_
#define _SPINBOX_H_

#include "form.h"
#include "button.h"
#include "statictext.h"
#include <vector>
#include <string>

namespace akui
{

class cSpinBox : public cForm
{

public:

    cSpinBox( s32 x, s32 y, u32 w, u32 h, cWindow * parent, const std::string & text );
    ~cSpinBox();

public:

    void draw();

    cWindow& loadAppearance(const std::string& aFileName );

    u32 selectedItemId() { return _selectedItemId; }

    void selectItem( u32 id );

    void selectNext();

    void selectPrev();

    void insertItem( const std::string & item, u32 position );

    void removeItem( u32 position );

    void setTextColor( COLOR color );

    void onCmponentClicked();
    Signal1< cSpinBox * > componentClicked;


protected:

    void onResize();

    void onMove();

    void onGainedFocus();

    void arrangeButton();

    void arrangeText();

protected:

    COLOR _focusedColor;
    COLOR _normalColor;
    COLOR _frameColor;
    cButton _prevButton;
    cButton _nextButton;
    cStaticText _itemText;
    u32 _selectedItemId;
    std::vector< std::string > _items;

};

}

#endif//_SPINBOX_H_
