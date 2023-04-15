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









#ifndef _UNICODE_H_
#define _UNICODE_H_

#include <nds.h>
#include <string>

typedef std::basic_string< u16 > uni_string;
typedef u16 UNICHAR;

extern u16 unicodeL2UTable[0x10000];
extern u16 unicodeU2LTable[0x10000];
extern u8 unicodeAnkTable[0x100];

bool initUnicode();

inline bool is_gbk( const u8 * code )
{
    if( *code > 0x80 )
    {
        if( 0 != *(code + 1) && *(code + 1) > 0x3E ) // ºº×Ö
        {
            return true;
        }
    }
    return false;
}

inline bool is_gb( const u8 * code )
{
    if( *code > 0xA0 && *code < 0xff )
    {
        if( 0 != *(code + 1) && *(code + 1) > 0xA0 && *(code + 1) < 0xff ) // gb2312ºº×Ö
        {
            return true;
        }
    }
    return false;
}

inline bool is_big5( const u8 * code )
{
    if( *code > 0x80 && *code < 0xff )
    {
        if( 0 != *(code + 1) &&
            (((*(code+1)>0x3f) && (*(code+1)<0x7f)) || (*(code+1)>0xA0 && *(code+1)<0xff)) ) // big5ºº×Ö
        {
            return true;
        }
    }
    return false;
}

inline bool is_shiftjis( const u8 * code )
{
    u8 qu = *code;
    if( (qu >= 0x81 && qu <= 0x9f)
        || (qu >= 0xe0 && qu <= 0xef)
        || (qu >= 0xfa && qu <= 0xfc))
    {
        u8 wei = *(code+1);
        if((wei >= 0x40 && wei <= 0x7e)
            || (wei >= 0x80 && wei <= 0xfc))
        {
            return true;
        }
    }
    return false;
}

std::string unicode_to_local_string( const u16 * unicode_string, size_t length, bool * defCharUsed = NULL );
uni_string local_to_unicode_string( const char * local_string, size_t length, bool * defCharUsed = NULL );



#endif//_UNICODE_H_
