/*===================================================================*/
/*                                                                   */
/*   Mapper 79 (American Video Entertainment/Sachen Custom Mapper)   */
/*                                                                   */
/*===================================================================*/

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 79                                             */
/*-------------------------------------------------------------------*/
void Map79_Init()
{
  /* Initialize Mapper */
  MapperInit = Map79_Init;

  /* Write to Mapper */
  MapperWrite = Map0_Write;

  /* Write to SRAM */
  MapperSram = Map0_Sram;

  /* Write to APU */
  MapperApu = Map79_Apu;

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
/*  Mapper 79 Write to Apu Function                                  */
/*-------------------------------------------------------------------*/
void Map79_Apu( word wAddr, byte byData )
{
  byte byPrgBank = ( byData & 0x08 ) >> 3;
  byte byChrBank = byData & 0x07;
 
  /* Set ROM Banks */
  byPrgBank <<= 2;
  byPrgBank %= ( S.NesHeader.ROMSize << 1 );

  W.ROMBANK0 = ROMPAGE( byPrgBank + 0 );
  W.ROMBANK1 = ROMPAGE( byPrgBank + 1 );
  W.ROMBANK2 = ROMPAGE( byPrgBank + 2 );
  W.ROMBANK3 = ROMPAGE( byPrgBank + 3 );

  /* Set PPU Banks */
  byChrBank <<= 3;
  byChrBank %= ( S.NesHeader.VROMSize  << 3 );

  W.PPUBANK[ 0 ] = VROMPAGE( byChrBank + 0 );
  W.PPUBANK[ 1 ] = VROMPAGE( byChrBank + 1 );
  W.PPUBANK[ 2 ] = VROMPAGE( byChrBank + 2 );
  W.PPUBANK[ 3 ] = VROMPAGE( byChrBank + 3 );
  W.PPUBANK[ 4 ] = VROMPAGE( byChrBank + 4 );
  W.PPUBANK[ 5 ] = VROMPAGE( byChrBank + 5 );
  W.PPUBANK[ 6 ] = VROMPAGE( byChrBank + 6 );
  W.PPUBANK[ 7 ] = VROMPAGE( byChrBank + 7 );
  NESCore_Develop_Character_Data();
}
