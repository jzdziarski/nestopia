/*===================================================================*/
/*                                                                   */
/*                      Mapper 93 (74161/32)                         */
/*                                                                   */
/*===================================================================*/

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 93                                             */
/*-------------------------------------------------------------------*/
void Map93_Init()
{
  /* Initialize Mapper */
  MapperInit = Map93_Init;

  /* Write to Mapper */
  MapperWrite = Map0_Write;

  /* Write to SRAM */
  MapperSram = Map93_Sram;

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
/*  Mapper 93 Write to Sram Function                                 */
/*-------------------------------------------------------------------*/
void Map93_Sram( word wAddr, byte byData )
{
  /* Set ROM Banks */
  if ( wAddr == 0x6000 )
  {
    byData <<= 1;
    byData %= ( S.NesHeader.ROMSize << 1 );
  
    W.ROMBANK0 = ROMPAGE( byData );
    W.ROMBANK1 = ROMPAGE( byData + 1 );
  }
}
