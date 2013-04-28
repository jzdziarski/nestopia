/*===================================================================*/
/*                                                                   */
/*                      Mapper 78 (74161/32 Irem)                    */
/*                                                                   */
/*===================================================================*/

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 78                                             */
/*-------------------------------------------------------------------*/
void Map78_Init()
{
  /* Initialize Mapper */
  MapperInit = Map78_Init;

  /* Write to Mapper */
  MapperWrite = Map78_Write;

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
  W.ROMBANK2 = ROMLASTPAGE( 1 );
  W.ROMBANK3 = ROMLASTPAGE( 0 );

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
/*  Mapper 78 Write Function                                         */
/*-------------------------------------------------------------------*/
void Map78_Write( word wAddr, byte byData )
{
  byte byPrgBank = byData & 0x0f;
  byte byChrBank = ( byData & 0xf0 ) >> 4;

  /* Set ROM Banks */
  byPrgBank <<= 1;
  byPrgBank %= ( S.NesHeader.ROMSize << 1 );
  W.ROMBANK0 = ROMPAGE( byPrgBank );
  W.ROMBANK1 = ROMPAGE( byPrgBank + 1);

  /* Set PPU Banks */
  byChrBank <<= 3;
  byChrBank %= ( S.NesHeader.VROMSize << 3 );
  W.PPUBANK[ 0 ] = VROMPAGE( byChrBank );  
  W.PPUBANK[ 1 ] = VROMPAGE( byChrBank + 1 ); 
  W.PPUBANK[ 2 ] = VROMPAGE( byChrBank + 2 ); 
  W.PPUBANK[ 3 ] = VROMPAGE( byChrBank + 3 );
  W.PPUBANK[ 4 ] = VROMPAGE( byChrBank + 4 ); 
  W.PPUBANK[ 5 ] = VROMPAGE( byChrBank + 5 ); 
  W.PPUBANK[ 6 ] = VROMPAGE( byChrBank + 6 ); 
  W.PPUBANK[ 7 ] = VROMPAGE( byChrBank + 7 ); 
  NESCore_Develop_Character_Data();  

  /* Set Name Table Mirroring */
  if ( ( wAddr & 0xfe00 ) != 0xfe00 )
  {
    if ( byData & 0x08 )
    {
      NESCore_Mirroring( 2 );
    } else {
      NESCore_Mirroring( 3 );
    }
  }
}
