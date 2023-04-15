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









#ifndef _HELPWND_H_
#define _HELPWND_H_

#include "form.h"
#include "formdesc.h"
#include "spinbox.h"
#include "statictext.h"
#include "message.h"
#include <string>
#include "dsrom.h"

class cHelpWnd : public akui::cForm
{
public:
    cHelpWnd( s32 x, s32 y, u32 w, u32 h, cWindow * parent, const std::string & text );

    ~cHelpWnd();

public:

    void draw();

    bool process( const akui::cMessage & msg );

    cWindow& loadAppearance(const std::string& aFileName );

protected:

    bool processKeyMessage( const akui::cKeyMessage & msg );

    void onOK();

    void onShow();

	void onPowerOff();

    akui::cButton _buttonOK;

	akui::cButton _buttonPowerOff;

    akui::cFormDesc _renderDesc;

    std::string _helpText;

    std::string _versionText;
};




#endif//_HELPWND_H_
