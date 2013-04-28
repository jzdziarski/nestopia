/*===================================================================*/
/*                                                                   */
/*                      Mapper 231 : 20-in-1                         */
/*                                                                   */
/*===================================================================*/

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 231                                            */
/*-------------------------------------------------------------------*/
void Map231_Init()
{
  /* Initialize Mapper */
  MapperInit = Map231_Init;

  /* Write to Mapper */
  MapperWrite = Map231_Write;

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
/*  Mapper 231 Write Function                                        */
/*-------------------------------------------------------------------*/
void Map231_Write( word wAddr, byte byData )
{
  byte	byBank = wAddr & 0x1E;

  if( wAddr & 0x0020 ) {
    W.ROMBANK0 = ROMPAGE(((byBank<<1)+0) % (S.NesHeader.ROMSize<<1));
    W.ROMBANK1 = ROMPAGE(((byBank<<1)+1) % (S.NesHeader.ROMSize<<1));
    W.ROMBANK2 = ROMPAGE(((byBank<<1)+2) % (S.NesHeader.ROMSize<<1));
    W.ROMBANK3 = ROMPAGE(((byBank<<1)+3) % (S.NesHeader.ROMSize<<1));
  } else {
    W.ROMBANK0 = ROMPAGE(((byBank<<1)+0) % (S.NesHeader.ROMSize<<1));
    W.ROMBANK1 = ROMPAGE(((byBank<<1)+1) % (S.NesHeader.ROMSize<<1));
    W.ROMBANK2 = ROMPAGE(((byBank<<1)+0) % (S.NesHeader.ROMSize<<1));
    W.ROMBANK3 = ROMPAGE(((byBank<<1)+1) % (S.NesHeader.ROMSize<<1));
  }

  if( wAddr & 0x0080 ) {
    NESCore_Mirroring( 0 );
  } else {
    NESCore_Mirroring( 1 );
  }
}
