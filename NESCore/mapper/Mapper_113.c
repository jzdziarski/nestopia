/*===================================================================*/
/*                                                                   */
/*                    Mapper 113 (PC-Sachen/Hacker)                  */
/*                                                                   */
/*===================================================================*/

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 113                                            */
/*-------------------------------------------------------------------*/
void Map113_Init()
{
  /* Initialize Mapper */
  MapperInit = Map113_Init;

  /* Write to Mapper */
  MapperWrite = Map113_Write;

  /* Write to SRAM */
  MapperSram = Map0_Sram;

  /* Write to APU */
  MapperApu = Map113_Apu;

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
/*  Mapper 113 Write to Apu Function                                 */
/*-------------------------------------------------------------------*/
void Map113_Apu( word wAddr, byte byData )
{
  byte byPrgBank, byChrBank;

  switch ( wAddr )
  {
    case 0x4100:
    case 0x4111:
    case 0x4120:
    case 0x4900:
      byPrgBank = byData >> 3;

      if ( ( S.NesHeader.ROMSize << 1 ) <= 8 && ( S.NesHeader.VROMSize << 3 ) == 128 )
      {
        byChrBank = ( ( byData >> 3 ) & 0x08 ) + ( byData & 0x07 );
      } else {
        byChrBank = byData & 0x07;
      }

      /* Set ROM Banks */
      byPrgBank = ( byPrgBank << 2 ) % ( S.NesHeader.ROMSize << 1 );

      W.ROMBANK0 = ROMPAGE( byPrgBank + 0 );
      W.ROMBANK1 = ROMPAGE( byPrgBank + 1 );
      W.ROMBANK2 = ROMPAGE( byPrgBank + 2 );
      W.ROMBANK3 = ROMPAGE( byPrgBank + 3 );

      /* Set PPU Banks */
      byChrBank = ( byChrBank << 3 ) % ( S.NesHeader.VROMSize << 3 );

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

/*-------------------------------------------------------------------*/
/*  Mapper 113 Write Function                                        */
/*-------------------------------------------------------------------*/
void Map113_Write( word wAddr, byte byData )
{
  byte byPrgBank, byChrBank;

  switch ( wAddr )
  {
    case 0x8008:
    case 0x8009:
      byPrgBank = byData >> 3;

      if ( ( S.NesHeader.ROMSize << 1 ) <= 8 && ( S.NesHeader.VROMSize << 3 ) == 128 )
      {
        byChrBank = ( ( byData >> 3 ) & 0x08 ) + ( byData & 0x07 );
      } else {
        byChrBank = byData & 0x07;
      }

      /* Set ROM Banks */
      byPrgBank = ( byPrgBank << 2 ) % ( S.NesHeader.ROMSize << 1 );

      W.ROMBANK0 = ROMPAGE( byPrgBank + 0 );
      W.ROMBANK1 = ROMPAGE( byPrgBank + 1 );
      W.ROMBANK2 = ROMPAGE( byPrgBank + 2 );
      W.ROMBANK3 = ROMPAGE( byPrgBank + 3 );

      /* Set PPU Banks */
      byChrBank = ( byChrBank << 3 ) % ( S.NesHeader.VROMSize << 3 );

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
