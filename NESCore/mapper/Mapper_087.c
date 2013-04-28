/*===================================================================*/
/*                                                                   */
/*                     Mapper 87 (74161/32)                          */
/*                                                                   */
/*===================================================================*/

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 87                                             */
/*-------------------------------------------------------------------*/
void Map87_Init()
{
  /* Initialize Mapper */
  MapperInit = Map87_Init;

  /* Write to Mapper */
  MapperWrite = Map0_Write;

  /* Write to SRAM */
  MapperSram = Map87_Sram;

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
/*  Mapper 87 Write to SRAM Function                                 */
/*-------------------------------------------------------------------*/
void Map87_Sram( word wAddr, byte byData )
{
  byte byChrBank;

  switch ( wAddr )
  {
    case 0x6000:
      byChrBank = ( byData & 0x02 ) >> 1;
      byChrBank <<= 3;
      byChrBank %= ( S.NesHeader.VROMSize << 3 );

      W.PPUBANK[ 0 ] = VROMPAGE( byChrBank + 0 );
      W.PPUBANK[ 1 ] = VROMPAGE( byChrBank + 1 );
      W.PPUBANK[ 2 ] = VROMPAGE( byChrBank + 2 );
      W.PPUBANK[ 3 ] = VROMPAGE( byChrBank + 3 );
      W.PPUBANK[ 4 ] = VROMPAGE( byChrBank + 4 );
      W.PPUBANK[ 5 ] = VROMPAGE( byChrBank + 5 );
      W.PPUBANK[ 6 ] = VROMPAGE( byChrBank + 6 );
      W.PPUBANK[ 7 ] = VROMPAGE( byChrBank + 7 );
      NESCore_Develop_Character_Data();
      break;
  }
}
