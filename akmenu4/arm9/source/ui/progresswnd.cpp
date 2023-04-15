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









#include "progresswnd.h"

namespace akui
{

cProgressWnd::cProgressWnd()// s32 x, s32 y, u32 w, u32 h, cWindow * parent, const std::string & text )
: cForm(0,0,0,0,NULL,"" ),
_bar(0,0,180,24,this,""),
_tip(0,0,180,20,this,"")
{
    setSize( cSize( 226, 62 ) );
    setPosition( cPoint( 14, 64 ) );
}

cProgressWnd::~cProgressWnd()
{
}

void cProgressWnd::init()
{
    loadAppearance( SFN_PROGRESS_WND_BG );
    addChildWindow( &_bar );
    _bar.setRelativePosition( cPoint(4, 9) );
    _bar.setPercent( 0 );

    addChildWindow( &_tip );
    //_tip.setTextColor( RGB15(31,31,31) );
	_tip.setTextColor( uiSettings().formTextColor );
    _tip.setRelativePosition( cPoint(4,_size.y-24) );
    _tip.setSize( cSize( _size.x - 8, 12 ) );
    arrangeChildren();
    hide();
    windowManager().addWindow( this );
}

void cProgressWnd::draw()
{
    _renderDesc.draw( windowRectangle(), _engine );
    cForm::draw();
}

bool cProgressWnd::process( const cMessage & msg )
{
    bool ret = false;
    return ret;
}

cWindow& cProgressWnd::loadAppearance(const std::string& aFileName )
{
    //_renderDesc.loadData( SFN_FORM_TITLE_L, SFN_FORM_TITLE_R, SFN_FORM_TITLE_M );
    _renderDesc.loadData( aFileName );
    _bar.loadAppearance( SFN_PROGRESS_BAR_BG );

    return *this;
}

void cProgressWnd::setPercent( u8 percent )
{
    _bar.setPercent( percent );
    windowManager().update();
    gdi().present( GE_MAIN );
}

void cProgressWnd::setTipText( const std::string & tipText )
{
    _tip.setText( tipText );
}

void cProgressWnd::onShow()
{
    _bar.setPercent( 0 );
    windowManager().setPopUpWindow( this );
}

void cProgressWnd::onHide()
{
    _bar.setPercent( 0 );
    windowManager().setPopUpWindow( NULL );
}

}
