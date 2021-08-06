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
#include <cstdio>
#include "gdi.h"
#include "dbgtool.h"
#include "sprite.h"
#include "../../share/memtool.h"
#include "input.h"
#include "globalsettings.h"
#include "fontfactory.h"

cGdi::cGdi()
{
    _transColor = 0;
    _mainEngineLayer = MEL_UP;
    _subEngineMode = SEM_TEXT;
}

cGdi::~cGdi()
{
    if( NULL != _bufferMain2 )
        delete[] _bufferMain2;
    if( NULL != _bufferSub2 )
        delete[] _bufferSub2;
}

void cGdi::init()
{
    swapLCD();
    activeFbMain();
    activeFbSub();
    cSprite::sysinit();
}

void cGdi::swapLCD()
{
    lcdSwap();
}

void cGdi::activeFbMain()
{
    vramSetBankB( VRAM_B_MAIN_BG_0x06000000 );
    vramSetBankD( VRAM_D_MAIN_BG_0x06020000 );

    vramSetBankA( VRAM_A_MAIN_SPRITE_0x06400000 );

    videoSetMode( MODE_5_2D
        | DISPLAY_BG2_ACTIVE
        | DISPLAY_BG3_ACTIVE
        | DISPLAY_SPR_ACTIVE
        | DISPLAY_SPR_1D_BMP_SIZE_128
        | DISPLAY_SPR_1D_BMP );

    BG2_CR = BG_BMP16_256x256 | BG_BMP_BASE(0) | BG_PRIORITY_1;
    BG2_XDX = 1 << 8; // 2 =放大倍数
    BG2_YDY = 1 << 8;  // 2 =放大倍数
    BG2_XDY = 0;
    BG2_YDX = 0;
    BG2_CY = 0;
    BG2_CX = 0;

    BG3_CR = BG_BMP16_256x256 | BG_BMP_BASE(8) | BG_PRIORITY_2;
    BG3_XDX = 1 << 8; // 2 =放大倍数
    BG3_YDY = 1 << 8;  // 2 =放大倍数
    BG3_XDY = 0;
    BG3_YDX = 0;
    BG3_CY = 0;
    BG3_CX = 0;

    _bufferMain1 = (u16 *)0x06000000;
    _bufferMain2 = (u16 *)new u32[256*192];
    _bufferMain3 = (u16 *)0x06020000;
    _bufferMain4 = _bufferMain2 + 256*192;

    setMainEngineLayer( MEL_UP );

    zeroMemory( _bufferMain1, 0x20000 );
    fillMemory( _bufferMain3, 0x20000, 0x8f008f00 );

    BLEND_CR = BLEND_ALPHA | BLEND_DST_BG2 | BLEND_DST_BG3;
    BLEND_AB = (4 << 8) | 7;

}

void cGdi::activeFbSub()
{
    // 分配显存存， 128k
    vramSetBankC( VRAM_C_SUB_BG_0x06200000 );    // 128k

    // 模式5，开两层BG，一层BMP，一层文字(用于调试)，bmp层现在默认关闭
    videoSetModeSub( MODE_5_2D | DISPLAY_BG2_ACTIVE );// | DISPLAY_BG2_ACTIVE );

    // 初始化为文字模式
    _subEngineMode = SEM_GRAPHICS;
    //_subEngineMode = SEM_TEXT;

    // BMP bg 的参数设置，从 VRAM地址 0x06200000 开始，优先级3
    SUB_BG2_CR = BG_BMP16_256x256 | BG_BMP_BASE(0) | BG_PRIORITY_1;
    SUB_BG2_XDX = 1<<8;
    SUB_BG2_YDY = 1<<8;
    SUB_BG2_XDY = 0;
    SUB_BG2_YDX = 0;
    SUB_BG2_CY = 0;
    SUB_BG2_CX = 0;

    _bufferSub1 = (u16 *)0x06200000;
    _bufferSub2 = (u16 *)new u32[256*192/2];
    _bufferSub3 = (u16 *)new u32[0x1200];

    fillMemory( _bufferSub2, 0x18000, 0xfc00fc00 );
    fillMemory( _bufferSub1, 0x18000, 0xfc00fc00 );

    // text BG
    // text bg 的字模占用 32(字节/字模) * 256(个ascii字) = 8192 字节显存，
    // 文字显示占用 32 x 32 * 2 = 2048 字节显存
    // 字模从 block 8 开始 = 0x06200000 + 8 * 0x4000      = 0x06220000
    // 文字信息从 block 72 开始 = 0x06200000 + 72 * 0x800 = 0x06224000
    // 优先级 2
    BG_PALETTE_SUB[255] = RGB15(31,31,31);    //by default font will be rendered with color 255
    SUB_BG0_CR = BG_TILE_BASE(0) | BG_MAP_BASE(8) | BG_PRIORITY_2;
    //consoleInitDefault((u16*)SCREEN_BASE_BLOCK_SUB(8), (u16*)CHAR_BASE_BLOCK_SUB(0), 16);
    consoleInitDefault( _bufferSub3 + 0x2000, _bufferSub3, 16);
}

void cGdi::drawLine( s16 x1, s16 y1, s16 x2, s16 y2, GRAPHICS_ENGINE engine )
{
    if((x1==x2)&&(y1==y2)) return;

    if(x1==x2) {
        int ys,ye;
        if(y1<y2) {
            ys=y1;
            ye=y2-1;
        } else {
            ys=y2+1;
            ye=y1;
        }
        for(int py=ys;py<=ye;py++) {
            drawPixel(x1,py,engine);
        }
        return;
    }

    if(y1==y2) {
        int xs,xe;
        if(x1<x2) {
            xs=x1;
            xe=x2-1;
        } else {
            xs=x2+1;
            xe=x1;
        }
        //for(int px=xs;px<=xe;px++) {
        //    drawPixel(px,y1,engine);
        //    //SetPixel(px,y1,Color);
        //}
        if( GE_MAIN == engine )
            fillRect( _penColor, _penColor, xs, y1, xe - xs + 1, 1, engine );
        else
            fillRect( _penColorSub, _penColorSub, xs, y1, xe - xs + 1, 1, engine );
        return;
    }

    if(abs(x2-x1)>abs(y2-y1)) {
        int px=0;
        float py=0;
        int xe=x2-x1;
        float ye=y2-y1;
        int xv;
        float yv;

        if(0<xe) {
            xv=1;
        } else {
            xv=-1;
        }
        yv=ye/abs(xe);

        while(px!=xe) {
            drawPixel(x1+px,y1+(int)py,engine);
            px+=xv;
            py+=yv;
        }
        return;
    }
    else {
        float px=0;
        int py=0;
        float xe=x2-x1;
        int ye=y2-y1;
        float xv;
        int yv;

        xv=xe/abs(ye);
        if(0<ye) {
            yv=1;
        } else {
            yv=-1;
        }

        while(py!=ye) {
            //if(AALineFlag==false){
            drawPixel(x1+(int)px,y1+py,engine);
            //}else{
            //    int Alpha=(int)(px*32);
            //    if(Alpha<0){
            //        while(Alpha<=0) Alpha+=32;
            //    }else{
            //        while(32<=Alpha) Alpha-=32;
            //    }
            //    SetPixelAlpha(x1+(int)px+0,y1+py,Color,32-Alpha);
            //    SetPixelAlpha(x1+(int)px+1,y1+py,Color,Alpha);
            //}
            px+=xv;
            py+=yv;
        }
        return;
    }
}

void cGdi::frameRect( s16 x, s16 y, u16 w, u16 h, GRAPHICS_ENGINE engine )
{
    drawLine( x, y, x + w - 1, y, engine );
    drawLine( x + w - 1, y, x + w - 1, y + h - 1, engine );
    drawLine( x + w - 1, y + h - 1, x, y + h - 1, engine );
    drawLine( x, y + h - 1, x, y, engine );
}

void cGdi::fillRect( u16 color1, u16 color2, s16 x, s16 y, u16 w, u16 h, GRAPHICS_ENGINE engine )
{
    ALIGN(4) u16 color[2] = { BIT(15) | color1 , BIT(15) | color2 };
    u16 * pSrc = (u16 *)color;
    u16 * pDest = NULL;


    if( GE_MAIN == engine )
        pDest = _bufferMain2 + (y << 8) + x + _layerPitch;//_bufferMain2 + y * 256 + x + _layerPitch;
    else
        pDest = _bufferSub2 + (y << 8) + x; //_bufferSub2 + y * 256 + x;

    bool destAligned = !(x & 1);

    u16 destInc = 256 - w;
    u16 halfWidth = w >> 1;
    u16 remain = w & 1;

    if( destAligned )
        for( u32 i = 0; i < h; ++i ) {
            swiFastCopy( pSrc, pDest, COPY_MODE_WORD | COPY_MODE_FILL | halfWidth );
            pDest += halfWidth << 1;
            if( remain )
                *pDest++ = *pSrc;
            pDest += destInc;
        }
    else
        for( u32 i = 0; i < h; ++i ) {
            for( u32 j = 0; j < w; ++j ) {
                *pDest++ = *pSrc;
            }
            pDest += destInc;
        }

}

void cGdi::bitBlt( const void * src, s16 srcW, s16 srcH, s16 destX, s16 destY, u16 destW, u16 destH, GRAPHICS_ENGINE engine )
{
    if( destW <= 0 )
        return;

    u16 * pSrc = (u16 *)src;
    u16 * pDest = NULL;

    if( GE_MAIN == engine )
        pDest = _bufferMain2 + (destY) * 256 + destX + _layerPitch;
    else
        pDest = _bufferSub2 + (destY) * 256 + destX;

    bool destAligned = !(destX & 1);

    if( destW > srcW )
        destW = srcW;
    if( destH > srcH )
        destH = srcH;

    u16 srcInc = srcW - destW;
    u16 destInc = 256 - destW;
    u16 destHalfWidth = destW >> 1;
    u16 lineSize = destW << 1;
    u16 remain = destW & 1;

    if( destAligned ) {
        for( u32 i = 0; i < destH; ++i ) {
            dmaCopyWords( 3, pSrc, pDest, lineSize );
            pDest += destHalfWidth << 1;
            pSrc += destHalfWidth << 1;
            if( remain )
                *pDest++ = *pSrc++;
            pDest += destInc;
            pSrc += srcInc;
        }
    }
}


void cGdi::bitBlt( const void * src, s16 destX, s16 destY, u16 destW, u16 destH, GRAPHICS_ENGINE engine )
{
    //dbg_printf("x %d y %d w %d h %d\n", destX, destY, destW, destH );
    u16 * pSrc = (u16 *)src;
    u16 * pDest = NULL;

    if( GE_MAIN == engine )
        pDest = _bufferMain2 + (destY) * 256 + destX + _layerPitch;
    else
        pDest = _bufferSub2 + (destY) * 256 + destX;

    u16 pitchPixel = (destW + (destW & 1));
    u16 destInc = 256 - pitchPixel;
    u16 halfPitch = pitchPixel >> 1;
    u16 remain = pitchPixel & 1;

    for( u16 i = 0; i < destH; ++i ) {
        swiFastCopy( pSrc, pDest, COPY_MODE_WORD | COPY_MODE_COPY | halfPitch );
        pDest += halfPitch << 1;
        pSrc += halfPitch << 1;
        if( remain )
            *pDest++ = *pSrc++;
        pDest += destInc;
    }
}

// maskBlt 要destW是偶数，速度可以快一倍
// 不是偶数也可以，但要求在内存中 src 的 pitch 凑成偶数
void cGdi::maskBlt( const void * src, s16 destX, s16 destY, u16 destW, u16 destH, GRAPHICS_ENGINE engine )
{
    //dbg_printf("x %d y %d w %d h %d\n", destX, destY, destW, destH );
    u16 * pSrc = (u16 *)src;
    u16 * pDest = NULL;
    bool destAligned = !(destX & 1);

    if( GE_MAIN == engine )
        pDest = _bufferMain2 + (destY)  * 256 + destX + _layerPitch;
    else
        pDest = _bufferSub2 + (destY) * 256 + destX;

    u16 pitch = (destW + (destW & 1));
    u16 destInc = 256 - pitch;
    u16 halfPitch = pitch >> 1;

    if( destAligned )
    for( u32 i = 0; i < destH; ++i ) {
        for( u32 j = 0; j < halfPitch; ++j ) {
            if( ((*(u32 *)pSrc) & 0x80008000) == 0x80008000 ) {
                *(u32 *)pDest = *(u32 *)pSrc;
                pSrc += 2; pDest += 2;
            }
            else {
                if( *pSrc & 0x8000 )
                    *pDest = *pSrc;
                pSrc++; pDest++;
                if( *pSrc & 0x8000 )
                    *pDest = *pSrc;
                pSrc++; pDest++;
            }
        }
        pDest += destInc;
    }
    else
    for( u16 i = 0; i < destH; ++i ) {
        for( u16 j = 0; j < pitch; ++j ) {
            if( *pSrc & 0x8000 )
                *pDest = *pSrc;
            pDest++; pSrc++;
        }
        pDest += destInc;
    }
}

void cGdi::maskBlt( const void * src, s16 srcW, s16 srcH, s16 destX, s16 destY, u16 destW, u16 destH, GRAPHICS_ENGINE engine )
{
    if( destW <= 0 )
        return;

    u16 * pSrc = (u16 *)src;
    u16 * pDest = NULL;

    if( GE_MAIN == engine )
        pDest = _bufferMain2 + (destY) * 256 + destX + _layerPitch;
    else
        pDest = _bufferSub2 + (destY) * 256 + destX;

    bool destAligned = !(destX & 1);

    if( destW > srcW )
        destW = srcW;
    if( destH > srcH )
        destH = srcH;

    u16 srcInc = srcW - destW;
    u16 destInc = 256 - destW;
    u16 destHalfWidth = destW >> 1;
    u16 pitch = (destW + (destW & 1));
    u16 remain = destW & 1;

    if( destAligned ) {
        for( u32 i = 0; i < destH; ++i ) {
            for( u32 j = 0; j < destHalfWidth; ++j ) {
                if( ((*(u32 *)pSrc) & 0x80008000) == 0x80008000 ) {
                    *(u32 *)pDest = *(u32 *)pSrc;
                    pSrc += 2; pDest += 2;
                }
                else {
                    if( *pSrc & 0x8000 )
                        *pDest = *pSrc;
                    pSrc++; pDest++;
                    if( *pSrc & 0x8000 )
                        *pDest = *pSrc;
                    pSrc++; pDest++;
                }
            }
            if( remain )
                *pDest++ = *pSrc++;
            pDest += destInc;
            pSrc += srcInc;
        }
    }
    else
    for( u16 i = 0; i < destH; ++i ) {
        for( u16 j = 0; j < pitch; ++j ) {
            if( *pSrc & 0x8000 )
                *pDest = *pSrc;
            pDest++; pSrc++;
        }
        pDest += destInc;
        pSrc += srcInc;
    }
}

void cGdi::textOutRect( s16 x, s16 y, u16 w, u16 h, const char * text, GRAPHICS_ENGINE engine )
{
    const u16 originX = x;
    //const u16 originY = y;
    const u8 * textPos = (const u8 *)text;
    while( *textPos != 0 )
    {
        if( '\r' == *textPos || '\n' == *textPos )
        {
            y += gs().fontHeight;
            x = originX;
            ++textPos;
            continue;
        } else if ( font().isWideChar( (const char*)textPos ) ) {
            if( x + gs().fontWidth < originX + w ) {
                if( GE_MAIN == engine )
                    font().drawWideChar( _bufferMain2, x, y, textPos, _penColor );
                else
                    font().drawWideChar( _bufferSub2, x, y, textPos, _penColorSub );
            }
            textPos += 2;
            x+= gs().fontWidth;
        } else {
            u8 aByte = *textPos;
            u8 startPoint = font().getCharStartPoint( aByte );
            u8 width = font().getCharScreenWidth( aByte );
            //if( aByte < 32 ) { aByte = '_'; }
            if( x + width < originX + w ) {
                if( GE_MAIN == engine )
                    font().drawByte( _bufferMain2, x - startPoint, y, aByte, _penColor );
                else
                    font().drawByte( _bufferSub2, x - startPoint, y, aByte, _penColorSub );
            }
            ++textPos;
            x += width;
        }
    }
}

void cGdi::present( GRAPHICS_ENGINE engine )
{
    //if( !_vsyncPassed )
    //    swiWaitForVBlank();

    if( GE_MAIN == engine ) { // 翻转主引擎
        //u16 * temp = _bufferMain1;
        //_bufferMain1 = _bufferMain2;
        //_bufferMain2 = temp;
        //BG2_CR ^= BG_BMP_BASE( 128 / 16 );

        dmaCopyWords( 3, _bufferMain2 + _layerPitch,
            _bufferMain1 + (_mainEngineLayer << 16), 256 * 192 * 2 );

        fillMemory( (void *)_bufferMain2, 256 * 192 * 2, 0 );

        //dmaCopyWordsAsynch( 3, (void *)_bufferMain4, ((u8 *)_bufferMain3), 256 * 192 * 2 );
        //fillMemory( (void *)_bufferMain4, 256 * 192 * 2, 0 );

    } else if ( GE_SUB == engine ) { // 翻转副引擎
        if( SEM_GRAPHICS == _subEngineMode )
            dmaCopyWords( 3, (void *)_bufferSub2, (void *)_bufferSub1, 256 * 192 * 2 );
        //else if( SEM_TEXT == _subEngineMode )
        //    dmaCopyWords( 3, (void *)_bufferSub3, (void *)_bufferSub1, 32768 );
        fillMemory( (void *)_bufferSub2, 0x18000, 0xfc00fc00 );
    }
    //dbg_printf( "\x1b[0;20%f\n", updateTimer() );
    _vsyncPassed = false;
}

void cGdi::switchSubEngineMode()
{
    // 需要保存和恢复文本模式的现场
    switch( _subEngineMode )
    {
    case SEM_GRAPHICS:   // 当前是图形模式的话，就恢复刚才的text现场
        videoSetModeSub( MODE_5_2D | DISPLAY_BG0_ACTIVE );
        consoleInitDefault((u16*)SCREEN_BASE_BLOCK_SUB(8), (u16*)CHAR_BASE_BLOCK_SUB(0), 16);
        dmaCopyWords( 3, (void *)_bufferSub3, (void *)_bufferSub1, 0x4800 );
        for( u8 i = 0; i < 23; ++i ) dbg_printf("\n");
        break;
    case SEM_TEXT:      // 当前是文字模式的话，保存现场，切到图形模式
        videoSetModeSub( MODE_5_2D | DISPLAY_BG2_ACTIVE );
        consoleInitDefault( _bufferSub3 + 0x2000, _bufferSub3, 16);
        dmaCopyWords( 3, (void *)_bufferSub1, (void *)_bufferSub3, 0x4800 );
        break;
    };
    _subEngineMode = (SUB_ENGINE_MODE)(_subEngineMode^1);
}
