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









#include <cstring>
#include "dbgtool.h"
#include "input.h"
#include "windowmanager.h"
#include "keymessage.h"
#include "timer.h"
#include "ipctool.h"

using namespace akui;

static INPUT inputs;
static INPUT lastInputs;
static double lastInputTime;
static u32 idleMs;

void initInput()
{
    lastInputTime = 0;
    idleMs = 0;
    keysSetRepeat( 30, 1 );
}

INPUT & updateInput()
{
    memset( &inputs, 0, sizeof(inputs) );
    inputs.touchPt = touchReadXY();
    if( inputs.touchPt.px == 0 && inputs.touchPt.py == 0 ) {
        if( lastInputs.touchHeld ) {
            inputs.touchUp = true;
            inputs.touchPt = lastInputs.touchPt;
            dbg_printf( "getInput() Touch UP! %d %d\n", inputs.touchPt.px, inputs.touchPt.py );
        } else {
            inputs.touchUp = false;
        }
        inputs.touchDown = false;
        inputs.touchHeld = false;
    } else {
        if( !lastInputs.touchHeld ) {
            inputs.touchDown = true;
            dbg_printf( "getInput() Touch DOWN! %d %d\n", inputs.touchPt.px, inputs.touchPt.py );
        } else {
            inputs.movedPt.px = inputs.touchPt.px - lastInputs.touchPt.px;
            inputs.movedPt.py = inputs.touchPt.py - lastInputs.touchPt.py;
            inputs.touchMoved = (0 != inputs.movedPt.px) || (0 != inputs.movedPt.py);
            inputs.touchDown = false;
        }
        inputs.touchUp = false;
        inputs.touchHeld = true;
    }
    //dbg_printf( "touch x %d y %d\n", inputs.touchPt.px, inputs.touchPt.py );
    //dbg_printf( "touchdown %d clicked %d\n", inputs.touchDown, inputs.clicked );
    scanKeys();
    inputs.keysDown = keysDown();
    inputs.keysUp = keysUp();
    inputs.keysHeld = keysHeld();
    inputs.keysDownRepeat = keysDownRepeat();
    if( lastInputs == inputs ) {
        idleMs = (u32)( (timer().getTime() - lastInputTime) * 1000);
    }
    else {
        //dbg_printf( "input idled %d\n", idleMs );
        lastInputTime = timer().getTime();
        idleMs = 0;
    }
    lastInputs = inputs;

    return inputs;
}

INPUT & getInput()
{
    return inputs;
}


u32 getInputIdleMs()
{
    return idleMs;
}

bool processInput( INPUT & inputs  )
{
    bool ret = false;
    if( inputs.keysDown & KEY_A )
        ret = ret || windowManager().onKeyDown( cKeyMessage::UI_KEY_A );
    if( inputs.keysDown & KEY_B )
        ret = ret || windowManager().onKeyDown( cKeyMessage::UI_KEY_B );
    if( inputs.keysDown & KEY_X )
        ret = ret || windowManager().onKeyDown( cKeyMessage::UI_KEY_X );
    if( inputs.keysDown & KEY_Y )
        ret = ret || windowManager().onKeyDown( cKeyMessage::UI_KEY_Y );
    if( inputs.keysDown & KEY_R )
        ret = ret || windowManager().onKeyDown( cKeyMessage::UI_KEY_R );
    if( inputs.keysDown & KEY_L )
        ret = ret || windowManager().onKeyDown( cKeyMessage::UI_KEY_L );
    if( inputs.keysDown & KEY_START || inputs.keysDownRepeat & KEY_START )
        ret = ret || windowManager().onKeyDown( cKeyMessage::UI_KEY_START );
    if( inputs.keysDown & KEY_SELECT )
        ret = ret || windowManager().onKeyDown( cKeyMessage::UI_KEY_SELECT );
    if( inputs.keysDown & KEY_LEFT || inputs.keysDownRepeat & KEY_LEFT )
        ret = ret || windowManager().onKeyDown( cKeyMessage::UI_KEY_LEFT );
    if( inputs.keysDown & KEY_RIGHT || inputs.keysDownRepeat & KEY_RIGHT )
        ret = ret || windowManager().onKeyDown( cKeyMessage::UI_KEY_RIGHT );
    if( inputs.keysDown & KEY_UP || inputs.keysDownRepeat & KEY_UP )
        ret = ret || windowManager().onKeyDown( cKeyMessage::UI_KEY_UP );
    if( inputs.keysDown & KEY_DOWN || inputs.keysDownRepeat & KEY_DOWN )
        ret = ret || windowManager().onKeyDown( cKeyMessage::UI_KEY_DOWN );

    if( inputs.touchDown )
        ret = ret || windowManager().onTouchDown( inputs.touchPt.px, inputs.touchPt.py );
    if( inputs.touchUp )
        ret = ret || windowManager().onTouchUp( inputs.touchPt.px, inputs.touchPt.py );
    if( inputs.touchMoved )
        ret = ret || windowManager().onTouchMove( inputs.movedPt.px, inputs.movedPt.py );


    if( inputs.keysDown & KEY_LID ) {
        dbg_printf("lid closed\n");
        IPC_ARM9 = IPC_MSG_SUSPEND; // just turn screen off, render loop is still running
    } else if( inputs.keysUp & KEY_LID ) {
        dbg_printf("lid opened\n");
        IPC_ARM9 = IPC_MSG_WAKEUP; // just turn screen off, render loop is still running
    }

    return ret;
}
