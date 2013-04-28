/*===================================================================*/
/*                                                                   */
/*                      Mapper 249 : MMC3                            */
/*                                                                   */
/*===================================================================*/

byte	Map249_Spdata;
byte	Map249_Reg[8];

byte	Map249_IRQ_Enable;
byte	Map249_IRQ_Counter;
byte	Map249_IRQ_Latch;
byte	Map249_IRQ_Request;

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 249                                            */
/*-------------------------------------------------------------------*/
void Map249_Init()
{
  /* Initialize Mapper */
  MapperInit = Map249_Init;

  /* Write to Mapper */
  MapperWrite = Map249_Write;

  /* Write to SRAM */
  MapperSram = Map0_Sram;

  /* Write to APU */
  MapperApu = Map249_Apu;

  /* Read from APU */
  MapperReadApu = Map0_ReadApu;

  /* Callback at VSync */
  MapperVSync = Map0_VSync;

  /* Callback at HSync */
  MapperHSync = Map249_HSync;

  /* Callback at PPU */
  MapperPPU = Map0_PPU;

  /* Callback at Rendering Screen ( 1:BG, 0:Sprite ) */
  MapperRenderScreen = Map0_RenderScreen;

  /* Set SRAM Banks */
  W.SRAMBANK = S.SRAM;

  /* Set Registers */
  int i ;
for (i = 0; i < 8; i++ ) {
    Map249_Reg[i] = 0x00;
  }

  /* Set ROM Banks */
  W.ROMBANK0 = ROMPAGE( 0 );
  W.ROMBANK1 = ROMPAGE( 1 );
  W.ROMBANK2 = ROMLASTPAGE( 1 );
  W.ROMBANK3 = ROMLASTPAGE( 0 );



  /* Set PPU Banks */
  if ( S.NesHeader.VROMSize > 0 ) {
    int nPage ;
for (nPage = 0; nPage < 8; ++nPage )
      W.PPUBANK[ nPage ] = VROMPAGE( nPage );
    NESCore_Develop_Character_Data();
  }

  /* Set Registers */
  Map249_IRQ_Enable = 0;	// Disable
  Map249_IRQ_Counter = 0;
  Map249_IRQ_Latch = 0;
  Map249_IRQ_Request = 0;

  Map249_Spdata = 0;


 
}

/*-------------------------------------------------------------------*/
/*  Mapper 249 Write Function                                        */
/*-------------------------------------------------------------------*/
void Map249_Write( word wAddr, byte byData )
{
  byte	byM0,byM1,byM2,byM3,byM4,byM5,byM6,byM7;

  switch( wAddr&0xFF01 ) {
  case	  0x8000:
  case    0x8800:
    Map249_Reg[0] = byData;
    break;
  case	  0x8001:
  case    0x8801:
    switch( Map249_Reg[0] & 0x07 ) {
    case	0x00:
      if( Map249_Spdata == 1 ) {
	byM0=byData&0x1;
	byM1=(byData&0x02)>>1;
	byM2=(byData&0x04)>>2;
	byM3=(byData&0x08)>>3;
	byM4=(byData&0x10)>>4;
	byM5=(byData&0x20)>>5;
	byM6=(byData&0x40)>>6;
	byM7=(byData&0x80)>>7;
	byData=(byM5<<7)|(byM4<<6)|(byM2<<5)|(byM6<<4)|(byM7<<3)|(byM3<<2)|(byM1<<1)|byM0;
      }
      W.PPUBANK[ 0 ] = VROMPAGE((byData&0xFE) % (S.NesHeader.VROMSize<<3));
      W.PPUBANK[ 1 ] = VROMPAGE((byData|0x01) % (S.NesHeader.VROMSize<<3));
      NESCore_Develop_Character_Data();
      break;
    case	0x01:
      if( Map249_Spdata == 1 ) {
	byM0=byData&0x1;
	byM1=(byData&0x02)>>1;
	byM2=(byData&0x04)>>2;
	byM3=(byData&0x08)>>3;
	byM4=(byData&0x10)>>4;
	byM5=(byData&0x20)>>5;
	byM6=(byData&0x40)>>6;
	byM7=(byData&0x80)>>7;
	byData=(byM5<<7)|(byM4<<6)|(byM2<<5)|(byM6<<4)|(byM7<<3)|(byM3<<2)|(byM1<<1)|byM0;
      }
      W.PPUBANK[ 2 ] = VROMPAGE((byData&0xFE) % (S.NesHeader.VROMSize<<3));
      W.PPUBANK[ 3 ] = VROMPAGE((byData|0x01) % (S.NesHeader.VROMSize<<3));
      NESCore_Develop_Character_Data();
      break;
    case	0x02:
      if( Map249_Spdata == 1 ) {
	byM0=byData&0x1;
	byM1=(byData&0x02)>>1;
	byM2=(byData&0x04)>>2;
	byM3=(byData&0x08)>>3;
	byM4=(byData&0x10)>>4;
	byM5=(byData&0x20)>>5;
	byM6=(byData&0x40)>>6;
	byM7=(byData&0x80)>>7;
	byData=(byM5<<7)|(byM4<<6)|(byM2<<5)|(byM6<<4)|(byM7<<3)|(byM3<<2)|(byM1<<1)|byM0;
      }
      W.PPUBANK[ 4 ] = VROMPAGE(byData % (S.NesHeader.VROMSize<<3));
      NESCore_Develop_Character_Data();
      break;
    case	0x03:
      if( Map249_Spdata == 1 ) {
	byM0=byData&0x1;
	byM1=(byData&0x02)>>1;
	byM2=(byData&0x04)>>2;
	byM3=(byData&0x08)>>3;
	byM4=(byData&0x10)>>4;
	byM5=(byData&0x20)>>5;
	byM6=(byData&0x40)>>6;
	byM7=(byData&0x80)>>7;
	byData=(byM5<<7)|(byM4<<6)|(byM2<<5)|(byM6<<4)|(byM7<<3)|(byM3<<2)|(byM1<<1)|byM0;
      }
      W.PPUBANK[ 5 ] = VROMPAGE(byData % (S.NesHeader.VROMSize<<3));
      NESCore_Develop_Character_Data();
      break;
    case	0x04:
      if( Map249_Spdata == 1 ) {
	byM0=byData&0x1;
	byM1=(byData&0x02)>>1;
	byM2=(byData&0x04)>>2;
	byM3=(byData&0x08)>>3;
	byM4=(byData&0x10)>>4;
	byM5=(byData&0x20)>>5;
	byM6=(byData&0x40)>>6;
	byM7=(byData&0x80)>>7;
	byData=(byM5<<7)|(byM4<<6)|(byM2<<5)|(byM6<<4)|(byM7<<3)|(byM3<<2)|(byM1<<1)|byM0;
      }
      W.PPUBANK[ 6 ] = VROMPAGE(byData % (S.NesHeader.VROMSize<<3));
      NESCore_Develop_Character_Data();
      break;
    case	0x05:
      if( Map249_Spdata == 1 ) {
	byM0=byData&0x1;
	byM1=(byData&0x02)>>1;
	byM2=(byData&0x04)>>2;
	byM3=(byData&0x08)>>3;
	byM4=(byData&0x10)>>4;
	byM5=(byData&0x20)>>5;
	byM6=(byData&0x40)>>6;
	byM7=(byData&0x80)>>7;
	byData=(byM5<<7)|(byM4<<6)|(byM2<<5)|(byM6<<4)|(byM7<<3)|(byM3<<2)|(byM1<<1)|byM0;
      }
      W.PPUBANK[ 7 ] = VROMPAGE(byData % (S.NesHeader.VROMSize<<3));
      NESCore_Develop_Character_Data();
      break;
    case	0x06:
      if( Map249_Spdata == 1 ) {
	if( byData < 0x20 ) {
	  byM0=byData&0x1;
	  byM1=(byData&0x02)>>1;
	  byM2=(byData&0x04)>>2;
	  byM3=(byData&0x08)>>3;
	  byM4=(byData&0x10)>>4;
	  byM5=0;
	  byM6=0;
	  byM7=0;
	  byData=(byM7<<7)|(byM6<<6)|(byM5<<5)|(byM2<<4)|(byM1<<3)|(byM3<<2)|(byM4<<1)|byM0;
	} else {
	  byData=byData-0x20;
	  byM0=byData&0x1;
	  byM1=(byData&0x02)>>1;
	  byM2=(byData&0x04)>>2;
	  byM3=(byData&0x08)>>3;
	  byM4=(byData&0x10)>>4;
	  byM5=(byData&0x20)>>5;
	  byM6=(byData&0x40)>>6;
	  byM7=(byData&0x80)>>7;
	  byData=(byM5<<7)|(byM4<<6)|(byM2<<5)|(byM6<<4)|(byM7<<3)|(byM3<<2)|(byM1<<1)|byM0;
	}
      }
      W.ROMBANK0 = ROMPAGE(byData % (S.NesHeader.ROMSize<<1));
      break;
    case	0x07:
      if( Map249_Spdata == 1 ) {
	if( byData < 0x20 ) {
	  byM0=byData&0x1;
	  byM1=(byData&0x02)>>1;
	  byM2=(byData&0x04)>>2;
	  byM3=(byData&0x08)>>3;
	  byM4=(byData&0x10)>>4;
	  byM5=0;
	  byM6=0;
	  byM7=0;
	  byData=(byM7<<7)|(byM6<<6)|(byM5<<5)|(byM2<<4)|(byM1<<3)|(byM3<<2)|(byM4<<1)|byM0;
	} else {
	  byData=byData-0x20;
	  byM0=byData&0x1;
	  byM1=(byData&0x02)>>1;
	  byM2=(byData&0x04)>>2;
	  byM3=(byData&0x08)>>3;
	  byM4=(byData&0x10)>>4;
	  byM5=(byData&0x20)>>5;
	  byM6=(byData&0x40)>>6;
	  byM7=(byData&0x80)>>7;
	  byData=(byM5<<7)|(byM4<<6)|(byM2<<5)|(byM6<<4)|(byM7<<3)|(byM3<<2)|(byM1<<1)|byM0;
	}
      }
      W.ROMBANK1 = ROMPAGE(byData % (S.NesHeader.ROMSize<<1));
      break;
    }
    break;
  case	  0xA000:
  case    0xA800:
    Map249_Reg[2] = byData;
    if( !S.ROM_FourScr ) {
      if( byData & 0x01 ) NESCore_Mirroring( 0 );
      else		  NESCore_Mirroring( 1 );
    }
    break;
  case	  0xA001:
  case    0xA801:
    Map249_Reg[3] = byData;
    break;
  case	  0xC000:
  case    0xC800:
    Map249_Reg[4] = byData;
    Map249_IRQ_Counter = byData;
    Map249_IRQ_Request = 0;
    break;
  case	  0xC001:
  case    0xC801:
    Map249_Reg[5] = byData;
    Map249_IRQ_Latch = byData;
    Map249_IRQ_Request = 0;
    break;
  case	  0xE000:
  case    0xE800:
    Map249_Reg[6] = byData;
    Map249_IRQ_Enable = 0;
    Map249_IRQ_Request = 0;
    break;
  case	  0xE001:
  case    0xE801:
    Map249_Reg[7] = byData;
    Map249_IRQ_Enable = 1;
    Map249_IRQ_Request = 0;
    break;
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 249 Write to APU Function                                 */
/*-------------------------------------------------------------------*/
void Map249_Apu( word wAddr, byte byData )
{
  if( wAddr == 0x5000 ) {
    switch( byData ) {
    case	0x00:
      Map249_Spdata = 0;
      break;
    case	0x02:
      Map249_Spdata = 1;
      break;
    }
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 249 H-Sync Function                                       */
/*-------------------------------------------------------------------*/
void Map249_HSync()
{
  if( ( /* S.PPU_Scanline >= 0 && */ S.PPU_Scanline <= 239) ) {
    if( S.PPU_R1 & R1_SHOW_BG || S.PPU_R1 & R1_SHOW_SP ) {
      if( Map249_IRQ_Enable && !Map249_IRQ_Request ) {
	if( S.PPU_Scanline == 0 ) {
	  if( Map249_IRQ_Counter ) {
	    Map249_IRQ_Counter--;
	  }
	}
	if( !(Map249_IRQ_Counter--) ) {
	  Map249_IRQ_Request = 0xFF;
	  Map249_IRQ_Counter = Map249_IRQ_Latch;
	}
      }
    }
  }
  if( Map249_IRQ_Request ) {
    IRQ_REQ;
  }
}

