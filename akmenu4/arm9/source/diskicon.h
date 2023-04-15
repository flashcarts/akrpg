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









#ifndef _DISKICON_H_
#define _DISKICON_H_

#include <nds.h>
#include "window.h"
#include "sprite.h"
#include "singleton.h"

class cDiskIcon : public akui::cWindow
{

public:

    cDiskIcon();

    ~cDiskIcon() {}

public:

    void draw();

    void turnOn();

    void turnOff();

    akui::cWindow & loadAppearance(const std::string& aFileName );

    void blink( float lightTime );

    float lightTime();

protected:

    bool _draw;

    float _lightTime;

    cSprite _icon;
};

typedef t_singleton< cDiskIcon > diskIcon_s;
inline cDiskIcon & diskIcon() { return diskIcon_s::instance(); }



#endif//_DISKIOICON_H_



