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









#include <algorithm>
#include "ui.h"
#include "windowmanager.h"
//#include "keymessage.h"
//#include "point.h"

namespace akui {

cWindowManager::cWindowManager() :
    _windowCapture(0),
    _windowBelowPen(0),
    _focusedWindow(0),
    _popUpWindow(0)
{
}

cWindowManager::~cWindowManager()
{
}

void cWindowManager::setFocusedWindow(cWindow* aWindow)
{
    if(aWindow != focusedWindow())
    {
        if(focusedWindow() != 0) focusedWindow()->disableFocused();
        _focusedWindow = aWindow;
        if(aWindow != 0) aWindow->enableFocused();
    }
}

cWindowManager& cWindowManager::addWindow(cWindow* aWindow)
{
    TopLevelWindow tlw(aWindow, largestZValue() + 1);
    _windows.push_back(tlw);

    _windows.sort(std::greater<TopLevelWindow>());

    return *this;

}

cWindow* cWindowManager::popUpWindow() const
{
    return _popUpWindow;
}

cWindowManager& cWindowManager::removeAllWindows()
{
    _focusedWindow = 0;
    _windows.clear();

    return *this;
}

cWindowManager& cWindowManager::removeWindow(cWindow* aWindow)
{
    if(isTopLevelWindow(aWindow))
    {
        TopLevelWindow tlw(aWindow, zValueOf(aWindow));
        _windows.remove(tlw);

        if(focusedWindow() != 0)
        {
            if(aWindow->doesHierarchyContain(focusedWindow()))
                _focusedWindow = 0;
        }
    }

    return *this;
}

bool cWindowManager::isTopLevelWindow(cWindow* aWindow) const
{
    bool ret = false;
    for(Windows::const_iterator it = _windows.begin(); it != _windows.end(); ++it)
    {
        if((*it).window == aWindow)
        {
            ret = true;
            break;
        }
    }
    return ret;
}

const cWindowManager& cWindowManager::update()
{
    Windows::iterator it = _windows.begin();

    while( it != _windows.end() )
    {
        const TopLevelWindow& tlw = *it;
        if (tlw.window->isDestroyed())
        {
            ++it;
            removeWindow(tlw.window);
            continue;
        }

        tlw.window->update();
        ++it;
    }

    // do the rendering
    for(Windows::reverse_iterator wit = _windows.rbegin(); wit != _windows.rend(); ++wit)
    {
        const TopLevelWindow& tlw = *wit;
        tlw.window->render();
    }

    if(_popUpWindow != 0) _popUpWindow->render();

    return *this;
}

cWindowManager& cWindowManager::setActiveWindow(cWindow* aWindow)
{
    cWindow* topLevelWindow = aWindow->topLevelWindow();
    if(isTopLevelWindow(topLevelWindow))
    {
        int zVal = zValueOf(topLevelWindow);
        Windows::iterator it = std::find(_windows.begin(), _windows.end(), TopLevelWindow(aWindow, zVal));
        if ( it != _windows.end() )
        {
            TopLevelWindow& tlw = *it;
            tlw.zVal = largestZValue() + 1;
        }
    }

    _windows.sort(std::greater<TopLevelWindow>());

    int z = 0;
    for(Windows::reverse_iterator it = _windows.rbegin(); it != _windows.rend(); ++it)
    {
        TopLevelWindow& tlw = *it;
        tlw.zVal = z;
        z++;
    }

    return *this;
}

int cWindowManager::zValueOf(cWindow* aWindow) const
{
    if(!isTopLevelWindow(aWindow))
        return -1;
        //THROW("Trying to get the zValue of a window that is not a top level window");

    int ret = 0;
    for(Windows::const_iterator it = _windows.begin(); it != _windows.end(); ++it)
    {
        const TopLevelWindow& tlw = *it;
        if(tlw.window == aWindow)
        {
            ret = tlw.zVal;
            break;
        }
    }

    return ret;
}

int cWindowManager::largestZValue() const
{
    int ret = 0;
    if(_windows.size() > 0)
    {
        Windows::const_iterator it = _windows.begin();
        ret = (*it).zVal;
        while(it != _windows.end())
        {
            const TopLevelWindow& tlw = *it;
            if(tlw.zVal > ret) ret = tlw.zVal;
            ++it;
        }
    }
    return ret;
}

const bool cWindowManager::process( cMessage & message ) const
{
    bool ret = false;
    if( (popUpWindow() != 0) && (popUpWindow()->isVisible()) )
    {
        ret = popUpWindow()->process(message);
    }
    else
    {
        for(Windows::const_iterator it = _windows.begin(); it != _windows.end(); ++it)
        {
            ret = ret || (*it).window->process(message);
        }
    }
    return ret;
}

cWindowManager& cWindowManager::checkForWindowBelowPen( const cPoint & touchPoint )
{
    // update the window below the cursor
    Windows::const_iterator it;
    bool done = false;
    if(_popUpWindow != 0)
    {
        if(_popUpWindow->doesHierarchyContain(_popUpWindow->windowBelow(touchPoint)))
        {
            _windowBelowPen = _popUpWindow;
            done = true;
        }
    }

    if(!done)
    {
        cWindow* wbp = 0;
        for(it = _windows.begin(); it != _windows.end(); ++it)
        {
            cWindow* windowToTest = (*it).window;

            if(windowToTest->isVisible())
            {
                cWindow* w = windowToTest->windowBelow(touchPoint);
                if(w != 0)
                {
                    wbp = w;
                    break;
                }
            }
        }
        _windowBelowPen = wbp;
    }

    return *this;
}

void cWindowManager::updateFocusIfNecessary( const cPoint & touchPoint )
{
    if( (popUpWindow() == 0) || (!popUpWindow()->windowRectangle().surrounds(touchPoint)) )
    {
        if( windowBelowPen() != focusedWindow() )
        {
            setFocusedWindow(windowBelowPen());
        }
    }
}

bool cWindowManager::processTouchMessage( cTouchMessage & message )
{
    bool isHandled = false;
    if(_windowCapture != 0)
    {
        isHandled = _windowCapture->process(message);
    }
    else
    {
        if( popUpWindow() != 0 ) {
            isHandled = popUpWindow()->process(message);
            //dbg_printf( "popUpWindow() %s process touch\n", popUpWindow()->text().c_str() );
            //dbg_printf("_popUpWindow %08x\n", _popUpWindow );
        }
        else {
            if( windowBelowPen() != 0 ) {
                isHandled = windowBelowPen()->process(message);
                //dbg_printf( "windowBelowPen() %s process touch\n", windowBelowPen()->text().c_str() );
            }
            else {
                isHandled = process(message); // XT : standard processing
                //dbg_printf( "default process touch\n" );
            }
        }
    }
    return isHandled;
}


bool cWindowManager::onKeyDown( unsigned char keyCode )
{
    cKeyMessage msg( cMessage::keyDown, keyCode );
    return process( msg );
}

bool cWindowManager::onKeyUp( unsigned char keyCode )
{
    cKeyMessage msg( cMessage::keyUp, keyCode );
    return process( msg );
}

bool cWindowManager::onTouchDown( int x, int y )
{
    checkForWindowBelowPen( cPoint(x, y) );
    cTouchMessage msg( cMessage::touchDown, cPoint( x, y ) );
    bool isHandled = processTouchMessage( msg );
    updateFocusIfNecessary( cPoint( x, y ) );
    return isHandled;
}

bool cWindowManager::onTouchUp( int x, int y )
{
    checkForWindowBelowPen( cPoint(x, y) );
    cTouchMessage msg( cMessage::touchUp, cPoint( x, y ) );
    dbg_printf( "window below pen (%s)\n", _windowBelowPen ? _windowBelowPen->text().c_str() : "NULL" );
    dbg_printf( "focusedWindow (%s)\n", focusedWindow() ? focusedWindow()->text().c_str() : "NULL" );
    if( popUpWindow() )
        return popUpWindow()->process( msg );
    if( focusedWindow() && windowBelowPen() != focusedWindow() )
        return focusedWindow()->process( msg );
    return processTouchMessage( msg );
}

bool cWindowManager::onTouchMove( int x, int y )
{
    const INPUT & input = getInput();
    cTouchMessage msg( cMessage::touchMove, cPoint( x, y ) );
    checkForWindowBelowPen( cPoint(input.touchPt.x, input.touchPt.y) );
    dbg_printf( "touch move %d %d\n", x, y );
    return processTouchMessage( msg );
}

}
