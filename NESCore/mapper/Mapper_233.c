/*===================================================================*/
/*                                                                   */
/*                     Mapper 233 : 42-in-1                          */
/*                                                                   */
/*===================================================================*/

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 233                                            */
/*-------------------------------------------------------------------*/
void Map233_Init()
{
  /* Initialize Mapper */
  MapperInit = Map233_Init;

  /* Write to Mapper */
  MapperWrite = Map233_Write;

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
  W.ROMBANK2 = ROMPAGE( 2 );
  W.ROMBANK3 = ROMPAGE( 3 );


 
}

/*-------------------------------------------------------------------*/
/*  Mapper 233 Write Function                                        */
/*-------------------------------------------------------------------*/
void Map233_Write( word wAddr, byte byData )
{
  if( byData & 0x20 ) {
    W.ROMBANK0 = ROMPAGE((((byData&0x1F)<<1)+0) % (S.NesHeader.ROMSize<<1));
    W.ROMBANK1 = ROMPAGE((((byData&0x1F)<<1)+1) % (S.NesHeader.ROMSize<<1)); 
    W.ROMBANK2 = ROMPAGE((((byData&0x1F)<<1)+0) % (S.NesHeader.ROMSize<<1)); 
    W.ROMBANK3 = ROMPAGE((((byData&0x1F)<<1)+1) % (S.NesHeader.ROMSize<<1)); 
  } else {
    W.ROMBANK0 = ROMPAGE((((byData&0x1E)<<1)+0) % (S.NesHeader.ROMSize<<1)); 
    W.ROMBANK1 = ROMPAGE((((byData&0x1E)<<1)+1) % (S.NesHeader.ROMSize<<1)); 
    W.ROMBANK2 = ROMPAGE((((byData&0x1E)<<1)+2) % (S.NesHeader.ROMSize<<1));
    W.ROMBANK3 = ROMPAGE((((byData&0x1E)<<1)+3) % (S.NesHeader.ROMSize<<1)); 
  }

  if( (byData&0xC0) == 0x00 ) {
    NESCore_Mirroring( 5 );
  } else if( (byData&0xC0) == 0x40 ) {
    NESCore_Mirroring( 1 );
  } else if( (byData&0xC0) == 0x80 ) {
    NESCore_Mirroring( 0 );
  } else {
    NESCore_Mirroring( 2 );
  }
}
