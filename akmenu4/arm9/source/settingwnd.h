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









#ifndef _SETTINGWND_H_
#define _SETTINGWND_H_

#include "form.h"
#include "formdesc.h"
#include "spinbox.h"
#include "statictext.h"
#include "message.h"
#include <string>

class cSettingWnd : public akui::cForm
{

public:
    cSettingWnd( s32 x, s32 y, u32 w, u32 h, cWindow * parent, const std::string & text );

    ~cSettingWnd();

public:

    void draw();

    bool process( const akui::cMessage & msg );

    cWindow& loadAppearance(const std::string& aFileName );

    void addSettingItem(
        const std::string & text,
        const std::vector< std::string > & itemTexts,
        size_t defaultValue );

    size_t getItemSelection( size_t itemId );

    size_t focusedItemId();

    void onItemClicked( akui::cSpinBox * item );

protected:

    void onOK();

    void onCancel();

    void onShow();

    void onUIKeyUP();

    void onUIKeyDOWN();

    void onUIKeyLEFT();

    void onUIKeyRIGHT();

    u32 _maxLabelLength;

    bool processKeyMessage( const akui::cKeyMessage & msg );

    std::vector< akui::cStaticText * > _labels;
    std::vector< akui::cSpinBox * > _items;

    akui::cButton _buttonOK;
    akui::cButton _buttonCancel;
    akui::cFormDesc _renderDesc;

};

#endif//_SETTINGWND_H_
