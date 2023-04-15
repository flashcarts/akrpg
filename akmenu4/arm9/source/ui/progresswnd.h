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









#ifndef _PROGRESSWND_H_
#define _PROGRESSWND_H_


#include "ui.h"
#include "progressbar.h"
#include "singleton.h"

namespace akui
{

class cProgressWnd : public cForm
{

public:

    cProgressWnd();// s32 x, s32 y, u32 w, u32 h, cWindow * parent, const std::string & text );

    ~cProgressWnd();

public:

    void init();

    void draw();

    bool process( const cMessage & msg );

    cWindow& loadAppearance(const std::string& aFileName );

    void setPercent( u8 percent );

    void setTipText( const std::string & tipText );

protected:

    void onShow();

    void onHide();

    cProgressBar _bar;

    cStaticText _tip;

    cBitmapDesc _renderDesc;
};


}

typedef t_singleton< akui::cProgressWnd > progressWnd_s;
inline akui::cProgressWnd & progressWnd() { return progressWnd_s::instance(); }


#endif//_PROGRESSWND_H_
