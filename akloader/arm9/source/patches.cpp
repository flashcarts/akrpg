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









#include <nds.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "resetpatch9_bin.h"
#include "patch7_bin.h"
#include "patches.h"


static inline void wait_press_b()
{
    printf("\npress B to continue.\n");
    scanKeys();
    u16 keys_up = 0;
    while( 0 == (keys_up & KEY_B) )
    {
        scanKeys();
        keys_up = keysUp();
    }
}

/*
static u32 uncompressRom( u8 *mainMemory, u32 codeLength, u32 memOffset )
{
    u8 * ADDR1 = NULL;
    u8 * ADDR1_END = NULL;
    u8 * ADDR2 = NULL;
    u8 * ADDR3 = NULL;

    u8 * pBuffer32 = (u8 *)(mainMemory);
    u8 * pBuffer32End = (u8 *)(mainMemory + codeLength);

    while( pBuffer32 < pBuffer32End ) {
        if( 0xDEC00621 == *(u32 *)pBuffer32 && 0x2106C0DE == *(u32 *)(pBuffer32 + 4) ) {
            ADDR1 = (u8 *)(*(u32 *)(pBuffer32 - 8));
            *(u32 *)(pBuffer32 - 8) = 0;
            break;
        }
        pBuffer32+=4;
    }
    if( 0 == ADDR1 )
        return 0;

    u32 A = *(u32 *)(ADDR1 + memOffset- 4);
    u32 B = *(u32 *)(ADDR1 + memOffset- 8);
    ADDR1_END = ADDR1 + A;
    ADDR2 = ADDR1 - (B >> 24);
    B &= ~0xff000000;
    ADDR3 = ADDR1 - B; // 2004000（代码要拷贝到的地址）
    u32 uncompressEnd = ((u32)ADDR1_END) - ((u32)mainMemory);

    while( !( ADDR2 <= ADDR3 ) ) {
        u32 marku8 = *(--ADDR2+ memOffset);
        //ADDR2 -= 1;
        int count = 8;
        while( true ) {
            count--;
            if( count < 0 )
                break;
            if( 0 == (marku8 & 0x80) ) {
                *(--ADDR1_END+ memOffset) = *(--ADDR2+ memOffset);
            } else {  //另一种算法
                int u8_r12 = *(--ADDR2+ memOffset);
                int u8_r7 = *(--ADDR2+ memOffset);
                u8_r7 |= (u8_r12 << 8);
                u8_r7 &= ~0xf000;
                u8_r7 += 2;
                u8_r12 += 0x20;

                do {
                    u8 realu8 = *(ADDR1_END+ memOffset + u8_r7);
                    *(--ADDR1_END+ memOffset) = realu8;
                    u8_r12 -= 0x10;
                } while (u8_r12 >= 0);
            }
            marku8 <<= 1;
            if( ADDR2 <= ADDR3 )  break;
        }
    }
    return uncompressEnd;

}
*/

void patchRom( const char * filename, u32 patchEntry9, u32 patchEntry7, u8 param )
{
    if( 0 == param )
        return;

    u32 * irqHandler7 = 0;
    u32 return7 = 0;
    for( u32 * i = (u32 *)NDSHeader.arm7destination; i < (u32 *)(NDSHeader.arm7destination+NDSHeader.arm7binarySize); ++i ) {
        if( 0x380fffc == *i && ( *(i+1) >=0x37f8000 && *(i+1) < 0x3800000 ) ) {
            irqHandler7 = i+1;
            return7 = *irqHandler7;
            break;
        }
    }

    u32 * irqHandler9 = 0;
    u32 return9 = 0;
    for( u32 * i = (u32 *)NDSHeader.arm9destination; i < (u32 *)(NDSHeader.arm9destination + NDSHeader.arm9binarySize); ++i ) {
        if( 0xffff0000 == *i ) {
            irqHandler9 = i-2;
            return9 = *irqHandler9;
            break;
        }
    }

//////////////////////////////

    // arm7
    // fix some addresses
    for( u32 * i = (u32 *)patch7_bin; i < (u32 *)(patch7_bin + patch7_bin_size); ++i ) {
        if( 0xEFC0DE01 == *i ) {          // redirect game irq handler
            *irqHandler7 = patchEntry7;
            *i = return7;
        } else if( 0xEFC0DE02 == *i ) {
            *i = param;
            //printf("patch param %08x\n", *i );
        }
    }
    memcpy( (void *)patchEntry7, patch7_bin, patch7_bin_size );
    loadARCodeTable( filename, patchEntry7, patchEntry7 + patch7_bin_size );

    if( param & BIT(2) ) {
        // arm9
        // fix some addresses
        for( u32 * i = (u32 *)resetpatch9_bin; i < (u32 *)(resetpatch9_bin + resetpatch9_bin_size); ++i ) {
            if( 0xEFC0DE01 == *i ) {          // redirect game irq handler
                *i = return9;
                //printf("irq entry replaced: %08x %08x\n", i, patchEntry );
            } else if( 0xEFC0DE02 == *i ) {
                *i = *irqHandler9;
            } else if( 0xEFC0DE03 == *i ) {
                *i = (u32)irqHandler9;
            }
        }
        *irqHandler9 = patchEntry9;
        memcpy( (void *)patchEntry9, resetpatch9_bin, resetpatch9_bin_size );
    }


    //////////////////
    *(u32 *)0x27fff68 = return7;
    for( u32 * i = (u32 *)NDSHeader.unknownRAM2; i < (u32 *)(NDSHeader.arm7destination+NDSHeader.arm7binarySize); ++i ) {
        if( 0x380fffc == *i && ( *(i+1) >=0x37f8000 && *(i+1) < 0x380fffc ) ) {
            //printf("[%08x]: %08x->%08x\n", (u32)i, *i, 0x27fff68 );
            *i = 0x27fff68;
        }
    }
    //wait_press_b();
}

void loadARCodeTable( const char * filename, u32 patchEntry7, u32 patchEntry7End )
{
    char * arcodeFilename = (char *)0x2290000;
    strncpy( arcodeFilename, filename, 512 );
    char * extName = strstr( arcodeFilename, ".nds" );
    strcpy( extName, ".cc" );

    FILE * fc = fopen( arcodeFilename, "rb" );
    //printf("%s\n", arcodeFilename );
    //wait_press_b();

    char * line = (char *)0x2290000;
    size_t fileSize = 0;
    if( NULL != fc ) {
        fseek( fc, 0, SEEK_END );
        fileSize = ftell( fc );
        fseek( fc, 0, SEEK_SET );
        fread( line, 1, fileSize, fc );
        fclose( fc );
    }

    unsigned int code = 0;
    unsigned int value = 0;
    u32 * codeTable = (u32 *)patchEntry7End;
    u32 * codeTableStart = codeTable;
    bool load = false;
    char * pline = line;
    while( pline < line + fileSize ) {
        if( '@' == *pline ) {
            load = true;
            while( *pline++ != '\n' && pline < line + fileSize ) {}
            continue;
        }
        if( '#' == *pline ) {
            load = false;
            while( *pline++ != '\n' && pline < line + fileSize ) {}
            continue;
        }

        if( load ) {
            if( (pline[0] >= '0' && pline[0] <= '9' ) || (pline[0] >= 'A' && line[0] <= 'F') || (pline[0] >= 'a' && pline[0] <= 'f' ) ) {
                code = strtoul( pline, NULL, 16 );
                value =  strtoul( pline + 8 , NULL, 16 );
                *codeTable++ =  code;
                *codeTable++ = value;
            }
        }
        while( *pline++ != '\n' && pline < line + fileSize ) {}
    }


    // fix some addresses
    for( u32 * i = (u32 *)patchEntry7; i < (u32 *)(patchEntry7End); ++i ) {
        if( 0xEFC0DE22 == *i ) {
            *i = (u32)codeTable;               // memory to store ar 'IF' condition flag stack
        }
        else if( 0xEFC0DE23 == *i ) {          // ar code table start
            *i = (u32)codeTableStart;
        }
        else if( 0xEFC0DE24 == *i ) {          // ar code table end
            *i = (u32)codeTable;
        }
    }
}
