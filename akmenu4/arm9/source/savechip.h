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









#ifndef _SAVECHIP_H_
#define _SAVECHIP_H_

#include <nds.h>

enum CHIP_TYPE
{
    CT_NONE = 0,
    CT_4K,
    CT_512K,
    CT_8M,
    CT_AUTO
};

void saveChipRead( u32 address, u8 * data, u32 length, u8 chipType );
void saveChipWrite( u32 address, u8 * data, u32 length, u8 chipType );
void saveChipSectorErase( u32 address );
//int saveChipFlashErase(bool erase4M);
int saveChipFlashErase(int type);
void saveChipEEPROMErase( int type );
u8 saveChipGetType();
u32 saveChipSize( u8 chipType );
u8 saveChipCMD( u8 cmd, u32 address );


#endif//_SAVECHIP_H_
