/*===================================================================*/
/*                                                                   */
/*                          Mapper 244                               */
/*                                                                   */
/*===================================================================*/

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 244                                            */
/*-------------------------------------------------------------------*/
void Map244_Init()
{
  /* Initialize Mapper */
  MapperInit = Map244_Init;

  /* Write to Mapper */
  MapperWrite = Map244_Write;

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
/*  Mapper 244 Write Function                                        */
/*-------------------------------------------------------------------*/
void Map244_Write( word wAddr, byte byData )
{
  if( wAddr>=0x8065 && wAddr<=0x80A4 ) {
    /* Set ROM Banks */
    W.ROMBANK0 = ROMPAGE(((((wAddr-0x8065)&0x3)<<2)+0) % (S.NesHeader.ROMSize<<1));
    W.ROMBANK1 = ROMPAGE(((((wAddr-0x8065)&0x3)<<2)+1) % (S.NesHeader.ROMSize<<1));
    W.ROMBANK2 = ROMPAGE(((((wAddr-0x8065)&0x3)<<2)+2) % (S.NesHeader.ROMSize<<1));
    W.ROMBANK3 = ROMPAGE(((((wAddr-0x8065)&0x3)<<2)+3) % (S.NesHeader.ROMSize<<1));
  }
  
  if( wAddr>=0x80A5 && wAddr<=0x80E4 ) {
    /* Set ROM Banks */
    W.ROMBANK0 = ROMPAGE(((((wAddr-0x80A5)&0x7)<<2)+0) % (S.NesHeader.ROMSize<<1));
    W.ROMBANK1 = ROMPAGE(((((wAddr-0x80A5)&0x7)<<2)+1) % (S.NesHeader.ROMSize<<1));
    W.ROMBANK2 = ROMPAGE(((((wAddr-0x80A5)&0x7)<<2)+2) % (S.NesHeader.ROMSize<<1));
    W.ROMBANK3 = ROMPAGE(((((wAddr-0x80A5)&0x7)<<2)+3) % (S.NesHeader.ROMSize<<1));
  }
}
