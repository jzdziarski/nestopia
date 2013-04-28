/*===================================================================*/
/*                                                                   */
/*                       Mapper 230 : 22-in-1                        */
/*                                                                   */
/*===================================================================*/

byte Map230_RomSw;

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 230                                            */
/*-------------------------------------------------------------------*/
void Map230_Init()
{
  /* Initialize Mapper */
  MapperInit = Map230_Init;

  /* Write to Mapper */
  MapperWrite = Map230_Write;

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

  /* Initialize Registers */
  if( Map230_RomSw ) {
    Map230_RomSw = 0;
  } else {
    Map230_RomSw = 1;
  }

  /* Set ROM Banks */
  if( Map230_RomSw ) {
    W.ROMBANK0 = ROMPAGE( 0 );
    W.ROMBANK1 = ROMPAGE( 1 );
    W.ROMBANK2 = ROMPAGE( 14 );
    W.ROMBANK3 = ROMPAGE( 15 );
  } else {
    W.ROMBANK0 = ROMPAGE( 16 );
    W.ROMBANK1 = ROMPAGE( 17 );
    W.ROMBANK2 = ROMLASTPAGE( 1 );
    W.ROMBANK3 = ROMLASTPAGE( 0 );
  }


 
}

/*-------------------------------------------------------------------*/
/*  Mapper 230 Write Function                                        */
/*-------------------------------------------------------------------*/
void Map230_Write( word wAddr, byte byData )
{
  if( Map230_RomSw ) {
    W.ROMBANK0 = ROMPAGE((((byData&0x07)<<1)+0) % (S.NesHeader.ROMSize<<1));
    W.ROMBANK1 = ROMPAGE((((byData&0x07)<<1)+1) % (S.NesHeader.ROMSize<<1));
  } else {
    if( byData & 0x20 ) {
      W.ROMBANK0 = ROMPAGE((((byData&0x1F)<<1)+16) % (S.NesHeader.ROMSize<<1));
      W.ROMBANK1 = ROMPAGE((((byData&0x1F)<<1)+17) % (S.NesHeader.ROMSize<<1));
      W.ROMBANK2 = ROMPAGE((((byData&0x1F)<<1)+16) % (S.NesHeader.ROMSize<<1));
      W.ROMBANK3 = ROMPAGE((((byData&0x1F)<<1)+17) % (S.NesHeader.ROMSize<<1));
    } else {
      W.ROMBANK0 = ROMPAGE((((byData&0x1E)<<1)+16) % (S.NesHeader.ROMSize<<1));
      W.ROMBANK1 = ROMPAGE((((byData&0x1E)<<1)+17) % (S.NesHeader.ROMSize<<1));
      W.ROMBANK2 = ROMPAGE((((byData&0x1E)<<1)+18) % (S.NesHeader.ROMSize<<1));
      W.ROMBANK3 = ROMPAGE((((byData&0x1E)<<1)+19) % (S.NesHeader.ROMSize<<1));
    }
    if( byData & 0x40 ) {
      NESCore_Mirroring( 1 );
    } else {
      NESCore_Mirroring( 0 );
    }
  }
}
