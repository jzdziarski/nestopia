/*===================================================================*/
/*                                                                   */
/*   Mapper 116 : CartSaint : Yuu Yuu AV Kyoretsuden                 */
/*                                                                   */
/*===================================================================*/

byte	Map116_Reg[8];
byte	Map116_Prg0, Map116_Prg1, Map116_Prg2, Map116_Prg3;
byte	Map116_Prg0L, Map116_Prg1L;
byte	Map116_Chr0, Map116_Chr1, Map116_Chr2, Map116_Chr3;
byte    Map116_Chr4, Map116_Chr5, Map116_Chr6, Map116_Chr7;

byte	Map116_IRQ_Enable;
byte	Map116_IRQ_Counter;
byte	Map116_IRQ_Latch;

byte	Map116_ExPrgSwitch;
byte	Map116_ExChrSwitch;

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 116                                            */
/*-------------------------------------------------------------------*/
void Map116_Init()
{
  /* Initialize Mapper */
  MapperInit = Map116_Init;

  /* Write to Mapper */
  MapperWrite = Map116_Write;

  /* Write to SRAM */
  MapperSram = Map0_Sram;

  /* Write to APU */
  MapperApu = Map0_Apu;

  /* Read from APU */
  MapperReadApu = Map0_ReadApu;

  /* Callback at VSync */
  MapperVSync = Map0_VSync;

  /* Callback at HSync */
  MapperHSync = Map116_HSync;

  /* Callback at PPU */
  MapperPPU = Map0_PPU;

  /* Callback at Rendering Screen ( 1:BG, 0:Sprite ) */
  MapperRenderScreen = Map0_RenderScreen;

  /* Set SRAM Banks */
  W.SRAMBANK = S.SRAM;

  /* Initialize Registers */
  int i ;
for (i = 0; i < 8; i++ ) {
    Map116_Reg[i] = 0x00;
  }

  Map116_Prg0 = Map116_Prg0L = 0;
  Map116_Prg1 = Map116_Prg1L = 1;
  Map116_Prg2 = ( S.NesHeader.ROMSize << 1 ) - 2;
  Map116_Prg3 = ( S.NesHeader.ROMSize << 1 ) - 1;

  Map116_ExPrgSwitch = 0;
  Map116_ExChrSwitch = 0;

  /* Set ROM Banks */
  Map116_Set_CPU_Banks();

  /* Set PPU Banks */
  if ( S.NesHeader.VROMSize > 0 ) {
    Map116_Chr0 = 0;
    Map116_Chr1 = 1;
    Map116_Chr2 = 2;
    Map116_Chr3 = 3;
    Map116_Chr4 = 4;
    Map116_Chr5 = 5;
    Map116_Chr6 = 6;
    Map116_Chr7 = 7;
    Map116_Set_PPU_Banks();
  } else {
    Map116_Chr0 = Map116_Chr2 = Map116_Chr4 = Map116_Chr5 = Map116_Chr6 = Map116_Chr7 = 0;
    Map116_Chr1 = Map116_Chr3 = 1;
  }

  Map116_IRQ_Enable = 0;	/* Disable */
  Map116_IRQ_Counter = 0;
  Map116_IRQ_Latch = 0;
  

 
}

/*-------------------------------------------------------------------*/
/*  Mapper 116 Write Function                                        */
/*-------------------------------------------------------------------*/
void Map116_Write( word wAddr, byte byData )
{
  switch( wAddr & 0xE001 ) {
  case	0x8000:
    Map116_Reg[0] = byData;
    Map116_Set_CPU_Banks();
    Map116_Set_PPU_Banks();
    break;
  case	0x8001:
    Map116_Reg[1] = byData;
    switch( Map116_Reg[0] & 0x07 ) {
    case	0x00:
      Map116_Chr0 = byData & 0xFE;
      Map116_Chr1 = Map116_Chr0+1;
      Map116_Set_PPU_Banks();
      break;
    case	0x01:
      Map116_Chr2 = byData & 0xFE;
      Map116_Chr3 = Map116_Chr2+1;
      Map116_Set_PPU_Banks();
      break;
    case	0x02:
      Map116_Chr4 = byData;
      Map116_Set_PPU_Banks();
      break;
    case	0x03:
      Map116_Chr5 = byData;
      Map116_Set_PPU_Banks();
      break;
    case	0x04:
      Map116_Chr6 = byData;
      Map116_Set_PPU_Banks();
      break;
    case	0x05:
      Map116_Chr7 = byData;
      Map116_Set_PPU_Banks();
      break;
    case	0x06:
      Map116_Prg0 = byData;
      Map116_Set_CPU_Banks();
      break;
    case	0x07:
      Map116_Prg1 = byData;
      Map116_Set_CPU_Banks();
      break;
    }
    break;
  case	0xA000:
    Map116_Reg[2] = byData;
    if ( !S.ROM_FourScr ) {
      if( byData & 0x01 ) NESCore_Mirroring( 0 );
      else		  NESCore_Mirroring( 1 );
    }
    break;
  case	0xA001:
    Map116_Reg[3] = byData;
    break;
  case	0xC000:
    Map116_Reg[4] = byData;
    Map116_IRQ_Counter = byData;
    Map116_IRQ_Enable = 0xFF;
    break;
  case	0xC001:
    Map116_Reg[5] = byData;
    Map116_IRQ_Latch = byData;
    break;
  case	0xE000:
    Map116_Reg[6] = byData;
    Map116_IRQ_Enable = 0;
    break;
  case	0xE001:
    Map116_Reg[7] = byData;
    Map116_IRQ_Enable = 0xFF;
    break;
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 116 H-Sync Function                                       */
/*-------------------------------------------------------------------*/
void Map116_HSync()
{
  if( ( /* S.PPU_Scanline >= 0 && */ S.PPU_Scanline <= 239) ) {
    if( S.PPU_R1 & R1_SHOW_BG || S.PPU_R1 & R1_SHOW_SP ) {
      if( Map116_IRQ_Enable ) {
	if( !(Map116_IRQ_Counter--) ) {
	  Map116_IRQ_Counter = Map116_IRQ_Latch;
	  IRQ_REQ;
	}
      }
    }
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 116 Set CPU Banks Function                                */
/*-------------------------------------------------------------------*/
void Map116_Set_CPU_Banks()
{
  if( Map116_Reg[0] & 0x40 ) {
    W.ROMBANK0 = ROMPAGE( ( S.NesHeader.ROMSize << 1 ) - 2 );
    W.ROMBANK1 = ROMPAGE( Map116_Prg1 % ( S.NesHeader.ROMSize << 1 ) );
    W.ROMBANK2 = ROMPAGE( Map116_Prg0 % ( S.NesHeader.ROMSize << 1 ) );
    W.ROMBANK3 = ROMPAGE( ( S.NesHeader.ROMSize << 1 ) - 1 );
  } else {
    W.ROMBANK0 = ROMPAGE( Map116_Prg0 % ( S.NesHeader.ROMSize << 1 ) );
    W.ROMBANK1 = ROMPAGE( Map116_Prg1 % ( S.NesHeader.ROMSize << 1 ) );
    W.ROMBANK2 = ROMPAGE( ( S.NesHeader.ROMSize << 1 ) - 2 );
    W.ROMBANK3 = ROMPAGE( ( S.NesHeader.ROMSize << 1 ) - 1 );
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 116 Set PPU Banks Function                                */
/*-------------------------------------------------------------------*/
void Map116_Set_PPU_Banks()
{
  if ( S.NesHeader.VROMSize > 0 ) {
    if( Map116_Reg[0] & 0x80 ) {
      W.PPUBANK[ 0 ] = VROMPAGE( ((Map116_ExChrSwitch<<8)+Map116_Chr4) % (S.NesHeader.VROMSize<<3));
      W.PPUBANK[ 1 ] = VROMPAGE( ((Map116_ExChrSwitch<<8)+Map116_Chr5) % (S.NesHeader.VROMSize<<3));
      W.PPUBANK[ 2 ] = VROMPAGE( ((Map116_ExChrSwitch<<8)+Map116_Chr6) % (S.NesHeader.VROMSize<<3));
      W.PPUBANK[ 3 ] = VROMPAGE( ((Map116_ExChrSwitch<<8)+Map116_Chr7) % (S.NesHeader.VROMSize<<3));
      W.PPUBANK[ 4 ] = VROMPAGE( ((Map116_ExChrSwitch<<8)+Map116_Chr0) % (S.NesHeader.VROMSize<<3));
      W.PPUBANK[ 5 ] = VROMPAGE( ((Map116_ExChrSwitch<<8)+Map116_Chr1) % (S.NesHeader.VROMSize<<3));
      W.PPUBANK[ 6 ] = VROMPAGE( ((Map116_ExChrSwitch<<8)+Map116_Chr2) % (S.NesHeader.VROMSize<<3));
      W.PPUBANK[ 7 ] = VROMPAGE( ((Map116_ExChrSwitch<<8)+Map116_Chr3) % (S.NesHeader.VROMSize<<3));
      NESCore_Develop_Character_Data();
    } else {
      W.PPUBANK[ 0 ] = VROMPAGE( ((Map116_ExChrSwitch<<8)+Map116_Chr0) % (S.NesHeader.VROMSize<<3));
      W.PPUBANK[ 1 ] = VROMPAGE( ((Map116_ExChrSwitch<<8)+Map116_Chr1) % (S.NesHeader.VROMSize<<3));
      W.PPUBANK[ 2 ] = VROMPAGE( ((Map116_ExChrSwitch<<8)+Map116_Chr2) % (S.NesHeader.VROMSize<<3));
      W.PPUBANK[ 3 ] = VROMPAGE( ((Map116_ExChrSwitch<<8)+Map116_Chr3) % (S.NesHeader.VROMSize<<3));
      W.PPUBANK[ 4 ] = VROMPAGE( ((Map116_ExChrSwitch<<8)+Map116_Chr4) % (S.NesHeader.VROMSize<<3));
      W.PPUBANK[ 5 ] = VROMPAGE( ((Map116_ExChrSwitch<<8)+Map116_Chr5) % (S.NesHeader.VROMSize<<3));
      W.PPUBANK[ 6 ] = VROMPAGE( ((Map116_ExChrSwitch<<8)+Map116_Chr6) % (S.NesHeader.VROMSize<<3));
      W.PPUBANK[ 7 ] = VROMPAGE( ((Map116_ExChrSwitch<<8)+Map116_Chr7) % (S.NesHeader.VROMSize<<3));
      NESCore_Develop_Character_Data();
    }
  }
}

