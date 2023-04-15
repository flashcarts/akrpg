/*
 * Copyright (C) 2008 Bliss. All rights reserved.
 * This code following BSD LICENSE.
 *
 */

#include "ewintools.h"
#include "../../share/ipctool.h"
#include "dbgtool.h"

#define _EWIN_PSRAM 0x08000000 // SDRAM start point
#define _EWIN_SRAM  0x0A000000
uint8 cEwin::SramGetPage(void) {
	vuint8 r;
	r = *((vuint8 *)(_EWIN_SRAM + 0x5500 + 0x55)) ;
	r = *((vuint8 *)(_EWIN_SRAM + 0x2A00 + 0xAA)) ;
	r = *((vuint8 *)(_EWIN_SRAM + 0x9000 + 0x55)) ; // read open

	r = *((vuint8 *)(_EWIN_SRAM));
	return (uint8) r;
}

uint8 cEwin::SramSetPage(uint8 page) {
	vuint8 r;
	r = *((vuint8 *)(_EWIN_SRAM + 0x5500 + 0x55)) ;
	r = *((vuint8 *)(_EWIN_SRAM + 0x2A00 + 0xAA)) ;
	r = *((vuint8 *)(_EWIN_SRAM + 0xB000 + 0x55 )) ; // write open

	*((vuint8*)(_EWIN_SRAM)) =  (page & 0xff); // set page

	r = *((vuint8 *)(_EWIN_SRAM));
	return (uint8) r;
}

bool cEwin::Initialize(void)  {
	//vuint8 r5;
	uint8 r5;

	r5 = SramGetPage();

	dbg_printf("r5-test(1): %x\n",r5);
   if ( r5 != 0xb0 && r5 != 0xb8 ) return false;

	SramSetPage(0x8);

	r5 = SramGetPage();

	dbg_printf("r5-test(2): %x\n",r5);

	if ( r5 == 0xB8 ) {
		IPC_ARM9 = IPC_MSG_RESET_EWIN;
		while( IPC_MSG_NONE != IPC_ARM9 ) {
			swiDelay(1);
		}
		return true;
	}

	return false;
}

void cEwin::WritePSRAM(u32 address,const u8* buffer,u32 size)
{
  uint32 i;
  u16* addr = (u16*)(address+_EWIN_PSRAM);
  u16* pData = (u16*)buffer;
  for(i=0;i<size;i+=2)
  {
    addr[i>>1] = pData[i>>1];
  }
}

void cEwin::EnableBrowser(void)
{
  for(u32 i=0;i<0x100;i+=4)
  {
    *((u32 *)(0x8000000+i))=0xffffffff;
  }

  *((u32 *)0x80000b0)=0xffff;
  *((u32 *)0x80000b4)=0x24242400;
  *((u32 *)0x80000b8)=0xffffffff;
  *((u32 *)0x80000bc)=0x7fffffff;
  *((u32 *)0x801fffc)=0x7fffffff;
  *((u16 *)0x8240002)=0x1;
}

