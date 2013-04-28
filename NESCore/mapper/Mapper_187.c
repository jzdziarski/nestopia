/*===================================================================*/
/*                                                                   */
/*            Mapper 187 : Street Fighter Zero 2 97                  */
/*                                                                   */
/*===================================================================*/

byte	Map187_Prg[4];
int	Map187_Chr[8];
byte	Map187_Bank[8];

byte	Map187_ExtMode;
byte	Map187_ChrMode;
byte	Map187_ExtEnable;

byte	Map187_IRQ_Enable;
byte	Map187_IRQ_Counter;
byte	Map187_IRQ_Latch;
byte	Map187_IRQ_Occur;
byte	Map187_LastWrite;

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 187                                            */
/*-------------------------------------------------------------------*/
void Map187_Init()
{
  /* Initialize Mapper */
  MapperInit = Map187_Init;

  /* Write to Mapper */
  MapperWrite = Map187_Write;

  /* Write to SRAM */
  MapperSram = Map0_Sram;

  /* Write to APU */
  MapperApu = Map187_Apu;

  /* Read from APU */
  MapperReadApu = Map187_ReadApu;

  /* Callback at VSync */
  MapperVSync = Map0_VSync;

  /* Callback at HSync */
  MapperHSync = Map187_HSync;

  /* Callback at PPU */
  MapperPPU = Map0_PPU;

  /* Callback at Rendering Screen ( 1:BG, 0:Sprite ) */
  MapperRenderScreen = Map0_RenderScreen;

  /* Set SRAM Banks */
  W.SRAMBANK = S.SRAM;

  /* Set Registers */
  int i ;
for (i = 0; i < 8; i++ ) {
    Map187_Chr[i] = 0x00;
    Map187_Bank[i] = 0x00;
  }

  /* Set ROM Banks */
  Map187_Prg[0] = (S.NesHeader.ROMSize<<1)-4;
  Map187_Prg[1] = (S.NesHeader.ROMSize<<1)-3;
  Map187_Prg[2] = (S.NesHeader.ROMSize<<1)-2;
  Map187_Prg[3] = (S.NesHeader.ROMSize<<1)-1;
  Map187_Set_CPU_Banks();

  Map187_ExtMode = 0;
  Map187_ChrMode = 0;
  Map187_ExtEnable = 0;

  Map187_IRQ_Enable = 0;
  Map187_IRQ_Counter = 0;
  Map187_IRQ_Latch = 0;

  Map187_LastWrite = 0;


 
}

/*-------------------------------------------------------------------*/
/*  Mapper 187 Write Function                                        */
/*-------------------------------------------------------------------*/
void Map187_Write( word wAddr, byte byData )
{
  Map187_LastWrite = byData;
  switch( wAddr ) {
  case	0x8003:
    Map187_ExtEnable = 0xFF;
    Map187_ChrMode = byData;
    if( (byData&0xF0) == 0 ) {
      Map187_Prg[2] = (S.NesHeader.ROMSize<<1)-2;
      Map187_Set_CPU_Banks();
    }
    break;
    
  case	0x8000:
    Map187_ExtEnable = 0;
    Map187_ChrMode = byData;
    break;

  case	0x8001:
    if( !Map187_ExtEnable ) {
      switch( Map187_ChrMode & 7 ) {
      case	0:
	byData &= 0xFE;
	Map187_Chr[4] = (int)byData+0x100;
	Map187_Chr[5] = (int)byData+0x100+1;
	Map187_Set_PPU_Banks();
	break;
      case	1:
	byData &= 0xFE;
	Map187_Chr[6] = (int)byData+0x100;
	Map187_Chr[7] = (int)byData+0x100+1;
	Map187_Set_PPU_Banks();
	break;
      case	2:
	Map187_Chr[0] = byData;
	Map187_Set_PPU_Banks();
	break;
      case	3:
	Map187_Chr[1] = byData;
	Map187_Set_PPU_Banks();
	break;
      case	4:
	Map187_Chr[2] = byData;
	Map187_Set_PPU_Banks();
	break;
      case	5:
	Map187_Chr[3] = byData;
	Map187_Set_PPU_Banks();
	break;
      case	6:
	if( (Map187_ExtMode&0xA0)!=0xA0 ) {
	  Map187_Prg[0] = byData;
	  Map187_Set_CPU_Banks();
	}
	break;
      case	7:
	if( (Map187_ExtMode&0xA0)!=0xA0 ) {
	  Map187_Prg[1] = byData;
	  Map187_Set_CPU_Banks();
	}
	break;
      default:
	break;
      }
    } else {
      switch( Map187_ChrMode ) {
      case	0x2A:
	Map187_Prg[1] = 0x0F;
	break;
      case	0x28:
	Map187_Prg[2] = 0x17;
	break;
      case	0x26:
	break;
      default:
	break;
      }
      Map187_Set_CPU_Banks();
    }
    Map187_Bank[Map187_ChrMode&7] = byData;
    break;
    
  case	0xA000:
    if( byData & 0x01 ) {
      NESCore_Mirroring( 0 );
    } else {
      NESCore_Mirroring( 1 );
    }
    break;
  case	0xA001:
    break;
    
  case	0xC000:
    Map187_IRQ_Counter = byData;
    Map187_IRQ_Occur = 0;
    break;
  case	0xC001:
    Map187_IRQ_Latch = byData;
    Map187_IRQ_Occur = 0;
    break;
  case	0xE000:
  case	0xE002:
    Map187_IRQ_Enable = 0;
    Map187_IRQ_Occur = 0;
    break;
  case	0xE001:
  case	0xE003:
    Map187_IRQ_Enable = 1;
    Map187_IRQ_Occur = 0;
    break;
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 187 Write to APU Function                                 */
/*-------------------------------------------------------------------*/
void Map187_Apu( word wAddr, byte byData )
{
  Map187_LastWrite = byData;
  if( wAddr == 0x5000 ) {
    Map187_ExtMode = byData;
    if( byData & 0x80 ) {
      if( byData & 0x20 ) {
	Map187_Prg[0] = ((byData&0x1E)<<1)+0;
	Map187_Prg[1] = ((byData&0x1E)<<1)+1;
	Map187_Prg[2] = ((byData&0x1E)<<1)+2;
	Map187_Prg[3] = ((byData&0x1E)<<1)+3;
      } else {
	Map187_Prg[2] = ((byData&0x1F)<<1)+0;
	Map187_Prg[3] = ((byData&0x1F)<<1)+1;
      }
    } else {
      Map187_Prg[0] = Map187_Bank[6];
      Map187_Prg[1] = Map187_Bank[7];
      Map187_Prg[2] = (S.NesHeader.ROMSize<<1)-2;
      Map187_Prg[3] = (S.NesHeader.ROMSize<<1)-1;
    }
    Map187_Set_CPU_Banks();
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 187 Read from APU Function                                */
/*-------------------------------------------------------------------*/
byte Map187_ReadApu( word wAddr )
{
  switch( Map187_LastWrite&0x03 ) {
  case 0:
    return 0x83;
  case 1:
    return 0x83;
  case 2:
    return 0x42;
  case 3:
    return 0x00;
  }
  return 0;
}

/*-------------------------------------------------------------------*/
/*  Mapper 187 H-Sync Function                                       */
/*-------------------------------------------------------------------*/
void Map187_HSync()
{
  if( ( /* S.PPU_Scanline >= 0 && */ S.PPU_Scanline <= 239) ) {
    if( S.PPU_R1 & R1_SHOW_BG || S.PPU_R1 & R1_SHOW_SP ) {
      if( Map187_IRQ_Enable ) {
	if( !Map187_IRQ_Counter ) {
	  Map187_IRQ_Counter--;
	  Map187_IRQ_Enable = 0;
	  Map187_IRQ_Occur = 0xFF;
	} else {
	  Map187_IRQ_Counter--;
	}
      }
    }
  }
  if ( Map187_IRQ_Occur ) {
	  IRQ_REQ;
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 187 Set CPU Banks Function                                */
/*-------------------------------------------------------------------*/
void Map187_Set_CPU_Banks()
{
  W.ROMBANK0 = ROMPAGE(((Map187_Prg[0]<<2)+0) % (S.NesHeader.ROMSize<<1));
  W.ROMBANK1 = ROMPAGE(((Map187_Prg[1]<<2)+1) % (S.NesHeader.ROMSize<<1));
  W.ROMBANK2 = ROMPAGE(((Map187_Prg[2]<<2)+2) % (S.NesHeader.ROMSize<<1));
  W.ROMBANK3 = ROMPAGE(((Map187_Prg[3]<<2)+3) % (S.NesHeader.ROMSize<<1));
}

/*-------------------------------------------------------------------*/
/*  Mapper 187 Set PPU Banks Function                                */
/*-------------------------------------------------------------------*/
void Map187_Set_PPU_Banks()
{
  W.PPUBANK[ 0 ] = VROMPAGE(((Map187_Chr[0]<<3)+0) % (S.NesHeader.VROMSize<<3));
  W.PPUBANK[ 1 ] = VROMPAGE(((Map187_Chr[1]<<3)+1) % (S.NesHeader.VROMSize<<3));
  W.PPUBANK[ 2 ] = VROMPAGE(((Map187_Chr[2]<<3)+2) % (S.NesHeader.VROMSize<<3));
  W.PPUBANK[ 3 ] = VROMPAGE(((Map187_Chr[3]<<3)+3) % (S.NesHeader.VROMSize<<3));
  W.PPUBANK[ 4 ] = VROMPAGE(((Map187_Chr[4]<<3)+4) % (S.NesHeader.VROMSize<<3));
  W.PPUBANK[ 5 ] = VROMPAGE(((Map187_Chr[5]<<3)+5) % (S.NesHeader.VROMSize<<3));
  W.PPUBANK[ 6 ] = VROMPAGE(((Map187_Chr[6]<<3)+6) % (S.NesHeader.VROMSize<<3));
  W.PPUBANK[ 7 ] = VROMPAGE(((Map187_Chr[7]<<3)+7) % (S.NesHeader.VROMSize<<3));
  NESCore_Develop_Character_Data();
}
