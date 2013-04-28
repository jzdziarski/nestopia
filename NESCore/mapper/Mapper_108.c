/*===================================================================*/
/*                                                                   */
/*                            Mapper 108                             */
/*                                                                   */
/*===================================================================*/

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 108                                            */
/*-------------------------------------------------------------------*/
void Map108_Init()
{
  /* Initialize Mapper */
  MapperInit = Map108_Init;

  /* Write to Mapper */
  MapperWrite = Map108_Write;

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
  W.SRAMBANK = ROMPAGE( 0 );

  /* Set ROM Banks */
  W.ROMBANK0 = ROMPAGE( 0xC % ( S.NesHeader.ROMSize << 1 ) );
  W.ROMBANK1 = ROMPAGE( 0xD % ( S.NesHeader.ROMSize << 1 ) );
  W.ROMBANK2 = ROMPAGE( 0xE % ( S.NesHeader.ROMSize << 1 ) );
  W.ROMBANK3 = ROMPAGE( 0xF % ( S.NesHeader.ROMSize << 1 ) );


 
}

/*-------------------------------------------------------------------*/
/*  Mapper 108 Write Function                                        */
/*-------------------------------------------------------------------*/
void Map108_Write( word wAddr, byte byData )
{
  /* Set SRAM Banks */
  W.SRAMBANK = ROMPAGE( byData % ( S.NesHeader.ROMSize << 1 ) );
}
