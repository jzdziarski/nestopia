/*===================================================================*/
/*                                                                   */
/*                    Mapper 099  VS-Unisystem                       */
/*                                                                   */
/*===================================================================*/

byte Map99_Coin;

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 099                                            */
/*-------------------------------------------------------------------*/
void Map99_Init()
{
  /* Initialize Mapper */
  MapperInit = Map99_Init;

  /* Write to Mapper */
  MapperWrite = Map0_Write;

  /* Write to SRAM */
  MapperSram = Map0_Sram;

  /* Write to APU */
  MapperApu = Map99_Apu;

  /* Read from APU */
  MapperReadApu = Map99_ReadApu;

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
  if ( S.NesHeader.ROMSize > 1 )
  {
    W.ROMBANK0 = ROMPAGE( 0 );
    W.ROMBANK1 = ROMPAGE( 1 );
    W.ROMBANK2 = ROMPAGE( 2 );
    W.ROMBANK3 = ROMPAGE( 3 );
  }
  else if ( S.NesHeader.ROMSize > 0 )
  {
    W.ROMBANK0 = ROMPAGE( 0 );
    W.ROMBANK1 = ROMPAGE( 1 );
    W.ROMBANK2 = ROMPAGE( 0 );
    W.ROMBANK3 = ROMPAGE( 1 );
  } else {
    W.ROMBANK0 = ROMPAGE( 0 );
    W.ROMBANK1 = ROMPAGE( 0 );
    W.ROMBANK2 = ROMPAGE( 0 );
    W.ROMBANK3 = ROMPAGE( 0 );
  }

  /* Set PPU Banks */
  if ( S.NesHeader.VROMSize > 0 )
  {
    int nPage ;
for (nPage = 0; nPage < 8; ++nPage )
      W.PPUBANK[ nPage ] = VROMPAGE( nPage );
    NESCore_Develop_Character_Data();
  }

  Map99_Coin = 0;


 
}

/*-------------------------------------------------------------------*/
/*  Mapper 99 Write to APU Function                                  */
/*-------------------------------------------------------------------*/
void Map99_Apu( word wAddr, byte byData )
{
  if( wAddr == 0x4016 ) {
    if( byData & 0x04 ) {
      W.PPUBANK[ 0 ] = VROMPAGE(  8 % ( S.NesHeader.VROMSize << 3 ) );
      W.PPUBANK[ 1 ] = VROMPAGE(  9 % ( S.NesHeader.VROMSize << 3 ) );
      W.PPUBANK[ 2 ] = VROMPAGE( 10 % ( S.NesHeader.VROMSize << 3 ) );
      W.PPUBANK[ 3 ] = VROMPAGE( 11 % ( S.NesHeader.VROMSize << 3 ) );
      W.PPUBANK[ 4 ] = VROMPAGE( 12 % ( S.NesHeader.VROMSize << 3 ) );
      W.PPUBANK[ 5 ] = VROMPAGE( 13 % ( S.NesHeader.VROMSize << 3 ) );
      W.PPUBANK[ 6 ] = VROMPAGE( 14 % ( S.NesHeader.VROMSize << 3 ) );
      W.PPUBANK[ 7 ] = VROMPAGE( 15 % ( S.NesHeader.VROMSize << 3 ) );
      NESCore_Develop_Character_Data();
    } else {
      W.PPUBANK[ 0 ] = VROMPAGE( 0 % ( S.NesHeader.VROMSize << 3 ) );
      W.PPUBANK[ 1 ] = VROMPAGE( 1 % ( S.NesHeader.VROMSize << 3 ) );
      W.PPUBANK[ 2 ] = VROMPAGE( 2 % ( S.NesHeader.VROMSize << 3 ) );
      W.PPUBANK[ 3 ] = VROMPAGE( 3 % ( S.NesHeader.VROMSize << 3 ) );
      W.PPUBANK[ 4 ] = VROMPAGE( 4 % ( S.NesHeader.VROMSize << 3 ) );
      W.PPUBANK[ 5 ] = VROMPAGE( 5 % ( S.NesHeader.VROMSize << 3 ) );
      W.PPUBANK[ 6 ] = VROMPAGE( 6 % ( S.NesHeader.VROMSize << 3 ) );
      W.PPUBANK[ 7 ] = VROMPAGE( 7 % ( S.NesHeader.VROMSize << 3 ) );
      NESCore_Develop_Character_Data();
    }
  }

  if( wAddr == 0x4020 ) {
    Map99_Coin = byData;
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 99 Read from APU Function                                 */
/*-------------------------------------------------------------------*/
byte Map99_ReadApu( word wAddr )
{
  if( wAddr == 0x4020 ) {
    return Map99_Coin;
  }
  return ( wAddr >> 8 );
}
