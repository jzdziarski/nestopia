/*===================================================================*/
/*                                                                   */
/*                  Mapper 97 (74161/32 Irem)                        */
/*                                                                   */
/*===================================================================*/

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 97                                             */
/*-------------------------------------------------------------------*/
void Map97_Init()
{
  /* Initialize Mapper */
  MapperInit = Map97_Init;

  /* Write to Mapper */
  MapperWrite = Map97_Write;

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
  W.ROMBANK0 = ROMLASTPAGE( 1 );
  W.ROMBANK1 = ROMLASTPAGE( 0 );
  W.ROMBANK2 = ROMPAGE( 0 );
  W.ROMBANK3 = ROMPAGE( 1 );

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
/*  Mapper 97 Write Function                                         */
/*-------------------------------------------------------------------*/
void Map97_Write( word wAddr, byte byData )
{
  /* Set ROM Banks */
  if ( wAddr < 0xc000 )
  {
    byte byPrgBank = byData & 0x0f;

    byPrgBank <<= 1;
    byPrgBank %= ( S.NesHeader.ROMSize << 1 );

    W.ROMBANK2 = ROMPAGE( byPrgBank );
    W.ROMBANK3 = ROMPAGE( byPrgBank + 1 );

    if ( ( byData & 0x80 ) == 0 )
    {
      NESCore_Mirroring( 0 );
    } else {
      NESCore_Mirroring( 1 );
    }
  }
}
