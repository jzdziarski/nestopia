/*===================================================================*/
/*                                                                   */
/*                     Mapper 235 : 150-in-1                         */
/*                                                                   */
/*===================================================================*/

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 235                                            */
/*-------------------------------------------------------------------*/
void Map235_Init()
{
  /* Initialize Mapper */
  MapperInit = Map235_Init;

  /* Write to Mapper */
  MapperWrite = Map235_Write;

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

  /* Set Registers */
  int i ;
for (i = 0; i < 0x2000; i++ ) {
    S.DRAM[i] = 0xFF;
  }

  /* Set ROM Banks */
  W.ROMBANK0 = ROMPAGE( 0 );
  W.ROMBANK1 = ROMPAGE( 1 );
  W.ROMBANK2 = ROMPAGE( 2 );
  W.ROMBANK3 = ROMPAGE( 3 );


 
}

/*-------------------------------------------------------------------*/
/*  Mapper 235 Write Function                                        */
/*-------------------------------------------------------------------*/
void Map235_Write( word wAddr, byte byData )
{
  byte	byPrg = ((wAddr&0x0300)>>3)|(wAddr&0x001F);
  byte	byBus = 0;

  if( (S.NesHeader.ROMSize<<1) == 64*2 ) {
    // 100-in-1
    switch( wAddr & 0x0300 ) {
    case	0x0000:	break;
    case	0x0100:	byBus = 1; break;
    case	0x0200:	byBus = 1; break;
    case	0x0300:	byBus = 1; break;
    }
  } else if( (S.NesHeader.ROMSize<<1) == 128*2 ) {
    // 150-in-1
    switch( wAddr & 0x0300 ) {
    case	0x0000:	break;
    case	0x0100:	byBus = 1; break;
    case	0x0200:	byPrg = (byPrg&0x1F)|0x20; break;
    case	0x0300:	byBus = 1; break;
    }
  } else if( (S.NesHeader.ROMSize<<1) == 192*2 ) {
    // 150-in-1
    switch( wAddr & 0x0300 ) {
    case	0x0000:	break;
    case	0x0100:	byBus = 1; break;
    case	0x0200:	byPrg = (byPrg&0x1F)|0x20; break;
    case	0x0300:	byPrg = (byPrg&0x1F)|0x40; break;
    }
  } else if( (S.NesHeader.ROMSize<<1) == 256*2 ) {
  }
  
  if( wAddr & 0x0800 ) {
    if( wAddr & 0x1000 ) {
      W.ROMBANK0 = ROMPAGE(((byPrg<<2)+2) % (S.NesHeader.ROMSize<<1));
      W.ROMBANK1 = ROMPAGE(((byPrg<<2)+3) % (S.NesHeader.ROMSize<<1));
      W.ROMBANK2 = ROMPAGE(((byPrg<<2)+2) % (S.NesHeader.ROMSize<<1));
      W.ROMBANK3 = ROMPAGE(((byPrg<<2)+3) % (S.NesHeader.ROMSize<<1));
    } else {
      W.ROMBANK0 = ROMPAGE(((byPrg<<2)+0) % (S.NesHeader.ROMSize<<1));
      W.ROMBANK1 = ROMPAGE(((byPrg<<2)+1) % (S.NesHeader.ROMSize<<1));
      W.ROMBANK2 = ROMPAGE(((byPrg<<2)+0) % (S.NesHeader.ROMSize<<1));
      W.ROMBANK3 = ROMPAGE(((byPrg<<2)+1) % (S.NesHeader.ROMSize<<1));
    }
  } else {
    W.ROMBANK0 = ROMPAGE(((byPrg<<2)+0) % (S.NesHeader.ROMSize<<1));
    W.ROMBANK1 = ROMPAGE(((byPrg<<2)+1) % (S.NesHeader.ROMSize<<1));
    W.ROMBANK2 = ROMPAGE(((byPrg<<2)+2) % (S.NesHeader.ROMSize<<1));
    W.ROMBANK3 = ROMPAGE(((byPrg<<2)+3) % (S.NesHeader.ROMSize<<1));
  }
  
  if( byBus ) {
    W.ROMBANK0 = S.DRAM;
    W.ROMBANK1 = S.DRAM;
    W.ROMBANK2 = S.DRAM;
    W.ROMBANK3 = S.DRAM;
  }

  if( wAddr & 0x0400 ) {
    NESCore_Mirroring( 3 );
  } else if( wAddr & 0x2000 ) {
    NESCore_Mirroring( 0 );
  } else {
    NESCore_Mirroring( 1 );
  }
}

