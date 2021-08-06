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









#ifndef _BIGCLOCK_H_
#define _BIGCLOCK_H_

#include "window.h"
#include "bmp15.h"
#include "datetime.h"
#include "singleton.h"


//class cClockNumber : public
class cBigClock : public akui::cWindow
{
public:

    cBigClock();

    ~cBigClock() {}

public:

    void init();

    void draw();

    void blinkColon();

    akui::cWindow & loadAppearance(const std::string& aFileName );

protected:

    void drawNumber( u8 id, u8 number );

    void drawColon();

    cBMP15 _numbers;
    cBMP15 _colon;

    bool _show;
    bool _colonShow;
};


typedef t_singleton< cBigClock > bigClock_s;
inline cBigClock & bigClock() { return bigClock_s::instance(); }


#endif//_BIGCLOCK_H_
