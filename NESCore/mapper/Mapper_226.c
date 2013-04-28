/*===================================================================*/
/*                                                                   */
/*                   Mapper 226 : 76-in-1                            */
/*                                                                   */
/*===================================================================*/

byte	Map226_Reg[2];

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 226                                            */
/*-------------------------------------------------------------------*/
void Map226_Init()
{
  /* Initialize Mapper */
  MapperInit = Map226_Init;

  /* Write to Mapper */
  MapperWrite = Map226_Write;

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

  /* Initialize Registers */
  Map226_Reg[0] = 0;
  Map226_Reg[1] = 0;
  

 
}

/*-------------------------------------------------------------------*/
/*  Mapper 226 Write Function                                        */
/*-------------------------------------------------------------------*/
void Map226_Write( word wAddr, byte byData )
{
  if( wAddr & 0x0001 ) {
    Map226_Reg[1] = byData;
  } else {
    Map226_Reg[0] = byData;
  }
  
  if( Map226_Reg[0] & 0x40 ) {
    NESCore_Mirroring( 1 );
  } else {
    NESCore_Mirroring( 0 );
  }
  
  byte	byBank = ((Map226_Reg[0]&0x1E)>>1)|((Map226_Reg[0]&0x80)>>3)|((Map226_Reg[1]&0x01)<<5);
  
  if( Map226_Reg[0] & 0x20 ) {
    if( Map226_Reg[0] & 0x01 ) {
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
  } else {
    W.ROMBANK0 = ROMPAGE(((byBank<<2)+0) % (S.NesHeader.ROMSize<<1));
    W.ROMBANK1 = ROMPAGE(((byBank<<2)+1) % (S.NesHeader.ROMSize<<1));
    W.ROMBANK2 = ROMPAGE(((byBank<<2)+2) % (S.NesHeader.ROMSize<<1));
    W.ROMBANK3 = ROMPAGE(((byBank<<2)+3) % (S.NesHeader.ROMSize<<1));
  }
}
