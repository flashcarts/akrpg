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









#include "sprite.h"


cSprite::cSprite( u8 id )
{
    init( id );
}

cSprite::~cSprite()
{}

void cSprite::sysinit()
{
    SpriteEntry * psprites = (SpriteEntry *)OAM;
    //SpriteRotation * pspriteRotations = (SpriteRotation * )OAM;

    for(int i = 0; i < 128; i++)
    {
       psprites[i].attribute[0] = ATTR0_DISABLED;
       psprites[i].attribute[1] = 0;
       psprites[i].attribute[2] = 0;
       psprites[i].attribute[3] = 0;
    }

    //DC_FlushRange( _sprites, 128 * sizeof(SpriteEntry) );
    //dmaCopy( _sprites, OAM, 128 * sizeof(SpriteEntry) );
}

void cSprite::init( u16 id )
{
    //_alpha = 0;
    _id = id;

    _size = SS_SIZE_32;

    _shape = SS_SHAPE_SQUARE;

    _bufferOffset = 0;

    _priority = 2;

    _entry = ((SpriteEntry *)OAM) + _id;
    _affine = ((SpriteRotation * )OAM) + _id;

    // initial x = 0, hidden, bitmap obj mode, square shape
    _entry->attribute[0] = ATTR0_DISABLED | ATTR0_BMP | ATTR0_SQUARE | 0;

    // initial y = 0, size = 32x32, Affine Transformation Parameter group 0
    _entry->attribute[1] = ATTR1_SIZE_32 | ATTR1_ROTDATA(_id) | 0;

    // initial alpha = 15, buffer offset = 0, display priority 0
    _entry->attribute[2] = ATTR2_ALPHA(15) | ATTR2_PRIORITY(0) | 0;

    setScale( 1, 1 );

    //for(int i=0;i<32*32;i++)
    //    SPRITE_GFX[i]=RGB15(0,0,27)|(1<<15); //dont forget alpha bit

    //update();
}


void cSprite::show()
{
    _entry->attribute[0] = (_entry->attribute[0] & (~0x0300)) | ATTR0_ROTSCALE_DOUBLE | ATTR0_BMP;
}

void cSprite::hide()
{
    _entry->attribute[0] = (_entry->attribute[0] & (~0x0300)) | ATTR0_DISABLED;
}

void cSprite::setAlpha( u8 alpha )
{
    _alpha = alpha & 0x1f;
    _entry->attribute[2] = (_entry->attribute[2] & (~0xf000)) | ATTR2_ALPHA(_alpha);
}

void cSprite::setPosition( u16 x, u8 y )
{
    u8 offset = 0;
    switch( _size )
    {
    case SS_SIZE_8:
        offset = 4;
        break;
    case SS_SIZE_16:
        offset = 8;
        break;
    case SS_SIZE_32:
        offset = 16;
        break;
    case SS_SIZE_64:
        offset = 32;
        break;
    default:
        offset = 0;
    };
    x -= offset;
    _x = x & 0x1FF;
    _entry->attribute[1] = (_entry->attribute[1] & (~0x1FF)) | _x;

    y -= offset;
    _y = y & 0xFF;
    _entry->attribute[0] = (_entry->attribute[0] & (~0xFF)) | _y;
}

void cSprite::setSize( SPRITE_SIZE size )
{
    _size = size;
    _entry->attribute[1] = (_entry->attribute[1] & (~0xC000)) | _size;

}

void cSprite::setShape( SPRITE_SHAPE shape )
{
    _shape = shape;
    _entry->attribute[1] = (_entry->attribute[0] & (~0xC000)) | _shape;
}

u16 * cSprite::buffer()
{
    return SPRITE_GFX + (_bufferOffset * 64);
}

//void cSprite::update()
//{
//    //DC_FlushRange( _sprites, 128 * sizeof(SpriteEntry) );
//    SpriteEntry * psprites = (SpriteEntry * )OAM;
//    dmaCopy( _sprites, &psprites[_id], sizeof(SpriteEntry) );
//}

void cSprite::setBufferOffset( u32 offset )
{
    _bufferOffset = offset;
    _entry->attribute[2] = (_entry->attribute[2] & (~0x3FF)) | offset;
}


void cSprite::setScale( float scaleX, float scaleY )
{
    _scaleX = scaleX;
    _scaleY = scaleY;

    scaleX = 1 / scaleX;
    scaleY = 1 / scaleY;

    u8 decimalX = (u8)((scaleX - (int)scaleX) * 256);
    u8 integerX = (u8)((int)scaleX) & 0x7f;
    u8 decimalY = (u8)((scaleY - (int)scaleY) * 256);
    u8 integerY = (u8)((int)scaleY) & 0x7f;

    _affine->hdx = (integerX << 8) | decimalX;
    _affine->hdy = 0;
    _affine->vdx = 0;
    _affine->vdy = (integerY << 8) | decimalY;
}

void cSprite::setPriority( u8 priority )
{
    _priority = priority;
    _entry->attribute[2] = (_entry->attribute[2] & (~0x0C00)) | ATTR2_PRIORITY(_priority);
}


bool cSprite::visible()
{
    return (_entry->attribute[0] & 0x0300) != 0x0200;
}
