/*===================================================================*/
/*                                                                   */
/*                   Mapper 135 : SACHEN CHEN                        */
/*                                                                   */
/*===================================================================*/

byte    Map135_Cmd;
byte	Map135_Chr0l, Map135_Chr1l, Map135_Chr0h, Map135_Chr1h, Map135_Chrch;

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 135                                            */
/*-------------------------------------------------------------------*/
void Map135_Init()
{
  /* Initialize Mapper */
  MapperInit = Map135_Init;

  /* Write to Mapper */
  MapperWrite = Map0_Write;

  /* Write to SRAM */
  MapperSram = Map0_Sram;

  /* Write to APU */
  MapperApu = Map135_Apu;

  /* Read from APU */
  MapperReadApu = Map0_ReadApu;

  /* Callback at VSync */
  MapperVSync = Map0_VSync;

  /* Callback at HSync */
  MapperHSync = Map0_HSync;

  /* Callback at PPU */
  MapperPPU = Map0_PPU;

  /* Callback at Rendering Screen ( 1:BG, 0:Sprite ) */
  MapperRenderScreen = Map0_RenderScreen;

  /* Set SRAM Banks */
  W.SRAMBANK = S.SRAM;

  /* Initialize Registers */
  Map135_Cmd = 0;
  Map135_Chr0l = Map135_Chr1l = Map135_Chr0h = Map135_Chr1h = Map135_Chrch = 0;

  /* Set ROM Banks */
  W.ROMBANK0 = ROMPAGE( 0 );
  W.ROMBANK1 = ROMPAGE( 1 );
  W.ROMBANK2 = ROMPAGE( 2 );
  W.ROMBANK3 = ROMPAGE( 3 );

  /* Set PPU Banks */
  Map135_Set_PPU_Banks();


 
}

/*-------------------------------------------------------------------*/
/*  Mapper 135 Write to APU Function                                 */
/*-------------------------------------------------------------------*/
void Map135_Apu( word wAddr, byte byData )
{
  switch( wAddr & 0x4101 ) {
  case	0x4100:
    Map135_Cmd = byData & 0x07;
    break;
  case	0x4101:
    switch( Map135_Cmd ) {
    case	0:
      Map135_Chr0l = byData & 0x07;
      Map135_Set_PPU_Banks();
      break;
    case	1:
      Map135_Chr0h = byData & 0x07;
      Map135_Set_PPU_Banks();
      break;
    case	2:
      Map135_Chr1l = byData & 0x07;
      Map135_Set_PPU_Banks();
      break;
    case	3:
      Map135_Chr1h = byData & 0x07;
      Map135_Set_PPU_Banks();
      break;
    case	4:
      Map135_Chrch = byData & 0x07;
      Map135_Set_PPU_Banks();
      break;
    case	5:
      /* Set ROM Banks */
      W.ROMBANK0 = ROMPAGE( (((byData%0x07)<<2) + 0 ) % (S.NesHeader.ROMSize << 1) );
      W.ROMBANK1 = ROMPAGE( (((byData%0x07)<<2) + 1 ) % (S.NesHeader.ROMSize << 1) );
      W.ROMBANK2 = ROMPAGE( (((byData%0x07)<<2) + 2 ) % (S.NesHeader.ROMSize << 1) );
      W.ROMBANK3 = ROMPAGE( (((byData%0x07)<<2) + 3 ) % (S.NesHeader.ROMSize << 1) );
      break;
    case	6:
      break;
    case	7:
      switch( (byData>>1)&0x03 ) {
      case	0: NESCore_Mirroring( 2 ); break;
      case	1: NESCore_Mirroring( 0  ); break;
      case	2: NESCore_Mirroring( 1  ); break;
      case	3: NESCore_Mirroring( 2 ); break;
      }
      break;
    }
    break;
  }
  //Map135_Wram[ wAddr & 0x1fff ] = byData;
}

/*-------------------------------------------------------------------*/
/*  Mapper 135 Set PPU Banks Function                                */
/*-------------------------------------------------------------------*/
void	Map135_Set_PPU_Banks()
{
  /* Set PPU Banks */
  W.PPUBANK[ 0 ] = VROMPAGE( (((0|(Map135_Chr0l<<1)|(Map135_Chrch<<4))<<1) + 0) % (S.NesHeader.VROMSize << 3) );
  W.PPUBANK[ 1 ] = VROMPAGE( (((0|(Map135_Chr0l<<1)|(Map135_Chrch<<4))<<1) + 1) % (S.NesHeader.VROMSize << 3) );
  W.PPUBANK[ 2 ] = VROMPAGE( (((1|(Map135_Chr0h<<1)|(Map135_Chrch<<4))<<1) + 0) % (S.NesHeader.VROMSize << 3) );
  W.PPUBANK[ 3 ] = VROMPAGE( (((1|(Map135_Chr0h<<1)|(Map135_Chrch<<4))<<1) + 1) % (S.NesHeader.VROMSize << 3) );
  W.PPUBANK[ 4 ] = VROMPAGE( (((0|(Map135_Chr1l<<1)|(Map135_Chrch<<4))<<1) + 0) % (S.NesHeader.VROMSize << 3) );
  W.PPUBANK[ 5 ] = VROMPAGE( (((0|(Map135_Chr1l<<1)|(Map135_Chrch<<4))<<1) + 1) % (S.NesHeader.VROMSize << 3) );
  W.PPUBANK[ 6 ] = VROMPAGE( (((1|(Map135_Chr1h<<1)|(Map135_Chrch<<4))<<1) + 0) % (S.NesHeader.VROMSize << 3) );
  W.PPUBANK[ 7 ] = VROMPAGE( (((1|(Map135_Chr1h<<1)|(Map135_Chrch<<4))<<1) + 1) % (S.NesHeader.VROMSize << 3) );
  NESCore_Develop_Character_Data();
}
