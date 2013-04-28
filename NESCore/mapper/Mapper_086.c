/*===================================================================*/
/*                                                                   */
/*                        Mapper 86 (Jaleco)                         */
/*                                                                   */
/*===================================================================*/

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 86                                             */
/*-------------------------------------------------------------------*/
void Map86_Init()
{
  /* Initialize Mapper */
  MapperInit = Map86_Init;

  /* Write to Mapper */
  MapperWrite = Map0_Write;

  /* Write to SRAM */
  MapperSram = Map86_Sram;

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
/*  Mapper 86 Write to Sram Function                                 */
/*-------------------------------------------------------------------*/
void Map86_Sram( word wAddr, byte byData )
{
  byte byChrBank;
  byte byPrgBank;

  switch ( wAddr )
  {
    case 0x6000:
      byChrBank = (byData & 0x03) | (( byData & 0x40 ) >> 4);
      byPrgBank = ( byData & 0x30 ) >> 4;

      byPrgBank = ( byPrgBank << 2 ) % ( S.NesHeader.ROMSize << 1 );
      byChrBank = ( byChrBank << 3 ) % ( S.NesHeader.VROMSize << 3 );

      /* Set ROM Banks */
      W.ROMBANK0 = ROMPAGE( byPrgBank + 0 );
      W.ROMBANK1 = ROMPAGE( byPrgBank + 1 );
      W.ROMBANK2 = ROMPAGE( byPrgBank + 2 );
      W.ROMBANK3 = ROMPAGE( byPrgBank + 3 );

      /* Set PPU Banks */
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
