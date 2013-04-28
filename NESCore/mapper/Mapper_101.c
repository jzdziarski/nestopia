/*===================================================================*/
/*                                                                   */
/*                   Mapper 101 ()                                   */
/*                                                                   */
/*===================================================================*/

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 101                                            */
/*-------------------------------------------------------------------*/
void Map101_Init()
{
  /* Initialize Mapper */
  MapperInit = Map101_Init;

  /* Write to Mapper */
  MapperWrite = Map101_Write;

  /* Write to SRAM */
  MapperSram = Map101_Write;

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
/*  Mapper 101 Write & Write to SRAM Function                        */
/*-------------------------------------------------------------------*/
void Map101_Write( word wAddr, byte byData )
{
  byData &= 0x03;
  byData <<= 3;
  byData %= ( S.NesHeader.VROMSize << 3 );

  /* Set PPU Banks */
  W.PPUBANK[ 0 ] = VROMPAGE( byData );
  W.PPUBANK[ 1 ] = VROMPAGE( byData + 1 );
  W.PPUBANK[ 2 ] = VROMPAGE( byData + 2 );
  W.PPUBANK[ 3 ] = VROMPAGE( byData + 3 );
  W.PPUBANK[ 4 ] = VROMPAGE( byData + 4 );
  W.PPUBANK[ 5 ] = VROMPAGE( byData + 5 );
  W.PPUBANK[ 6 ] = VROMPAGE( byData + 6 );
  W.PPUBANK[ 7 ] = VROMPAGE( byData + 7 );
  NESCore_Develop_Character_Data();
}
