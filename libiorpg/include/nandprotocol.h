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









#ifndef _NANDPROTOCOL_H_
#define _NANDPROTOCOL_H_

#include <nds.h>

#ifdef __cplusplus
extern "C" {
#endif

void npReadPage512( u32 addr, void * buffer, void * redudant );

void npReadPage528( u32 addr, void * buffer );

void npReadPage2112( u32 addr, void * buffer );

void npRead512Redundant( u32 addr, void * buffer );

void npRead2KRedundant( u32 addr, void * buffer );

void npWritePage512( u32 addr, const void * data, u16 logicBlk, u8 * subPageCache );

void npWritePage528( u32 addr, const void * data, u16 logicBlk );

void npEraseBlock( u32 addr );

void npPageCopySafe( u32 src, u32 dest );

void npPageCopyFast( u32 src, u32 dest );

u32 npMakeECC256( u16 * data_buf );

bool npCheckEcc( u8 * nandEcc, u8 * readEcc );

bool npEccCorrectData( u8 * data, u8 * nandEcc, u8 * readEcc );

#ifdef __cplusplus
}
#endif

#endif//_NANDPROTOCOL_H_
