#ifndef __SRAM_H__
#define __SRAM_H__

#include <nds.h>

#define SRAM_PAGES 128
#define SRAM_SAVE_PAGES 32
#define SRAM_PAGE_SIZE 0x1000
#define SRAM_SAVE_SIZE SRAM_SAVE_PAGES*SRAM_PAGE_SIZE

#define EWIN_SRAM_SAVE_PAGES 2 
#define EWIN_SRAM_PAGE_SIZE 0x10000

class cSram
{
  public:
    struct sSaveInfo
    {
      u32 marker;
      u32 offset;
      u32 size;
      u32 header;
      sSaveInfo(): marker(0x42474b41),offset(0),size(SRAM_SAVE_SIZE),header(0) {};
      sSaveInfo(u32 aSize): marker(0x42474b41),offset(0),size(aSize),header(0) {};
    };
  public:
    static void SaveSramToFile(const char* romName,u16 aStartPage);
    static void LoadSramFromFile(const char* romName,u16 aStartPage);
    static void CreateDefaultFile(const char* romName,u32 size);
  private:
    static bool SaveFileName(const char* romName,char* saveName);
    static void BlankSram(u16 aStartPage);
    static void NormalizeSize(sSaveInfo& anInfo);
};

#endif
