/*---------------------------------------------------------------------------------
 * Last Updated: 2008/05/21
 * @by bliss (bliss@hanirc.org)
 *
 * Copyright (C) 2008 Bliss. All rights reserved.
 * This code following BSD LICENSE.
 *
 * FONT HEADER: AKFN(4) + TYPE(1) + HEIGHT(1)
 * FONT TYPE 1(T): EUC-KR ( H: 0xA1 ~ 0xFE , L: 0xA1 ~ 0xFE )
 * FONT TYPE 2	 : CP949  ( H: 0x81 ~ 0xFE , L: 0x41 ~ 0x5A, 0x61 ~ 0x7A, 0x81 ~ 0xFE )
 * FONT TYPE 3	 : UNICODE( 0x0000 ~ 0xFFFE(0xFFFF:Unknown) , Index Size: 0x20000 )
 *
---------------------------------------------------------------------------------*/

#include "font.h"
#include <fat.h>

class cFontKor : public cFont
{

public:

    cFontKor( u8 language, const std::string & filename ) : cFont( language, filename)
    {
    }

    bool isWideChar( const char * aWord )
    {
		return ( NULL != _fontWideChar ) && 
			( *aWord >= 0x81 && *aWord <= 0xFE ) 
			&& ( 
				(*(aWord + 1) >= 0x41 && *(aWord +1) <= 0x5A ) ||
				(*(aWord + 1) >= 0x61 && *(aWord +1) <= 0x7A ) ||
				(*(aWord + 1) >= 0x81 && *(aWord +1) <= 0xFE ) 
					);
    }

protected:
	
    u8 * getWideCharPixels( const u8 aWord[2] )
    {
		u32 offset = 6;
		u32 fontOffset = 0;

		if ( _fontType == 1 ) { // cp949
			//fontOffset = (aWord[0] - 0x81 ) * 190 + aWord[1] - 0x41 ;
			u8 ff = aWord[0] - 0x81;
			u16 ss = aWord[1] - 0x41;
			if ( aWord[1] >  0x5a ) ss -= 6;
			if ( aWord[1] >  0x7a ) ss -= 6;
			fontOffset  = ff * 178 + ss;
		}
		else if ( _fontType == 2 ) { // unicode
			UNICHAR uni[4];

			u8 local[3];
			local[0] = aWord[0]; 
			local[1] = aWord[1];
			local[2] = 0x0;

			_FAT_unicode_local_to_unicode( local,(UNICHAR *)uni);	

			fontOffset = *((u16 *)uni);
			
			unsigned short *charmap = (unsigned short *)(_fontWideChar + offset);
			fontOffset = ( !charmap[fontOffset] ) ?  charmap[0xffff] : charmap[fontOffset];
			offset += sizeof(unsigned short) * 0x10000;
		}
		else // euckr
			fontOffset = ( aWord[0] < 0xa1 || aWord[1] < 0xa1 ) ? 0x2283 : (aWord[0] - 0xa1 ) * 94  + aWord[1] - 0xa1 ;

		fontOffset = offset + fontOffset * _fontDataSize;
		return (_fontWideChar + fontOffset);
    }

};
