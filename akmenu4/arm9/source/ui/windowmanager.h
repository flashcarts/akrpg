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









#ifndef _AKUI_WINDOWMANAGER_H_
#define _AKUI_WINDOWMANAGER_H_

#include <string>
#include <list>
#include "window.h"
#include "input.h"

namespace akui {
// 这个类管理各种在屏幕上的组件，处理输入和渲染
class cWindowManager
{
private:

    // top level window 的意思就是最上层那个父窗口
    //! An internal structure which maintains information on top level windows
    struct TopLevelWindow
    {
        cWindow* window;
        int     zVal;
        TopLevelWindow(cWindow* aWindow, int aZVal ) : window(aWindow), zVal(aZVal) {}
        TopLevelWindow(const TopLevelWindow& tlw) { (*this) = tlw; }
        TopLevelWindow& operator = (const TopLevelWindow& tlw)
        { window = tlw.window; zVal = tlw.zVal; return *this; }
        bool operator == (const TopLevelWindow& tlw) const
        { return ((window == tlw.window) /*&& (zVal == tlw.zVal)*/); } // Modif SB
        bool operator != (const TopLevelWindow& tlw) const
        { return !((*this) == tlw); }
        bool operator > (const TopLevelWindow& tlw) const
        { return (zVal > tlw.zVal); }
    };

    typedef std::list<TopLevelWindow>  Windows;

public:

    cWindowManager();

    ~cWindowManager();

    //! returns a pointer to the window that currently has the focus
    cWindow* focusedWindow() const { return _focusedWindow; }

    //! Sets the focused window
    void setFocusedWindow(cWindow* aWindow);

    //! Add a top level window to the window manager
    cWindowManager& addWindow(cWindow* aWindow);

    //! Returns a pointer to the top popUp window if there is one, otherwise 0
    cWindow* popUpWindow() const;

    //! Remove all top level windows from the window manager without deleting them!!
    cWindowManager& removeAllWindows();

    //! Remove a particluar window from the window manager without deleting it!!
    cWindowManager& removeWindow(cWindow* aWindow);

    //!  Returns true if the passed in window is a top level window in the
    //! window manager's list
    bool isTopLevelWindow(cWindow* aWindow) const;

    //! Checks timers and renders the Gooey windows
    const cWindowManager& update();

    //! \brief Makes the passed in window's top level window the active window. It will be
    //! drawn infront of all other windows
    cWindowManager& setActiveWindow(cWindow* aWindow);

    //! sets a control to be regarded as popUp
    cWindowManager& setPopUpWindow(cWindow* aWindow)
    {
        _popUpWindow = aWindow;
        if( NULL != _popUpWindow )
            dbg_printf("setPopUpWindow() %s\n", _popUpWindow->text().c_str() );
        else
            dbg_printf("setPopUpWindow() NULL\n" );
        dbg_printf("_popUpWindow %08x\n", _popUpWindow );
        return *this;
    }

    //! Checks to see which window is below the cursor - get the result by calling windowBelowPen()
    cWindowManager& checkForWindowBelowPen( const cPoint & touchPoint );

    //! Returns a pointer to the window that is currently below the mouse cursor
    cWindow* windowBelowPen() const { return _windowBelowPen; }

    void updateFocusIfNecessary( const cPoint & touchPoint );

    //! pass the message on to all windows maintained by the manager for handling
    const bool process( cMessage & message ) const;

    bool processTouchMessage( cTouchMessage & message );

    bool onKeyDown( unsigned char keyCode );

    bool onKeyUp( unsigned char keyCode );

    bool onTouchDown( int x, int y );

    bool onTouchUp( int x, int y );

    bool onTouchMove( int x, int y );

    int zValueOf(cWindow* aWindow) const; //!< Returns the z value of the passed in window
protected:

    Windows _windows;        //!< the windows maintained by this class
    cWindow * _windowCapture;
    cWindow* _windowBelowPen;
    cWindow* _focusedWindow;  //!< A pointer to the window that currently has the input focus
    cWindow* _popUpWindow;   //!< A pop-up window (only one can be open at a time)

    int largestZValue() const; //!< Returns the largest z value of all windows

};

typedef t_singleton< cWindowManager > cWindowManager_s;
inline cWindowManager & windowManager() { return cWindowManager_s::instance(); }

}

#endif//_AKUI_WINDOWMANAGER_H_
