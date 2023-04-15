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

/*
 *
 * Updated by bliss (bliss@hanirc.org)
 *
 *  moonshell font type
 *  bitmap font of a various size.
 *  width of bitmap fonts
 *
 */







#include <nds.h>
#include "font.h"
#include "files.h"
#include "dbgtool.h"
#include "systemfilenames.h"
#include "globalsettings.h"
#include "asc10_fon_bin.h"
#include "asc12_fon_bin.h"
#include <fat.h>
#include <string.h>

cFont::cFont()
{
    _fontAscii = asc10_fon_bin;
    _fontWideChar = NULL;
    _language = 0;
	_fontHeight= 10;
	_fontType = 0;
}

cFont::cFont( u8 language, const std::string & filename )
{
    _fontAscii = asc10_fon_bin;
    _fontWideChar = NULL;
    _language = language;

	size_t fontSize = 0; 
    loadFile( NULL, filename, 0, fontSize );
    if( 0 == fontSize ) {
		return;
	}

    _fontWideChar = new u8[fontSize];
    bool ret = loadFile( _fontWideChar, filename, 0, fontSize );
    if( !ret ) {
        delete[] _fontWideChar;
        _fontWideChar = NULL;
    }

	//if ( gs().language == 8 ) _fontHeight = *(_fontWideChar+10) - 0x30 + 10;
	_fontHeight = 10;
	_fontType = 0;
	if ( !memcmp(_fontWideChar, "AKFN", 4) ) {
		_fontType = *(_fontWideChar + 4 );
		_fontHeight = *(_fontWideChar + 5);
		if ( _fontType < 1 ) _fontType = 0xff;
	}
	_fontDataSize = ((_fontHeight * _fontHeight) >> 3) + (((_fontHeight*_fontHeight)%8)? 1 : 0) ;

	// add kzat3 2007.12.30 for japnese half-wide charactor
	// We need add local.ank in __rpg\language\lang_jp
    fontSize = 0;
	loadFile( NULL, SFN_LOCAL_ANK_FONT_FILE, 0, fontSize );
	if (fontSize != 6144) return;
	ret = loadFile( (u8 *)_fontAscii, SFN_LOCAL_ANK_FONT_FILE, 0, fontSize );
	if ( !ret ) {
		delete[] _fontAscii;
		_fontAscii = (u8 *)asc10_fon_bin;
	}
}


cFont::~cFont()
{
    if( NULL !=    _fontWideChar )
        delete[] _fontWideChar;
}

u32 cFont::getStringScreenWidth( const char * str, size_t len )
{
    if( NULL == str || 0 == len )
        return 0;

    size_t strLen = strlen( str );
    if( len > strLen )
        len = strLen;

    const char * endstr = str + len;
    u32 width = 0;
    const char * p = str;
    while( *p != 0 && p < endstr ) {
        if( isWideChar( p ) ) {
            //width += gs().fontWidth;
            width += getWideCharScreenWidth( (const u8 *)p );
            p += 2;
        }
        else {
            if( *p != '\n' && *p != '\r' )
                width += getCharScreenWidth( *p );
            ++p;
        }
    }
    return width;
}


std::string cFont::breakLine( const std::string & text, u32 maxLineWidth )
{
    if( 0 == maxLineWidth )
        return text;

    std::string ret;

	// 找空格
	// 找到之后，和上次空格相减，传入 计算width 函数
	// 如果tempwdith超过 maxwidth，把上次的空格换成 \n
	// tempwidth 清零，继续


    const char * p = text.c_str();
    bool hasSpace = false;
    u32 tempWidth = 0;

    while( *p != 0 ) {
        if( isWideChar( p ) ) {
            //tempWidth += gs().fontWidth;
            tempWidth += getWideCharScreenWidth( (const u8 *)p );
            if( tempWidth > maxLineWidth ) {
                ret.push_back( '\n' );
                tempWidth = 0;
            }
            ret.push_back( *p++ );
            ret.push_back( *p++ );
        } else {
            if( ' ' == *p )
                hasSpace = true;
            tempWidth += getCharScreenWidth( *p );
            if( tempWidth > maxLineWidth ) {
                if( hasSpace ) {
                    u32 lastSpacePos = ret.find_last_of( ' ' );
                    ret[lastSpacePos] = '\n';
                    tempWidth = getStringScreenWidth(
                        text.c_str() + lastSpacePos, (size_t)(p - text.c_str()) - lastSpacePos );
                    hasSpace = false;
                } else {
                    ret.push_back( '\n' );
                    tempWidth = 0;
                }
            }
            ret.push_back( *p++ );
        }
    }

    return ret;
}

void cFont::drawByte( u16 * mem, s16 x, s16 y, u8 byte, u16 color )
{
    u32 offset = 24 * (byte);
    const u8 * fontPos = _fontAscii + offset;
    for( u8 i = 0; i < 12; ++i ) {
        for( u8 b = 0x80, k = 0; b > 0; b >>= 1, ++k ) {
            if( (*fontPos) & b ) {
                //gdi->drawPixel( x + k, y + i, engine );
                *(mem + x + k + ((y + i) << 8)) = color;
            }
        }
        ++fontPos;
        for( u8 b = 0x80, k = 8; b > 0x20; b >>= 1, ++k ) {
            if( (*fontPos) & b ) {
                //gdi->drawPixel( x + k, y + i, engine );
                *(mem + x + k + ((y + i) << 8)) = color;
            }
        }
        ++fontPos;
    }
}

u8 cFont::getWideCharScreenWidth( const u8 aWord[2] ) 
{ 
	if ( _fontType  == 0 ) return gs().fontWidth;

    u8 * fontPos = getWideCharPixels( aWord );
    if( NULL == fontPos ) return 0;

    u8 *p = fontPos;
    u8 px = 0, end = 0, width = 0 , b = 0x80;

	if ( _fontHeight == 10 ) {
		u8 * endp = fontPos + 13;
		size_t pixelCount = 0;

		p = fontPos + 10;
		px = 8;
		// last 2x10
		while( p < endp )
		{
			if( *p & b ) end = px;
			++pixelCount; ++px; b >>= 1;
			if( 0 == b ) { b = 0x80; ++p; }
			if( !(pixelCount & 1) ) {
				px = 8;
				if ( width < end ) width = end;
			}
		}
		// first 8x10
		if ( !width ) {
			p = fontPos;
			pixelCount = 0;
			px = 0;
			endp = fontPos + 10;
			while( p < endp )
			{
				if( *p & b ) end = px;
				++pixelCount; ++px; b >>= 1;
				if( 0 == b ) { b = 0x80; ++p; }
				if( !(pixelCount & 7) ) {
					px = 0;
					if ( width < end ) width = end;
				}
			}
		}
	}
	else {
		int i, bits = _fontHeight * _fontHeight;
		for (i=0; i<bits; i++) {
			if (i % 8 == 0) p = fontPos++;
			b = (*p >> (7 - (i % 8))) & 1;
			if ( b ) end = px;
			px += 1;
			if (px ==  _fontHeight) {
				px = 0;
				if ( width < end ) width = end;
			}
		}
	}

	return width ? width + 2 : gs().fontWidth; // space or not
}

void cFont::drawWideChar( u16 * mem, s16 x, s16 y, const u8 aWord[2], u16 color )
{

	if ( _fontType == 0 && gs().language == 8 ) {
		drawUniFontChar( mem, x, y, aWord, color );
		return;
	}
    u8 * fontPos = getWideCharPixels( aWord );

    if( NULL == fontPos )
        return;

    u8 * p = fontPos;
    u8 * endp = fontPos + 10;
    u8 px = x;
    y += (gs().fontHeight - 10) >> 1;
    u8 py = y;
    byte b = 0x80;
    size_t pixelCount = 0;

	// not 10x10 Font -- bliss
	if ( _fontHeight != 10 ) {
		int i, bits = _fontHeight * _fontHeight;
		if ( _fontHeight > 11 ) py -= 1;

		for (i=0; i<bits; i++) {
			if (i % 8 == 0) p = fontPos++;
			b = (*p >> (7 - (i % 8))) & 1;
			if ( b ) *(mem + px + (py << 8)) = color;
			px += 1;

			if (px == x + _fontHeight) {
				px = x;
			  	py += 1;
			}
		}
		return;
	}

	// 先画 8 x 10
    while( p < endp )
    {
		// 每个点 
        if( *p & b ) {
            *(mem + px + (py << 8)) = color;
        }
        ++pixelCount;
        ++px;
        b >>= 1;
        if( 0 == b ) {
            b = 0x80;
            ++p;
        }
		// 每8个点，换行
        if( !(pixelCount & 7) ) {
            px = x;
            ++py;
        }
    }
	// 再画 2 x 10
    pixelCount = 0;
    endp = fontPos + 13;
    x = x + 8;
    py = y;
    px = x;
    while( p < endp )
    {
		// 每个点 
        if( *p & b ) {
            *(mem + px + (py << 8)) = color;
        }
        ++pixelCount;
        ++px;
        b >>= 1;
        if( 0 == b ) {
            b = 0x80;
            ++p;
        }

		// 每2个点，换行
        if( !(pixelCount & 1) ) {
            px = x;
            ++py;
        }
    }
}

// modified by bliss - get from moonshell 
void cFont::drawUniFontChar( u16 * mem, s16 x, s16 y, const u8 aWord[2], u16 color )
{

	char uni[3];

	u8 local[3];
	local[0] = aWord[0]; 
	local[1] = aWord[1];
	local[2] = 0x0;

	_FAT_unicode_local_to_unicode( local,(UNICHAR *)uni);	

	u16 ccode = *((u16 *) uni);
	if ( !ccode ) return;

	u32 FontHeight=*(u16*)&_fontWideChar[0]; // first byte is FontHeight
	u8 *FontData = _fontWideChar + 4; // font data will start at forth byte.
	u32 px, py;

	u32 *pOffsetPtr=(u32*)&FontData[ccode*4]; 
	u8 *pDataPtr=(u8*)&FontData[*pOffsetPtr];

	u32 FontProWidth = *pDataPtr;
	pDataPtr++;

	if ( FontProWidth == 0 ) return;

	u16 *dstbuf = mem + ( x - 1 ) + ( y << 8 );

	u32 BitCount = 8;
	u32 BitImage = *pDataPtr;
	pDataPtr++;

	for(py=0;py<FontHeight ;py++){ 
		for(px=0;px<FontProWidth;px++){ 
			if((BitImage & 0x01)!=0) {
				dstbuf[px] = color;
			}
			BitCount--;
			BitImage>>=1;

			if(BitCount==0){ 
				BitCount=8;
				BitImage=*pDataPtr;
				pDataPtr++;
			}
		}
		dstbuf+=256; 
	}
}
