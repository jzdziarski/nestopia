/*===================================================================*/
/*                                                                   */
/*                     Mapper 229 : 31-in-1                          */
/*                                                                   */
/*===================================================================*/

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 229                                            */
/*-------------------------------------------------------------------*/
void Map229_Init()
{
  /* Initialize Mapper */
  MapperInit = Map229_Init;

  /* Write to Mapper */
  MapperWrite = Map229_Write;

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
/*  Mapper 229 Write Function                                        */
/*-------------------------------------------------------------------*/
void Map229_Write( word wAddr, byte byData )
{
  if( wAddr & 0x001E ) {
    byte byPrg = wAddr&0x001F;

    W.ROMBANK0 = ROMPAGE(((byPrg<<1)+0) % (S.NesHeader.ROMSize<<1));
    W.ROMBANK1 = ROMPAGE(((byPrg<<1)+1) % (S.NesHeader.ROMSize<<1));
    W.ROMBANK2 = ROMPAGE(((byPrg<<1)+0) % (S.NesHeader.ROMSize<<1));
    W.ROMBANK3 = ROMPAGE(((byPrg<<1)+1) % (S.NesHeader.ROMSize<<1));

    byte byChr = wAddr & 0x0FFF;

    W.PPUBANK[ 0 ] = VROMPAGE(((byChr<<3) + 0) % (S.NesHeader.VROMSize<<3));
    W.PPUBANK[ 1 ] = VROMPAGE(((byChr<<3) + 1) % (S.NesHeader.VROMSize<<3));
    W.PPUBANK[ 2 ] = VROMPAGE(((byChr<<3) + 2) % (S.NesHeader.VROMSize<<3));
    W.PPUBANK[ 3 ] = VROMPAGE(((byChr<<3) + 3) % (S.NesHeader.VROMSize<<3));
    W.PPUBANK[ 4 ] = VROMPAGE(((byChr<<3) + 4) % (S.NesHeader.VROMSize<<3));
    W.PPUBANK[ 5 ] = VROMPAGE(((byChr<<3) + 5) % (S.NesHeader.VROMSize<<3));
    W.PPUBANK[ 6 ] = VROMPAGE(((byChr<<3) + 6) % (S.NesHeader.VROMSize<<3));
    W.PPUBANK[ 7 ] = VROMPAGE(((byChr<<3) + 7) % (S.NesHeader.VROMSize<<3));
    NESCore_Develop_Character_Data();
  } else {
    W.ROMBANK0 = ROMPAGE( 0 );
    W.ROMBANK1 = ROMPAGE( 1 );
    W.ROMBANK2 = ROMPAGE( 2 );
    W.ROMBANK3 = ROMPAGE( 3 );

    W.PPUBANK[ 0 ] = VROMPAGE( 0 );
    W.PPUBANK[ 1 ] = VROMPAGE( 1 );
    W.PPUBANK[ 2 ] = VROMPAGE( 2 );
    W.PPUBANK[ 3 ] = VROMPAGE( 3 );
    W.PPUBANK[ 4 ] = VROMPAGE( 4 );
    W.PPUBANK[ 5 ] = VROMPAGE( 5 );
    W.PPUBANK[ 6 ] = VROMPAGE( 6 );
    W.PPUBANK[ 7 ] = VROMPAGE( 7 );
    NESCore_Develop_Character_Data();
  }

  if( wAddr & 0x0020 ) {
    NESCore_Mirroring( 0 );
  } else {
    NESCore_Mirroring( 1 );
  }
}
