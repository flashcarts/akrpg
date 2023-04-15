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









#ifndef _INPUT_H_
#define _INPUT_H_

#include <nds.h>

typedef struct T_INPUT
{
    u32 keysHeld;
    u32 keysUp;
    u32 keysDown;
    u32 keysDownRepeat;
    touchPosition touchPt;
    touchPosition movedPt;
    bool touchDown;
    bool touchUp;
    bool touchHeld;
    bool touchMoved;
    bool operator==( const T_INPUT & src ) {
        return keysHeld == src.keysHeld
            && keysUp == src.keysUp
            && keysDown == src.keysDown
            && keysDownRepeat == src.keysDownRepeat
            && touchPt.x == src.touchPt.x
            && touchPt.y == src.touchPt.y
            && movedPt.x == src.movedPt.x
            && movedPt.y == src.movedPt.y
            && touchDown == src.touchDown
            && touchUp == src.touchUp
            && touchHeld == src.touchHeld
            && touchMoved == src.touchMoved;
    }
} INPUT;

void initInput();
INPUT & updateInput();
INPUT & getInput();
u32 getInputIdleMs();
bool processInput( INPUT & inputs  );

#endif//_INPUT_H_
