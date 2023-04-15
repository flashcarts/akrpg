/*
 * Copyright (C) 2008 Bliss. All rights reserved.
 * This code following BSD LICENSE.
 *
 */


#ifndef __EWIN_H__
#define __EWIN_H__

#include <nds.h>

class cEwin
{
  public:
    static void EnableBrowser(void);
    static void WritePSRAM(u32 address,const u8* buffer,u32 size);
  public:
    static uint8 SramGetPage(void);
    static uint8 SramSetPage(uint8 page);
	static bool Initialize(void);
};

#endif
