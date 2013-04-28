/*===================================================================*/
/*                                                                   */
/*   Mapper 115 : CartSaint : Yuu Yuu Hakusho Final JusticePao(?)    */
/*                                                                   */
/*===================================================================*/

byte	Map115_Reg[8];
byte	Map115_Prg0, Map115_Prg1, Map115_Prg2, Map115_Prg3;
byte	Map115_Prg0L, Map115_Prg1L;
byte	Map115_Chr0, Map115_Chr1, Map115_Chr2, Map115_Chr3;
byte    Map115_Chr4, Map115_Chr5, Map115_Chr6, Map115_Chr7;

byte	Map115_IRQ_Enable;
byte	Map115_IRQ_Counter;
byte	Map115_IRQ_Latch;

byte	Map115_ExPrgSwitch;
byte	Map115_ExChrSwitch;

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 115                                            */
/*-------------------------------------------------------------------*/
void Map115_Init()
{
  /* Initialize Mapper */
  MapperInit = Map115_Init;

  /* Write to Mapper */
  MapperWrite = Map115_Write;

  /* Write to SRAM */
  MapperSram = Map115_Sram;

  /* Write to APU */
  MapperApu = Map0_Apu;

  /* Read from APU */
  MapperReadApu = Map0_ReadApu;

  /* Callback at VSync */
  MapperVSync = Map0_VSync;

  /* Callback at HSync */
  MapperHSync = Map115_HSync;

  /* Callback at PPU */
  MapperPPU = Map0_PPU;

  /* Callback at Rendering Screen ( 1:BG, 0:Sprite ) */
  MapperRenderScreen = Map0_RenderScreen;

  /* Set SRAM Banks */
  W.SRAMBANK = S.SRAM;

  /* Initialize Registers */
  int i ;
for (i = 0; i < 8; i++ ) {
    Map115_Reg[i] = 0x00;
  }

  Map115_Prg0 = Map115_Prg0L = 0;
  Map115_Prg1 = Map115_Prg1L = 1;
  Map115_Prg2 = ( S.NesHeader.ROMSize << 1 ) - 2;
  Map115_Prg3 = ( S.NesHeader.ROMSize << 1 ) - 1;

  Map115_ExPrgSwitch = 0;
  Map115_ExChrSwitch = 0;

  /* Set ROM Banks */
  Map115_Set_CPU_Banks();

  /* Set PPU Banks */
  if ( S.NesHeader.VROMSize > 0 ) {
    Map115_Chr0 = 0;
    Map115_Chr1 = 1;
    Map115_Chr2 = 2;
    Map115_Chr3 = 3;
    Map115_Chr4 = 4;
    Map115_Chr5 = 5;
    Map115_Chr6 = 6;
    Map115_Chr7 = 7;
    Map115_Set_PPU_Banks();
  } else {
    Map115_Chr0 = Map115_Chr2 = Map115_Chr4 = Map115_Chr5 = Map115_Chr6 = Map115_Chr7 = 0;
    Map115_Chr1 = Map115_Chr3 = 1;
  }

  Map115_IRQ_Enable = 0;	/* Disable */
  Map115_IRQ_Counter = 0;
  Map115_IRQ_Latch = 0;
  

 
}

/*-------------------------------------------------------------------*/
/*  Mapper 115 Write Function                                        */
/*-------------------------------------------------------------------*/
void Map115_Write( word wAddr, byte byData )
{
  switch( wAddr & 0xE001 ) {
  case	0x8000:
    Map115_Reg[0] = byData;
    Map115_Set_CPU_Banks();
    Map115_Set_PPU_Banks();
    break;
  case	0x8001:
    Map115_Reg[1] = byData;
    switch( Map115_Reg[0] & 0x07 ) {
    case	0x00:
      Map115_Chr0 = byData & 0xFE;
      Map115_Chr1 = Map115_Chr0+1;
      Map115_Set_PPU_Banks();
      break;
    case	0x01:
      Map115_Chr2 = byData & 0xFE;
      Map115_Chr3 = Map115_Chr2+1;
      Map115_Set_PPU_Banks();
      break;
    case	0x02:
      Map115_Chr4 = byData;
      Map115_Set_PPU_Banks();
      break;
    case	0x03:
      Map115_Chr5 = byData;
      Map115_Set_PPU_Banks();
      break;
    case	0x04:
      Map115_Chr6 = byData;
      Map115_Set_PPU_Banks();
      break;
    case	0x05:
      Map115_Chr7 = byData;
      Map115_Set_PPU_Banks();
      break;
    case	0x06:
      Map115_Prg0 = Map115_Prg0L = byData;
      Map115_Set_CPU_Banks();
      break;
    case	0x07:
      Map115_Prg1 = Map115_Prg1L = byData;
      Map115_Set_CPU_Banks();
      break;
    }
    break;
  case	0xA000:
    Map115_Reg[2] = byData;
    if ( !S.ROM_FourScr ) {
      if( byData & 0x01 ) NESCore_Mirroring( 0 );
      else		  NESCore_Mirroring( 1 );
    }
    break;
  case	0xA001:
    Map115_Reg[3] = byData;
    break;
  case	0xC000:
    Map115_Reg[4] = byData;
    Map115_IRQ_Counter = byData;
    Map115_IRQ_Enable = 0xFF;
    break;
  case	0xC001:
    Map115_Reg[5] = byData;
    Map115_IRQ_Latch = byData;
    break;
  case	0xE000:
    Map115_Reg[6] = byData;
    Map115_IRQ_Enable = 0;
    break;
  case	0xE001:
    Map115_Reg[7] = byData;
    Map115_IRQ_Enable = 0xFF;
    break;
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 115 Write to SRAM Function                                */
/*-------------------------------------------------------------------*/
void Map115_Sram( word wAddr, byte byData )
{
  switch ( wAddr ) {
  case	0x6000:
    Map115_ExPrgSwitch = byData;
    Map115_Set_CPU_Banks();
    break;
  case	0x6001:
    Map115_ExChrSwitch = byData&0x1;
    Map115_Set_PPU_Banks();
    break;
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 115 H-Sync Function                                       */
/*-------------------------------------------------------------------*/
void Map115_HSync()
{
  if( ( /* S.PPU_Scanline >= 0 && */ S.PPU_Scanline <= 239) ) {
    if( S.PPU_R1 & R1_SHOW_BG || S.PPU_R1 & R1_SHOW_SP ) {
      if( Map115_IRQ_Enable ) {
	if( !(Map115_IRQ_Counter--) ) {
	  Map115_IRQ_Counter = Map115_IRQ_Latch;
	  IRQ_REQ;
	}
      }
    }
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 115 Set CPU Banks Function                                */
/*-------------------------------------------------------------------*/
void Map115_Set_CPU_Banks()
{
  if( Map115_ExPrgSwitch & 0x80 ) {
    Map115_Prg0 = ((Map115_ExPrgSwitch<<1)&0x1e);
    Map115_Prg1 = Map115_Prg0+1;

    W.ROMBANK0 = ROMPAGE( Map115_Prg0 % ( S.NesHeader.ROMSize << 1 ) );
    W.ROMBANK1 = ROMPAGE( Map115_Prg1 % ( S.NesHeader.ROMSize << 1 ) );
    W.ROMBANK2 = ROMPAGE( ( Map115_Prg0+2 ) % ( S.NesHeader.ROMSize << 1 ) );
    W.ROMBANK3 = ROMPAGE( ( Map115_Prg1+2 ) % ( S.NesHeader.ROMSize << 1 ) );
  } else {
    Map115_Prg0 = Map115_Prg0L;
    Map115_Prg1 = Map115_Prg1L;
    if( Map115_Reg[0] & 0x40 ) {
      W.ROMBANK0 = ROMPAGE( ( S.NesHeader.ROMSize << 1 ) - 2 );
      W.ROMBANK1 = ROMPAGE( Map115_Prg1 % ( S.NesHeader.ROMSize << 1 ) );
      W.ROMBANK2 = ROMPAGE( Map115_Prg0 % ( S.NesHeader.ROMSize << 1 ) );
      W.ROMBANK3 = ROMPAGE( ( S.NesHeader.ROMSize << 1 ) - 1 );
    } else {
      W.ROMBANK0 = ROMPAGE( Map115_Prg0 % ( S.NesHeader.ROMSize << 1 ) );
      W.ROMBANK1 = ROMPAGE( Map115_Prg1 % ( S.NesHeader.ROMSize << 1 ) );
      W.ROMBANK2 = ROMPAGE( ( S.NesHeader.ROMSize << 1 ) - 2 );
      W.ROMBANK3 = ROMPAGE( ( S.NesHeader.ROMSize << 1 ) - 1 );
    }
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 115 Set PPU Banks Function                                */
/*-------------------------------------------------------------------*/
void Map115_Set_PPU_Banks()
{
  if ( S.NesHeader.VROMSize > 0 ) {
    if( Map115_Reg[0] & 0x80 ) {
      W.PPUBANK[ 0 ] = VROMPAGE( ((Map115_ExChrSwitch<<8)+Map115_Chr4) % (S.NesHeader.VROMSize<<3));
      W.PPUBANK[ 1 ] = VROMPAGE( ((Map115_ExChrSwitch<<8)+Map115_Chr5) % (S.NesHeader.VROMSize<<3));
      W.PPUBANK[ 2 ] = VROMPAGE( ((Map115_ExChrSwitch<<8)+Map115_Chr6) % (S.NesHeader.VROMSize<<3));
      W.PPUBANK[ 3 ] = VROMPAGE( ((Map115_ExChrSwitch<<8)+Map115_Chr7) % (S.NesHeader.VROMSize<<3));
      W.PPUBANK[ 4 ] = VROMPAGE( ((Map115_ExChrSwitch<<8)+Map115_Chr0) % (S.NesHeader.VROMSize<<3));
      W.PPUBANK[ 5 ] = VROMPAGE( ((Map115_ExChrSwitch<<8)+Map115_Chr1) % (S.NesHeader.VROMSize<<3));
      W.PPUBANK[ 6 ] = VROMPAGE( ((Map115_ExChrSwitch<<8)+Map115_Chr2) % (S.NesHeader.VROMSize<<3));
      W.PPUBANK[ 7 ] = VROMPAGE( ((Map115_ExChrSwitch<<8)+Map115_Chr3) % (S.NesHeader.VROMSize<<3));
      NESCore_Develop_Character_Data();
    } else {
      W.PPUBANK[ 0 ] = VROMPAGE( ((Map115_ExChrSwitch<<8)+Map115_Chr0) % (S.NesHeader.VROMSize<<3));
      W.PPUBANK[ 1 ] = VROMPAGE( ((Map115_ExChrSwitch<<8)+Map115_Chr1) % (S.NesHeader.VROMSize<<3));
      W.PPUBANK[ 2 ] = VROMPAGE( ((Map115_ExChrSwitch<<8)+Map115_Chr2) % (S.NesHeader.VROMSize<<3));
      W.PPUBANK[ 3 ] = VROMPAGE( ((Map115_ExChrSwitch<<8)+Map115_Chr3) % (S.NesHeader.VROMSize<<3));
      W.PPUBANK[ 4 ] = VROMPAGE( ((Map115_ExChrSwitch<<8)+Map115_Chr4) % (S.NesHeader.VROMSize<<3));
      W.PPUBANK[ 5 ] = VROMPAGE( ((Map115_ExChrSwitch<<8)+Map115_Chr5) % (S.NesHeader.VROMSize<<3));
      W.PPUBANK[ 6 ] = VROMPAGE( ((Map115_ExChrSwitch<<8)+Map115_Chr6) % (S.NesHeader.VROMSize<<3));
      W.PPUBANK[ 7 ] = VROMPAGE( ((Map115_ExChrSwitch<<8)+Map115_Chr7) % (S.NesHeader.VROMSize<<3));
      NESCore_Develop_Character_Data();
    }
  }
}

