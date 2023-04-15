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









#ifndef _GDI_H_
#define _GDI_H_

#include <nds.h>
#include <vector>
#include "singleton.h"
#include "gdi.h"
#include "bmp15.h"

#define SYSTEM_FONT_HEIGHT 12
#define COLOR               u16

enum GRAPHICS_ENGINE
{
    GE_MAIN = 0,
    GE_SUB = 1
};

enum MAIN_ENGINE_LAYER
{
    MEL_UP = 0,
    MEL_DOWN = 1
};

enum SUB_ENGINE_MODE
{
    SEM_TEXT = 0,
    SEM_GRAPHICS = 1
};

class cGdi
{
public:

    cGdi();

    virtual ~cGdi();

public:

    void init();

    void swapLCD();

    void activeFbMain();        // fb = frame buffer

    void activeFbSub();

    void drawPixel( u8 x, u8 y, GRAPHICS_ENGINE engine ) {
        //if( y > (u8)SCREEN_HEIGHT - 1 ) y = (u8)SCREEN_WIDTH - 1;
        if( GE_MAIN == engine )
            *(_bufferMain2 + ((u32)y << 8) + x + _layerPitch ) = _penColor;        //_bufferMain2[y * SCREEN_WIDTH + x] = _penColor;
        else
            _bufferSub2[((u32)y << 8) + x] = _penColor;        //_bufferSub2[y * SCREEN_WIDTH + x] = _penColor;
    }

    void drawLine( s16 x1, s16 y1, s16 x2, s16 y2, GRAPHICS_ENGINE engine );

    void frameRect( s16 x, s16 y, u16 w, u16 h, GRAPHICS_ENGINE engine );

    void fillRect( u16 color1, u16 color2, s16 x, s16 y, u16 w, u16 h, GRAPHICS_ENGINE engine );

	void fillRectBlend( u16 color1, u16 color2, s16 x, s16 y, u16 w, u16 h,u16 opacity, GRAPHICS_ENGINE engine); // bliss

    void maskBlt( const void * src, s16 destX, s16 destY, u16 destW, u16 destH, GRAPHICS_ENGINE engine );

    void maskBlt( const void * src, s16 srcW, s16 srcH, s16 destX, s16 destY, u16 destW, u16 destH, GRAPHICS_ENGINE engine );

    void bitBlt( const void * src, s16 srcW, s16 srcH, s16 destX, s16 destY, u16 destW, u16 destH, GRAPHICS_ENGINE engine );

    void bitBlt( const void * src, s16 destX, s16 destY, u16 destW, u16 destH, GRAPHICS_ENGINE engine );

    void setPenColor( u16 color, GRAPHICS_ENGINE engine ) {
        if( GE_MAIN == engine )  _penColor = color | BIT(15);
        else _penColorSub = color | BIT(15);
    }

    void setTransColor( u16 color ) { _transColor = color | BIT(15); }

    void textOutRect( s16 x, s16 y, u16 w, u16 h, const char * text, GRAPHICS_ENGINE engine );

    inline void textOut( s16 x, s16 y, const char * text, GRAPHICS_ENGINE engine ) {
        textOutRect( x, y, 256, 192, text, engine );
    }

    void setMainEngineLayer( MAIN_ENGINE_LAYER layer ) { _mainEngineLayer = layer; _layerPitch = layer * 256 * 192; }

    void present( GRAPHICS_ENGINE engine );

    void switchSubEngineMode();

    void setVsyncPassed( bool passed ) { _vsyncPassed = passed; }

    bool vsyncPassed() { return _vsyncPassed; }

private:

    std::vector< u16 > _penColorStack;
    bool _vsyncPassed;
    u16 _penColor;
    u16 _penColorSub;
    u16 _transColor;
    u16 * _bufferMain1;
    u16 * _bufferMain2;
    u16 * _bufferMain3;
    u16 * _bufferMain4;
    MAIN_ENGINE_LAYER _mainEngineLayer;
    SUB_ENGINE_MODE _subEngineMode;
    u32 _layerPitch;
    u16 * _bufferSub1;
    u16 * _bufferSub2;
    u16 * _bufferSub3;
    //const u8 * _fontAscii12;
    //const u8 * _fontAscii10;
};

typedef t_singleton< cGdi > cGdi_s;
inline cGdi & gdi() { return cGdi_s::instance(); }


#endif//_GDI_H_
