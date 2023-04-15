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









#include <stdio.h>
#include <string.h>
#include "msgdisplay.h"
#include "../../share/memtool.h"
#include "msgs_bin.h"


#define PMSGS(x) ((u8 *)(msgs_bin + x * 768))
#define MSGCOUNT (msgs_bin_size / 768)

static u16 * videoBuffer1;
static u16 * videoBuffer2;

void initMsg()
{
    vramSetBankA( VRAM_A_MAIN_BG_0x06000000 );
    vramSetBankB( VRAM_B_MAIN_BG_0x06020000 );
    videoSetMode(MODE_5_2D | DISPLAY_BG2_ACTIVE);

    BG2_CR = BG_BMP16_256x256;
    BG2_XDX = 1 << 8; // 2 =放大倍数
    BG2_YDY = 1 << 8;  // 2 =放大倍数
    BG2_XDY = 0;
    BG2_YDX = 0;
    BG2_CY = 0;
    BG2_CX = 0;

    videoBuffer1 = (u16 *)0x06000000;
    videoBuffer2 = (u16 *)0x06020000;

}

void cleanMsg()
{
    fillMemory( (void *)videoBuffer2, 0x20000, 0x80008000 );
}

void setMsg( u32 msgID, u16 line )
{
    if( msgID >= MSGCOUNT )
        return;

    u8 * msgBuffer = PMSGS( msgID );
    u32 bufferOffset = 256 * line;

    for( u32 i = 0; i < 256 * 24 / 8; ++i )    {
        for( u8 j = 0; j < 8; ++j ) {
            videoBuffer2[i*8+j+bufferOffset] = // 0 = black pixel, 1 = white pixel
                ((msgBuffer[i] >> (7-j)) & 1) ? (1 << 15) | RGB15(11,11,31) : 0x8000;
        }
    }
}

void showMsg()
{
    u16 * temp = videoBuffer1;
    videoBuffer1 = videoBuffer2;
    videoBuffer2 = temp;
    BG2_CR ^= BG_BMP_BASE( 128 / 16 );
}

void showMsg( u32 msgID, u16 line )
{
    cleanMsg();
    setMsg( msgID, line );
    showMsg();
}
