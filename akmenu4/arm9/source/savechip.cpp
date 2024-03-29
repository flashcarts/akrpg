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









#include <cstring>
#include "savechip.h"
#include "dbgtool.h"
#include "rpgprotocol.h"
#include "diskicon.h"

#define EepromWaitBusy()   while (CARD_CR1 & /*BUSY*/0x80);

//void saveChipReadEeprom(u32 address, u8 *data, u32 length, u8 chipType);
void saveChipWriteT1(u32 address, u8 *apData, u32 aLength, u8 chipType);
void saveChipWriteT2(u32 address, u8 *apData, u32 aLength, u8 chipType);
void saveChipWriteT3(u32 address, u8 *apData, u32 aLength, u8 chipType);

u8 saveChipCMD( u8 cmd, u32 address )
{
    //    int address=i & 1;
    u8 ret;
    int k;
    CARD_CR1 = /*E*/0x8000 | /*SEL*/0x2000 | /*MODE*/0x40;

    CARD_CR1 = 0xFFFF;
    CARD_EEPDATA = cmd;    //

    EepromWaitBusy();

    CARD_EEPDATA = (address >> 16) & 0xFF;
    EepromWaitBusy();

    CARD_EEPDATA = (address >> 8) & 0xFF;
    EepromWaitBusy();

    CARD_EEPDATA = (address) & 0xFF;
    EepromWaitBusy();

    for(k=0;k<256;k++)
    {
        ret = CARD_EEPDATA;
        if(ret!=0xFF)break;
        EepromWaitBusy();
    }
    CARD_CR1 = /*MODE*/0x40;
    return ret;
}


u8 saveChipReadID(int i)
{
    int address=i & 1;
    u8 ret;
    int k;
    CARD_CR1 = /*E*/0x8000 | /*SEL*/0x2000 | /*MODE*/0x40;

    CARD_CR1 = 0xFFFF;
    CARD_EEPDATA = 0xAB;    // READ ID

    EepromWaitBusy();

    CARD_EEPDATA = (address >> 16) & 0xFF;
    EepromWaitBusy();

    CARD_EEPDATA = (address >> 8) & 0xFF;
    EepromWaitBusy();

    CARD_EEPDATA = (address) & 0xFF;
    EepromWaitBusy();

    for(k=0;k<16;k++)
    {
        ret = CARD_EEPDATA;
        if(ret!=0xFF)break;
        EepromWaitBusy();
    }
    CARD_CR1 = /*MODE*/0x40;
    return ret;
}



u8 saveChipGetType(void)
{
    u8 c00;
    u8 c05;
    u8 c9f;
    u8 c03;

    REG_EXMEMCNT &= ~0x0880;            // DS Card access ARM9:bit11=0   GBA Cart access ARM9:bit7=0
    c03=saveChipCMD(0x03,0);
    c05=saveChipCMD(0x05,0);
    c9f=saveChipCMD(0x9f,0);
    c00=saveChipCMD(0x00,0);
    if((c00==0x00) && (c9f==0x00))return 0;    //    PassMe? 判別不可
    if((c00==0xff) && (c05==0xff) && (c9f==0xff))return -1;

    if((c00==0xff) &&  (c05 & 0xFD) == 0xF0 && (c9f==0xff))return 1;
    if((c00==0xff) &&  (c05 & 0xFD) == 0x00 && (c9f==0xff))return 2;
    if((c00==0xff) &&  (c05 & 0xFD) == 0x00 && (c9f==0x00))return 3;
    if((c00==0xff) &&  (c05 & 0xFD) == 0x00 && (c9f==0x12))return 3;    //    NEW TYPE 3
    if((c00==0xff) &&  (c05 & 0xFD) == 0x00 && (c9f==0x13))return 3;    //    NEW TYPE 3+ 4Mbit
    if((c00==0xff) &&  (c05 & 0xFD) == 0x00 && (c9f==0x14))return 3;    //    NEW TYPE 3++ 8Mbit

    return 0;
}

u32 saveChipSizeT3(void)
{
    u8 c9f;
    c9f=saveChipCMD(0x9f,0);

    if(c9f==0x12) {
        return 256*1024;        //    TYPE 3  2Mbit(256KByte)
    }
    if(c9f==0x13) {
        return 512*1024;    //     NEW TYPE 3+ 4Mbit(512KByte)
    }
    if(c9f==0x14) {
        return 1024*1204;   //   NEW TYPE 3++ 8Mbit(512KByte)
    }
    return 0;
}

int saveChipSizeT2(void)
{
    int tp=2;
#define OFOT0 (8*1024-1)    //    8KBの末端
#define OFOT1 (2*8*1024-1)    //    16KBの末端
    u8 buf1[1];    //    +0k data    read -> write
    u8 buf2[1];    //    +8k data    read -> read
    u8 buf3[1];    //    +0k ~data       write
    u8 buf4[1];    //    +8k data new    comp buf2
    saveChipRead(OFOT0,buf1,1,tp);
    saveChipRead(OFOT1,buf2,1,tp);
    buf3[0]=~buf1[0];
    saveChipWrite(OFOT0,buf3,1,tp);
    saveChipRead(OFOT1,buf4,1,tp);
    saveChipWrite(OFOT0,buf1,1,tp);
    if(buf4[0]!=buf2[0])    //    +8kも書き換わっている
    {
        return 8*1024;    //     8KB(64kbit)
    }
    return 64*1024;        //    64KB(512kbit)
}

u32 saveChipSize(int tp)
{
    if(-1 <= tp && tp <= 3)
    {
        //            -1    0    1    2         3
        //        int eepsz[5]={ 0,8192, 512,    8192,        256*1024};
        int eepsz[5]={ 0,8192, 512,    64*1024,    256*1024};
        if(tp==2)
        {
            return saveChipSizeT2();
        }
        if(tp==3)
        {
            return saveChipSizeT3();
        }
        tp++;
        return eepsz[tp];
    }
    return 0;
}


//////////////////////////////////////////////////////////////////////
void saveChipRead( u32 address, u8 *data, u32 length, u8 chipType )
{
    CARD_CR1 = /*E*/0x8000 | /*SEL*/0x2000 | /*MODE*/0x40;
    //    CARD_CR1 = 0xFFFF;
    CARD_EEPDATA = 0x03 | ((chipType == 1) ? address>>8<<3 : 0);
    EepromWaitBusy();

    if (chipType == 3)
    {
        CARD_EEPDATA = (address >> 16) & 0xFF;
        EepromWaitBusy();
    }
    if (chipType >= 2)
    {
        CARD_EEPDATA = (address >> 8) & 0xFF;
        EepromWaitBusy();
    }
    CARD_EEPDATA = (address) & 0xFF;
    EepromWaitBusy();

    while (length > 0)
    {
        CARD_EEPDATA = 0;
        EepromWaitBusy();
        *data++ = CARD_EEPDATA;
        length--;
    }
    EepromWaitBusy();
    CARD_CR1 = /*MODE*/0x40;
}
//////////////////////////////////////////////////////////////////////


void saveChipWrite( u32 address, u8 *apData, u32 aLength, u8 chipType )
{
    switch(chipType)
    {
    case 1:
        saveChipWriteT1(address,apData,aLength,chipType);
        return;
    case 2:
        saveChipWriteT2(address,apData,aLength,chipType);
        return;
    case 3:
        saveChipWriteT3(address,apData,aLength,chipType);
        return;
    }
}


#define T1BLOCKSZ (16)
//  TYPE1 4kbit EEPROM
void saveChipWriteT1(u32 address, u8 *apData, u32 aLength, u8 chipType)
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

        // program maximum of 16 bytes
        CARD_CR1 = /*E*/0x8000 | /*SEL*/0x2000 | /*MODE*/0x40;
        //    WRITE COMMAND 0x02 + A8 << 3
        CARD_EEPDATA = 0x02 | (address & BIT(8)) >> (8-3) ;
        EepromWaitBusy();
        CARD_EEPDATA = address & 0xFF;
        EepromWaitBusy();


        for (i = 0; address < last && i < T1BLOCKSZ; i++, address++)
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

//  TYPE2 64kbit EEPROM
void saveChipWriteT2(u32 address, u8 *apData, u32 aLength, u8 chipType)
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
        CARD_EEPDATA = address >> 8;
        EepromWaitBusy();
        CARD_EEPDATA = address & 0xFF;
        EepromWaitBusy();


        for (i = 0; address < last && i < 128; i++, address++)
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


#define T3BLOCKSZ (256)
//  TYPE3 2Mbit FLASH MEMORY
void saveChipWriteT3(u32 address, u8 *apData, u32 aLength, u8 chipType)
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


void saveChipEEPROMErase( int type )
{
    u32 save_size  = 0;
    if( 1 == type ) save_size = 512;
    if( 2 == type ) save_size = 65536;
    if( 0 == save_size )
        return ;
    u8 * buffer = new u8[save_size];
    memset( buffer, 0xff, save_size );
    saveChipWrite( 0, buffer, save_size, type );
    delete[] buffer;
}

//    Chip Erase NEW : clear FLASH MEMORY (TYPE 3 ONLY)
int saveChipFlashErase(bool erase4M)
{
    int sz;
    void saveChipSectorErase(u32 address);
    sz = saveChipSizeT3();

    int sectorCount = 4;
    if( /*sz == 512*1024 && */erase4M )
        sectorCount = 8;

    for( int i = 0; i < sectorCount; ++i ) {
        saveChipSectorErase(0x10000 * i);
        diskIcon().blink( 1.0f );
    }
    return 0;
}

//    COMMAND Sec.erase 0xD8
void saveChipSectorErase(u32 address)
{
    //    int i;
    // set WEL (Write Enable Latch)
    CARD_CR1 = /*E*/0x8000 | /*SEL*/0x2000 | /*MODE*/0x40;
    CARD_EEPDATA = 0x06;
    EepromWaitBusy();

    CARD_CR1 = /*MODE*/0x40;

    // SectorErase 0xD8
    CARD_CR1 = /*E*/0x8000 | /*SEL*/0x2000 | /*MODE*/0x40;
    CARD_EEPDATA = 0xD8;
    EepromWaitBusy();
    CARD_EEPDATA = (address >> 16) & 0xFF;
    EepromWaitBusy();
    CARD_EEPDATA = (address >> 8) & 0xFF;
    EepromWaitBusy();
    CARD_EEPDATA = address & 0xFF;
    EepromWaitBusy();

    CARD_CR1 = /*MODE*/0x40;

    // wait erase to finish
    CARD_CR1 = /*E*/0x8000 | /*SEL*/0x2000 | /*MODE*/0x40;
    CARD_EEPDATA = 0x05;
    EepromWaitBusy();

    do
    {
        CARD_EEPDATA = 0;
        EepromWaitBusy();
    } while (CARD_EEPDATA & 0x01);    // WIP (Write In Progress) ?
    //        EepromWaitBusy();
    CARD_CR1 = /*MODE*/0x40;
}


//    Chip Erase  : clear FLASH MEMORY (TYPE 3 ONLY)
//    COMMAND 0xC7
int saveChipFlashErase2(void)
{
    // set WEL (Write Enable Latch)
    CARD_CR1 = /*E*/0x8000 | /*SEL*/0x2000 | /*MODE*/0x40;
    CARD_EEPDATA = 0x06;
    EepromWaitBusy();

    CARD_CR1 = /*MODE*/0x40;

    // Chip Erase 0xC7
    CARD_CR1 = /*E*/0x8000 | /*SEL*/0x2000 | /*MODE*/0x40;
    CARD_EEPDATA = 0xC7;
    EepromWaitBusy();

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
    return 0;
}

//////////////////////////////////////////////////////////////////////
