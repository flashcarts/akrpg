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









#include "nandprotocol.h"
#include "rpgprotocol.h"
#include "dbgtool.h"


void npReadPage512( u32 addr, void * buffer )
{
    addr &= ~0x1ff;
    ioRpgReadNand( addr, buffer, 512 );
}

void npReadPage528( u32 addr, void * buffer )
{
    addr &= ~0x1ff;
    ioRpgReadNand( addr, buffer, 528 );
}

void npReadPage2112( u32 addr, void * buffer )
{
    addr &= ~0x7ff;
    ioRpgReadNand( addr, buffer, 2112 );
}

void npRead512Redundant( u32 addr, void * buffer )
{
    addr &= ~0x1ff;
    ioRpgReadNandRedundant( addr, buffer );
}

void npRead2KRedundant( u32 addr, void * buffer )
{
    addr &= ~0x7ff;
    addr += 512 * 3;
    ioRpgReadNandRedundant( addr, buffer );
}

void npWritePage512( u32 addr, const void * data, u16 logicBlk, u8 * subPageCache )
{
    addr &= ~0x1ff;
    // 数据
    static ALIGN(4) u8 writeData[528];

    u8 * redundatData = NULL;
    if( NULL == subPageCache ) {
        redundatData = writeData + 512;
        ioRpgMemCopy( writeData, data, 512 );
    } else {
        redundatData = subPageCache + 512;
        ioRpgMemCopy( subPageCache, data, 512 );
    }

    // 计算ECC
    u32 ecc1 = npMakeECC256( (u16 *)(data) );        // first 256 byte ecc
    u32 ecc2 = npMakeECC256( (u16 *)(((u8 *)data) + 256) );  // second 256 byte ecc
    u8 * pecc1 = (u8 *)&ecc1;
    u8 * pecc2 = (u8 *)&ecc2;

    // redundant 数据
    redundatData[0] = 0xFF;
    redundatData[1] = 0xFF;
    redundatData[2] = pecc1[0];
    redundatData[3] = pecc1[1];
    redundatData[4] = pecc1[2];
    redundatData[5] = pecc2[0];
    redundatData[6] = pecc2[1];
    redundatData[7] = pecc2[2];
    redundatData[8] = redundatData[10] = (logicBlk >> 8) & 0xFF;     // logic block 的高8位
    redundatData[9] = redundatData[11] = logicBlk & 0xFF;            // 低8位
    redundatData[12] = redundatData[13] = redundatData[14] = redundatData[15] = 0xFF;

    if( NULL == subPageCache ) {
        ioRpgWriteNand( addr, writeData, 528 );
    }
}

void npWritePage528( u32 addr, const void * data, u16 logicBlk )
{
    addr &= ~0x1ff;
    // 直接拷贝，不计算 ecc (实际有另外的模块已经计算过了)，但是要计算逻辑块
    u8 * redundatData = ((u8 *)data) + 512;

    redundatData[8] = redundatData[10] = (logicBlk >> 8) & 0xFF;     // logic block 的高8位
    redundatData[9] = redundatData[11] = logicBlk & 0xFF;            // 低8位
    redundatData[12] = redundatData[13] = redundatData[14] = redundatData[15] = 0xFF;
    ioRpgWriteNand( addr, data, 528 );
}

void npEraseBlock( u32 addr )
{
    ioRpgEraseNand( addr );
}

void npPageCopySafe( u32 src, u32 dest )
{
    ioRpgPageCopyNandToNand( src, dest );
}

void npPageCopyFast( u32 src, u32 dest )
{
    ioRpgPageCopyNandInternal( src, dest );
}

u32 npMakeECC256( u16 * data_buf )
{
    u32    i;
    u32    tmp;
    u32    uiparity = 0;
    u32    parityCol, ecc = 0;
    u32    parityCol4321 = 0, parityCol4343 = 0, parityCol4242 = 0, parityColTot = 0;
    u32*    Data = (u32*)(data_buf);
    u32    Xorbit=0;

    for( i = 0; i < 8; i++)
    {
        parityCol = *Data++;
        tmp = *Data++; parityCol ^= tmp; parityCol4242 ^= tmp;
        tmp = *Data++; parityCol ^= tmp; parityCol4343 ^= tmp;
        tmp = *Data++; parityCol ^= tmp; parityCol4343 ^= tmp; parityCol4242 ^= tmp;
        tmp = *Data++; parityCol ^= tmp; parityCol4321 ^= tmp;
        tmp = *Data++; parityCol ^= tmp; parityCol4242 ^= tmp; parityCol4321 ^= tmp;
        tmp = *Data++; parityCol ^= tmp; parityCol4343 ^= tmp; parityCol4321 ^= tmp;
        tmp = *Data++; parityCol ^= tmp; parityCol4242 ^= tmp; parityCol4343 ^= tmp; parityCol4321 ^= tmp;

        parityColTot ^= parityCol;

        tmp = (parityCol >> 16) ^ parityCol;
        tmp = (tmp >> 8) ^ tmp;
        tmp = (tmp >> 4) ^ tmp;
        tmp = ((tmp >> 2) ^ tmp) & 0x03;
        if ((tmp == 0x01) || (tmp == 0x02))
        {
            uiparity ^= i;
            Xorbit ^= 0x01;
        }
    }

    tmp = (parityCol4321 >> 16) ^ parityCol4321;
    tmp = (tmp << 8) ^ tmp;
    tmp = (tmp >> 4) ^ tmp;
    tmp = (tmp >> 2) ^ tmp;
    ecc |= ((tmp << 1) ^ tmp) & 0x200;    // p128

    tmp = (parityCol4343 >> 16) ^ parityCol4343;
    tmp = (tmp >> 8) ^ tmp;
    tmp = (tmp << 4) ^ tmp;
    tmp = (tmp << 2) ^ tmp;
    ecc |= ((tmp << 1) ^ tmp) & 0x80;    // p64

    tmp = (parityCol4242 >> 16) ^ parityCol4242;
    tmp = (tmp >> 8) ^ tmp;
    tmp = (tmp << 4) ^ tmp;
    tmp = (tmp >> 2) ^ tmp;
    ecc |= ((tmp << 1) ^ tmp) & 0x20;    // p32

    tmp = parityColTot & 0xFFFF0000;
    tmp = tmp >> 16;
    tmp = (tmp >> 8) ^ tmp;
    tmp = (tmp >> 4) ^ tmp;
    tmp = (tmp << 2) ^ tmp;
    ecc |= ((tmp << 1) ^ tmp) & 0x08;    // p16

    tmp = parityColTot & 0xFF00FF00;
    tmp = (tmp >> 16) ^ tmp;
    tmp = (tmp >> 8);
    tmp = (tmp >> 4) ^ tmp;
    tmp = (tmp >> 2) ^ tmp;
    ecc |= ((tmp << 1) ^ tmp) & 0x02;    // p8

    tmp = parityColTot & 0xF0F0F0F0 ;
    tmp = (tmp << 16) ^ tmp;
    tmp = (tmp >> 8) ^ tmp;
    tmp = (tmp << 2) ^ tmp;
    ecc |= ((tmp << 1) ^ tmp) & 0x800000;    // p4

    tmp = parityColTot & 0xCCCCCCCC ;
    tmp = (tmp << 16) ^ tmp;
    tmp = (tmp >> 8) ^ tmp;
    tmp = (tmp << 4) ^ tmp;
    tmp = (tmp >> 2);
    ecc |= ((tmp << 1) ^ tmp) & 0x200000;    // p2

    tmp = parityColTot & 0xAAAAAAAA ;
    tmp = (tmp << 16) ^ tmp;
    tmp = (tmp >> 8) ^ tmp;
    tmp = (tmp >> 4) ^ tmp;
    tmp = (tmp << 2) ^ tmp;
    ecc |= (tmp & 0x80000);    // p1

    ecc |= (uiparity & 0x01) <<11;    // parit256_1
    ecc |= (uiparity & 0x02) <<12;    // parit512_1
    ecc |= (uiparity & 0x04) <<13;    // parit1024_1

    if (Xorbit)
    {
        ecc |= (ecc ^ 0x00A8AAAA)>>1;
    }
    else
    {
        ecc |= (ecc >> 1);
    }

    ecc = ~ecc;
    /**(ecc_buf + 2) = (u8) (ecc >> 16);
    *(ecc_buf + 1) = (u8) (ecc >> 8);
    *(ecc_buf + 0) = (u8) (ecc);*/
    return ecc;
    //return ReverseU32(ecc);
}


bool npCheckEcc( u8 * nandEcc, u8 * readEcc )
{
    return (nandEcc[0] == readEcc[0])
            && (nandEcc[1] == readEcc[1])
            && (nandEcc[2] == readEcc[2])
            && (nandEcc[3] == readEcc[3])
            && (nandEcc[4] == readEcc[4])
            && (nandEcc[5] == readEcc[5]);
}

// return value:
// true:  no error or 1 bit ecc error, corrected
// false:  2 bit ecc error cannot correct

bool npEccCorrectData( u8 * data, u8 * nandEcc, u8 * readEcc )
{
    u8 a,b,c,x,i;

    for (i=0;  i<2;  i++)
    {
        b = readEcc[0] ^ nandEcc[0];
        a = readEcc[1] ^ nandEcc[1];
        c = readEcc[2] ^ nandEcc[2];
        if (a|b|c)
        {
            if ((((a^(a>>1)) & 0x55) == 0x55) && // Does each pair of parity bits contain
                (((b^(b>>1)) & 0x55) == 0x55) && // one error and one match?
                 (((c^(c>>1)) & 0x54) == 0x54))
            {
                x = (a & 0x80); // If so, there's a one-bit error in data[].
                if (a & 0x20) x |= 0x40; // Find which byte is in error...
                if (a & 0x08) x |= 0x20;
                if (a & 0x02) x |= 0x10;
                if (b & 0x80) x |= 0x08;
                if (b & 0x20) x |= 0x04;
                if (b & 0x08) x |= 0x02;
                if (b & 0x02) x |= 0x01;
                a = 0; // ... and which bit...
                if (c & 0x80) a |= 0x04;
                if (c & 0x20) a |= 0x02;
                if (c & 0x08) a |= 0x01;
                data[x+ (u16)(i<<8)] ^= (1 << a); // ... and correct it.
                // one bit is ok
            }
            else {
                return false;
            }  //  two bits return error
        }
        readEcc[0] = readEcc[3];     // next 256 byte ecc
        readEcc[1] = readEcc[4];
        readEcc[2] = readEcc[5];
        nandEcc[0] = nandEcc[3];
        nandEcc[1] = nandEcc[4];
        nandEcc[2] = nandEcc[5];
    }
    return true;
}
