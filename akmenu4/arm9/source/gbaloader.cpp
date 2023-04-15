#include <nds.h>
#include <fat.h>
#include <stdio.h>
#include <sys/stat.h>
#include <malloc.h>
#include <string.h>
#include "exptools.h"
#include "../../share/ipctool.h"
#include "progresswnd.h"
#include "language.h"
#include "sram.h"
#include "gbapatcher.h"
#include "gbaloader.h"

#include "gbaoptions.h"
#include "ewintools.h"
#include "dbgtool.h"

#define LEN 0x100000
#define LEN_NOR 0x8000
#define MAX_PSRAM 0x1000000
#define MAX_NOR 0x2000000

// writers >

class CGbaWriterPSRAM: public CGbaWriter
{
  public:
    void Open(void);
    void Write(u32 address,const u8* buffer,u32 size);
    void Close(void);
};

void CGbaWriterPSRAM::Open(void)
{
}

void CGbaWriterPSRAM::Write(u32 address,const u8* buffer,u32 size)
{
  cExpansion::WritePSRAM(address,buffer,size);
}

void CGbaWriterPSRAM::Close(void)
{
}

class CGbaWriterNor: public CGbaWriter
{
  private:
    u8* iBuffer;
    u32 iCurPage;
  private:
    enum
    {
      EPageSize=0x40000,
      EGuardSize=256
    };
  private:
    void Commit(void);
    void Fill(void);
  public:
    void Open(void);
    void Write(u32 address,const u8* buffer,u32 size);
    void Close(void);
};

void CGbaWriterNor::Commit(void)
{
  cExpansion::Block_Erase(iCurPage);
  for(u32 ii=0;ii<8;ii++)
    cExpansion::WriteNorFlash(iCurPage+ii*0x8000,iBuffer+ii*0x8000,0x8000);
}

void CGbaWriterNor::Fill(void)
{
  memcpy(iBuffer,(void*)(0x08000000+iCurPage),EPageSize+EGuardSize);
}

void CGbaWriterNor::Open(void)
{
  iCurPage=0x666;
  iBuffer=(u8*)malloc(EPageSize+EGuardSize);
}

void CGbaWriterNor::Write(u32 address,const u8* buffer,u32 size)
{
  if(iBuffer)
  {
    if(iCurPage==0x666) //first time
    {
      iCurPage=address&~0x3ffff;
      Fill();
    }
    u32 newPage=address&~0x3ffff;
    if(newPage==iCurPage)
    {
      memcpy(iBuffer+(address-iCurPage),buffer,size);
    }
    else
    {
      Commit();
      u8 overlap[EGuardSize]; bool overlaped=false;
      memcpy(overlap,iBuffer+EPageSize,EGuardSize);
      if(memcmp(overlap,(void*)(0x08000000+iCurPage+EPageSize),EGuardSize))
      {
        if(newPage-iCurPage==EPageSize)
          overlaped=true;
        else
        {
          iCurPage+=EPageSize;
          Fill();
          memcpy(iBuffer,overlap,EGuardSize);
          Commit();
        }
      }
      iCurPage=newPage;
      Fill();
      if(overlaped) memcpy(iBuffer,overlap,EGuardSize);
      Write(address,buffer,size);
    }
  }
}

void CGbaWriterNor::Close(void)
{
  if(iBuffer)
  {
    if(iCurPage!=0x666)
    {
      Commit();
    }
    free(iBuffer);
  }
}

// writers <

CGbaLoader::CGbaLoader(const std::string& aFileName): iFileName(aFileName)
{
}

bool CGbaLoader::Load(void)
{
  bool load=false,nor=false;
  struct stat st;
  if(-1==stat(iFileName.c_str(),&st))
  {
    return false;
  }

  int fnlen = iFileName.length();
  if( !(fnlen > 4 && (strcasecmp(iFileName.c_str()+fnlen-4, ".gba") == 0 ))) return false;	// only loading gba file
  iSize=st.st_size;

  if( (iSize>MAX_PSRAM) || (gs().norMode3in1Exp == 1) )
  {
    nor=true;
  }
  if(iSize>MAX_NOR)
  {
    akui::messageBox( NULL, LANG("gba warn", "title"), LANG("gba warn", "text"), MB_OK );
    return false;
  }

  FILE* gbaFile=fopen(iFileName.c_str(),"rb"); 

  GBAOPTION gbaOption; 
  memset(&gbaOption,0,sizeof(gbaOption));
  if(gbaFile)
  {
    sGBAHeader header;
    fread(&header,1,sizeof(header),gbaFile);
    fclose(gbaFile);
    if(header.is96h==0x96)
    {
		memcpy(&gbaOption, header.title, sizeof(GBAOPTION) - 8);
		gbaOptionManager().getRomOption(gbaOption);
		if ( gbaOption.norMode ) {
			nor = ( (iSize>MAX_PSRAM) || (gbaOption.norMode == 2) ) ? true : false;
		}

		gs().gbaSleepHack = !gbaOption.patchSleep;
		gs().gbaSramPatch = !gbaOption.patchSram;

		/////
		if ( gs().enable3in1Exp ) {
			load=LoadInternal(nor);
		}
		else if ( gs().enableEwinExp ) {
			load=LoadInternalEwin();
		}
    }
	else {
		akui::messageBox( NULL, LANG("gba warn", "title"), LANG("gba warn", "text"), MB_OK );
	}
  }

  if(load && !gbaOption.gbaMode)
  {
	INPUT & inputs = updateInput();
	if ((inputs.keysHeld&(KEY_B))!=(KEY_B)) LaunchGBA();
  }
  return load;
}
void CGbaLoader::LaunchGBA(void) {
    LoadBorder();
    fatUnmount((PARTITION_INTERFACE)0);
    BootGBA();
    while(true) swiWaitForVBlank();
}
void CGbaLoader::LoadBorder(void)
{
  u16* frame=NULL;
  cBMP15 frameBMP=createBMP15FromFile(SFN_GBAFRAME);
  if(frameBMP.valid()&&frameBMP.width()==SCREEN_WIDTH&&frameBMP.height()==SCREEN_HEIGHT)
  {
    frame=(u16*)malloc(SCREEN_WIDTH*SCREEN_HEIGHT*2);
    memcpy(frame,frameBMP.buffer(),SCREEN_WIDTH*SCREEN_HEIGHT*2); //tricky thing
  }

  videoSetMode(MODE_5_2D | DISPLAY_BG3_ACTIVE);
  videoSetModeSub(MODE_5_2D | DISPLAY_BG3_ACTIVE);
  vramSetMainBanks(VRAM_A_MAIN_BG_0x06000000, VRAM_B_MAIN_BG_0x06020000, VRAM_C_SUB_BG_0x06200000, VRAM_D_LCD);

  // for the main screen
  BG3_CR = BG_BMP16_256x256 | BG_BMP_BASE(0) | BG_WRAP_OFF;
  BG3_XDX = 1 << 8; //scale x
  BG3_XDY = 0; //rotation x
  BG3_YDX = 0; //rotation y
  BG3_YDY = 1 << 8; //scale y
  BG3_CX = 0; //translation x
  BG3_CY = 0; //translation y

  if(frame)
  {
    dmaCopy(frame,(void*)BG_BMP_RAM(0),SCREEN_WIDTH*SCREEN_HEIGHT*2);
    dmaCopy(frame,(void*)BG_BMP_RAM(8),SCREEN_WIDTH*SCREEN_HEIGHT*2);
    free(frame);
  }
  else
  {
    memset((void*)BG_BMP_RAM(0),0,0x18000);
    memset((void*)BG_BMP_RAM(8),0,0x18000);
  }
}

/*
void CGbaLoader::LoadBorder(void)
{
  videoSetMode(MODE_5_2D | DISPLAY_BG3_ACTIVE);
  videoSetModeSub(MODE_5_2D | DISPLAY_BG3_ACTIVE);
  vramSetMainBanks(VRAM_A_MAIN_BG_0x06000000, VRAM_B_MAIN_BG_0x06020000, VRAM_C_SUB_BG_0x06200000, VRAM_D_LCD);

  // for the main screen
  BG3_CR = BG_BMP16_256x256 | BG_BMP_BASE(0) | BG_WRAP_OFF;
  BG3_XDX = 1 << 8; //scale x
  BG3_XDY = 0; //rotation x
  BG3_YDX = 0; //rotation y
  BG3_YDY = 1 << 8; //scale y
  BG3_CX = 0; //translation x
  BG3_CY = 0; //translation y

  memset((void*)BG_BMP_RAM(0),0,0x18000);
  memset((void*)BG_BMP_RAM(8),0,0x18000);
}
*/
void CGbaLoader::BootGBA(void)
{
  sysSetBusOwners(BUS_OWNER_ARM7,BUS_OWNER_ARM7);
  if(PersonalData->_user_data.gbaScreen)
    REG_POWERCNT=1;
  else
    REG_POWERCNT=POWER_SWAP_LCDS|1;
  IPC_ARM9=IPC_MSG_GBA;
}

bool CGbaLoader::LoadPSRAM(void)
{
  bool load=false;
  FILE* gbaFile=fopen(iFileName.c_str(),"rb");
  if(gbaFile)
  {
	size_t bufsize = LEN;
    u8* buf=(u8*)malloc(bufsize);
	if ( !buf ) {
		bufsize = bufsize /2;
		buf=(u8*)malloc(bufsize);
	}
    if(buf)
    {
      REG_EXMEMCNT&=~0x0880;
      cExpansion::SetRompage(381);
      cExpansion::OpenNorWrite();
      cExpansion::SetSerialMode();

      progressWnd().setTipText(LANG("progress window","gba load" ));
      progressWnd().show();
      progressWnd().setPercent(0);

      for(u32 address=0;address<iSize&&address<MAX_PSRAM;address+=bufsize)
      {
        memset(buf,0xff,bufsize);
        fread(buf,bufsize,1,gbaFile);
        cExpansion::WritePSRAM(address,buf,bufsize);
        progressWnd().setPercent(address*100/iSize);
      }

      progressWnd().setPercent(100);
      progressWnd().hide();

      CGbaWriterPSRAM writer;
      u32 saveSize=CGbaPatcher(iSize,&writer,(u32*)0x08060000).Patch();
      cSram::CreateDefaultFile(iFileName.c_str(),saveSize);

      cExpansion::CloseNorWrite();
      load=true;

      free(buf);
    }
    fclose(gbaFile);
  }
  return load;
}

bool CGbaLoader::LoadNor(void)
{
  bool load=false;
  FILE* gbaFile=fopen(iFileName.c_str(),"rb");
  if(gbaFile)
  {
    u8* buf=(u8*)malloc(LEN_NOR);
    if(buf)
    {
      //erase
      cExpansion::OpenNorWrite();
      cExpansion::SetSerialMode();
      progressWnd().setTipText(LANG("progress window","erase nor" ));
      progressWnd().show();
      progressWnd().setPercent(0);
      for(u32 address=0;address<iSize&&address<MAX_NOR;address+=0x40000)
      {
        cExpansion::Block_Erase(address);
        progressWnd().setPercent(address*100/iSize);
      }
      progressWnd().setPercent(100);
      progressWnd().hide();
      //write
      progressWnd().setTipText(LANG("progress window","gba load" ));
      progressWnd().show();
      progressWnd().setPercent(0);
      for(u32 address=0;address<iSize&&address<MAX_NOR;address+=LEN_NOR)
      {
        memset(buf,0xff,LEN_NOR);
        fread(buf,LEN_NOR,1,gbaFile);
        cExpansion::WriteNorFlash(address,buf,LEN_NOR);
        progressWnd().setPercent(address*100/iSize);
      }
      progressWnd().setPercent(100);
      progressWnd().hide();

      CGbaWriterNor writer;
      u32 saveSize=CGbaPatcher(iSize,&writer,(u32*)0x08000000).Patch();
      cSram::CreateDefaultFile(iFileName.c_str(),saveSize);
/*
  FILE *log;
  log=fopen("fat0:/test.bin","wb");
  fwrite((void*)0x08000000,iSize,1,log);
  fclose(log);
// */
      cExpansion::CloseNorWrite();
      load=true;
      free(buf);
    }
    fclose(gbaFile);
  }
  return load;
}

void CGbaLoader::InitNor(void)
{
  cExpansion::SetRompage(0);
  cExpansion::SetRampage(16);
}

void CGbaLoader::InitPSRAM(void)
{
  cExpansion::SetRompage(384);
  cExpansion::SetRampage(96);
}

bool CGbaLoader::StoreOldSave(std::string& aFileName)
{
  bool res=false;
  const u8 sign[]="ACEKARD R.P.G GBA SIGN*";
  u8 buffer[sizeof(sign)];
  cExpansion::SetRampage(0);
  cExpansion::ReadSram(0x0A000000,buffer,sizeof(buffer));
  if(memcmp(buffer,sign,sizeof(buffer))) //old save
  {
    if(aFileName!="")
    {
      cSram::SaveSramToFile(aFileName.c_str(),0);
      res=true;
    }
    cExpansion::SetRampage(0);
    cExpansion::WriteSram(0x0A000000,sign,sizeof(sign));
  }
  return res;
}

bool CGbaLoader::LoadInternal(bool nor)
{
  bool load=false;
  cExpansion::Reset();
  std::string oldFile,oldFileNOR;
  CIniFile f;
  oldFile=f.LoadIniFile(SFN_LAST_GBA_SAVEINFO)?f.GetString("Save Info","lastLoaded",""):"";
  oldFileNOR=f.LoadIniFile(SFN_LAST_GBA_SAVEINFO)?f.GetString("Save Info","lastLoadedNOR",""):"";
  if(StoreOldSave(oldFile))
  {
    oldFile="";
    f.SetString("Save Info","lastLoaded","");
    f.SaveIniFile(SFN_LAST_GBA_SAVEINFO);
  }
  u32 state=0;
  if(nor) state|=1;
  if(!nor && oldFile==iFileName) state|=2;
  if(nor && oldFileNOR==iFileName) state|=4;

  switch(state)
  {
    case 0:
      if(oldFile!="") cSram::SaveSramToFile(oldFile.c_str(),96);
      cSram::LoadSramFromFile(iFileName.c_str(),96);
      f.SetString("Save Info","lastLoaded",iFileName);
      f.SaveIniFile(SFN_LAST_GBA_SAVEINFO);
      load=LoadPSRAM();
      InitPSRAM();
      break;
    case 1:
      if(oldFileNOR!="") cSram::SaveSramToFile(oldFileNOR.c_str(),16);
      cSram::LoadSramFromFile(iFileName.c_str(),16);
      f.SetString("Save Info","lastLoadedNOR",iFileName);
      f.SaveIniFile(SFN_LAST_GBA_SAVEINFO);
      load=LoadNor();
      InitNor();
      break;
    case 2:
    case 3:
      load=LoadPSRAM();
      InitPSRAM();
      break;
    case 4:
    case 5:
    case 6:
    case 7:
      load=true;
	  cExpansion::ChipReset();
      InitNor();
      break;
    default:
      cExpansion::Reset();
      break;
  }
  return load;
}

#define MAX_EWIN 0x2000000

class CGbaWriterEWIN: public CGbaWriter
{
  public:
    void Open(void);
    void Write(u32 address,const u8* buffer,u32 size);
    void Close(void);
};

void CGbaWriterEWIN::Open(void)
{
}

void CGbaWriterEWIN::Write(u32 address,const u8* buffer,u32 size)
{
  cEwin::WritePSRAM(address,buffer,size);
}

void CGbaWriterEWIN::Close(void)
{
}


bool CGbaLoader::LoadPsramEwin(void)
{
  bool load=false;
  FILE* gbaFile=fopen(iFileName.c_str(),"rb");
  dbg_printf("LoadPsramEwin\n");
  if(gbaFile)
  {
	size_t bufsize = LEN;
    u8* buf=(u8*)malloc(bufsize);
	if ( !buf ){ 
		bufsize = bufsize / 2;
	   	buf = (u8*)malloc(bufsize);
	}
    if(buf)
    {
      //REG_EXMEMCNT&=~0x0880;
      //write
      progressWnd().setTipText(LANG("progress window","gba load" ));
      progressWnd().show();
      progressWnd().setPercent(0);
      for(u32 address=0;address<iSize&&address<MAX_EWIN;address+=bufsize)
      {
        memset(buf,0xff,bufsize);
        fread(buf,bufsize,1,gbaFile);
        cEwin::WritePSRAM(address,buf,bufsize);
        progressWnd().setPercent(address*100/iSize);
      }
      progressWnd().setPercent(100);
      progressWnd().hide();

      CGbaWriterEWIN writer;
      u32 saveSize=CGbaPatcher(iSize,&writer,(u32*)0x08000000).Patch();
      cSram::CreateDefaultFile(iFileName.c_str(),saveSize);
	  
      load=true;
      free(buf);
    }
    fclose(gbaFile);
  }
  return load;
}


bool CGbaLoader::LoadInternalEwin()
{
  bool load=false;
  std::string oldFile;
  CIniFile f;
  oldFile=f.LoadIniFile(SFN_LAST_GBA_SAVEINFO)?f.GetString("Save Info","lastLoadedEwin",""):"";

  dbg_printf("LoadInternalEwin\n");
  if ( oldFile != iFileName ) {
      if(oldFile!="") cSram::SaveSramToFile(oldFile.c_str(),0x8);
	  cEwin::SramGetPage();
      cSram::LoadSramFromFile(iFileName.c_str(),0x8);
      f.SetString("Save Info","lastLoadedEwin",iFileName);
      f.SaveIniFile(SFN_LAST_GBA_SAVEINFO);
	  load=LoadPsramEwin();
	  cEwin::SramSetPage(0x8);
//	  cEwin::SramGetPage();
  }
  else {
	  load=LoadPsramEwin();
	  cEwin::SramSetPage(0x8);
//	  cEwin::SramGetPage();
  }

  return load;
}
