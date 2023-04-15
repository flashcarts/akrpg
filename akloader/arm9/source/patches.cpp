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
#include <fat.h>

#include "resetpatch9_bin.h"
#include "resetpatch9_extra_bin.h"
#include "patch7_bin.h"
#include "patch7_ac_bin.h"
#include "patch7_slowfix_bin.h" // gelu's d/l patch fix
#include "save_patch_bin.h"
#include "patches.h"
#include "dirtools.h"

#include "rpgprotocol.h"
#include "nanddriver.h"

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
#define EepromWaitBusy()   while (CARD_CR1 & /*BUSY*/0x80);
#define T3BLOCKSZ (256)
//  TYPE3 2Mbit FLASH MEMORY
static void saveChipWriteT3(u32 address, u8 *apData, u32 aLength, u8 chipType)
{
    int i;
    u32 last=address+aLength;

    while (address < last)
    {
        // set WEL (Write Enable Latch)
        CARD_CR1 = /*E*/0x8000 | /*SEL*/0x2000 | /*MODE*/0x40;
        CARD_EEPDATA = 0x06;
        EepromWaitBusy();

        CARD_CR1 = /*MODE*/0x40;

        // program maximum of 32 bytes
        CARD_CR1 = /*E*/0x8000 | /*SEL*/0x2000 | /*MODE*/0x40;
        CARD_EEPDATA = 0x02;
        EepromWaitBusy();
        CARD_EEPDATA = (address >> 16) & 0xFF;
        EepromWaitBusy();
        CARD_EEPDATA = (address >> 8) & 0xFF;
        EepromWaitBusy();
        CARD_EEPDATA = address & 0xFF;
        EepromWaitBusy();


        for (i = 0; address < last && i < T3BLOCKSZ; i++, address++)
        {
            CARD_EEPDATA = *apData;
            EepromWaitBusy();
            apData++;
        }

        CARD_CR1 = /*MODE*/0x40;

        // wait programming to finish
        CARD_CR1 = /*E*/0x8000 | /*SEL*/0x2000 | /*MODE*/0x40;
        CARD_EEPDATA = 0x05;
        EepromWaitBusy();

        do
        {
            CARD_EEPDATA = 0;
            EepromWaitBusy();
        } while (CARD_EEPDATA & 0x01);    // WIP (Write In Progress) ?
        EepromWaitBusy();
        CARD_CR1 = /*MODE*/0x40;
    }
}


static inline void mem_patch( u32 *dest, u32 *src, u32 s ) {
	for ( u32 i = 0 ; i < s; ++i ) {
		*(dest+ i ) = *(src + i);
	}
}

// Auto patch for Pokemon nds-gba linkage
static void pokemonPatch(u32 gameCode, u8 *param) {
	u8 bit6 = *param & BIT(6);
	u8 bit7 = *param & BIT(7);

	*param &= ~(BIT(7)); // clear bit6,7
	*param &= ~(BIT(6));

	gameCode &=  0x00FFFFFF;

    if( (bit6 || bit7) && ( gameCode == 0x00414441 || gameCode == 0x00415041)) { 
		u8 pc = 2;

		for( u32 * i = (u32 *)NDSHeader.arm9destination; i < (u32 *)(NDSHeader.arm9destination + NDSHeader.arm9binarySize); ++i ) {
			if( *i == 0xe1a04c00 && *(i+1) == 0xe3a0040a  ) {
				pc-=1;
				*(i-1) = 0xe3a00009;
				*(i+2) = 0xe3a000c2;

				u32 *j = i;
				while( !(*j == 0x0a005555 && *(j+1) == 0x0a002aaa) ) { j++; }
				*(j+0) = 0x0e005555;
				*(j+1) = 0x0e002aaa;
			}
			if ( *i == 0xe59f3024 && *(i+1) == 0xe3a020aa && *(i+2) == 0xe59f1020 
					&& *(i+3) == 0xe5c32000 && *(i+4) == 0xe3a02055
					&& *(i+5) == 0xe5c12000 && *(i+6) == 0xe3a010b0 ) {
				// bank switching
				pc-=1;
				if ( bit7 ) { // patch for 3in1 expansion
					u32 pp[] = { 
						0xe1a00000, 0xe3500000, 0x03a01010, 0x13a01020, 0xe3a02527, 0xe1c210b0, 0xe12fff1e 
					};
					if ( bit6 ) { // patch for psram mode
						pp[2] = 0x03a01060;
						pp[3] = 0x13a01070;
					}
					mem_patch( i, pp , 7); // sizeof(pp) is not correct about A*AK Roms:(
				}

				else if ( bit6 ) { // patch for ewin expansion
					u32 pp[] = { 
						0xe3500000, 0x03a01008, 0x13a01009, 0xe3a0240a, 0xe2823c55, 0xe5d33055,
						0xe2823c2a, 0xe5d330aa, 0xe2823a0b, 0xe5d33055, 0xe5c21000, 0xe12fff1e
						//0xe3a0140a, 0xe2812c55, 0xe5d23055, 0xe2812c2a, 0xe5d230aa, 0xe2812a0b, 
						//0xe5d23055, 0xe3500000, 0x03a030b8, 0x13a030b9, 0xe5c13000, 0xe12fff1e
					};
					mem_patch( i, pp , 12);
				}
			}
			if ( !pc ) break;
		}
		if ( !pc && bit7 ) {
			// ready to bank switching
			// it's too big to binary patch. anyway, there is no problem.
			*(vu16 *)0x9fe0000 = 0xd200;
			*(vu16 *)0x8000000 = 0x1500;
			*(vu16 *)0x8020000 = 0xd200;
			*(vu16 *)0x8040000 = 0x1500;
		}
		//wait_press_b();
	}
}

static void getLFN(const char *path,char *longFilename) {
    char dir[512];
	char fn[256];
    char * pdir = dir;
    dirFromFullPath( path, dir );
    char * filename = filenameFromFullPath( path );

	struct stat st;
	DIR_ITER * d = diropen( pdir );

	while (dirnextl(d, fn, longFilename, &st) == 0) {
		if (!(st.st_mode & S_IFDIR) && !strcasecmp(fn,filename)) return;
	}

	*longFilename = 0;

}

void loadARCodeTable( const char * filename, u32 patchEntry7, u32 patchEntry7End, u32 arCodeTableStart)
{
    char * arcodeFilename = (char *)0x2290000;
    strncpy( arcodeFilename, filename, 512 );


	char lfn[1024];

	_FAT_unicode_set_utf8(true);
	getLFN(filename,lfn);
	if ( lfn[0] != 0 ) {
		char * p = strrchr( arcodeFilename , '/' ); // replace sfn to lfn
		strcpy(p+1, lfn);
	}

    char * extName = strstr( arcodeFilename, ".nds" );
    strcpy( extName, ".cc" );

    FILE * fc = fopen( arcodeFilename, "rb" );
	if ( fc == NULL ) {
		extName = strstr( arcodeFilename, ".cc");
		strcpy( extName, ".nds.cc" );
		fc = fopen( arcodeFilename, "rb" );
	}
    
	//printf("%s\n", arcodeFilename );
	//if ( fc ) printf("FOPEN is OK\n");
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
	_FAT_unicode_set_utf8(false);

    unsigned int code = 0;
    unsigned int value = 0;
    u32 * codeTable = arCodeTableStart ? (u32 *)arCodeTableStart : (u32 *)patchEntry7End;
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
			if ( arCodeTableStart && ((codeTable - codeTableStart) >= (0x2000 - 0x8) ) ) break; // code table size check
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

void patchRom( const char * filename, u32 patchEntry9, u32 patchEntry7, u8 param )
{
	u32 gameCode = *(u32 *)NDSHeader.gameCode;
	u32 gameCode2 = gameCode & 0x00FFFFFF;

	pokemonPatch(gameCode, &param); //  bit6,7 are used for expansion pack, pokemon da/pe linkage patch

    // patch save module for some damn games.... like "2048 - Moon Harvest JPN"
    if( gameCode2 == 0x00334259 ) { // YB3*: Moon Harvest
        u32 patchEntry = 0x23fe000;
        memcpy( (void *)patchEntry, save_patch_bin, save_patch_bin_size );

        u32 Arm7SavePatchEntry = 0;

        for( u32 * i = (u32 *)patchEntry; i < (u32 *)(patchEntry + save_patch_bin_size); ++i ) {
            if( 0xEFC0DEE1 == *i ) {
                Arm7SavePatchEntry = (u32)(i+1);
            }
        }

        // save module
        u32 arm7DyPatchEntry = 0;
        for( u32 * i = (u32 *)patchEntry; i < (u32 *)(patchEntry + save_patch_bin_size); ++i ) {
            if( 0xEFC0DEB1 == *i )
                arm7DyPatchEntry = (u32)(i+2);
            else if( 0xEFC0DEB6 == *i ) {
                *i = Arm7SavePatchEntry;
            }
        }

        // dynamic patch
        *(u32 *)(0x2380000) = 0xE51FF004;//ldr pc, [pc, #-4]
        *(u32 *)(0x2380004) = arm7DyPatchEntry;

		patchEntry7 += save_patch_bin_size;
    } // save patch end


	if( gameCode2 == 0x004D4441 ) { // animal crossing
		param = 0;
		//patchEntry7 = 0x23ff100;
	}

	// save fix for 2385 - Daigasso Band Brothers DX -- bliss
	if ( gameCode2 == 0x00425841 ) {
		u8 check_header[16]; // read sram
		ioRpgReadSram(SRAM_SAVEDATA_START,check_header,16);
		if ( check_header[0] == 0xff ) {
			// clear save memory
			ALIGN(4) u8 buffer[0x28];
			memset(buffer,0x0,0x28);
			saveChipWriteT3(0xe0000,buffer , 0x28, 3);
			saveChipWriteT3(0xf0000,buffer , 0x28, 3);
			//saveChipWriteT3(0x100000,buffer , 0x28, 3);
		}
	}

    if( 0 == param )
        return;

	//// hooks

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
	u32 arCodeTableStart = 0;
	u32 patchEntry9Extra = 0;
	if ( param & BIT(2) ) {
		patchEntry9Extra = patchEntry7; // 0x23fe000
		patchEntry7 += resetpatch9_extra_bin_size;
		arCodeTableStart = 0x23fc000;
	}
	else if( gameCode2 != 0x004D4441 && gameCode2 != 0x00334259 ) { // !(animal crossing || Moon Harvest)
		patchEntry7 = 0x23fc000;
	}

//////////////////////////////
    // arm7
    // fix some addresses
	const u8 *p7_bin = patch7_bin;
	u32 p7_bin_size = patch7_bin_size;

	if ( (param & BIT(0)) && ((gameCode2 == 0x00584E41) ||(gameCode2 == 0x00334659) ) ) {
		p7_bin = patch7_slowfix_bin;
		p7_bin_size = patch7_slowfix_bin_size;
	}
	else if ( gameCode2 == 0x004D4441 ) {
		p7_bin = patch7_ac_bin;
		p7_bin_size = patch7_ac_bin_size;
	}

	for( u32 * i = (u32 *)p7_bin; i < (u32 *)(p7_bin + p7_bin_size); ++i ) {
		if( 0xEFC0DE01 == *i ) {          // redirect game irq handler
			*i = return7;
		} else if( 0xEFC0DE02 == *i ) {
			*i = param;
		}
	}
	*irqHandler7 = patchEntry7;
	memcpy( (void *)patchEntry7, p7_bin, p7_bin_size );
	loadARCodeTable( filename, patchEntry7, patchEntry7 + p7_bin_size, arCodeTableStart );

	// arm9
	// fix some addresses
    if( param & BIT(2) ) {
        for( u32 * i = (u32 *)resetpatch9_extra_bin; i < (u32 *)(resetpatch9_extra_bin + resetpatch9_extra_bin_size); ++i ) {
			if( 0xEFC0DE01 == *i ) {          // redirect game irq handler
                *i = return9;
                //printf("irq entry replaced: %08x %08x\n", i, patchEntry );
            } else if( 0xEFC0DE02 == *i ) {
                *i = *irqHandler9;
            } else if( 0xEFC0DE03 == *i ) {
                *i = (u32)irqHandler9;
            }
		}
        for( u32 * i = (u32 *)resetpatch9_bin; i < (u32 *)(resetpatch9_bin + resetpatch9_bin_size); ++i ) {
             if ( 0xEFC0DE01 == *i ) {
                *i = (u32)patchEntry9Extra;
            }
        }
        *irqHandler9 = patchEntry9;
        memcpy( (void *)patchEntry9, resetpatch9_bin, resetpatch9_bin_size );
        memcpy( (void *)patchEntry9Extra, resetpatch9_extra_bin, resetpatch9_extra_bin_size );
        *(u32 *)0x27fff6c = 0; // arm9ResetFlag
    }


    //////////////////
    *(u32 *)0x27fff68 = return7;
    for( u32 * i = (u32 *)NDSHeader.unknownRAM2; i < (u32 *)(NDSHeader.arm7destination+NDSHeader.arm7binarySize); ++i ) {
        if (gameCode2 == 0x00543841) { // mario party DS -- for MIC, gelu's patch
            if ( *i == return7 ) *i = patchEntry7;
        } else 
        if( 0x380fffc == *i && ( *(i+1) >=0x37f8000 && *(i+1) < 0x380fffc ) ) {
            //printf("[%08x]: %08x->%08x\n", (u32)i, *i, 0x27fff68 );
            *i = 0x27fff68;
        }
    }
	//printf("%08x\n", return7);
	//wait_press_b();
}

