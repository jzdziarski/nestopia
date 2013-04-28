/*===================================================================*/
/*                                                                   */
/*            Mapper 194 : Meikyuu Jiin Dababa                       */
/*                                                                   */
/*===================================================================*/

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 194                                            */
/*-------------------------------------------------------------------*/
void Map194_Init()
{
  /* Initialize Mapper */
  MapperInit = Map194_Init;

  /* Write to Mapper */
  MapperWrite = Map194_Write;

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
  W.ROMBANK0 = ROMPAGE( (S.NesHeader.ROMSize<<1) - 4 );
  W.ROMBANK1 = ROMPAGE( (S.NesHeader.ROMSize<<1) - 3 );
  W.ROMBANK2 = ROMPAGE( (S.NesHeader.ROMSize<<1) - 2 );
  W.ROMBANK3 = ROMPAGE( (S.NesHeader.ROMSize<<1) - 1 );


 
}

/*-------------------------------------------------------------------*/
/*  Mapper 194 Write Function                                        */
/*-------------------------------------------------------------------*/
void Map194_Write( word wAddr, byte byData )
{
  W.SRAMBANK = ROMPAGE( byData % ( S.NesHeader.ROMSize << 1 ) );
}
