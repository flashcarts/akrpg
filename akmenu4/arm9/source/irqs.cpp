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









#include "irqs.h"
#include "dbgtool.h"
#include "input.h"
#include "windowmanager.h"
#include "diskicon.h"
#include "calendarwnd.h"
#include "calendar.h"
#include "bigclock.h"
#include "timer.h"
#include "animation.h"
#include "files.h"
#include "ipctool.h"
#include "userwnd.h"

using namespace akui;


bool cIRQ::_vblankStarted(false);

void cIRQ::init()
{
    irqInit();
    irqSet( IRQ_VBLANK, vBlank );
    irqSet( IRQ_CARD_LINE, cardMC );
}

void cIRQ::cardMC()
{
    dbg_printf("cardMC\n");
    diskIcon().blink(0.1f);
    REG_IF &= ~IRQ_CARD_LINE;
}

void cIRQ::vblankStart()
{
    _vblankStarted = true;
}

void cIRQ::vblankStop()
{
    _vblankStarted = false;
}

void cIRQ::vBlank()
{
    if( !_vblankStarted )
        return;

    // get inputs when file copying because the main route
    // can't do any thing at that time
    if( true == copyingFile) {
        if( false == stopCopying ) {
            INPUT & input = updateInput();
            if( (input.keysDown & KEY_B) ) {
                stopCopying = true;
            }
        }
    }

    timer().updateTimer();

    static u32 vBlankCounter = 0;

    if( vBlankCounter++ > 30 ) {
        vBlankCounter = 0;
        bigClock().blinkColon();
        calendarWnd().draw();
        calendar().draw();
        bigClock().draw();
        userWindow().draw();
        gdi().present( GE_SUB );
    }

    animationManager().update();

    if( CARD_CR2 & CARD_BUSY )
        diskIcon().turnOn();
    else
        diskIcon().turnOff();

    //gdi().setVsyncPassed( true );
}
