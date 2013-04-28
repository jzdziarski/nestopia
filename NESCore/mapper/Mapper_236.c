/*===================================================================*/
/*                                                                   */
/*                   Mapper 236 : 800-in-1                           */
/*                                                                   */
/*===================================================================*/

byte    Map236_Bank, Map236_Mode;

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 236                                            */
/*-------------------------------------------------------------------*/
void Map236_Init()
{
  /* Initialize Mapper */
  MapperInit = Map236_Init;

  /* Write to Mapper */
  MapperWrite = Map236_Write;

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
  W.ROMBANK2 = ROMLASTPAGE( 1 );
  W.ROMBANK3 = ROMLASTPAGE( 0 );

  /* Set Registers */
  Map236_Bank = Map236_Mode = 0;


 
}

/*-------------------------------------------------------------------*/
/*  Mapper 236 Write Function                                        */
/*-------------------------------------------------------------------*/
void Map236_Write( word wAddr, byte byData )
{
  if( wAddr >= 0x8000 && wAddr <= 0xBFFF ) {
    Map236_Bank = ((wAddr&0x03)<<4)|(Map236_Bank&0x07);
  } else {
    Map236_Bank = (wAddr&0x07)|(Map236_Bank&0x30);
    Map236_Mode = wAddr&0x30;
  }

  if( wAddr & 0x20 ) {
    NESCore_Mirroring( 0 );
  } else {
    NESCore_Mirroring( 1 );
  }

  switch( Map236_Mode ) {
  case	0x00:
    Map236_Bank |= 0x08;
    W.ROMBANK0 = ROMPAGE(((Map236_Bank<<1)+0) % (S.NesHeader.ROMSize<<1));
    W.ROMBANK1 = ROMPAGE(((Map236_Bank<<1)+1) % (S.NesHeader.ROMSize<<1));
    W.ROMBANK2 = ROMPAGE((((Map236_Bank|0x07)<<1)+0) % (S.NesHeader.ROMSize<<1));
    W.ROMBANK3 = ROMPAGE((((Map236_Bank|0x07)<<1)+1) % (S.NesHeader.ROMSize<<1));
    break;
  case	0x10:
    Map236_Bank |= 0x37;
    W.ROMBANK0 = ROMPAGE(((Map236_Bank<<1)+0) % (S.NesHeader.ROMSize<<1));
    W.ROMBANK1 = ROMPAGE(((Map236_Bank<<1)+1) % (S.NesHeader.ROMSize<<1));
    W.ROMBANK2 = ROMPAGE((((Map236_Bank|0x07)<<1)+0) % (S.NesHeader.ROMSize<<1));
    W.ROMBANK3 = ROMPAGE((((Map236_Bank|0x07)<<1)+1) % (S.NesHeader.ROMSize<<1));
    break;
  case	0x20:
    Map236_Bank |= 0x08;
    W.ROMBANK0 = ROMPAGE((((Map236_Bank&0xFE)<<1)+0) % (S.NesHeader.ROMSize<<1));
    W.ROMBANK1 = ROMPAGE((((Map236_Bank&0xFE)<<1)+1) % (S.NesHeader.ROMSize<<1));
    W.ROMBANK2 = ROMPAGE((((Map236_Bank&0xFE)<<1)+2) % (S.NesHeader.ROMSize<<1));
    W.ROMBANK3 = ROMPAGE((((Map236_Bank&0xFE)<<1)+3) % (S.NesHeader.ROMSize<<1));
    break;
  case	0x30:
    Map236_Bank |= 0x08;
    W.ROMBANK0 = ROMPAGE(((Map236_Bank<<1)+0) % (S.NesHeader.ROMSize<<1));
    W.ROMBANK1 = ROMPAGE(((Map236_Bank<<1)+1) % (S.NesHeader.ROMSize<<1));
    W.ROMBANK2 = ROMPAGE(((Map236_Bank<<1)+0) % (S.NesHeader.ROMSize<<1));
    W.ROMBANK3 = ROMPAGE(((Map236_Bank<<1)+1) % (S.NesHeader.ROMSize<<1));
    break;
  }
}
