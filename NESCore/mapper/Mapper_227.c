/*===================================================================*/
/*                                                                   */
/*                    Mapper 227 : 1200-in-1                         */
/*                                                                   */
/*===================================================================*/

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 227                                            */
/*-------------------------------------------------------------------*/
void Map227_Init()
{
  /* Initialize Mapper */
  MapperInit = Map227_Init;

  /* Write to Mapper */
  MapperWrite = Map227_Write;

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
  W.ROMBANK2 = ROMPAGE( 0 );
  W.ROMBANK3 = ROMPAGE( 1 );


 
}

/*-------------------------------------------------------------------*/
/*  Mapper 227 Write Function                                        */
/*-------------------------------------------------------------------*/
void Map227_Write( word wAddr, byte byData )
{
  byte	byBank = ((wAddr&0x0100)>>4)|((wAddr&0x0078)>>3);

  if( wAddr & 0x0001 ) {
    W.ROMBANK0 = ROMPAGE(((byBank<<2)+0) % (S.NesHeader.ROMSize<<1));
    W.ROMBANK1 = ROMPAGE(((byBank<<2)+1) % (S.NesHeader.ROMSize<<1));
    W.ROMBANK2 = ROMPAGE(((byBank<<2)+2) % (S.NesHeader.ROMSize<<1));
    W.ROMBANK3 = ROMPAGE(((byBank<<2)+3) % (S.NesHeader.ROMSize<<1));
  } else {
    if( wAddr & 0x0004 ) {
      W.ROMBANK0 = ROMPAGE(((byBank<<2)+2) % (S.NesHeader.ROMSize<<1));
      W.ROMBANK1 = ROMPAGE(((byBank<<2)+3) % (S.NesHeader.ROMSize<<1));
      W.ROMBANK2 = ROMPAGE(((byBank<<2)+2) % (S.NesHeader.ROMSize<<1));
      W.ROMBANK3 = ROMPAGE(((byBank<<2)+3) % (S.NesHeader.ROMSize<<1));
    } else {
      W.ROMBANK0 = ROMPAGE(((byBank<<2)+0) % (S.NesHeader.ROMSize<<1));
      W.ROMBANK1 = ROMPAGE(((byBank<<2)+1) % (S.NesHeader.ROMSize<<1));
      W.ROMBANK2 = ROMPAGE(((byBank<<2)+0) % (S.NesHeader.ROMSize<<1));
      W.ROMBANK3 = ROMPAGE(((byBank<<2)+1) % (S.NesHeader.ROMSize<<1));
    }
  }
  
  if( !(wAddr & 0x0080) ) {
    if( wAddr & 0x0200 ) {
      W.ROMBANK2 = ROMPAGE((((byBank&0x1C)<<2)+14) % (S.NesHeader.ROMSize<<1));
      W.ROMBANK3 = ROMPAGE((((byBank&0x1C)<<2)+15) % (S.NesHeader.ROMSize<<1));
    } else {
      W.ROMBANK2 = ROMPAGE((((byBank&0x1C)<<2)+0) % (S.NesHeader.ROMSize<<1));
      W.ROMBANK3 = ROMPAGE((((byBank&0x1C)<<2)+1) % (S.NesHeader.ROMSize<<1));
    }
  }
  if( wAddr & 0x0002 ) {
    NESCore_Mirroring( 0 );
  } else {
    NESCore_Mirroring( 1 );
  }
}
