/*===================================================================*/
/*                                                                   */
/*                      Mapper 122 (Sunsoft)                         */
/*                                                                   */
/*===================================================================*/

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 122                                            */
/*-------------------------------------------------------------------*/
void Map122_Init()
{
  /* Initialize Mapper */
  MapperInit = Map122_Init;

  /* Write to Mapper */
  MapperWrite = Map0_Write;

  /* Write to SRAM */
  MapperSram = Map122_Sram;

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



}

/*-------------------------------------------------------------------*/
/*  Mapper 122 Write to Sram Function                                */
/*-------------------------------------------------------------------*/
void Map122_Sram( word wAddr, byte byData )
{
  if ( wAddr == 0x6000 )
  {
    byte byChrBank0 = byData & 0x07;
    byte byChrBank1 = ( byData & 0x70 ) >> 4;

    byChrBank0 = ( byChrBank0 << 2 ) % ( S.NesHeader.VROMSize << 3 );
    byChrBank1 = ( byChrBank1 << 2 ) % ( S.NesHeader.VROMSize << 3 );

    W.PPUBANK[ 0 ] = VROMPAGE( byChrBank0 + 0 );
    W.PPUBANK[ 1 ] = VROMPAGE( byChrBank0 + 1 );
    W.PPUBANK[ 2 ] = VROMPAGE( byChrBank0 + 2 );
    W.PPUBANK[ 3 ] = VROMPAGE( byChrBank0 + 3 );
    W.PPUBANK[ 4 ] = VROMPAGE( byChrBank1 + 0 );
    W.PPUBANK[ 5 ] = VROMPAGE( byChrBank1 + 1 );
    W.PPUBANK[ 6 ] = VROMPAGE( byChrBank1 + 2 );
    W.PPUBANK[ 7 ] = VROMPAGE( byChrBank1 + 3 );
    NESCore_Develop_Character_Data();
  }
}
