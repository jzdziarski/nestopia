/*===================================================================*/
/*                                                                   */
/*                   Mapper 133 : SACHEN CHEN                        */
/*                                                                   */
/*===================================================================*/

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 133                                            */
/*-------------------------------------------------------------------*/
void Map133_Init()
{
  /* Initialize Mapper */
  MapperInit = Map133_Init;

  /* Write to Mapper */
  MapperWrite = Map0_Write;

  /* Write to SRAM */
  MapperSram = Map0_Sram;

  /* Write to APU */
  MapperApu = Map133_Apu;

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

  /* Set PPU Banks */
  if ( S.NesHeader.VROMSize > 0 ) {
    int nPage ;
for (nPage = 0; nPage < 8; ++nPage )
      W.PPUBANK[ nPage ] = VROMPAGE( nPage );
    NESCore_Develop_Character_Data();
  }


 
}

/*-------------------------------------------------------------------*/
/*  Mapper 133 Write to APU Function                                 */
/*-------------------------------------------------------------------*/
void Map133_Apu( word wAddr, byte byData )
{
  if ( wAddr == 0x4120 ) {
    /* Set ROM Banks */
    W.ROMBANK0 = ROMPAGE( ((byData&0x04) + 0 ) % (S.NesHeader.ROMSize << 1) );
    W.ROMBANK1 = ROMPAGE( ((byData&0x04) + 1 ) % (S.NesHeader.ROMSize << 1) );
    W.ROMBANK2 = ROMPAGE( ((byData&0x04) + 2 ) % (S.NesHeader.ROMSize << 1) );
    W.ROMBANK3 = ROMPAGE( ((byData&0x04) + 3 ) % (S.NesHeader.ROMSize << 1) );
  
    /* Set PPU Banks */
    W.PPUBANK[ 0 ] = VROMPAGE( (((byData&0x03)<<3) + 0) % (S.NesHeader.VROMSize << 3) );
    W.PPUBANK[ 1 ] = VROMPAGE( (((byData&0x03)<<3) + 1) % (S.NesHeader.VROMSize << 3) );
    W.PPUBANK[ 2 ] = VROMPAGE( (((byData&0x03)<<3) + 2) % (S.NesHeader.VROMSize << 3) );
    W.PPUBANK[ 3 ] = VROMPAGE( (((byData&0x03)<<3) + 3) % (S.NesHeader.VROMSize << 3) );
    W.PPUBANK[ 4 ] = VROMPAGE( (((byData&0x03)<<3) + 4) % (S.NesHeader.VROMSize << 3) );
    W.PPUBANK[ 5 ] = VROMPAGE( (((byData&0x03)<<3) + 5) % (S.NesHeader.VROMSize << 3) );
    W.PPUBANK[ 6 ] = VROMPAGE( (((byData&0x03)<<3) + 6) % (S.NesHeader.VROMSize << 3) );
    W.PPUBANK[ 7 ] = VROMPAGE( (((byData&0x03)<<3) + 7) % (S.NesHeader.VROMSize << 3) );
    NESCore_Develop_Character_Data();
  }
  //Map133_Wram[ wAddr & 0x1fff ] = byData;
}
