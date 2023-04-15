#include "sram.h"
#include "exptools.h"
#include "ewintools.h"
#include <malloc.h>
#include <string.h>
#include <stdio.h>
#include "progresswnd.h"
#include "language.h"
#include <fat.h>
#include <unistd.h>

#include "gbaoptions.h"
//#include "dbgtool.h"

bool cSram::SaveFileName(const char* romName,char* saveName)
{
  saveName[0]=0;
  if(strlen(romName)>251) return false;

  sGBAHeader header;
  FILE* gbaFile=fopen(romName,"rb");
  fread(&header,1,sizeof(header),gbaFile);
  fclose(gbaFile);
  GBAOPTION gbaOption;
  memset(&gbaOption,0,sizeof(gbaOption));
  memcpy(&gbaOption, header.title, sizeof(GBAOPTION) - 8);
  gbaOptionManager().getRomOption(gbaOption);

  if ( gbaOption.saveSlots )
	  sprintf(saveName, "%s.%c" , romName, '0' + gbaOption.saveSlots);
  else 
	  strcpy(saveName,romName);

  strcat(saveName,".sav");
  return true;
}

void cSram::BlankSram(u16 aStartPage)
{
  u8* buf=(u8*)malloc(SRAM_PAGE_SIZE);
  int pageEwin = -1;

  if(buf)
  {
    if ( gs().enable3in1Exp ) cExpansion::OpenNorWrite();
    memset(buf,0,SRAM_PAGE_SIZE);
    for(u32 i=0;i<SRAM_SAVE_PAGES;i++)
    {
		if ( gs().enable3in1Exp) {
		  cExpansion::SetRampage(i+aStartPage);
		  cExpansion::WriteSram(0x0A000000,buf,SRAM_PAGE_SIZE);
		}
		else {
			if ( !(i % (SRAM_SAVE_PAGES/EWIN_SRAM_SAVE_PAGES) ) ) {
				pageEwin+=1;
				cEwin::SramSetPage( pageEwin + aStartPage );
			}
			//dbg_printf("SRAM:page(%d),address(0x%x)\n",pageEwin, 0x0A000000 + i*SRAM_PAGE_SIZE - pageEwin* EWIN_SRAM_PAGE_SIZE);
			cExpansion::WriteSram(0x0A000000 + i*SRAM_PAGE_SIZE - pageEwin* EWIN_SRAM_PAGE_SIZE ,buf,SRAM_PAGE_SIZE);
		}
    }
    free(buf);
  }
  if ( gs().enable3in1Exp) cExpansion::CloseNorWrite();
}

void cSram::LoadSramFromFile(const char* romName,u16 aStartPage)
{
  char saveName[256];
  if(!SaveFileName(romName,saveName)) return;
  int pageEwin = -1;

  FILE* saveFile=fopen(saveName,"rb");
  if(saveFile)
  {
    sSaveInfo saveInfo;
    fread(&saveInfo,sizeof(saveInfo),1,saveFile);
	if ( saveInfo.marker == 0x42474b41 ) {
		NormalizeSize(saveInfo);
	} else { // raw type save 
		struct stat st;
		stat(saveName, &st);
		size_t filesize = st.st_size;
		if ( filesize < 0x10000 ) {
			fclose(saveFile);
			BlankSram(aStartPage);
			unlink(saveName);
			return;
		}
		memset(&saveInfo, 0, sizeof(saveInfo));
		if ( filesize >= 0x10000 ) saveInfo.size = 0x10000;
		if ( filesize >= 0x20000 ) saveInfo.size = 0x20000;

		fseek(saveFile,0, SEEK_SET);
	}
    u8* buf=(u8*)malloc(saveInfo.size);
    if(buf)
    {
      BlankSram(aStartPage);
      progressWnd().setTipText(LANG("progress window","gba save load" ));
      progressWnd().show();
      progressWnd().setPercent(0);
      memset(buf,0,saveInfo.size);
      size_t ret=fread(buf,saveInfo.size,1,saveFile);
      progressWnd().setPercent(10);
      if(ret==1&&!ferror(saveFile))
      {
        int page=saveInfo.offset/SRAM_PAGE_SIZE,offset=saveInfo.offset%SRAM_PAGE_SIZE,size=saveInfo.size,bufOffset=0;
        while(page<SRAM_SAVE_PAGES&&size>0)
        {

		 int maxSize=SRAM_PAGE_SIZE-offset,bufSize=(size>maxSize)?maxSize:size;

			if ( gs().enable3in1Exp ) {
			  cExpansion::SetRampage(page+aStartPage);
			  cExpansion::WriteSram(0x0A000000,buf+bufOffset,bufSize);
			}
			else if ( gs().enableEwinExp ) {
				if ( !(page % (SRAM_SAVE_PAGES/EWIN_SRAM_SAVE_PAGES) ) ) {
					pageEwin+=1;
					cEwin::SramSetPage( pageEwin + aStartPage );
				}
				//dbg_printf("SRAM:page(%d),address(0x%x)\n",pageEwin, 0x0A000000 + page*SRAM_PAGE_SIZE - pageEwin* EWIN_SRAM_PAGE_SIZE);
				cExpansion::WriteSram(0x0A000000 + page*SRAM_PAGE_SIZE - pageEwin* EWIN_SRAM_PAGE_SIZE ,buf+bufOffset,bufSize);
			}
          bufOffset+=bufSize;
          size-=bufSize;
          offset=0;
          page++;
          progressWnd().setPercent(10+bufOffset*90/saveInfo.size);
        }
      }
      progressWnd().setPercent(100);
      progressWnd().hide();
      free(buf);
    }
    fclose(saveFile);

    if ( gs().enable3in1Exp ) cExpansion::SetRampage(0);
	else cEwin::SramSetPage(0x8);
  }
  else
  {
    BlankSram(aStartPage);
  }
}

void cSram::SaveSramToFile(const char* romName,u16 aStartPage)
{
  char saveName[256];
  if(!SaveFileName(romName,saveName)) return;
  sSaveInfo saveInfo;
  int pageEwin = -1;

  FILE* saveFile=fopen(saveName,"rb");
  if(saveFile)
  {
    fread(&saveInfo,sizeof(saveInfo),1,saveFile);
    fclose(saveFile);
    saveFile=NULL;
	if ( saveInfo.marker != 0x42474b41 ) { // raw type save 
		memset(&saveInfo,0,sizeof(saveInfo));
		saveInfo.marker = 0x42474b41;
		struct stat st;
		stat(saveName, &st);
		saveInfo.size = 0x10000;
		if ( st.st_size >= 0x20000 ) saveInfo.size = 0x20000;
	}
  } 

  NormalizeSize(saveInfo);
  saveFile=fopen(saveName,"wb");
  if(saveFile)
  {
    fwrite(&saveInfo,sizeof(saveInfo),1,saveFile);
    u8* buf=(u8*)malloc(saveInfo.size);
    if(buf)
    {
      if(gs().enable3in1Exp) cExpansion::OpenNorWrite();
      progressWnd().setTipText(LANG("progress window","gba save store" ));
      progressWnd().show();
      progressWnd().setPercent(0);
      memset(buf,0,saveInfo.size);

      int page=saveInfo.offset/SRAM_PAGE_SIZE,offset=saveInfo.offset%SRAM_PAGE_SIZE,size=saveInfo.size,bufOffset=0;
      while(page<SRAM_SAVE_PAGES&&size>0)
      {
		int maxSize=SRAM_PAGE_SIZE-offset,bufSize=(size>maxSize)?maxSize:size;

		if ( gs().enable3in1Exp) {
			cExpansion::SetRampage(page+aStartPage);
			cExpansion::ReadSram(0x0A000000,buf+bufOffset,bufSize);
		}
		else if ( gs().enableEwinExp ) {
			if ( !(page % (SRAM_SAVE_PAGES/EWIN_SRAM_SAVE_PAGES) ) ) {
				pageEwin+=1;
				cEwin::SramSetPage( pageEwin + aStartPage );
			}
			//dbg_printf("SRAM:page(%d),address(0x%x)\n",pageEwin, 0x0A000000 + page*SRAM_PAGE_SIZE - pageEwin* EWIN_SRAM_PAGE_SIZE);
			cExpansion::ReadSram(0x0A000000 + page*SRAM_PAGE_SIZE - pageEwin* EWIN_SRAM_PAGE_SIZE ,buf+bufOffset,bufSize);
		}
        bufOffset+=bufSize;
        size-=bufSize;
        offset=0;
        page++;
        progressWnd().setPercent(bufOffset*90/saveInfo.size);
      }
      fwrite(buf,saveInfo.size,1,saveFile);
      progressWnd().setPercent(100);
      progressWnd().hide();
      if ( gs().enable3in1Exp) cExpansion::CloseNorWrite();
      free(buf);
    }
    fclose(saveFile);
  }
}

void cSram::CreateDefaultFile(const char* romName,u32 size)
{
  char saveName[256];
  if(!SaveFileName(romName,saveName)) return;
  sSaveInfo saveInfo(size);
  NormalizeSize(saveInfo);
  FILE* saveFile=fopen(saveName,"rb");
  if(saveFile)
  {
    fclose(saveFile);
  }
  else
  {
    FILE* saveFile=fopen(saveName,"wb");
    if(saveFile)
    {
      fwrite(&saveInfo,sizeof(saveInfo),1,saveFile);
      fclose(saveFile);
    }
  }
}

void cSram::NormalizeSize(sSaveInfo& anInfo)
{
  if((anInfo.offset+anInfo.size)>SRAM_SAVE_SIZE)
  {
    if(anInfo.offset<=SRAM_SAVE_SIZE)
    {
      anInfo.size=SRAM_SAVE_SIZE-anInfo.offset;
    }
    else
    {
      anInfo.size=0;
    }
  }
}
