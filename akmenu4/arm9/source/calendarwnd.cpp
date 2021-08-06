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









#include "calendarwnd.h"
#include "gdi.h"
#include "bmp15.h"
#include "systemfilenames.h"
#include "globalsettings.h"

using namespace akui;

cCalendarWnd::cCalendarWnd() : cForm( 0, 0, 256, 192, NULL, "calendar window" )
{

}

cCalendarWnd::~cCalendarWnd()
{

}

void cCalendarWnd::init()
{
    setEngine( GE_SUB );
    loadAppearance( SFN_UPPER_SCREEN_BG );
}

cWindow& cCalendarWnd::loadAppearance(const std::string& aFileName )
{
    _background = createBMP15FromFile( aFileName );
    return *this;
}


void cCalendarWnd::draw()
{
    if( _background.valid() )
        gdi().bitBlt( _background.buffer(), 0, 0, 256, 192, selectedEngine() );
}
