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









#ifndef _SPRITE_H_
#define _SPRITE_H_

#include <nds.h>

enum SPRITE_SIZE
{
    SS_SIZE_8 = ATTR1_SIZE_8,
    SS_SIZE_16 = ATTR1_SIZE_16,
    SS_SIZE_32 = ATTR1_SIZE_32,
    SS_SIZE_64 = ATTR1_SIZE_64
};

enum SPRITE_SHAPE
{
    SS_SHAPE_SQUARE = ATTR0_SQUARE,
    SS_SHAPE_WIDE = ATTR0_WIDE,
    SS_SHAPE_TALL = ATTR0_TALL
};


class cSprite
{
public:

    cSprite() { init(0); }

    cSprite( u8 id );

    ~cSprite();

    static void sysinit();
    void init( u16 id );
    //void update();
    void setAlpha( u8 alpha );
    void setPosition( u16 x, u8 y );
    void setSize( SPRITE_SIZE size );
    void setShape( SPRITE_SHAPE shape );
    void setBufferOffset( u32 offset );
    void setScale( float scaleX, float scaleY );
    void setRotation( float angle );
    void show();
    void hide();
    bool visible();
    void setPriority( u8 priority );

    u16 * buffer();

    //cSprite & operator=( const cSprite & src );

protected:

    SpriteEntry * _entry;
    SpriteRotation * _affine;
    u8 _id;
    SPRITE_SIZE _size;
    SPRITE_SHAPE _shape;
    u16 _x;
    u8 _y;
    float _scaleX;
    float _scaleY;
    float _rotation;
    u8 _alpha;
    u8 _priority;
    u16 _bufferOffset;
};


//class cSpritePool
//{
//public:
//    cSprite * createSprite( SPRITE_SIZE size, SPRITE_SHAPE shape );
//
//    void destroySprite( cSprite * pointer );
//
//};



#endif//_SPRITE_H_
