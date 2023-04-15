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
#include "form.h"
#include "timer.h"
//#include "files.h"
//#include "dbgtool.h"
//#include "windowmanager.h"

namespace akui {

cForm::cForm( s32 x, s32 y, u32 w, u32 h, cWindow * parent, const std::string & text )
:cWindow( parent, text )
//_renderDesc(NULL)
{
    _size = cSize( w, h );
    _position = cPoint( x, y );
    _modalRet = -1;
}


cForm::~cForm()
{
    //if( _renderDesc )
    //    delete _renderDesc;
}


cForm& cForm::addChildWindow(cWindow* aWindow)
{
    _childWindows.push_back(aWindow);
    aWindow->setPosition( _position + aWindow->relativePosition() );
    //layouter_->addWindow(aWindow);
    return *this;
}

cForm& cForm::removeChildWindow(cWindow* aWindow)
{
    _childWindows.remove(aWindow);
    //layouter_->removeWindow(aWindow);
    return *this;
}

cForm& cForm::arrangeChildren()
{
    std::list< cWindow * >::iterator it;
    for(it = _childWindows.begin(); it != _childWindows.end(); ++it)
    {
        (*it)->setPosition( _position + (*it)->relativePosition() );
    }
    return *this;
}


void cForm::draw()
{
    std::list< cWindow * >::iterator it;
    for(it = _childWindows.begin(); it != _childWindows.end(); ++it)
    {
        (*it)->render();
    }
}


bool cForm::process( const cMessage & msg )
{
    dbg_printf("cForm::process\n" );
    bool ret = false;
    if(isVisible() && isEnabled())
    {
        if( msg.id() > cMessage::touchMessageStart && msg.id() < cMessage::touchMessageEnd )
        {
            std::list< cWindow * >::iterator it;
            for( it = _childWindows.begin(); it != _childWindows.end(); ++it)
            {
                cWindow * window = *it;
                ret = window->process(msg);
                if( ret ) {
                    dbg_printf("(%s) processed\n", window->text().c_str() );
                    break;
                }
            }
        }
    }

    // NOTE: cForm does not translate key messages to children in this case

    //if( !ret ) {
    //    dbg_printf("change child focus\n");
    //    if( msg.id() > cMessage::keyMessageStart && msg.id() < cMessage::keyMessageEnd ) {
    //        ret = processKeyMessage( (cKeyMessage &)msg );
    //    }
    //}

    if(!ret)
    {
        ret = cWindow::process(msg);
    }

    return ret;
}

bool cForm::processKeyMessage( const cKeyMessage & msg )
{
    bool ret = false;
    if( msg.id() == cMessage::keyDown ) {

        if( msg.keyCode() >=5 && msg.keyCode() <= 8 ) {

        std::list< cWindow * >::iterator it = _childWindows.begin();
        for( it = _childWindows.begin(); it != _childWindows.end(); ++it)
        {
            cWindow * window = *it;
            if( window->isFocused() ) {
                if( msg.keyCode() == cKeyMessage::UI_KEY_DOWN || msg.keyCode() == cKeyMessage::UI_KEY_RIGHT ) {
                    ++it;
                    if( it == _childWindows.end() )
                        it = _childWindows.begin();
                    if( (*it)->isVisible() && (*it)->isEnabled() ) {
                        windowManager().setFocusedWindow( (*it) );
                        ret = true;
                        break;
                    }
                } else if( msg.keyCode() == cKeyMessage::UI_KEY_UP || msg.keyCode() == cKeyMessage::UI_KEY_LEFT ) {
                    if( it == _childWindows.begin() ){
                        it = _childWindows.end();
                    }
                    --it;
                    if( (*it)->isVisible() && (*it)->isEnabled() ) {
                        windowManager().setFocusedWindow( (*it) );
                        ret = true;
                        break;
                    }
                }
            }
        }
        if( _childWindows.end() == it ) {
            if( _childWindows.front()->isVisible() && _childWindows.front()->isEnabled() ) {
                windowManager().setFocusedWindow( _childWindows.front() );
                ret = true;
            }
        }
        }
    }
    return ret;
}

cWindow* cForm::windowBelow(const cPoint& p)
{
    cWindow* ret = cWindow::windowBelow(p); // 先看自己在不在点下面

    if(ret != 0)
    {
        std::list<cWindow*>::iterator it;
        for(it = _childWindows.begin(); it != _childWindows.end(); ++it)
        {
            cWindow* window = *it;
            cWindow* cw = window->windowBelow(p);
            //dbg_printf( "check child (%s)\n", window->text().c_str() );
            if(cw != 0)
            {
                ret = cw;
                break;
            }
        }
    }

    return ret;
}

void cForm::onResize()
{
    arrangeChildren();
}

void cForm::onMove()
{
    arrangeChildren();
}

u32 cForm::modalRet()
{
    return _modalRet;
}

u32 cForm::doModal()
{
    windowManager().addWindow( this );
    bool parentIsPopup = ( windowManager().popUpWindow() == _parent );
    show();
    windowManager().setPopUpWindow( this );

    do { // manually update system loop
        timer().updateFps();
        INPUT & inputs = updateInput();
        processInput( inputs );
        windowManager().update();
        gdi().present( GE_MAIN );
        //dbg_printf( "modal window looping\n" );
    } while( modalRet() == (u32 )-1 );

    windowManager().removeWindow( this );
    if( parentIsPopup )
        windowManager().setPopUpWindow( _parent );
    else
        windowManager().setPopUpWindow( NULL );

    updateInput();
    windowManager().update();
    gdi().present( GE_MAIN );
    return modalRet();
}

void cForm::onOK()
{
    _modalRet = 1;
}

void cForm::onCancel()
{
    _modalRet = 0;
}

void cForm::centerScreen()
{
    _position.x = (SCREEN_WIDTH - _size.x) / 2;
    _position.y = (SCREEN_HEIGHT - _size.y) / 2;
}

}
