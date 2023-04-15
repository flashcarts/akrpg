#ifndef __EXPTOOLS_H__
#define __EXPTOOLS_H__

#include <nds.h>

class cExpansion
{
  public:
    static void OpenNorWrite(void);
    static void CloseNorWrite(void);
    static void SetRompage(u16 page);
    static void SetRampage(u16 page);
    static u16 GetRampage();
    static void SetSerialMode(void);
    static void SetShake(u16 data);
    static void EnableBrowser(void);
    static void Block_Erase(u32 blockAdd);
    static void WriteNorFlash(u32 address,const u8* buffer,u32 size);
    static void WritePSRAM(u32 address,const u8* buffer,u32 size);
    static void WriteSram(uint32 address,const u8* data,uint32 size);
    static void ReadSram(uint32 address,u8* data,uint32 size);
	static void ChipReset();
	static uint32 ReadNorFlashID(void);
  public:
    static void Reset(void);
  private:
    enum
    {
      FlashBase=0x08000000
    };
};

#endif
