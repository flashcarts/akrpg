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









#ifndef _CALENDAR_H_
#define _CALENDAR_H_

#include "window.h"
#include "bmp15.h"
#include "datetime.h"
#include "singleton.h"
#include "point.h"


class cCalendar : public akui::cWindow
{
public:

    cCalendar();

    ~cCalendar() {}

public:

    void init();

    void draw();

    akui::cWindow & loadAppearance(const std::string& aFileName );

protected:

    u8 weekDayOfFirstDay();

    void drawDayNumber( u8 day );
    void drawMonthText( u8 month );
    void drawMonthNumber( u8 position, u8 n );
    void drawYearText( u32 year );
    void drawYearNumber( u8 position, u8 n );

    akui::cPoint _dayPosition;
    akui::cSize _daySize;
    akui::cPoint _monthPosition;
    akui::cPoint _yearPosition;
    COLOR _dayHighlightColor;
    cBMP15 _dayNumbers;       // index 10 means colon
    cBMP15 _yearNumbers;

    bool _showYear;
    bool _showMonth;
    bool _showDay;

    bool _colonShow;
};


typedef t_singleton< cCalendar > calendar_s;
inline cCalendar & calendar() { return calendar_s::instance(); }


#endif//_CALENDAR_H_
