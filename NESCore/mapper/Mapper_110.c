/*===================================================================*/
/*                                                                   */
/*                          Mapper 110                               */
/*                                                                   */
/*===================================================================*/

byte	Map110_Reg0, Map110_Reg1;

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 110                                            */
/*-------------------------------------------------------------------*/
void Map110_Init()
{
  /* Initialize Mapper */
  MapperInit = Map110_Init;

  /* Write to Mapper */
  MapperWrite = Map0_Write;

  /* Write to SRAM */
  MapperSram = Map0_Sram;

  /* Write to APU */
  MapperApu = Map110_Apu;

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

  /* Set ROM Banks */
  W.ROMBANK0 = ROMPAGE( 0 );
  W.ROMBANK1 = ROMPAGE( 1 );
  W.ROMBANK2 = ROMPAGE( 2 );
  W.ROMBANK3 = ROMPAGE( 3 );

  /* Set PPU Banks */
  if ( S.NesHeader.VROMSize > 0 ) {
    int nPage ;
for (nPage = 0; nPage < 8; ++nPage )
      W.PPUBANK[ nPage ] = VROMPAGE( nPage );
    NESCore_Develop_Character_Data();
  }

  /* Initialize Registers */
  Map110_Reg0 = 0;
  Map110_Reg1 = 0;


 
}

/*-------------------------------------------------------------------*/
/*  Mapper 110 Write to APU Function                                 */
/*-------------------------------------------------------------------*/
void Map110_Apu( word wAddr, byte byData )
{
  switch( wAddr ) {
  case	0x4100:
    Map110_Reg1 = byData & 0x07;
    break;
  case	0x4101:
    switch( Map110_Reg1 ) {
    case	5:
      W.ROMBANK0 = ROMPAGE( ((byData << 2) + 0) % ( S.NesHeader.ROMSize << 1 ) );
      W.ROMBANK1 = ROMPAGE( ((byData << 2) + 1) % ( S.NesHeader.ROMSize << 1 ) );
      W.ROMBANK2 = ROMPAGE( ((byData << 2) + 2) % ( S.NesHeader.ROMSize << 1 ) );
      W.ROMBANK3 = ROMPAGE( ((byData << 2) + 3) % ( S.NesHeader.ROMSize << 1 ) );
      break;

    case	0:
      Map110_Reg0 = byData & 0x01;
      W.PPUBANK[ 0 ] = VROMPAGE(((Map110_Reg0 << 3) + 0) % (S.NesHeader.VROMSize<<3));
      W.PPUBANK[ 1 ] = VROMPAGE(((Map110_Reg0 << 3) + 1) % (S.NesHeader.VROMSize<<3));
      W.PPUBANK[ 2 ] = VROMPAGE(((Map110_Reg0 << 3) + 2) % (S.NesHeader.VROMSize<<3));
      W.PPUBANK[ 3 ] = VROMPAGE(((Map110_Reg0 << 3) + 3) % (S.NesHeader.VROMSize<<3));
      W.PPUBANK[ 4 ] = VROMPAGE(((Map110_Reg0 << 3) + 4) % (S.NesHeader.VROMSize<<3));
      W.PPUBANK[ 5 ] = VROMPAGE(((Map110_Reg0 << 3) + 5) % (S.NesHeader.VROMSize<<3));
      W.PPUBANK[ 6 ] = VROMPAGE(((Map110_Reg0 << 3) + 6) % (S.NesHeader.VROMSize<<3));
      W.PPUBANK[ 7 ] = VROMPAGE(((Map110_Reg0 << 3) + 7) % (S.NesHeader.VROMSize<<3));
      NESCore_Develop_Character_Data();
      break;

    case	2:
      Map110_Reg0 = byData;
      W.PPUBANK[ 0 ] = VROMPAGE(((Map110_Reg0 << 3) + 0) % (S.NesHeader.VROMSize<<3));
      W.PPUBANK[ 1 ] = VROMPAGE(((Map110_Reg0 << 3) + 1) % (S.NesHeader.VROMSize<<3));
      W.PPUBANK[ 2 ] = VROMPAGE(((Map110_Reg0 << 3) + 2) % (S.NesHeader.VROMSize<<3));
      W.PPUBANK[ 3 ] = VROMPAGE(((Map110_Reg0 << 3) + 3) % (S.NesHeader.VROMSize<<3));
      W.PPUBANK[ 4 ] = VROMPAGE(((Map110_Reg0 << 3) + 4) % (S.NesHeader.VROMSize<<3));
      W.PPUBANK[ 5 ] = VROMPAGE(((Map110_Reg0 << 3) + 5) % (S.NesHeader.VROMSize<<3));
      W.PPUBANK[ 6 ] = VROMPAGE(((Map110_Reg0 << 3) + 6) % (S.NesHeader.VROMSize<<3));
      W.PPUBANK[ 7 ] = VROMPAGE(((Map110_Reg0 << 3) + 7) % (S.NesHeader.VROMSize<<3));
      NESCore_Develop_Character_Data();
      break;

    case	4:
      Map110_Reg0 = Map110_Reg0 | (byData<<1);
      W.PPUBANK[ 0 ] = VROMPAGE(((Map110_Reg0 << 3) + 0) % (S.NesHeader.VROMSize<<3));
      W.PPUBANK[ 1 ] = VROMPAGE(((Map110_Reg0 << 3) + 1) % (S.NesHeader.VROMSize<<3));
      W.PPUBANK[ 2 ] = VROMPAGE(((Map110_Reg0 << 3) + 2) % (S.NesHeader.VROMSize<<3));
      W.PPUBANK[ 3 ] = VROMPAGE(((Map110_Reg0 << 3) + 3) % (S.NesHeader.VROMSize<<3));
      W.PPUBANK[ 4 ] = VROMPAGE(((Map110_Reg0 << 3) + 4) % (S.NesHeader.VROMSize<<3));
      W.PPUBANK[ 5 ] = VROMPAGE(((Map110_Reg0 << 3) + 5) % (S.NesHeader.VROMSize<<3));
      W.PPUBANK[ 6 ] = VROMPAGE(((Map110_Reg0 << 3) + 6) % (S.NesHeader.VROMSize<<3));
      W.PPUBANK[ 7 ] = VROMPAGE(((Map110_Reg0 << 3) + 7) % (S.NesHeader.VROMSize<<3));
      NESCore_Develop_Character_Data();
      break;

    case	6:
      Map110_Reg0 = Map110_Reg0 | (byData<<2);
      W.PPUBANK[ 0 ] = VROMPAGE(((Map110_Reg0 << 3) + 0) % (S.NesHeader.VROMSize<<3));
      W.PPUBANK[ 1 ] = VROMPAGE(((Map110_Reg0 << 3) + 1) % (S.NesHeader.VROMSize<<3));
      W.PPUBANK[ 2 ] = VROMPAGE(((Map110_Reg0 << 3) + 2) % (S.NesHeader.VROMSize<<3));
      W.PPUBANK[ 3 ] = VROMPAGE(((Map110_Reg0 << 3) + 3) % (S.NesHeader.VROMSize<<3));
      W.PPUBANK[ 4 ] = VROMPAGE(((Map110_Reg0 << 3) + 4) % (S.NesHeader.VROMSize<<3));
      W.PPUBANK[ 5 ] = VROMPAGE(((Map110_Reg0 << 3) + 5) % (S.NesHeader.VROMSize<<3));
      W.PPUBANK[ 6 ] = VROMPAGE(((Map110_Reg0 << 3) + 6) % (S.NesHeader.VROMSize<<3));
      W.PPUBANK[ 7 ] = VROMPAGE(((Map110_Reg0 << 3) + 7) % (S.NesHeader.VROMSize<<3));
      NESCore_Develop_Character_Data();
      break;

    default:
      break;
    }
    break;
  default:
    break;
  }
}
