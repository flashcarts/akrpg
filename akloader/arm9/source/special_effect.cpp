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









#include <nds.h>
#include "special_effect.h"
#include "../../share/timetool.h"

void seFadeWhite( bool fadeOut )
{
    BLEND_CR = BLEND_SRC_BG2 | BLEND_FADE_WHITE;
    SUB_BLEND_CR = BLEND_SRC_BG2 | BLEND_FADE_WHITE;
    if( fadeOut ) {
        for( int i = 0; i <= 16; ++i )
        {
            swiWaitForVBlank();
            BLEND_Y = i;
            SUB_BLEND_Y = i;
            waitMs( 32 );
        }
    } else {
        for( int i = 16; i >=0; --i )
        {
            swiWaitForVBlank();
            BLEND_Y = i;
            SUB_BLEND_Y = i;
            waitMs( 32 );
        }
    }
}

void seFadeBlack( bool fadeOut )
{
    BLEND_CR = BLEND_SRC_BG2 | BLEND_FADE_BLACK;
    SUB_BLEND_CR = BLEND_SRC_BG2 | BLEND_FADE_BLACK;
    if( fadeOut ) {
        for( int i = 0; i <= 16; ++i )
        {
            swiWaitForVBlank();
            BLEND_Y = i;
            SUB_BLEND_Y = i;
            waitMs( 32 );
        }
    } else {
        for( int i = 16; i >=0; --i )
        {
            swiWaitForVBlank();
            BLEND_Y = i;
            SUB_BLEND_Y = i;
            waitMs( 32 );
        }
    }
}
