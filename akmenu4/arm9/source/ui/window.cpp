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









#include "ui.h"
#include "window.h"
#include "windowmanager.h"

namespace akui {

cWindow::cWindow(cWindow* aParent, const std::string& aText) :
    _parent(aParent),
    _text(aText),
    _size(cSize(0, 0)),
    _position(cPoint(0, 0)),
    _relative_position(cPoint(0, 0)),
    _isVisible(true),
    _isEnabled(true),
    _isSizeSetByUser(false),
    _isDestroyed(false),
    _engine( GE_MAIN )
{
}

cWindow::~cWindow()
{
    if( isFocused() )
        windowManager().setFocusedWindow( NULL );
}

cWindow& cWindow::setWindowRectangle(const cRect& rect)
{
    setSize(rect.size());
    setPosition(rect.position());
    return *this;
}




cRect cWindow::windowRectangle() const
{
    return cRect(position(), position() + size());
}
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<


bool cWindow::isFocused() const
{
    return windowManager().focusedWindow() == this;
}


cWindow& cWindow::enableFocused()
{
    onGainedFocus();
    return *this;
}

cWindow& cWindow::disableFocused()
{
    onLostFocus();
    return *this;
}

cWindow* cWindow::windowBelow(const cPoint & p)
{
    cWindow* ret = 0;
    if(isVisible())
    {
        if(windowRectangle().surrounds(p)) ret = this;
    }
    return ret;
}


cWindow& cWindow::show()
{
    _isVisible = true;
    onShow();
    return *this;

}

cWindow& cWindow::hide()
{
    _isVisible = false;
    onHide();
    return *this;

}

bool cWindow::doesHierarchyContain(cWindow* aWindow) const
{
    return (aWindow == this);
}
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

cWindow* cWindow::topLevelWindow() const
{
    cWindow* ret = (cWindow*)(this);
    cWindow* test = ret;
    while(test != 0)
    {
        ret = test;
        test = ret->parent();
    }
    return ret;
}

cWindow& cWindow::setAsActiveWindow()
{
    windowManager().setActiveWindow(this);
    return *this;
}

bool cWindow::process( const cMessage & msg )
{
    //dbg_printf("%08x call default process()\n", this );
    return false;
}

cWindow& cWindow::render()
{
    ////dbg_printf("cWindow::render this is %08x\n", this );
    if( isVisible() )
        draw();
    return *this;
}

cWindow& cWindow::setSize(const cSize& aSize)
{
    _size = aSize;
    onResize();
    _isSizeSetByUser = true;
    return *this;
}

cWindow& cWindow::setPosition(const cPoint& aPosition)
{
    _position = aPosition;
    onMove();
    return *this;
}

cWindow& cWindow::setText(const std::string& aText)
{
    _text = aText;
    onTextChanged();
    return *this;
}

} // namespace akui

