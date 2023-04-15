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









#include "unicode.h"
#include "systemfilenames.h"
#include "globalsettings.h"
#include "dbgtool.h"

u16 unicodeL2UTable[0x10000];
u16 unicodeU2LTable[0x10000];
u8 unicodeAnkTable[0x100];
static bool useConvertTables = false;

inline void clearMem( u32 dest, u32 size )
{
    int zero = 0;
    swiFastCopy( &zero, (u32*)dest, 0x01000000 | (size>>2));
}

typedef struct _IRREGULAR_CHAR {
    u16 u;
    char l;
} IRREGULAR_CHAR;
static IRREGULAR_CHAR specialChars[] = {
{0x20ac, 0x80},
{0x201a, 0x82},
{0x201e, 0x84},
{0x2026, 0x85},
{0x02c6, 0x88},
{0x0152, 0x8c},
{0x2018, 0x91},
{0x2019, 0x92},
{0x201c, 0x93},
{0x201d, 0x94},
{0x2022, 0x95},
{0x02dc, 0x98},
{0x2122, 0x99},
{0x203a, 0x9b},
{0x0153, 0x9c} };

static char isSpecialChar( u16 src ) {
    for( size_t i = 0; i < sizeof(specialChars) / sizeof(IRREGULAR_CHAR); ++i ) {
        if( src == specialChars[i].u )
            return (char)specialChars[i].l;// for DS system only
    }
    if( (src >= 0xe000 && src <= 0xe01c) || ( 0xe028 == src) ) {
        return (src - 0xe000 + 1); // for DS system only
    }
    return 0;
}
static std::string _unicode_to_ank_char( const u16 * uni_string )
{
    u16 local = 0;
    const u16 * src = (u16 *)uni_string;
    std::string ret;
    while( 0x0000 != *src )
    {
        local = (*src);
        char ic = isSpecialChar( local );
        if( ic != 0x00 ) {
            ret += ic;
        } else if( !(*src >= 0x20 && *src <= 0xff) && *src != '\r' && *src != '\n' ) {  // unvisible character
            ret += '_';
        }
        else {
            ret += local & 0xff;
            if( local & 0xff00 )
                ret += (local >> 8) & 0xff;
        }
        ++src;
    }
    return ret;
}

std::string unicode_to_local_string( const u16 * unicode_string, size_t length, bool * defCharUsed )
{
    if( defCharUsed )
        *defCharUsed = false;
    std::string ret;
    const u16 * p = unicode_string;

    if( !useConvertTables ) {
        return _unicode_to_ank_char( p );
    }

    while( *p != 0 && p < unicode_string + length )
    {
        u16 localChar = 0;
        char ic = isSpecialChar( *p );
        if( ic != 0x00 ) {
            localChar =  (u16)(' '<<8) | ic;
        } else
        if( *p < unicodeU2LTable[1] ) { // p's value < table size;
            localChar = unicodeU2LTable[(*p) + 2];
        }
        else {
            localChar = '?';
            // 使用 '?' 作为默认字符好处是，文件系统的文件名里不会包含 '?' 字符，错误容易发现
        }
        if( '?' == localChar && (u16)'?' != *p ) {
            if( defCharUsed )
                *defCharUsed = true;
            dbg_printf( "caution: uni %04x->%04x, CP%d\n",
                *p, localChar, unicodeU2LTable[0] );
        }

        ret.push_back( (char)(localChar & 0xFF) );
        if( (localChar & 0xFF00) ) {
            ret.push_back( (char)((localChar & 0xFF00)>>8) );
        }
        ++p;
    }
    return ret;
}

static uni_string _ank_char_to_unicode( const u8 * src )
{
    uni_string ret;
    while( 0 != *src )
    {
        ret.push_back( (u16)(*src) );
        src++;
    }
    return ret;
}

uni_string local_to_unicode_string( const char * local_string, size_t length, bool * defCharUsed )
{
    uni_string ret;
    const u8 * src = (u8 *)local_string;
    if( !useConvertTables ) {
        return _ank_char_to_unicode( (const u8 *)src );
    }

    while( 0 != *src )
    {
        u16 lc = 0;
        u16 uni = 0;

        lc=(u16)*src;
        src++;

        if( 0 == unicodeAnkTable[lc] && 0 != *src )
        {
            lc=(lc << 8)+((u16)*src);
            src++;
        }

        if( lc < 0xFFF0 )
        {
            uni = unicodeL2UTable[lc+2];
            if( uni == (UNICHAR)0)
                uni = (UNICHAR)'?';
        }
        else
        {
            uni=(UNICHAR)'?';
        }
        ret.push_back( uni );
    }

    return ret;
}

bool initUnicode()
{
    clearMem( (u32)unicodeL2UTable, 0x10000 );
    clearMem( (u32)unicodeU2LTable, 0x10000 );
    clearMem( (u32)unicodeAnkTable, 0x100 );
    useConvertTables = false;

    FILE * l2uf = fopen( ( SFN_LOCAL_TO_UNICODE ).c_str(), "rb" );
    if( l2uf ) {
        fseek( l2uf, 0, SEEK_END );
        u32 length = ftell( l2uf );
        fseek( l2uf, 0, SEEK_SET );
        fread( unicodeL2UTable, 1, length, l2uf );

        //size_t readed = fread( (u8 *)unicodeL2UTable, 1, 0x200, l2uf );
        //size_t count = 0;
        //while( readed == 0x200 ) {
        //    count += readed;
        //    readed = fread( (u8 *)unicodeL2UTable + count, 1, 0x200, l2uf );
        //    //dbg_printf("readed %d, count %d\n", readed, count );
        //}
        fclose( l2uf );
        l2uf = NULL;
    } else {
        return false;
    }

    dbg_printf("unicodeL2UTable ok\n");

    FILE * u2lf = fopen( ( SFN_UNICODE_TO_LOCAL ).c_str(), "rb" );
    if( u2lf ) {
        fseek( u2lf, 0, SEEK_END );
        u32 length = ftell( u2lf );
        fseek( u2lf, 0, SEEK_SET );
        fread( unicodeU2LTable, 1, length, u2lf );
        //size_t readed = fread( (u8 *)unicodeU2LTable, 1, 0x200, u2lf );
        //size_t count = 0;
        //while( readed == 0x200 ) {
        //    count += readed;
        //    readed = fread( (u8 *)unicodeU2LTable + count, 1, 0x200, u2lf );
        //    //dbg_printf("readed %d, count %d\n", readed, count );
        //}
        fclose( u2lf );
        u2lf = NULL;
    } else {
        return false;
    }

    dbg_printf("unicodeU2LTable ok\n");

    FILE * ankf = fopen( ( SFN_LOCAL_ANK_FILE ).c_str(), "rb" );
    if( ankf ) {
        size_t readed = fread( unicodeAnkTable, 1, 0x200, ankf );
        size_t count = 0;
        while( readed == 0x200 ) {
            count += readed;
            readed = fread( unicodeAnkTable + count, 1, 0x200, ankf );
        }
        fclose( ankf );
        ankf = NULL;
    } else {
        return false;
    }
    //wait_press_b();
    useConvertTables = true;
    return true;
}
