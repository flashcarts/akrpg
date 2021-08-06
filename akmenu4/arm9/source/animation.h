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









#ifndef _ANIMATION_H_
#define _ANIMATION_H_

#include "ui.h"

class cAnimation
{

public:

    cAnimation();

    virtual ~cAnimation();

public:

    virtual void update() = 0;

    void show();

    void hide();

    bool visible() { return _visible; }

protected:

    akui::cPoint _position;
    akui::cSize _size;
    bool _visible;

};

class cAnimationManager
{

public:

    cAnimationManager();

    ~cAnimationManager();

public:

    void update();

    void addAnimation( cAnimation * animation );

    void removeAnimation( cAnimation * animation );

protected:

    std::list< cAnimation * > _animations;

};

typedef t_singleton< cAnimationManager > animationManager_s;
inline cAnimationManager & animationManager() { return animationManager_s::instance(); }


#endif//_ANIMATION_H_

