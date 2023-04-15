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









#include "timer.h"

vu64 cTimer::_overFlow = 0;
cTimer::cTimer()
{
    //initTimer();
}

void cTimer::timerInterruptHandler()
{
    _overFlow += 65536;
}

void cTimer::initTimer()
{
    _lastTime = 0;
    _currentTime = 0;
    _overFlow = 0;
    _fps = 0.f;
    _fpsCounter = 0;
    irqEnable( IRQ_TIMER0 );
    irqSet( IRQ_TIMER0, cTimer::timerInterruptHandler );
    TIMER0_DATA = 0; // set reload value
    TIMER0_CR = TIMER_ENABLE | TIMER_DIV_1 | TIMER_IRQ_REQ;
}

double cTimer::updateTimer()
{
    _currentTime = (_overFlow + TIMER0_DATA) * _factor;
    return _currentTime;
}

double cTimer::updateFps()
{
    if( _fpsCounter++ > 60 )
    {
        double elapsedTime = _currentTime - _lastTime;
        _fps = _fpsCounter / elapsedTime;
        _fpsCounter = 0;
        _lastTime = _currentTime;
    }
    return _fps;
}

double cTimer::getTime()
{
    return _currentTime;
}

vu64 cTimer::getTick()
{
    irqDisable( IRQ_TIMER0 );
    DC_FlushAll();
    static vu64 lastTick = 0;
    vu64 tick = _overFlow + TIMER0_DATA;
    if( tick < lastTick )
        tick += 65536;// 有时候 TIMER0_DATA 已经归0，但overflow 还没有加上，这个时候需要加上65536
    lastTick = tick;
    irqEnable( IRQ_TIMER0 );
    return tick;
}

double cTimer::tickToUs( u64 tick )
{
    return tick * 1.f/(33.514*1000000.f) * 1000 * 1000;
}

double cTimer::getFps()
{
    return _fps;
}
