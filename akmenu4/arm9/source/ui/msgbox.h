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









#ifndef _AKUI_MESSAGEBOX_H_
#define _AKUI_MESSAGEBOX_H_

#include "window.h"
#include "form.h"
#include "formdesc.h"
#include "button.h"

namespace akui {

#define MB_OK                1
#define MB_CANCEL            2
#define MB_OK_CANCEL        3
#define MB_YES                4
#define MB_NO                8
#define MB_YES_NO            12

#define ID_OK                1
#define ID_CANCEL            0
#define ID_YES                1
#define ID_NO                0


class cMessageBox : public cForm
{
public:
    friend u32 messageBox( cWindow * parent, const std::string & title, const std::string & msg, u32 style );

    cMessageBox( s32 x, s32 y, u32 w, u32 h, cWindow * parent, const std::string & title, const std::string & msg, u32 style );

    ~cMessageBox();

public:

    void draw();

    bool process( const cMessage & msg );

    cWindow& loadAppearance(const std::string& aFileName );

    //u32 doModal();

    //u32 msgRet() { return _msgRet; }

protected:

    void onOK();

    void onCANCEL();

    void onYES() { onOK(); }

    void onNO() { onCANCEL(); }

    bool processKeyMessage( const cKeyMessage & msg );

    bool processTouchMessage( const cTouchMessage & msg );

    std::string _msg;

    cPoint _textPoision;

    u32 _style;

    cButton * _buttonOK;
    cButton * _buttonCANCEL;
    cButton * _buttonYES;
    cButton * _buttonNO;
    cFormDesc _renderDesc;
};

u32 messageBox( cWindow * parent, const std::string & title, const std::string & msg, u32 style );

} // namespace akui

#endif//_MESSAGEBOX_H_
