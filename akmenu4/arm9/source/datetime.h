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









#ifndef _DATETIME_H_
#define _DATETIME_H_

#include <nds.h>
#include "singleton.h"
#include "../../share/stringtool.h"

class cDateTime
{
public:

    cDateTime() {}
    ~cDateTime() {}

public:

    static const char * weekdayStrings[];
    u16 year() { return IPC->time.rtc.year + 2000; }
    u8 month() { return IPC->time.rtc.month; }
    u8 day() { return IPC->time.rtc.day; }
    u8 weekday() { return IPC->time.rtc.weekday; }

    u8 hours() { return IPC->time.rtc.hours; }
    u8 minutes() { return IPC->time.rtc.minutes; }
    u8 seconds() { return IPC->time.rtc.seconds; }

    std::string getDateString() { return formatString( "%d/%d%/%d %s\n", year(), month(), day(), weekdayStrings[weekday()] ); }
    std::string getTimeString() { return formatString( "%d:%d%:%d\n", hours(), minutes(), seconds() ); }
    std::string getTimeStampString() { return formatString( "%04d%02d%02d%02d%02d%02d", year(), month(), day(), hours(), minutes(), seconds() ); }
};

typedef t_singleton< cDateTime > dateTime_s;
inline cDateTime & datetime() { return dateTime_s::instance(); }

#endif//_DATETIME_H_
