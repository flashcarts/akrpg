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









#ifndef _AKUI_BUTTON_H_
#define _AKUI_BUTTON_H_


#include "rectangle.h"
#include "window.h"
#include "renderdesc.h"
#include "bmp15.h"


namespace akui
{

class cButtonDesc;

class cButton : public cWindow
{

public:
    enum State
    {
        up = 0,
        down = 1
    };

    enum Style {
        single = 0,
        press = 1,
        toggle = 2
    };

    cButton( s32 x, s32 y, u32 w, u32 h, cWindow * parent, const std::string & text );

    ~cButton();

public:

    void draw();

    cWindow& loadAppearance(const std::string& aFileName );

    bool process( const cMessage & msg );

    State state() { return _state; }

    void setTextColor( COLOR color ) { _textColor = color; }

    COLOR textColor() { return _textColor; }

    void setStyle( Style style ) { _style = style; }

    Style style() { return _style; }

    void onPressed();

    void onReleased();

    void onClicked();

    Signal0 clicked;

    Signal0 pressed;

protected:

    bool processTouchMessage( const akui::cTouchMessage & msg );

    bool _captured;

    State _state;

    COLOR _textColor;

    cButtonDesc * _renderDesc;

    Style _style;

};

// form desc£¬Ö»¸ºÔð»­±³¾°
class cButtonDesc : public cRenderDesc
{
public:
    cButtonDesc();

    ~cButtonDesc();

public:

    cButtonDesc & setButton( cButton * button )  { _button = button; return *this; }

    void draw( const cRect & area, GRAPHICS_ENGINE engine ) const;

    void loadData( const std::string & filename );

protected:

    cButton *  _button;
    cBMP15 _background;
    COLOR _textColor;
};
}


#endif//_AKUI_BUTTON_H_
