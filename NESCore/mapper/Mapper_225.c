/*===================================================================*/
/*                                                                   */
/*                   Mapper 225 : 72-in-1                            */
/*                                                                   */
/*===================================================================*/

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 225                                            */
/*-------------------------------------------------------------------*/
void Map225_Init()
{
  /* Initialize Mapper */
  MapperInit = Map225_Init;

  /* Write to Mapper */
  MapperWrite = Map225_Write;

  /* Write to SRAM */
  MapperSram = Map0_Sram;

  /* Write to APU */
  MapperApu = Map0_Apu;

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


 
}

/*-------------------------------------------------------------------*/
/*  Mapper 225 Write Function                                        */
/*-------------------------------------------------------------------*/
void Map225_Write( word wAddr, byte byData )
{
  byte byPrgBank = (wAddr & 0x0F80) >> 7;
  byte byChrBank = wAddr & 0x003F;
  
  W.PPUBANK[ 0 ] = VROMPAGE(((byChrBank<<3)+0) % ( S.NesHeader.VROMSize << 3 ) );
  W.PPUBANK[ 1 ] = VROMPAGE(((byChrBank<<3)+1) % ( S.NesHeader.VROMSize << 3 ) );
  W.PPUBANK[ 2 ] = VROMPAGE(((byChrBank<<3)+2) % ( S.NesHeader.VROMSize << 3 ) );
  W.PPUBANK[ 3 ] = VROMPAGE(((byChrBank<<3)+3) % ( S.NesHeader.VROMSize << 3 ) );
  W.PPUBANK[ 4 ] = VROMPAGE(((byChrBank<<3)+4) % ( S.NesHeader.VROMSize << 3 ) );
  W.PPUBANK[ 5 ] = VROMPAGE(((byChrBank<<3)+5) % ( S.NesHeader.VROMSize << 3 ) );
  W.PPUBANK[ 6 ] = VROMPAGE(((byChrBank<<3)+6) % ( S.NesHeader.VROMSize << 3 ) );
  W.PPUBANK[ 7 ] = VROMPAGE(((byChrBank<<3)+7) % ( S.NesHeader.VROMSize << 3 ) );
  NESCore_Develop_Character_Data();

  if( wAddr & 0x2000 ) {
    NESCore_Mirroring( 0 );
  } else {
    NESCore_Mirroring( 1 );
  }
  
  if( wAddr & 0x1000 ) {
    // 16KBbank
    if( wAddr & 0x0040 ) {
      W.ROMBANK0 = ROMPAGE(((byPrgBank<<2)+2) % ( S.NesHeader.ROMSize << 1 ) );
      W.ROMBANK1 = ROMPAGE(((byPrgBank<<2)+3) % ( S.NesHeader.ROMSize << 1 ) );
      W.ROMBANK2 = ROMPAGE(((byPrgBank<<2)+2) % ( S.NesHeader.ROMSize << 1 ) );
      W.ROMBANK3 = ROMPAGE(((byPrgBank<<2)+3) % ( S.NesHeader.ROMSize << 1 ) );
    } else {
      W.ROMBANK0 = ROMPAGE(((byPrgBank<<2)+0) % ( S.NesHeader.ROMSize << 1 ) );
      W.ROMBANK1 = ROMPAGE(((byPrgBank<<2)+1) % ( S.NesHeader.ROMSize << 1 ) );
      W.ROMBANK2 = ROMPAGE(((byPrgBank<<2)+0) % ( S.NesHeader.ROMSize << 1 ) );
      W.ROMBANK3 = ROMPAGE(((byPrgBank<<2)+1) % ( S.NesHeader.ROMSize << 1 ) );
    }
  } else {
    W.ROMBANK0 = ROMPAGE(((byPrgBank<<2)+0) % ( S.NesHeader.ROMSize << 1 ) );
    W.ROMBANK1 = ROMPAGE(((byPrgBank<<2)+1) % ( S.NesHeader.ROMSize << 1 ) );
    W.ROMBANK2 = ROMPAGE(((byPrgBank<<2)+2) % ( S.NesHeader.ROMSize << 1 ) );
    W.ROMBANK3 = ROMPAGE(((byPrgBank<<2)+3) % ( S.NesHeader.ROMSize << 1 ) );
  }
}
