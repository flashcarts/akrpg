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









#ifndef _USERWINDOW_H_
#define _USERWINDOW_H_

#include "window.h"
#include "bmp15.h"
#include "datetime.h"
#include "singleton.h"
#include "point.h"


class cUserWindow : public akui::cWindow
{
public:

    cUserWindow();

    ~cUserWindow() {}

public:


    void draw();

    akui::cWindow & loadAppearance(const std::string& aFileName ){return *this;}

protected:

    void init();

    cBMP15 _userPicture;
    int _px;
    int _py;
    std::string _userText;
    int _tx;
    int _ty;
    int _tw;
    int _th;
    u16 _userTextColor;
    std::string _userName;
    int _ux;
    int _uy;
    u16 _userNameColor;
    bool _showUserName;

};

typedef t_singleton< cUserWindow > UserWindow_s;
inline cUserWindow & userWindow() { return UserWindow_s::instance(); }


#endif//_UserWindow_H_
