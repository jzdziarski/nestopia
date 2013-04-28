/*===================================================================*/
/*                                                                   */
/*              Mapper 92 (Jaleco Early Mapper #1)                   */
/*                                                                   */
/*===================================================================*/

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 92                                             */
/*-------------------------------------------------------------------*/
void Map92_Init()
{
  /* Initialize Mapper */
  MapperInit = Map92_Init;

  /* Write to Mapper */
  MapperWrite = Map92_Write;

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
/*  Mapper 92 Write Function                                         */
/*-------------------------------------------------------------------*/
void Map92_Write( word wAddr, byte byData )
{
  byte byRomBank;
  byte byChrBank;
  
  byData = (byte)wAddr & 0xff;
  byRomBank = ( byData & 0x0f ) << 1;
  byChrBank = byData & 0x0f;

  if ( wAddr >= 0x9000 )
  {
    switch ( byData & 0xf0 )
    {
      /* Set ROM Banks */
      case 0xd0:
        byRomBank %= ( S.NesHeader.ROMSize << 1 );
        W.ROMBANK2 = ROMPAGE( byRomBank );
        W.ROMBANK3 = ROMPAGE( byRomBank + 1 );     
        break;
        
      /* Set PPU Banks */
      case 0xe0:
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
        break;
    }
  } else {
    switch ( byData & 0xf0 )
    {
      /* Set ROM Banks */
      case 0xb0:
        byRomBank %= ( S.NesHeader.ROMSize << 1 );
        W.ROMBANK2 = ROMPAGE( byRomBank );
        W.ROMBANK3 = ROMPAGE( byRomBank + 1 );     
        break;

      /* Set PPU Banks */
      case 0x70:
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
        break;
    }
  }
}
