/*===================================================================*/
/*                                                                   */
/*                      Mapper 180  (Nichibutsu)                     */
/*                                                                   */
/*===================================================================*/

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 180                                            */
/*-------------------------------------------------------------------*/
void Map180_Init()
{
  /* Initialize Mapper */
  MapperInit = Map180_Init;

  /* Write to Mapper */
  MapperWrite = Map180_Write;

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
  if ( S.NesHeader.VROMSize > 0 )
  {
    int nPage ;
for (nPage = 0; nPage < 8; ++nPage )
      W.PPUBANK[ nPage ] = VROMPAGE( nPage );
    NESCore_Develop_Character_Data();
  }


 
}

/*-------------------------------------------------------------------*/
/*  Mapper 180 Write Function                                        */
/*-------------------------------------------------------------------*/
void Map180_Write( word wAddr, byte byData )
{
  /* Set ROM Banks */ 
  byData &= 0x07;
  byData <<= 1;
  byData %= ( S.NesHeader.ROMSize << 1 );
  W.ROMBANK2 = ROMPAGE( byData );
  W.ROMBANK3 = ROMPAGE( byData + 1 );
}
