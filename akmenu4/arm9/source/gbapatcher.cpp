/*
 * gelu's gbapatcher
 *
 * @modified by bliss about flash1m_v102,103
 *
 */
#include "gbapatcher.h"
#include <stdio.h>
#include "globalsettings.h"
#include "gba_nes_patch_bin.h"

#define sizeofa(array) (sizeof(array)/sizeof(array[0]))

#define CHECK_SIZES(n,c,s) struct __check_##n##_sizeof {char c1[c-s];char c2[s-c];char e;} //tnx to yjh
#define CHECK_SIZEOF(n,s) CHECK_SIZES(n,sizeof(n),s)

CGbaWriter::~CGbaWriter()
{
}

CGbaPatcher::CGbaPatcher(u32 aSize,CGbaWriter* aWriter,u32* aData): 
	iCount(0),
	iCount2(0),iCount3(0),iCount4(0),
	iSize(aSize),
	iData(aData),
	iTrimSize(aSize),iResultSize(aSize),
	iSaveSize(128*1024),
	iWriter(aWriter)
{
/*
#ifdef __TEST
  iData=(u32*)globalBuffer;
#else
  iData=(u32*)0x08060000;
#endif
*/
}

#ifdef __TEST
void CGbaPatcher::DumpPatchInfo(void)
{
  for(u32 ii=0;ii<iCount;ii++)
  {
    switch(iPatchInfo[ii].iType)
    {
      case ESRAMLabel:
        printf("ESRAMLabel");
        break;
      case EEEPROMLabel:
        printf("EEEPROMLabel");
        break;
      case EEEPROMRead:
        printf("EEEPROMRead");
        break;
      case EEEPROMWrite:
        printf("EEEPROMWrite");
        break;
      case EEEPROMWrite124:
        printf("EEEPROMWrite124");
        break;
      case EEEPROMWrite126:
        printf("EEEPROMWrite126");
        break;
      case EFLASH1M10XLabel:
        printf("EFLASH1M10XLabel");
        break;
      case EFLASH1M102_1:
        printf("EFLASH1M102_1");
        break;
      case EFLASH1M102_2:
        printf("EFLASH1M102_2");
        break;
      case EFLASH1M102_3:
        printf("EFLASH1M102_3");
        break;
      case EFLASH1M102_4:
        printf("EFLASH1M102_4");
        break;
      case EFLASH1M102_5:
        printf("EFLASH1M102_5");
        break;
      case EFLASH1M103_1:
        printf("EFLASH1M103_1");
        break;
      case EFLASH1M103_2:
        printf("EFLASH1M103_2");
        break;
      case EFLASH1M103_3:
        printf("EFLASH1M103_3");
        break;
      case EFLASH1M103_4:
        printf("EFLASH1M103_4");
        break;
      case EFLASHLabel:
        printf("EFLASHLabel");
        break;
      case EFLASH_1:
        printf("EFLASH_1");
        break;
      case EFLASH_2:
        printf("EFLASH_2");
        break;
      case EFLASH_3:
        printf("EFLASH_3");
        break;
      case EFLASH_4:
        printf("EFLASH_4");
        break;
      case EFLASHV123_1:
        printf("EFLASHV123_1");
        break;
      case EFLASHV123_2:
        printf("EFLASHV123_2");
        break;
      case EFLASHV123_3:
        printf("EFLASHV123_3");
        break;
      case EFLASHV123_4:
        printf("EFLASHV123_4");
        break;
      case EFLASHV123_5:
        printf("EFLASHV123_5");
        break;
      case EFLASH512Label:
        printf("EFLASH512Label");
        break;
      case EFLASH512_1:
        printf("EFLASH512_1");
        break;
      case EFLASH512_2:
        printf("EFLASH512_2");
        break;
      default:
        printf("Unknown_%d",iPatchInfo[ii].iType);
        break;
    }
    printf(": 0x%x\n",iPatchInfo[ii].iOffset*4);
  }
}

#endif

inline int test_printf( const char* format, ... )
{
#ifdef __TEST
    va_list args;
    va_start( args, format );
    int ret = vprintf( format, args );
    va_end(args);
    return ret;
#else
    return 0;
#endif//__TEST
}


void CGbaPatcher::Add(u32 anOffset,TPatchType aType)
{
  if(iCount<EMax)
  {
    SPatchInfo info={anOffset,aType};
    iPatchInfo[iCount++]=info;
  }
}

void CGbaPatcher::Add2(u32 anOffset,u32 aValue)
{
  if(iCount2<(EMax*2))
  {
    SPatchInfo2 info={anOffset,aValue};
    iPatchInfo2[iCount2++]=info;
  }
}

void CGbaPatcher::Add3(u32 aLocation,u32 anAddress,u32 aRa,u32 aRb)
{
  if(iCount3<EMax)
  {
    SPatchInfo3 info={aLocation,anAddress,aRa,aRb};
    iPatchInfo3[iCount3++]=info;
  }
}

void CGbaPatcher::Add4(u32 anOffset,u16 aValue)
{
  if(iCount4<EMax)
  {
    SPatchInfo4 info={anOffset,aValue};
    iPatchInfo4[iCount4++]=info;
  }
}


u32 CGbaPatcher::Patch(void)
{
  switch(setjmp(iJumpBuf))
  {
    case 0:
      PatchInternal();
      break;
    case 1:
      break;
    default:
      break;
  }
  return iSaveSize;
}

void CGbaPatcher::Error(const char* anError)
{
#ifdef __TEST
  printf("error: %s\n",anError);
#else
#endif
  longjmp(iJumpBuf,1);
}

void CGbaPatcher::PatchInternal(void)
{
  u32 search_size=iSize/4;
  //char* id=(char*)0x080600a0;
  for(u32 ii=0;ii<search_size;ii++)
  {
    switch(Data()[ii])
    {
      case 0x4d415253: //SRAM_V|SRAM_F_V
        if(Data()[ii+1]==0x565f465f||(Data()[ii+1]&0xffff)==0x565f)
        {
          Add(ii,ESRAMLabel);
          ii++;
        }
        break;
      case 0x52504545: //EEPROM_V
        if(Data()[ii+1]==0x565f4d4f)
        {
          Add(ii,EEEPROMLabel);
          ii++;
        }
      case 0xb0a2b570: //eeprom 121/122/124 read
        if(Data()[ii+1]==0x04001c0d&&Data()[ii+2]==0x48030c03&&Data()[ii+3]==0x88806800&&Data()[ii+4]==0xd3054283)
        {
          Add(ii,EEEPROMRead);
          ii+=4;
        }
        break;
      case 0xb0a9b530: //eeprom 121/122 write
        if(Data()[ii+1]==0x04001c0d&&Data()[ii+2]==0x48030c04&&Data()[ii+3]==0x88806800&&Data()[ii+4]==0xd3054284)
        {
          Add(ii,EEEPROMWrite);
          ii+=4;
        }
        break;
      case 0xb0acb5f0: //eeprom 124 write
        if(Data()[ii+1]==0x04001c0d&&Data()[ii+2]==0x06120c01&&Data()[ii+3]==0x48030e17&&Data()[ii+4]==0x88806800&&Data()[ii+5]==0xd3054281)
        {
          Add(ii,EEEPROMWrite124);
          ii+=5;
        }
        break;
      case 0x4647b5f0: //eeprom 126 write
        if(Data()[ii+1]==0xb0acb480&&Data()[ii+2]==0x04001c0e&&Data()[ii+3]==0x06120c05&&Data()[ii+4]==0x46900e12&&Data()[ii+5]==0x68004803&&Data()[ii+6]==0x42858880&&Data()[ii+7]==0x4802d306)
        {
          Add(ii,EEEPROMWrite126);
          ii+=7;
        }
        break;
      case 0x53414c46:
        //FLASH1M_V10X
        if(Data()[ii+1]==0x5f4d3148&&(Data()[ii+2]&0xfeffffff)==0x32303156)
        {
          Add(ii,EFLASH1M10XLabel);
          ii+=2;
        }
        //FLASH_V1
        else if(Data()[ii+1]==0x31565f48)
        {
          Add(ii,EFLASHLabel);
          ii++;
        }
        //FLASH512_V13
        else if(Data()[ii+1]==0x32313548&&Data()[ii+2]==0x3331565f)
        {
          Add(ii,EFLASH512Label);
          ii+=2;
        }
        break;
      case 0x0e000600: //FLASH1M_V102 code1
        if(Data()[ii+1]==0x21aa4b05&&Data()[ii+2]==0x4a057019)
        {
          Add(ii,EFLASH1M102_1);
          ii+=2;
        }
        break;
      case 0xfd88f7ff: //FLASH1M_V102 code2
        if(Data()[ii+1]==0x0c030400&&Data()[ii+2]==0x24014a03&&Data()[ii+3]==0x0000e007)
        {
          Add(ii,EFLASH1M102_2);
          ii+=2;
        }
        break;
      case 0xb090b5f0:
        //FLASH1M_V102 code3
        if(Data()[ii+1]==0x0c060400&&Data()[ii+2]==0x68004803&&Data()[ii+3]==0x42868940&&Data()[ii+4]==0x4802d306&&Data()[ii+5]==0x0000e052)
        {
          Add(ii,EFLASH1M102_3);
          ii+=5;
        }
        //FLASH1M_V103 code3
        else if(Data()[ii+1]==0x0c060400&&Data()[ii+2]==0x68004803&&Data()[ii+3]==0x42868940&&Data()[ii+4]==0x4802d306&&Data()[ii+5]==0x0000e054)
        {
          Add(ii,EFLASH1M103_3);
          ii+=5;
        }
        //FLASH1M_V103 code4
        else if(Data()[ii+1]==0x04011c0e&&Data()[ii+2]==0x06120c0c&&Data()[ii+3]==0x4d180e17&&Data()[ii+4]==0x68406828&&Data()[ii+5]==0xd2374286)
        {
          Add(ii,EFLASH1M103_4);
          ii+=5;
        }
        //FLASH1M_V102 code5
        else if(Data()[ii+1]==0x04001c0f&&Data()[ii+2]==0x48030c04&&Data()[ii+3]==0x89406800&&Data()[ii+4]==0xd3054284&&Data()[ii+5]==0xe0404801)
        {
          Add(ii,EFLASH1M102_5);
          ii+=5;
        }
        //FLASH_V123 code5
        else if(Data()[ii+1]==0x04001c0f&&Data()[ii+2]==0x2c0f0c04&&Data()[ii+3]==0x4801d904)
        {
          Add(ii,EFLASHV123_5);
          ii+=3;
        }
        //FLASH512_V13 code5
        else if(Data()[ii+1]==0x04001c0f&&Data()[ii+2]==0x48030c04&&Data()[ii+3]==0x89406800&&Data()[ii+4]==0xd3054284&&Data()[ii+5]==0xe0414801)
        {
          Add(ii,EFLASH512_2);
          ii+=5;
        }
        break;
      case 0x4c0ab510: //FLASH1M_V102 code4
        if(Data()[ii+1]==0x702222aa&&Data()[ii+2]==0x22554b09&&Data()[ii+3]==0x22a0701a&&Data()[ii+4]==0x78027022&&Data()[ii+5]==0x4b07700a&&Data()[ii+6]==0x681b7802&&Data()[ii+7]==0xf0002001)
        {
          Add(ii,EFLASH1M102_4);
          ii+=7;
        }
        break;
      case 0xb093b590: //FLASH_V1 code1
        if(Data()[ii+1]==0x1d39466f&&Data()[ii+2]==0xf0001c08&&(Data()[ii+3]&0xffffff00)==0x1d38f900&&Data()[ii+4]==0x64791c41&&Data()[ii+5]==0x21aa4809&&Data()[ii+6]==0x48097001&&Data()[ii+7]==0x70012155)
        {
          Add(ii,EFLASH_1);
          ii+=7;
        }
        break;
      case 0xb092b580: //FLASH_V1 code2
        if(Data()[ii+1]==0x481f466f&&Data()[ii+2]==0x880a491e&&Data()[ii+3]==0x1c114b1e&&Data()[ii+4]==0x4b1e4019&&Data()[ii+5]==0x8a12681a&&Data()[ii+6]==0x1c0a4311&&Data()[ii+7]==0x481c8002)
        {
          Add(ii,EFLASH_2);
          ii+=7;
        }
        break;
      case 0xb094b580:
        //FLASH_V1 code3
        if(Data()[ii+1]==0x1c39466f&&Data()[ii+2]==0x1c388008&&Data()[ii+3]==0x290f8801&&Data()[ii+4]==0x4801d904&&Data()[ii+5]==0x0000e056&&Data()[ii+6]==0x000080ff&&Data()[ii+7]==0x49234823)
        {
          Add(ii,EFLASH_3);
          ii+=7;
        }
        //FLASH_V1 code4
        else if(Data()[ii+1]==0x6079466f&&Data()[ii+2]==0x80081c39&&Data()[ii+3]==0x88011c38&&Data()[ii+4]==0xd903290f&&Data()[ii+5]==0xe0734800&&Data()[ii+6]==0x000080ff&&Data()[ii+7]==0x88011c38)
        {
          Add(ii,EFLASH_4);
          ii+=7;
        }
        break;
      case 0xffaaf7ff: //FLASH_V123 code1
        if(Data()[ii+1]==0x0c030400&&Data()[ii+2]==0x24014a03&&Data()[ii+3]==0x0000e007)
        {
          Add(ii,EFLASHV123_1);
          ii+=3;
        }
        break;
      case 0xb0a0b5f0:
        //FLASH_V123 code2
        if(Data()[ii+1]==0x1c161c0d&&Data()[ii+2]==0x04001c1f&&Data()[ii+3]==0x4a080c04)
        {
          Add(ii,EFLASHV123_2);
          ii+=3;
        }
        //FLASH512_V13 code5
        else if(Data()[ii+1]==0x1c161c0d&&Data()[ii+2]==0x04031C1F&&Data()[ii+3]==0x4a0f0c1c)
        {
          Add(ii,EFLASH512_1);
          ii+=3;
        }
        break;
      case 0xb090b570: //FLASH_V123 code3
        if(Data()[ii+1]==0x88294d15&&Data()[ii+2]==0x40314e15&&Data()[ii+3]==0x68004815)
        {
          Add(ii,EFLASHV123_3);
          ii+=3;
        }
        break;
      case 0x4646b570: //FLASH_V123 code4
        if(Data()[ii+1]==0xb090b440&&Data()[ii+2]==0x0c030400&&Data()[ii+3]==0xd83b2b0f)
        {
          Add(ii,EFLASHV123_4);
          ii+=3;
        }
        break;
      case 0x701020aa: //FLASH1M_V103 code1
        if(Data()[ii+1]==0x20554905&&Data()[ii+2]==0x20907008&&Data()[ii+3]==0xa9107010)
        {
          Add(ii,EFLASH1M103_1);
          ii+=3;
        }
        break;
      case 0xf0010500: //FLASH1M_V103 code2a
        if(Data()[ii+1]==0x0600f8d3&&Data()[ii+2]==0x43040e00&&Data()[ii+3]==0x20aa4907&&Data()[ii+4]==0x4a077008&&Data()[ii+5]==0x70102055&&Data()[ii+6]==0x700820f0&&Data()[ii+7]==0xa9107008)
        {
          Add(ii,EFLASH1M103_2);
          ii+=7;
        }
        break;
      case 0xf0050500: //FLASH1M_V103 code2b
        if(Data()[ii+1]==0x0600F945&&Data()[ii+2]==0x43040e00&&Data()[ii+3]==0x20aa4907&&Data()[ii+4]==0x4a077008&&Data()[ii+5]==0x70102055&&Data()[ii+6]==0x700820f0&&Data()[ii+7]==0xa9107008)
        {
          Add(ii,EFLASH1M103_2);
          ii+=7;
        }
        break;
    }
  }
#ifdef __TEST
  DumpPatchInfo();
#endif
  iWriter->Open();
  if ( gs().gbaSramPatch ) {
	if(!PatchNes()&&!PatchSRAM()&&!PatchEEPROM()&&!PatchFLASH1M_V10X()) PatchFLASH();
	PatchDragonBallZ();
  }

  if(gs().gbaSleepHack)
  {
	  if(!PatchSleep()) 
	  {
		  CommonSleepSearch();
		  CommonSleepPatch();

	  }
  }
  iWriter->Close();
}

bool CGbaPatcher::PatchSRAM(void)
{
  test_printf("CGbaPatcher::PatchSRAM\n");
  bool res=false;
  for(u32 ii=0;ii<iCount;ii++)
  {
    if(iPatchInfo[ii].iType==ESRAMLabel)
    {
      res=true;
      iSaveSize=32*1024;
      break;
    }
  }
  return res;
}

bool CGbaPatcher::PatchEEPROM(void)
{
  test_printf("CGbaPatcher::PatchEEPROM\n");
  bool res=false;
  const u8 patch_eeprom_1[]=
  {
    0x00,0x04, // LSL     R0, R0, #0x10
    0x0a,0x1c, // ADD     R2, R1, #0
    0x40,0x0b, // LSR     R0, R0, #0xD
    0xe0,0x21,0x09,0x05, // MOVL    R1, 0xE000000
    0x41,0x18, // ADD     R1, R0, R1
    0x07,0x31, // ADD     R1, #7
    0x00,0x23, // MOV     R3, #0
    //l1:
    0x08,0x78, // LDRB    R0, [R1]
    0x10,0x70, // STRB    R0, [R2]
    0x01,0x33, // ADD     R3, #1
    0x01,0x32, // ADD     R2, #1
    0x01,0x39, // SUB     R1, #1
    0x07,0x2b, // CMP     R3, #7
    0xf8,0xd9, // BLS     l1
    0x00,0x20, // MOV     R0, #0
    0x70,0x47  // BX      LR
  };
  const u8 patch_eeprom_2[]=
  {
    0x00,0x04, // LSL     R0, R0, #0x10
    0x0a,0x1c, // ADD     R2, R1, #0
    0x40,0x0b, // LSR     R0, R0, #0xD
    0xe0,0x21,0x09,0x05, // MOVL    R1, 0xE000000
    0x41,0x18, // ADD     R1, R0, R1
    0x07,0x31, // ADD     R1, #7
    0x00,0x23, // MOV     R3, #0
    //l1:
    0x10,0x78, // LDRB    R0, [R2]
    0x08,0x70, // STRB    R0, [R1]
    0x01,0x33, // ADD     R3, #1
    0x01,0x32, // ADD     R2, #1
    0x01,0x39, // SUB     R1, #1
    0x07,0x2b, // CMP     R3, #7
    0xf8,0xd9, // BLS     l1
    0x00,0x20, // MOV     R0, #0
    0x70,0x47  // BX      LR
  };
  u32 eeprom_ver=0,eeprom_count=0;
  for(u32 ii=0;ii<iCount;ii++)
  {
    if(iPatchInfo[ii].iType==EEEPROMLabel)
    {
      u32 ver=Data()[iPatchInfo[ii].iOffset+2]&0xffffff;
      ver=100*(ver&0xff-'0')+10*(((ver&0xff00)>>8)-'0')+(((ver&0xff0000)>>16)-'0');
      test_printf("eeprom ver: %d\n",ver);
      if(eeprom_count)
      {
        if(ver!=eeprom_ver) Error("EEPROM: two different versions");
      }
      else
      {
        eeprom_ver=ver;
      }
      eeprom_count++;
    }
  }
  if(eeprom_ver==111)
  {
    res=true;
    iSaveSize=512;
    PatchEEPROM111();
  }
  else if(eeprom_count)
  {
    res=true;
    iSaveSize=8*1024;
    for(u32 ii=0;ii<iCount;ii++)
    {
      switch(iPatchInfo[ii].iType)
      {
        case EEEPROMRead:
          if(eeprom_ver==120||eeprom_ver==121||eeprom_ver==122||eeprom_ver==124||eeprom_ver==126)
            iWriter->Write(iPatchInfo[ii].iOffset*4,patch_eeprom_1,sizeof(patch_eeprom_1));
          else
            Error("EEEPROMRead: invalid version");
          break;
        case EEEPROMWrite:
          if(eeprom_ver==120||eeprom_ver==121||eeprom_ver==122)
            iWriter->Write(iPatchInfo[ii].iOffset*4,patch_eeprom_2,sizeof(patch_eeprom_2));
          else
            Error("EEEPROMWrite: invalid version");
          break;
        case EEEPROMWrite124:
          if(eeprom_ver==124)
            iWriter->Write(iPatchInfo[ii].iOffset*4,patch_eeprom_2,sizeof(patch_eeprom_2));
          else
            Error("EEEPROMWrite124: invalid version");
          break;
        case EEEPROMWrite126:
          if(eeprom_ver==126)
            iWriter->Write(iPatchInfo[ii].iOffset*4,patch_eeprom_2,sizeof(patch_eeprom_2));
          else
            Error("EEEPROMWrite126: invalid version");
          break;
        default:
          break;
      }
    }
  }
  return res;
}

bool CGbaPatcher::PatchFLASH1M_V10X(void)
{
  test_printf("CGbaPatcher::PatchFLASH1M_V10X\n");
  bool res=false;
  const u8 patch_flash_1[]=
  {
    0x00,0xb5, // PUSH    {LR}
    0x00,0xf0,0x55,0xfb, // BL      Save3a
    0x02,0xbc, // POP     {R1}
    0x08,0x47  // BX      R1
  };
  const u8 patch_flash_2[]=
  {
    0x13,0x23,0x1b,0x02, // MOVL    R3, 0x1300
    0x62,0x20, // MOV     R0, #0x62
    0x03,0x43  // ORR     R3, R0
  };
  const u8 patch_flash_3_3in1[]=
  {
    0x00,0xb5, // PUSH    {LR}
    0x00,0x20, // MOV     R0, #0
    0x02,0xbc, // POP     {R1}
    0x08,0x47, // BX      R1
    //Save3a:
    0x70,0xb4, // PUSH    {R4-R6}
    0x0a,0x49, // LDR     R1, =0x9FE0000
    0xd2,0x22,0x12,0x02, // MOVL    R2, 0xD200
    0x0a,0x80, // STRH    R2, [R1]
    0x09,0x49, // LDR     R1, =0x8000000
    0x15,0x23,0x1b,0x02, // MOVL    R3, 0x1500
    0x0b,0x80, // STRH    R3, [R1]
    0x08,0x49, // LDR     R1, =0x8020000
    0x0a,0x80, // STRH    R2, [R1]
    0x08,0x49, // LDR     R1, =0x8040000
    0x0b,0x80, // STRH    R3, [R1]
    0x08,0x49, // LDR     R1, =0x9C00000
    0x00,0x01, // LSL     R0, R0, #4
    0x10,0x30, // ADD     R0, #0x10 --- 0x60 for page96
    0x08,0x80, // STRH    R0, [R1]
    0x07,0x49, // LDR     R1, =0x9FC0000
    0x0b,0x80, // STRH    R3, [R1]
    0x70,0xbc, // POP     {R4-R6}
    0x70,0x47, // BX      LR
    0x00, // DCB    0
    0x00, // DCB    0
    0x00,0x00,0xfe,0x09, // DCD 0x9FE0000
    0x00,0x00,0x00,0x08, // DCD 0x8000000
    0x00,0x00,0x02,0x08, // DCD 0x8020000
    0x00,0x00,0x04,0x08, // DCD 0x8040000
    0x00,0x00,0xc0,0x09, // DCD 0x9C00000
    0x00,0x00,0xfc,0x09  // DCD 0x9FC0000
  };
  const u8 patch_flash_4[]=
  {
    0x00,0x00 // LSL     R0, R0, #0
  };
  const u32 patch_flash_4_offsets[]=
  {
    0x06,0x0c,0x10
  };

  const u8 patch_flash_5_3in1[]=
  {
    0x7c,0xb5, // PUSH    {R2-R6,LR}
    0x00,0x06, // LSL     R0, R0, #0x18
    0x04,0x0f, // LSR     R4, R0, #0x1C
    0x00,0x01, // LSL     R0, R0, #4
    0x00,0x0c, // LSR     R0, R0, #0x10 
    0x0a,0x1c, // ADD     R2, R1, #0
    0xe0,0x21,0x09,0x05, // MOVL    R1, 0xE000000
    0x09,0x18, // ADD     R1, R1, R0
    0x01,0x23,0x1b,0x03, // MOVL    R3, 0x1000
    0x0e,0x48, // LDR     R0, =0x9FE0000
    0xd2,0x25,0x2d,0x02, // MOVL    R5, 0xD200
    0x05,0x80, // STRH    R5, [R0] - 30
    0x0d,0x48, // LDR     R0, =0x8000000
    0x15,0x26,0x36,0x02, // MOVL    R6, 0x1500
    0x06,0x80, // STRH    R6, [R0]
    0x0c,0x48, // LDR     R0, =0x8020000
    0x05,0x80, // STRH    R5, [R0]
    0x0c,0x48, // LDR     R0, =0x8040000
    0x06,0x80, // STRH    R6, [R0]
    0x0c,0x48, // LDR     R0, =0x9C00000
    0x24,0x01, // LSL     R4, R4, #4 
    0x10,0x34, // ADD     R4, #0x10 -- 0x60 for page96
    0x04,0x80, // STRH    R4, [R0]
    0x0b,0x48, // LDR     R0, =0x9FC0000
    0x06,0x80, // STRH    R6, [R0]
    //l1:
    0x10,0x78, // LDRB    R0, [R2]
    0x08,0x70, // STRB    R0, [R1]
    0x01,0x3b, // SUB     R3, #1
    0x01,0x32, // ADD     R2, #1
    0x01,0x31, // ADD     R1, #1
    0x00,0x2b, // CMP     R3, #0
    0xf8,0xd1, // BNE     l1
    0x00,0x20, // MOV     R0, #0
    0x7c,0xbc, // POP     {R2-R6}
    0x02,0xbc, // POP     {R1}
    0x08,0x47, // BX      R1
    0x00,0x00,0xfe,0x09, // DCD 0x9FE0000
    0x00,0x00,0x00,0x08, // DCD 0x8000000
    0x00,0x00,0x02,0x08, // DCD 0x8020000
    0x00,0x00,0x04,0x08, // DCD 0x8040000
    0x00,0x00,0xc0,0x09, // DCD 0x9C00000
    0x00,0x00,0xfc,0x09  // DCD 0x9FC0000
  };
  const u8 patch_flash_6[]=
  {
    0x00,0xb5, // PUSH    {LR}
    0x00,0xf0,0x5b,0xfb, // BL      Save6a
    0x02,0xbc, // POP     {R1}
    0x08,0x47  // BX      R1
  };
  const u8 patch_flash_7a[]=
  {
    0x10,0x78 // LDRB    R0, [R2]
  };
  const u8 patch_flash_7b[]=
  {
    0x08,0x78 // LDRB    R0, [R1]
  };
  const u8 patch_flash_8[]=
  {
    0x1b,0x24,0x24,0x01, // MOVL    R4, 0x1B0
    0x24,0x01, // LSL     R4, R4, #4
    0x32,0x20, // MOV     R0, #0x32
    0x04,0x43, // ORR     R4, R0
    0x07,0x49, // LDR     R1, =0xE005555
    0xaa,0x20, // MOV     R0, #0xAA
    0x08,0x78, // LDRB    R0, [R1]
    0x07,0x4a, // LDR     R2, =0xE002AAA
    0x55,0x20, // MOV     R0, #0x55
    0x10,0x78, // LDRB    R0, [R2]
    0xf0,0x20, // MOV     R0, #0xF0
    0x08,0x78, // LDRB    R0, [R1]
    0x08,0x78  // LDRB    R0, [R1]

  };
  const u8 patch_flash_9[]=
  {
    0x00,0xb5, // PUSH    {LR}
    0x00,0x20, // MOV     R0, #0
    0x02,0xbc, // POP     {R1}
    0x08,0x47  // BX      R1
  };
  const u8 patch_flash_a_3in1[]=
  {
    0x78,0xb5, // PUSH    {R3-R6,LR}
    0x00,0x06, // LSL     R0, R0, #0x18
    0x03,0x0f, // LSR     R3, R0, #0x1C
    0x00,0x01, // LSL     R0, R0, #4
    0x00,0x0c, // LSR     R0, R0, #0x10 
    //0x00,0x04, // LSL     R0, R0, #0x10 
    0x40,0x18, // ADD     R0, R0, R1
    0xe0,0x21,0x09,0x05, // MOVL    R1, 0xE000000
    0x09,0x18, // ADD     R1, R1, R0
    0x0b,0x48, // LDR     R0, =0x9FE0000 
    0xd2,0x25,0x2d,0x02, // MOVL    R5, 0xD200
    0x05,0x80, // STRH    R5, [R0]
    0x0a,0x48, // LDR     R0, =0x8000000
    0x15,0x26,0x36,0x02, // MOVL    R6, 0x1500
    0x06,0x80, // STRH    R6, [R0]
    0x09,0x48, // LDR     R0, =0x8020000
    0x05,0x80, // STRH    R5, [R0]
    0x09,0x48, // LDR     R0, =0x8040000 
    0x06,0x80, // STRH    R6, [R0]
    0x09,0x48, // LDR     R0, =0x9C00000
    0x1b,0x01, // LSL     R3, R3, #4
    0x10,0x33, // ADD     R3, #0x10 -- 0x60 for page 96
    0x03,0x80, // STRH    R3, [R0]
    0x08,0x48, // LDR     R0, =0x9FC0000
    0x06,0x80, // STRH    R6, [R0]
    0x0a,0x70, // STRB    R2, [R1]
    0x00,0x20, // MOV     R0, #0
    0x78,0xbc, // POP     {R3-R6}
    0x02,0xbc, // POP     {R1}
    0x08,0x47, // BX      R1
    0x00,0x00,0xfe,0x09, // DCD 0x9FE0000
    0x00,0x00,0x00,0x08, // DCD 0x8000000
    0x00,0x00,0x02,0x08, // DCD 0x8020000
    0x00,0x00,0x04,0x08, // DCD 0x8040000
    0x00,0x00,0xc0,0x09, // DCD 0x9C00000
    0x00,0x00,0xfc,0x09  // DCD 0x9FC0000
  };

  // patch code for ewin expansion pack - bliss
  const u8 patch_flash_3_ewin[]=
  {
	0x00, 0xb5,		// push	{lr}
	0x00, 0x20,		// mov	r0, #0
	0x02, 0xbc,		// pop	{r1}
	0x08, 0x47,		// bx	r1

	0x70, 0xb4,		// push	{r4, r5, r6}
	0x06, 0x4b,		// ldr	r3, =0xe0005555
	0x1b, 0x78,		// ldrb	r3, [r3]
	0x06, 0x4b,		// ldr	r3, =0xe0002aaa
	0x1b, 0x78,		// ldrb	r3, [r3]
	0x06, 0x4b,		// ldr	r3, =0xe000b055
	0x1b, 0x78,		// ldrb	r3, [r3]
	0xe0, 0x23,		// mov	r3, #0xE0
	0x1b, 0x05,		// lsl	r3, r3, #0x14
	0x0c, 0x30,		// add	r0, #0xC
	0x18, 0x70,		// strb	r0, [r3]
	0x1a, 0x78,		// ldrb	r2, [r3]
	0x70, 0xbc,		// pop	{r4, r5, r6}
	0x70, 0x47,		// bx	lr

	0x55, 0x55, 0x00, 0x0e, // 0x0E005555
	0xaa, 0x2a, 0x00, 0x0e, // 0x0E002AAA
	0x55, 0xb0, 0x00, 0x0e	// 0x0E00B055

  };

  const u8 patch_flash_5_ewin[]= // save
  {
	0x7c,0xb5,		// push	{r2, r3, r4, r5, r6, lr}
	0x00,0x06,		// lsls	r0, r0, #24
	0x04,0x0f,		// lsrs	r4, r0, #28
	0x00,0x01,		// lsls	r0, r0, #4
	0x00,0x0c,		// lsrs	r0, r0, #16
	0x0a,0x1c,		// adds	r2, r1, #0
	0xe0,0x21,		// movs	r1, #224
	0x09,0x05,		// lsls	r1, r1, #20
	0x09,0x18,		// adds	r1, r1, r0
	0x01,0x23,		// movs	r3, #1
	0x1b,0x03,		// lsls	r3, r3, #12
	0x0b,0x4d,		// ldr	r5, =0x0E005555
	0x2d,0x78,		// ldrb	r5, [r5, #0]
	0x0b,0x4d,		// ldr	r5, =0x0E002AAA
	0x2d,0x78,		// ldrb	r5, [r5, #0]
	0x0b,0x4d,		// ldr	r5, =0x0E00B055
	0x2d,0x78,		// ldrb	r5, [r5]
	0xe0,0x25,		// movs	r5, #224
	0x2d,0x05,		// lsls	r5, r5, #20
	0x0c,0x34,		// adds	r4, #12
	0x2c,0x70,		// strb	r4, [r5, #0]
	0x2d,0x78,		// ldrb	r5, [r5, #0]
	// LOOP:
	0x10,0x78,		// ldrb	r0, [r2, #0]
	0x08,0x70,		// strb	r0, [r1, #0]
	0x01,0x3b,		// subs	r3, #1
	0x01,0x32,		// adds	r2, #1
	0x01,0x31,		// adds	r1, #1
	0x00,0x2b,		// cmp	r3, #0
	0xf8,0xd1,		// bne	LOOP
	0x00,0x20,		// movs	r0, #0
	0x7c,0xbc,		// pop	{r2, r3, r4, r5, r6}
	0x02,0xbc,		// pop	{r1}
	0x08,0x47,		// bx	r1

	0x00,0x00,
	0x55,0x55,0x00,0x0e,
	0xaa,0x2a,0x00,0x0e,
	0x55,0xb0,0x00,0x0e

  };
  const u8 patch_flash_a_ewin[]=
  {
	0x78,0xb5,		// push	{r3, r4, r5, r6, lr}
	0x00,0x06,		// lsls	r0, r0, #24
	0x03,0x0f,		// lsrs	r3, r0, #28
	0x00,0x01,		// lsls	r0, r0, #4
	0x00,0x0c,		// lsrs	r0, r0, #16
	0x40,0x18,		// adds	r0, r0, r1
	0xe0,0x21,		// movs	r1, #224
	0x09,0x05,		// lsls	r1, r1, #20
	0x09,0x18,		// adds	r1, r1, r0
	0x08,0x4d,		// ldr	r5, =0x0E005555
	0x2d,0x78,		// ldrb	r5, [r5, #0]
	0x08,0x4d,		// ldr	r5, =0x0E002AAA
	0x2d,0x78,		// ldrb	r5, [r5, #0]
	0x08,0x4d,		// ldr	r5, =0x0E00B055
	0x2d,0x78,		// ldrb	r5, [r5, #0]
	0xe0,0x25,		// movs	r5, #224
	0x2d,0x05,		// lsls	r5, r5, #20
	0x0c,0x33,		// adds	r3, #12
	0x2b,0x70,		// strb	r3, [r5, #0]
	0x2d,0x78,		// ldrb	r5, [r5, #0]
	0x0a,0x70,		// strb	r2, [r1, #0]
	0x00,0x20,		// movs	r0, #0
	0x78,0xbc,		// pop	{r3, r4, r5, r6}
	0x02,0xbc,		// pop	{r1}
	0x08,0x47,		// bx	r1

	0x00,0x00,		
	0x55,0x55,0x00,0x0e,
	0xaa,0x2a,0x00,0x0e,
	0x55,0xb0,0x00,0x0e
  };

  size_t s_fa = sizeof(patch_flash_a_3in1);
  size_t s_f5 = sizeof(patch_flash_5_3in1);
  size_t s_f3 = sizeof(patch_flash_3_3in1);

  u8 *patch_flash_a = (u8 *)patch_flash_a_3in1;
  u8 *patch_flash_5 = (u8 *)patch_flash_5_3in1;
  u8 *patch_flash_3 = (u8 *)patch_flash_3_3in1;


  if ( gs().enableEwinExp ) { // patch for ewin
	  patch_flash_a = (u8 *)patch_flash_a_ewin;
	  patch_flash_5 = (u8 *)patch_flash_5_ewin;
	  patch_flash_3 = (u8 *)patch_flash_3_ewin; 
	  s_fa = sizeof(patch_flash_a_ewin); 
	  s_f5 = sizeof(patch_flash_5_ewin); 
	  s_f3 = sizeof(patch_flash_3_ewin);
  }
  else if ( gs().enable3in1Exp && (u32)iData == 0x08060000 ) { 
	  // psram mode - sram page is 0x60 ~ 0x70
	  patch_flash_3[38] = 0x60;
	  patch_flash_5[50] = 0x60;
	  patch_flash_a[46] = 0x60;

  }
  //wait_press_b();

  u32 flash_ver=0,flash_count=0;
  for(u32 ii=0;ii<iCount;ii++)
  {
    if(iPatchInfo[ii].iType==EFLASH1M10XLabel)
    {
      u32 ver=Data()[iPatchInfo[ii].iOffset+2]&0xffffff00;
      ver=100*(((ver&0xff00)>>8)-'0')+10*(((ver&0xff0000)>>16)-'0')+(((ver&0xff000000)>>24)-'0');
      test_printf("flash ver: %d\n",ver);
      if(flash_count)
      {
        if(ver!=flash_ver) Error("FLASH1M: two different versions");
      }
      else
      {
        flash_ver=ver;
      }
      flash_count++;
      break;
    }
  }
  if(flash_count)
  {
    res=true;
    iSaveSize=128*1024;
    for(u32 ii=0;ii<iCount;ii++)
    {
      switch(iPatchInfo[ii].iType)
      {
        case EFLASH1M102_1:
          if(flash_ver==102)
            iWriter->Write(iPatchInfo[ii].iOffset*4,patch_flash_1,sizeof(patch_flash_1));
          else if(flash_ver==103)
            iWriter->Write(iPatchInfo[ii].iOffset*4,patch_flash_6,sizeof(patch_flash_6));
          else
            Error("EFLASH1M102_1: invalid version");
          break;
        case EFLASH1M102_2:
          if(flash_ver==102||flash_ver==103)
            iWriter->Write(iPatchInfo[ii].iOffset*4,patch_flash_2,sizeof(patch_flash_2));
          else
            Error("EFLASH1M102_2: invalid version");
          break;
        case EFLASH1M102_3:
          if(flash_ver==102) 
			iWriter->Write(iPatchInfo[ii].iOffset*4,patch_flash_3,s_f3);
          else
            Error("EFLASH1M102_3: invalid version");
          break;
        case EFLASH1M102_4:
          if(flash_ver==102)
          {
            for(u32 jj=0;jj<sizeofa(patch_flash_4_offsets);jj++)
              iWriter->Write(iPatchInfo[ii].iOffset*4+patch_flash_4_offsets[jj],patch_flash_4,sizeof(patch_flash_4));
          }
          else if(flash_ver==103) ; //do nothing
          else
            Error("EFLASH1M102_4: invalid version");
          break;
        case EFLASH1M102_5:
          if(flash_ver==102||flash_ver==103) 
			iWriter->Write(iPatchInfo[ii].iOffset*4,patch_flash_5,s_f5);
          else
            Error("EFLASH1M102_5: invalid version");
          break;
        case EFLASH1M103_1:
          if(flash_ver==102) ; //do nothing
          else if(flash_ver==103)
          {
            iWriter->Write(iPatchInfo[ii].iOffset*4+0x02,patch_flash_7a,sizeof(patch_flash_7a));
            iWriter->Write(iPatchInfo[ii].iOffset*4+0x08,patch_flash_7b,sizeof(patch_flash_7b));
            iWriter->Write(iPatchInfo[ii].iOffset*4+0x0c,patch_flash_7a,sizeof(patch_flash_7a));
          }
          else
            Error("EFLASH1M103_1: invalid version");
          break;
        case EFLASH1M103_2:
          if(flash_ver==103)
          {
            iWriter->Write(iPatchInfo[ii].iOffset*4+2,patch_flash_8,sizeof(patch_flash_8));
          }
          else
            Error("EFLASH1M103_2: invalid version");
          break;
        case EFLASHV123_3:
          if(flash_ver==102) ; //do nothing
          else if(flash_ver==103)
          {
            iWriter->Write(iPatchInfo[ii].iOffset*4,patch_flash_9,sizeof(patch_flash_9));
          }
          else
            Error("FLASH1M103, EFLASHV123_3: invalid version");
          break;
        case EFLASH1M103_3:
          if(flash_ver==103)
          {
			iWriter->Write(iPatchInfo[ii].iOffset*4,patch_flash_3,s_f3);
          }
          else
            Error("EFLASH1M103_3: invalid version");
          break;
        case EFLASH1M103_4:
          if(flash_ver==103)
          {
			iWriter->Write(iPatchInfo[ii].iOffset*4,patch_flash_a,s_fa);
          }
          else
            Error("EFLASH1M103_4: invalid version");
          break;
        default:
          break;
      }
    }
  }
  return res;
}

bool CGbaPatcher::PatchFLASH(void)
{
  test_printf("CGbaPatcher::PatchFLASH\n");
  bool res=false;
  const u8 patch_flash_1[]=
  {
    0x00,0xb5, // PUSH    {LR}
    0x1b,0x20,0x00,0x02, // MOVL    R0, 0x1B00
    0x32,0x21, // MOV     R1, #0x32
    0x08,0x43, // ORR     R0, R1
    0x02,0xbc, // POP     {R1}
    0x08,0x47  // BX      R1
  };
  const u8 patch_flash_2[]=
  {
    0x00,0xb5, // PUSH    {LR}
    0x00,0x20, // MOV     R0, #0
    0x02,0xbc, // POP     {R1}
    0x08,0x47  // BX      R1
  };
  const u8 patch_flash_3[]=
  {
    0x1b,0x23,0x1b,0x02, // MOVL    R3, 0x1B00
    0x32,0x20, // MOV     R0, #0x32
    0x03,0x43  // ORR     R3, R0

  };
  const u8 patch_flash_4[]=
  {
    0x7c,0xb5, // PUSH    {R2-R6,LR}
    0x00,0x07, // LSL     R0, R0, #0x1C
    0x00,0x0c, // LSR     R0, R0, #0x10
    0x0a,0x1c, // ADD     R2, R1, #0
    0xe0,0x21,0x09,0x05, // MOVL    R1, 0xE000000
    0x09,0x18, // ADD     R1, R1, R0
    0x01,0x23,0x1b,0x03, // MOVL    R3, 0x1000
    //l1:
    0x10,0x78, // LDRB    R0, [R2]
    0x08,0x70, // STRB    R0, [R1]
    0x01,0x3b, // SUB     R3, #1
    0x01,0x32, // ADD     R2, #1
    0x01,0x31, // ADD     R1, #1
    0x00,0x2b, // CMP     R3, #0
    0xf8,0xd1, // BNE     l1
    0x00,0x20, // MOV     R0, #0
    0x7c,0xbc, // POP     {R2-R6}
    0x02,0xbc, // POP     {R1}
    0x08,0x47  // BX      R1
  };
  const u8 patch_flash_5[]=
  {
    0x70,0xb5, // PUSH    {R4-R6,LR}
    0xa0,0xb0, // SUB     SP, SP, #0x80
    0x00,0x03, // LSL     R0, R0, #0xC
    0x40,0x18, // ADD     R0, R0, R1
    0xe0,0x21,0x09,0x05, // MOVL    R1, 0xE000000
    0x09,0x18, // ADD     R1, R1, R0
    //l1:
    0x08,0x78, // LDRB    R0, [R1]
    0x10,0x70, // STRB    R0, [R2]
    0x01,0x3b, // SUB     R3, #1
    0x01,0x32, // ADD     R2, #1
    0x01,0x31, // ADD     R1, #1
    0x00,0x2b, // CMP     R3, #0
    0xf8,0xd1, // BNE     l1
    0x00,0x20, // MOV     R0, #0
    0x20,0xb0, // ADD     SP, SP, #0x80
    0x70,0xbc, // POP     {R4-R6}
    0x02,0xbc, // POP     {R1}
    0x08,0x47  // BX      R1
  };
  const u8 patch_flash_6[]=
  {
    0x7c,0xb5, // PUSH    {R2-R6,LR}
    0x90,0xb0, // SUB     SP, SP, #0x40
    0x00,0x03, // LSL     R0, R0, #0xC
    0x0a,0x1c, // ADD     R2, R1, #0
    0xe0,0x21,0x09,0x05, // MOVL    R1, 0xE000000
    0x09,0x18, // ADD     R1, R1, R0
    0x01,0x23,0x1b,0x03, // MOVL    R3, 0x1000
    //l1
    0x10,0x78, // LDRB    R0, [R2]
    0x08,0x70, // STRB    R0, [R1]
    0x01,0x3b, // SUB     R3, #1
    0x01,0x32, // ADD     R2, #1
    0x01,0x31, // ADD     R1, #1
    0x00,0x2b, // CMP     R3, #0
    0xf8,0xd1, // BNE     l1
    0x00,0x20, // MOV     R0, #0
    0x10,0xb0, // ADD     SP, SP, #0x40
    0x7c,0xbc, // POP     {R2-R6}
    0x02,0xbc, // POP     {R1}
    0x08,0x47  // BX      R1
  };
  u32 flash_ver=0,flash_count=0;
  for(u32 ii=0;ii<iCount;ii++)
  {
    if(iPatchInfo[ii].iType==EFLASHLabel)
    {
      u32 ver=Data()[iPatchInfo[ii].iOffset+2]&0xffff;
      ver=100*(((Data()[iPatchInfo[ii].iOffset+1]&0xff000000)>>24)-'0')+10*((ver&0xff)-'0')+(((ver&0xff00)>>8)-'0');
      test_printf("flash ver: %d\n",ver);
      if(flash_count)
      {
        if(ver!=flash_ver) Error("FLASH: two different versions");
      }
      else
      {
        flash_ver=ver;
      }
      flash_count++;
    }
    else if(iPatchInfo[ii].iType==EFLASH512Label)
    {
      u32 ver=130+(Data()[iPatchInfo[ii].iOffset+3]&0xff-'0');
      test_printf("flash 512 ver: %d\n",ver);
      if(flash_count)
      {
        if(ver!=flash_ver) Error("FLASH512: two different versions");
      }
      else
      {
        flash_ver=ver;
      }
      flash_count++;
    }
  }
  if(flash_count)
  {
    res=true;
    iSaveSize=64*1024;
    for(u32 ii=0;ii<iCount;ii++)
    {
      switch(iPatchInfo[ii].iType)
      {
        case EFLASH1M102_2:
          if(flash_ver==130||flash_ver==131||flash_ver==133)
            iWriter->Write(iPatchInfo[ii].iOffset*4,patch_flash_3,sizeof(patch_flash_3));
          else
            Error("EFLASH512/EFLASH1M102_2: invalid version");
          break;
        case EFLASH_1:
          if(flash_ver==120||flash_ver==121)
            iWriter->Write(iPatchInfo[ii].iOffset*4,patch_flash_1,sizeof(patch_flash_1));
          else
            Error("EFLASH_1: invalid version");
          break;
        case EFLASH_2:
          if(flash_ver==120||flash_ver==121)
            iWriter->Write(iPatchInfo[ii].iOffset*4,patch_flash_2,sizeof(patch_flash_2));
          else
            Error("EFLASH_2: invalid version");
          break;
        case EFLASH_3:
          if(flash_ver==120||flash_ver==121)
            iWriter->Write(iPatchInfo[ii].iOffset*4,patch_flash_2,sizeof(patch_flash_2));
          else
            Error("EFLASH_3: invalid version");
          break;
        case EFLASH_4:
          if(flash_ver==120||flash_ver==121)
            iWriter->Write(iPatchInfo[ii].iOffset*4,patch_flash_4,sizeof(patch_flash_4));
          else
            Error("EFLASH_4: invalid version");
          break;
        case EFLASHV123_1:
          if(flash_ver==123||flash_ver==124||flash_ver==125||flash_ver==126)
            iWriter->Write(iPatchInfo[ii].iOffset*4,patch_flash_3,sizeof(patch_flash_3));
          else
            Error("EFLASHV123_1: invalid version");
          break;
        case EFLASHV123_2:
          if(flash_ver==123||flash_ver==124||flash_ver==125||flash_ver==126)
            iWriter->Write(iPatchInfo[ii].iOffset*4,patch_flash_5,sizeof(patch_flash_5));
          else
            Error("EFLASHV123_2: invalid version");
          break;
        case EFLASHV123_3:
          if(flash_ver==123||flash_ver==124||flash_ver==125||flash_ver==126||flash_ver==130||flash_ver==131||flash_ver==133)
            iWriter->Write(iPatchInfo[ii].iOffset*4,patch_flash_2,sizeof(patch_flash_2));
          else
            Error("EFLASHV123_3: invalid version");
          break;
        case EFLASHV123_4:
          if(flash_ver==123||flash_ver==124||flash_ver==125||flash_ver==126||flash_ver==130||flash_ver==131||flash_ver==133)
            iWriter->Write(iPatchInfo[ii].iOffset*4,patch_flash_2,sizeof(patch_flash_2));
          else
            Error("EFLASHV123_4: invalid version");
          break;
        case EFLASHV123_5:
          if(flash_ver==123||flash_ver==124||flash_ver==125||flash_ver==126)
            iWriter->Write(iPatchInfo[ii].iOffset*4,patch_flash_6,sizeof(patch_flash_6));
          else
            Error("EFLASHV123_5: invalid version");
          break;
        case EFLASH512_1:
          if(flash_ver==130||flash_ver==131||flash_ver==133)
            iWriter->Write(iPatchInfo[ii].iOffset*4,patch_flash_5,sizeof(patch_flash_5));
          else
            Error("EFLASH512_1: invalid version");
          break;
        case EFLASH512_2:
          if(flash_ver==130||flash_ver==131||flash_ver==133)
            iWriter->Write(iPatchInfo[ii].iOffset*4,patch_flash_6,sizeof(patch_flash_6));
          else
            Error("EFLASH512_2: invalid version");
          break;
        default:
          break;
      }
    }
  }
  return res;
}

bool CGbaPatcher::PatchSleep(void)
{
  test_printf("CGbaPatcher::PatchSleep\n");
  bool res=false;
  const u32 baldur_style=0;
  const u32 rayman3_style=8;
  const u32 zelda2_style=13;
  const u32 mario_style=30;
  //const u32 drilldozer_style=43;
  const u32 drmario_style=44;
  const u32 goldensun2_style=48;
  const u32 nes_style=53;
  const u32 goldensun_style=66;
  const u32 game_codes[]=
  {
    //0
    0x50444742, // Baldur's Gate - Dark Alliance (Europe) (En,Fr,De,Es,It).gba
    0x45444742, // Baldur's Gate - Dark Alliance (USA).gba
    0x45574342, // Catwoman (USA, Europe) (En,Fr,De,Es,It,Nl).gba
    0x50434142, // Action Man - Robot Atak (Europe) (En,Fr,De,Es,It).gba
    0x45384842, // Harry Potter and the Goblet of Fire (USA, Europe) (En,Fr,De,Es,It,Nl,Da).gba
    0x45385242, // Ghost Rider (USA, Europe) (En,Fr,De,Es,It,Nl).gba
    0x50373242, // Top Spin 2 (Europe) (En,Fr,De,Es,It).gba
    0x45373242, // Top Spin 2 (USA) (En,Fr,De,Es,It).gba
    //8
    0x505a5941, // Rayman 3 (Europe) (En,Fr,De,Es,It,Nl,Sv,No,Da,Fi).gba
    0x455a5941, // Rayman 3 (USA) (En,Fr,Es).gba
    0x50355842, // Rayman 10th Anniversary - Rayman Advance + Rayman 3 (Europe) (En,Fr,De,Es,It+En,Fr,De,Es,It,Nl,Sv,No,Da,Fi).gba
    0x45355842, // Rayman 10th Anniversary - Rayman Advance + Rayman 3 (USA) (En,Fr,De,Es,It+En,Fr,Es).gba
    0x505A5742, // 2 Games in 1 - Winnie the Pooh's Rumbly Tumbly Adventure + Rayman 3 (Europe) (En,Fr,De,Es,It,Nl+En,Fr,De,Es,It,Nl,Sv,No,Da,Fi).gba
    //13
    0x50584d42, // Metroid - Zero Mission (Europe) (En,Fr,De,Es,It).gba
    0x4a584d42, // Metroid - Zero Mission (Japan).gba
    0x45584d42, // Metroid - Zero Mission (USA).gba
    0x43584d42, // Miteluode - Lingdian Renwu (China).gba
    0x504d5a42, // Legend of Zelda, The - The Minish Cap (Europe) (En,Fr,De,Es,It).gba
    0x454d5a42, // Legend of Zelda, The - The Minish Cap (USA).gba
    0x45485a42, // Legend of Zelda, The - The Minish Cap (USA) (Kiosk Demo).gba
    0x4a4d5a42, // Zelda no Densetsu - Fushigi no Boushi (Japan).gba
    0x50544d41, // Metroid Fusion (Europe) (En,Fr,De,Es,It).gba
    0x4a544d41, // Metroid Fusion (Japan).gba
    0x45544d41, // Metroid Fusion (USA, Australia).gba
    0x43544d41, // Miteluode Ronghe (China).gba
    0x504c5a41, // Legend of Zelda, The - A Link to the Past & Four Swords (Europe) (En,Fr,De,Es,It).gba
    0x454c5a41, // Legend of Zelda, The - A Link to the Past & Four Swords (USA, Australia).gba
    0x4a4c5a41, // Zelda no Densetsu - Kamigami no Triforce & 4tsu no Tsurugi (Japan).gba
    0x4a324141, // Super Mario Advance 2 - Super Mario World + Mario Brothers (Japan).gba
    0x45383842, // Mario & Luigi - Superstar Saga (USA) (Kiosk Demo).gba
    //30
    0x50413341, // Super Mario Advance 3 - Yoshi's Island (Europe) (En,Fr,De,Es,It).gba
    0x45413341, // Super Mario Advance 3 - Yoshi's Island (USA).gba
    0x4a413341, // Super Mario Advance 3 - Yoshi's Island + Mario Brothers (Japan).gba
    0x43413341, // Yaoxi Dao (China).gba
    0x4a345841, // Super Mario Advance 4 - Super Mario 3 + Mario Brothers (Japan).gba
    0x4a345841, // Super Mario Advance 4 - Super Mario 3 + Mario Brothers (Japan) (Rev B).gba
    0x50345841, // Super Mario Advance 4 - Super Mario Bros. 3 (Europe) (En,Fr,De,Es,It).gba
    0x50345841, // Super Mario Advance 4 - Super Mario Bros. 3 (Europe) (En,Fr,De,Es,It) (Rev A).gba
    0x45345841, // Super Mario Advance 4 - Super Mario Bros. 3 (USA).gba
    0x45345841, // Super Mario Advance 4 - Super Mario Bros. 3 (USA, Australia) (Rev A).gba
    0x50383841, // Mario & Luigi - Superstar Saga (Europe) (En,Fr,De,Es,It).gba
    0x45383841, // Mario & Luigi - Superstar Saga (USA, Australia).gba
    0x4a383841, // Mario & Luigi RPG (Japan).gba
    //43
    0x45393456, // Drill Dozer (USA).gba
    //44
    0x50505a42, // 2 Games in 1 - Dr. Mario & Puzzle League (Europe) (En,Fr,De,Es,It).gba
    0x45505a42, // 2 Games in 1 - Dr. Mario & Puzzle League (USA, Australia).gba
    //46
    0x4a415642, // bit Generations - Coloris (Japan).gba
    //47
    0x4a425642, // bit Generations - Dialhex (Japan).gba
    //48
    0x45464741, // Golden Sun - The Lost Age (USA, Europe).gba
    0x44464741, // Golden Sun - Die vergessene Epoche (Germany).gba
    0x46464741, // Golden Sun - L'Age Perdu (France).gba
    0x49464741, // Golden Sun - L'Era Perduta (Italy).gba
    0x53464741, // Golden Sun - La Edad Perdida (Spain).gba
    //53
    0x454d4246, // Classic NES Series - Bomberman (USA, Europe).gba
    0x45444146, // Classic NES Series - Castlevania (USA, Europe).gba
    0x454b4446, // Classic NES Series - Donkey Kong (USA, Europe).gba
    0x454d4446, // Classic NES Series - Dr. Mario (USA, Europe).gba
    0x45424546, // Classic NES Series - Excitebike (USA, Europe).gba
    0x45434946, // Classic NES Series - Ice Climber (USA, Europe).gba
    0x454c5a46, // Classic NES Series - Legend of Zelda (USA, Europe).gba
    0x45524d46, // Classic NES Series - Metroid (USA, Europe).gba
    0x45375046, // Classic NES Series - Pac-Man (USA, Europe).gba
    0x454d5346, // Classic NES Series - Super Mario Bros. (USA, Europe).gba
    0x45565846, // Classic NES Series - Xevious (USA, Europe).gba
    0x45424c46, // Classic NES Series - Zelda II - The Adventure of Link (USA, Europe).gba
    //65
    0x45534741, // Golden Sun (USA, Europe).gba
    //66
    0x46534741, // Golden Sun (France).gba
    0x44534741, // Golden Sun (Germany).gba
    0x49534741, // Golden Sun (Italy).gba
    0x53534741  // Golden Sun (Spain).gba
    //70
  };
  const u8 game_rev[]=
  {
    0,0,0,0,0,0,0,0, //baldur style
    0,0,0,0,0, //rayman3 style
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, //zelda2 style
    0,0,0,0,0,2,0,1,0,1,0,0,0, //mario style
    0, //drilldozer style
    0,0, //drmario style
    0, //coloris
    0, //dialhex
    0,0,0,0,0, //goldensun2 style
    0,0,0,0,0,0,0,0,0,0,0,0, //nes style
    0,0,0,0,0 //goldensun style
  };
  CHECK_SIZEOF(game_codes,70*4);
  CHECK_SIZEOF(game_rev,70);
  const u32 offsets_baldur[]=
  {
    0x6a32b4,
    0x6a32b8,
    0x2e320,
    0x29f98,
    0x6012c,
    0x2e19c,
    0x5e744,
    0x5e780
  };
  const u8 patch_sleep_baldur[]=
  {
    0x08,0x46 // MOV     R0, R1
  };
  const u32 offsets_rayman3[]=
  {
    0x15aba,
    0x15aca,
    0x15ac2,
    0x15ad2,
    0x815aba
  };
  const u8 patch_sleep_rayman3[]=
  {
    0x40,0x22 // MOV     R2, #0x40
  };
  const u32 offsets_zelda2[]=
  {
    0x721f4,
    0x720c4,
    0x720a8,
    0x75350,
    0x55e04,
    0x56284,
    0x561dc,
    0x56108,
    0x7ee26,
    0x7f72a,
    0x7ee26,
    0x7f72a,
    0x68f1e,
    0x68352,
    0x68e8a,
    0x4ca,
    0x366e
  };
  const u8 patch_sleep_zelda2[]=
  {
    0x40,0x21 // MOV     R1, #0x40
  };
  const u32 offsets_mario[]=
  {
    0x2a1c,
    0xfd9d2,
    0x2978,
    0xfb8e6,
    0x2978,
    0xfb686,
    0x2978,
    0xfc02e,
    0x56e,
    0xb4ea8,
    0x56e,
    0xb67b8,
    0x56e,
    0xb9220,
    0x56e,
    0xb9700,
    0x56e,
    0xb61c0,
    0x56e,
    0xb68a0,
    0x18aae,
    0xf504c4,
    0x18a9a,
    0xf504c4,
    0x18aae,
    0xf504c4
  };
  const u8 patch_sleep_drilldozer[]=
  {
    0x40,0x23 // MOV     R3, #0x40
  };
  const u8 patch_sleep_drilldozer2[]=
  {
    0x30,0x88 // LDRH    R0, [R6]
  };
  const u32 offsets_drmario[]=
  {
    0x91830,
    0x422d0a,
    0x91830,
    0x41b5d0
  };
  const u8 patch_sleep_puzzleleague[]=
  {
    0x88,0x21 // MOV     R1, #0x88
  };
  const u8 patch_sleep_drmario[]=
  {
    0xc0,0x46 // NOP
  };
  const u8 patch_sleep_coloris1[]=
  {
    0xc0,0x1a // SUB     R0, R0, R3
  };
  const u8 patch_sleep_coloris2[]=
  {
    0x40,0x18 // ADD     R0, R0, R1
  };
  const u8 patch_sleep_coloris3[]=
  {
    0x00,0x10 // DCD 0x1000
  };
  const u8 patch_sleep_dialhex1[]=
  {
    0x2f,0x88 // LDRH    R7, [R5]
  };
  const u8 patch_sleep_dialhex2[]=
  {
    0x10,0x1c, // ADD     R0, R2, #0
    0xc0,0x46  // NOP
  };
  const u8 patch_sleep_dialhex3[]=
  {
    0x38,0x1c, // ADD     R0, R7, #0
    0x00,0x27, // MOV     R7, #0
    0x17,0x80, // STRH    R7, [R2]
    0xc0,0x46  // NOP
  };
  const u32 offsets_goldensun2[]=
  {
    0x137ac,
    0x17270,
    0x137d8,
    0x1729c
  };
  const u8 patch_sleep_goldensun2[]=
  {
    0x03,0xf0,0x64,0xfd, // BL      xxx
    0x03,0xdf,           // SWI     3
    0x03,0xf0,0x63,0xfd  // BL      yyy
  };
  const u8 patch_sleep_common[]=
  {
    0x00,0x20, // MOV     R0, #0
    0x00,0xe0, // B       loc_6
    0x01,0x20, // MOV     R0, #1
               // loc_6:
    0x03,0x4b, // LDR     R3, =0x4000200
    0x03,0x49, // LDR     R1, =0x2000
    0x1a,0x88, // LDRH    R2, [R3]
    0x4a,0x40, // EOR     R2, R1
    0x1a,0x80, // STRH    R2, [R3]
    0x19,0xdf, // SWI     0x19
    0x70,0x47, // BX      LR
    0x00,0x02,0x00,0x04, // dword_14        DCD 0x4000200
    0x00,0x20,0x00,0x00  // dword_18        DCD 0x2000

  };
  const u32 offsets_nes[]=
  {
    0xbf8,
    0x1788,
    0x870,
    0xc54,
    0x7ec,
    0x85c,
    0xac4,
    0xb78,
    0xb10,
    0x85c,
    0x9b8,
    0x2e2c
  };
  const u8 patch_sleep_nes1[]=
  {
    0x00,0x00,0x9f,0xe5, // LDR     R0, =0x80FFB40
    0x10,0xff,0x2f,0xe1, // BX      R0
    0x40,0xfb,0x0f,0x08  // DCD 0x80FFB40
  };
  const u8 patch_sleep_nes2[]=
  {
    0x30,0x40,0x2d,0xe9, // STMFD   SP!, {R4,R5,LR}
    0x28,0x50,0x9f,0xe5, // LDR     R5, =0x4000200
    0x01,0x3a,0xa0,0xe3, // MOV     R3, #0x1000
    0xb0,0x40,0xd5,0xe1, // LDRH    R4, [R5]
    0xb0,0x30,0xc5,0xe1, // STRH    R3, [R5]
    0x00,0x00,0xa0,0xe3, // MOV     R0, #0
    0x00,0x00,0x19,0xef, // SWI     0x190000
    0x00,0x00,0x03,0xef, // SWI     0x30000
    0x01,0x00,0xa0,0xe3, // MOV     R0, #1
    0x00,0x00,0x19,0xef, // SWI     0x190000
    0xb0,0x40,0xc5,0xe1, // STRH    R4, [R5]
    0x30,0x40,0xbd,0xe8, // LDMFD   SP!, {R4,R5,LR}
    0x1e,0xff,0x2f,0xe1, // BX      LR
    0x00,0x02,0x00,0x04  // DCD 0x4000200
  };
  const u32 offsets_goldensun[]=
  {
    0x33fe,
    0x6cec,
    0x341e,
    0x6d0c,
    0x342e,
    0x6d1c,
    0x344e,
    0x6d3c
  };
  const u8 patch_sleep_goldensun[]=
  {
    0x03,0xf0,0x79,0xfc, // BL      xxx
    0x03,0xdf,           // SWI     3
    0x03,0xf0,0x78,0xfc  // BL      yyy
  };
  s32 index=-1;
  u32 code=Data()[43]; u8 rev=(Data()[47])&0xff;
  for(u32 ii=0;ii<sizeofa(game_codes);ii++)
  {
    if(code==game_codes[ii]&&rev==game_rev[ii])
    {
      index=ii;
      break;
    }
  }
  if(index!=-1)
  {
    res=true;
    switch(index)
    {
      case 0: //baldursgate style
      case 1:
      case 2:
      case 3:
      case 4:
      case 5:
      case 6:
      case 7:
        iWriter->Write(offsets_baldur[index-baldur_style],patch_sleep_baldur,sizeof(patch_sleep_baldur));
        break;
      case 8: //rayman3 style
      case 9:
      case 10:
      case 11:
      case 12:
        iWriter->Write(offsets_rayman3[index-rayman3_style],patch_sleep_rayman3,sizeof(patch_sleep_rayman3));
        break;
      case 13: //zelda2 style
      case 14:
      case 15:
      case 16:
      case 17:
      case 18:
      case 19:
      case 20:
      case 21:
      case 22:
      case 23:
      case 24:
      case 25:
      case 26:
      case 27:
      case 28:
      case 29:
        iWriter->Write(offsets_zelda2[index-zelda2_style],patch_sleep_zelda2,sizeof(patch_sleep_zelda2));
        break;
      case 30: //mario style
      case 31:
      case 32:
      case 33:
      case 34:
      case 35:
      case 36:
      case 37:
      case 38:
      case 39:
      case 40:
      case 41:
      case 42:
        {
          u32 pos=(index-mario_style)*2;
          iWriter->Write(offsets_mario[pos++],patch_sleep_zelda2,sizeof(patch_sleep_zelda2));
          iWriter->Write(offsets_mario[pos],patch_sleep_zelda2,sizeof(patch_sleep_zelda2));
        }
        break;
      case 43: //drilldozer style
        {
          u32 pos=0x2bd70;
          iWriter->Write(pos,patch_sleep_drilldozer,sizeof(patch_sleep_drilldozer));
          iWriter->Write(pos+6,patch_sleep_baldur,sizeof(patch_sleep_baldur));
          iWriter->Write(pos+104,patch_sleep_drilldozer2,sizeof(patch_sleep_drilldozer2));
        }
        break;
      case 44: //drmario style
      case 45:
        {
          u32 pos=(index-drmario_style)*2;
          iWriter->Write(offsets_drmario[pos++],patch_sleep_puzzleleague,sizeof(patch_sleep_puzzleleague));
          iWriter->Write(offsets_drmario[pos],patch_sleep_drmario,sizeof(patch_sleep_drmario));
        }
        break;
      case 46: //coloris
        iWriter->Write(0x7fd8,patch_sleep_coloris1,sizeof(patch_sleep_coloris1));
        iWriter->Write(0x7fda,patch_sleep_drmario,sizeof(patch_sleep_drmario));
        iWriter->Write(0x7ff8,patch_sleep_coloris2,sizeof(patch_sleep_coloris2));
        iWriter->Write(0x803c,patch_sleep_coloris3,sizeof(patch_sleep_coloris3));
        break;
      case 47: //dialhex
        iWriter->Write(0x7f1e,patch_sleep_dialhex1,sizeof(patch_sleep_dialhex1));
        iWriter->Write(0x7f24,patch_sleep_dialhex2,sizeof(patch_sleep_dialhex2));
        iWriter->Write(0x7f46,patch_sleep_dialhex3,sizeof(patch_sleep_dialhex3));
        break;
      case 48: //goldensun2 style
      case 49:
      case 50:
      case 51:
      case 52:
        {
          u32 pos=(index-goldensun2_style)?2:0;
          res=false;
          for(u32 ii=0;ii<iCount;ii++)
          {
            if(iPatchInfo[ii].iType==EFLASHV123_3&&iPatchInfo[ii].iOffset*4==offsets_goldensun2[pos+1])
            {
              res=true;
              iWriter->Write(offsets_goldensun2[pos++],patch_sleep_goldensun2,sizeof(patch_sleep_goldensun2));
              iWriter->Write(offsets_goldensun2[pos]+8,patch_sleep_common,sizeof(patch_sleep_common));
            }
          }
        }
        break;
      case 53: //nes style
      case 54:
      case 55:
      case 56:
      case 57:
      case 58:
      case 59:
      case 60:
      case 61:
      case 62:
      case 63:
      case 64:
        iWriter->Write(offsets_nes[index-nes_style],patch_sleep_nes1,sizeof(patch_sleep_nes1));
        iWriter->Write(0xffb40,patch_sleep_nes2,sizeof(patch_sleep_nes2));
        break;
      case 65:
        //yep, bug in game, no any patching needed :)
        break;
      case 66: //goldensun style
      case 67:
      case 68:
      case 69:
        {
          u32 pos=(index-goldensun_style)*2;
          res=false;
          for(u32 ii=0;ii<iCount;ii++)
          {
            if(iPatchInfo[ii].iType==EFLASHV123_3&&iPatchInfo[ii].iOffset*4==offsets_goldensun[pos+1])
            {
              res=true;
              iWriter->Write(offsets_goldensun[pos++],patch_sleep_goldensun,sizeof(patch_sleep_goldensun));
              iWriter->Write(offsets_goldensun[pos]+8,patch_sleep_common,sizeof(patch_sleep_common));
            }
          }
        }
        break;
    }
  }
  return res;
}

void CGbaPatcher::PatchEEPROM111(void)
{
  const u32 game_codes[]=
  {
    0x4a414d41, //0002
    0x45485441, //0033
    0x45595241, //0046
    0x50595241, //0051
    0x44485441, //0053
    0x46485441  //0059
  };
  const u32 patch_1_offsets[]=
  {
    0xb666c,
    0x37570,
    0x485ec,
    0x485ec,
    0x37580,
    0x37520
  };
  const u32 patch_1_data[]=
  {
    0x083eba71,
    0x087ffaf1,
    0x087b75b1,
    0x087b71a1,
    0x087ff641,
    0x087fe2a1
  };
  const u32 patch_2_offsets[]=
  {
    0xb6940,
    0x37844,
    0x488c0,
    0x488c0,
    0x37854,
    0x377f4
  };
  const u32 patch_3_offsets[]=
  {
    0x3eba70,
    0x7ffaf0,
    0x7b75b0,
    0x7b71a0,
    0x7ff640,
    0x7fe2a0
  };
  const u32 patch_3_data[]=
  {
    0x080b668d,
    0x08037591,
    0x0804860d,
    0x0804860d,
    0x080375a1,
    0x08037541
  };
  const u8 patch_3[]=
  {
    0x39,0x68,0x27,0x48,0x81,0x42,0x23,0xd0,0x89,0x1c,0x08,0x88,0x01,0x28,0x02,0xd1,
    0x24,0x48,0x78,0x60,0x33,0xe0,0x00,0x23,0x00,0x22,0x89,0x1c,0x10,0xb4,0x01,0x24,
    0x08,0x68,0x20,0x40,0x5b,0x00,0x03,0x43,0x89,0x1c,0x52,0x1c,0x06,0x2a,0xf7,0xd1,
    0x10,0xbc,0x39,0x60,0xdb,0x01,0x02,0x20,0x00,0x02,0x1b,0x18,0x0e,0x20,0x00,0x06,
    0x1b,0x18,0x7b,0x60,0x39,0x1c,0x08,0x31,0x08,0x88,0x09,0x38,0x08,0x80,0x16,0xe0,
    0x15,0x49,0x00,0x23,0x00,0x22,0x10,0xb4,0x01,0x24,0x08,0x68,0x20,0x40,0x5b,0x00,
    0x03,0x43,0x89,0x1c,0x52,0x1c,0x06,0x2a,0xf7,0xd1,0x10,0xbc,0xdb,0x01,0x02,0x20,
    0x00,0x02,0x1b,0x18,0x0e,0x20,0x00,0x06,0x1b,0x18,0x08,0x3b,0x3b,0x60,0x0b,0x48,
    0x39,0x68,0x01,0x60,0x0a,0x48,0x79,0x68,0x01,0x60,0x0a,0x48,0x39,0x1c,0x08,0x31,
    0x0a,0x88,0x80,0x21,0x09,0x06,0x0a,0x43,0x02,0x60,0x07,0x48,0x00,0x47,0x00,0x00,
    0x00,0x00,0x00,0x0d,0x00,0x00,0x00,0x0e,0x04,0x00,0x00,0x0e,0xd4,0x00,0x00,0x04,
    0xd8,0x00,0x00,0x04,0xdc,0x00,0x00,0x04
  };

  s32 index=-1;
  for(u32 ii=0;ii<sizeofa(game_codes);ii++)
  {
    if(Data()[43]==game_codes[ii]&&Data()[patch_1_offsets[ii]/4]==0x6839480e)
    {
      index=ii;
      break;
    }
  }
  if(index==-1) return;
  u32 patch_1_2[2];
  patch_1_2[0]=0x47004800;
  patch_1_2[1]=patch_1_data[index];
  iWriter->Write(patch_1_offsets[index],(u8*)patch_1_2,sizeof(patch_1_2));
  patch_1_2[0]=0x20e0e027;
  patch_1_2[1]=0x88010500;
  iWriter->Write(patch_2_offsets[index],(u8*)patch_1_2,sizeof(patch_1_2));
  iWriter->Write(patch_3_offsets[index],patch_3,sizeof(patch_3));
  iWriter->Write(patch_3_offsets[index]+sizeof(patch_3),(u8*)(patch_3_data+index),sizeof(patch_3_data[0]));
}

bool CGbaPatcher::PatchDragonBallZ(void)
{
  bool res=false;
  const u32 game_codes[]=
  {
    0x45464c42, //2 Games in 1 - Dragon Ball Z - The Legacy of Goku I & II (USA).gba
    0x50474c41, //Dragon Ball Z - The Legacy of Goku (Europe) (En,Fr,De,Es,It).gba
    0x45474c41, //Dragon Ball Z - The Legacy of Goku (USA).gba
    0x50464c41, //Dragon Ball Z - The Legacy of Goku II (Europe) (En,Fr,De,Es,It).gba
    0x45464c41, //Dragon Ball Z - The Legacy of Goku II (USA).gba
    0x4a464c41  //Dragon Ball Z - The Legacy of Goku II International (Japan).gba
  };
  const u32 patch_1_offsets[]=
  {
    0x033C,
    0x0340,
    0x0356,
    0x035A,
    0x035E,
    0x0384,
    0x0388,
    0x494C,
    0x4950,
    0x4978,
    0x497C,
    0x998E,
    0x9992
  };
  const u32 patch_2_offsets[]=
  {
    0x356,
    0x35e,
    0x37e,
    0x382
  };
  s32 index=-1;
  for(u32 ii=0;ii<sizeofa(game_codes);ii++)
  {
    if(Data()[43]==game_codes[ii])
    {
      index=ii;
      break;
    }
  }
  if(index!=-1)
  {
    u16 patch=0;
    res=true;
    switch(index)
    {
      case 0: //2in1 us
        for(u32 ii=0;ii<sizeofa(patch_2_offsets);ii++)
        {
          iWriter->Write(patch_2_offsets[ii]+0x40000,(u8*)&patch,sizeof(patch));
        }
        patch=0x1001;
        iWriter->Write(0xbb9016,(u8*)&patch,sizeof(patch));
        break;
      case 1: //I eu
        patch=0x46c0;
        for(u32 ii=0;ii<sizeofa(patch_1_offsets);ii++)
        {
          iWriter->Write(patch_1_offsets[ii],(u8*)&patch,sizeof(patch));
        }
        break;
      case 2: //I us
        for(u32 ii=0;ii<sizeofa(patch_2_offsets);ii++)
        {
          iWriter->Write(patch_2_offsets[ii],(u8*)&patch,sizeof(patch));
        }
        break;
      case 3: //II eu
        patch=0x1001;
        iWriter->Write(0x6f42b2,(u8*)&patch,sizeof(patch));
        break;
      case 4: //II us
        patch=0x1001;
        iWriter->Write(0x3b8e9e,(u8*)&patch,sizeof(patch));
        break;
      case 5: //II jp
        patch=0x1001;
        iWriter->Write(0x3fc8f6,(u8*)&patch,sizeof(patch));
        break;
      default:
        res=false;
        break;
    }
  }
  return res;
}
bool CGbaPatcher::PatchNes(void)
{
  bool res=false;
  u32 jump=Data()[0];
  if((jump&0xff000000)==0xea000000)
  {
    size_t index=(jump&0xffffff)+2;
    if(Data()[index]==0xe28f503c&&Data()[index+1]==0xe8b503d3&&Data()[index+2]==0xe129f007&&Data()[index+3]==0xe281deba&&Data()[index+4]==0xe129f008&&Data()[index+5]==0xe281debe&&Data()[index+6]==0xe129f009&&Data()[index+7]==0xe281dc0b&&Data()[index+8]==0xe92d0003&&Data()[index+9]==0xef110000&&Data()[index+10]==0xe8bd8001)
    {
      res=true;
      iSaveSize=8*1024;
      u32 patch=0xea000000|(0x3fdf5-index);
      iWriter->Write(36+index*4,(u8*)&patch,sizeof(patch));
      iWriter->Write(0xff800,gba_nes_patch_bin,gba_nes_patch_bin_size);
      patch=Data()[index+17];
      iWriter->Write(0xff840,(u8*)&patch,sizeof(patch));
      size_t index2=(patch-0x08000000)/4;
      if(Data()[index2-1]==0x3032)
      {
        patch=0x060000f8;
        iWriter->Write(0xff86c,(u8*)&patch,sizeof(patch));
      }
    }
  }
  return res;
}
