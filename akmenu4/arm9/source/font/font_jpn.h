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
 * 2008/05/23
 *
 * Fixed to support various size fonts.
 *
 * by bliss
 *
 */






#include "font.h"


class cFontJpn : public cFont
{

public:

    cFontJpn( u8 language, const std::string & filename ) : cFont( language, filename)
    {
    }

    bool isWideChar( const char * aWord )
    {
        return ( NULL != _fontWideChar )
            && ( (*aWord >= 0x81 && *aWord <= 0x9F)
                || (*aWord >= 0xE0 && *aWord <= 0xEE)
                || (*aWord >= 0xFA && *aWord <= 0xFC) )
            && ( 0 != *(aWord + 1) )
            && ( ((*(aWord + 1) >= 0x40) && (*(aWord + 1) <= 0xFC)) ); // kanjiºº×Ö
    }

protected:

    u8 * getWideCharPixels( const u8 aWord[2] )
    {

        u8 qu;
        u16 wei;
        bool no_match = false;
        u8 qu_org = aWord[0];
        u8 qu_offset = 0;
        if (qu_org >= 0x81 && qu_org <= 0x9f){
            qu_offset = 0x81;
        }
        else if (qu_org >= 0xe0 && qu_org <= 0xee){
            qu_offset = 0x81 + (0xe0 - 0x9f) - 1;   // modify kzat3 2007.12.30
        }
        else if (qu_org >= 0xfa && qu_org <= 0xfc){
            qu_offset = 0x81 + (0xe0 - 0xa0) + (0xfa - 0xf0) + 1;   // modify kzat3 2007.12.30
        }
        else{
            no_match = true;
        }

        if (no_match){
            qu = 0x81;
            wei = 0x40;
        }
        else{
            qu = qu_org - qu_offset;    //qu ma

            u16 wei_org = aWord[1];
            u16 wei_offset = 0;
            if (wei_org >= 0x40 && wei_org <= 0xfc){
                wei_offset = 0x40;
            } else{
                no_match = true;
            }

            if (no_match){
                qu = 0x81;
                wei = 0x40;
            }
            else{
                wei = wei_org - wei_offset;    //wei ma
            }

			if ( _fontType && wei_org >= 0x7f ) wei -= 1 ; // bliss
        }

        //u32 fontOffset = (aWord[0] - 0x81) * 191 + aWord[1] - 0x40;
        u32 fontOffset = ( ( _fontType ? 188L : 189L )* qu + wei); // bliss
        u8 * fontPos = _fontWideChar + fontOffset * _fontDataSize + (_fontType ? 6 : 0);// + 200 ;
        return fontPos;

    }

};
//
//
//
//
//
//#include "font.h"
//
//
//
//
//
//class cFontJpn : public cFont
//{
//public:
//
//    cFontJpn( u8 language, const std::string & filename ) {}
//
//    ~cFontJpn() {}
//
//public:
//
//    bool isWideChar( const char * aWord ) {
//        return ( *aWord >= 0x81 ) &&  0 != *(aWord + 1) && *(aWord + 1) >= 0x40; // ºº×Ö
//    }
//
//protected:
//
//    u8 * getWideCharPixels( const u8 aWord[2] ) { return NULL; }
//
//};
