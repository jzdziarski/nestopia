/*===================================================================*/
/*                                                                   */
/*                Mapper 242 : Wai Xing Zhan Shi                     */
/*                                                                   */
/*===================================================================*/

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 242                                            */
/*-------------------------------------------------------------------*/
void Map242_Init()
{
  /* Initialize Mapper */
  MapperInit = Map242_Init;

  /* Write to Mapper */
  MapperWrite = Map242_Write;

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
/*  Mapper 242 Write Function                                        */
/*-------------------------------------------------------------------*/
void Map242_Write( word wAddr, byte byData )
{
  if( wAddr & 0x01 ) {
    /* Set ROM Banks */
    W.ROMBANK0 = ROMPAGE((((wAddr&0xF8)>>1)+0) % (S.NesHeader.ROMSize<<1));
    W.ROMBANK1 = ROMPAGE((((wAddr&0xF8)>>1)+1) % (S.NesHeader.ROMSize<<1));
    W.ROMBANK2 = ROMPAGE((((wAddr&0xF8)>>1)+2) % (S.NesHeader.ROMSize<<1));
    W.ROMBANK3 = ROMPAGE((((wAddr&0xF8)>>1)+3) % (S.NesHeader.ROMSize<<1));
  }
}
