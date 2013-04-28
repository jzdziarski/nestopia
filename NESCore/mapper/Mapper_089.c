/*===================================================================*/
/*                                                                   */
/*                        Mapper 89 (Sunsoft)                        */
/*                                                                   */
/*===================================================================*/

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 89                                             */
/*-------------------------------------------------------------------*/
void Map89_Init()
{
  /* Initialize Mapper */
  MapperInit = Map89_Init;

  /* Write to Mapper */
  MapperWrite = Map89_Write;

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
/*  Mapper 89 Write Function                                         */
/*-------------------------------------------------------------------*/
void Map89_Write( word wAddr, byte byData )
{
  if ( ( wAddr & 0xFF00 ) == 0xC000 )
  {
    byte byPrgBank = (byData & 0x70) >> 4;
    byte byChrBank = ((byData & 0x80) >> 4) | (byData & 0x07);

    /* Set ROM Banks */
    W.ROMBANK0 = ROMPAGE( (byPrgBank*2+0) % (S.NesHeader.ROMSize << 1) );
    W.ROMBANK1 = ROMPAGE( (byPrgBank*2+1) % (S.NesHeader.ROMSize << 1) );

    W.PPUBANK[ 0 ] = VROMPAGE( (byChrBank*8+0) % (S.NesHeader.VROMSize << 3) );
    W.PPUBANK[ 1 ] = VROMPAGE( (byChrBank*8+1) % (S.NesHeader.VROMSize << 3) );
    W.PPUBANK[ 2 ] = VROMPAGE( (byChrBank*8+2) % (S.NesHeader.VROMSize << 3) );
    W.PPUBANK[ 3 ] = VROMPAGE( (byChrBank*8+3) % (S.NesHeader.VROMSize << 3) );
    W.PPUBANK[ 4 ] = VROMPAGE( (byChrBank*8+4) % (S.NesHeader.VROMSize << 3) );
    W.PPUBANK[ 5 ] = VROMPAGE( (byChrBank*8+5) % (S.NesHeader.VROMSize << 3) );
    W.PPUBANK[ 6 ] = VROMPAGE( (byChrBank*8+6) % (S.NesHeader.VROMSize << 3) );
    W.PPUBANK[ 7 ] = VROMPAGE( (byChrBank*8+7) % (S.NesHeader.VROMSize << 3) );
    NESCore_Develop_Character_Data();

    if ( byData & 0x08 )
    {
      NESCore_Mirroring( 2 );
    }
    else
    {
      NESCore_Mirroring( 3 );
    }
  }
}
