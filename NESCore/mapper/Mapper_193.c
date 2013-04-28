/*===================================================================*/
/*                                                                   */
/*         Mapper 193 : MEGA SOFT (NTDEC) : Fighting Hero            */
/*                                                                   */
/*===================================================================*/

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 193                                            */
/*-------------------------------------------------------------------*/
void Map193_Init()
{
  /* Initialize Mapper */
  MapperInit = Map193_Init;

  /* Write to Mapper */
  MapperWrite = Map0_Write;

  /* Write to SRAM */
  MapperSram = Map193_Sram;

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

  /* Set PPU Banks */
  if ( S.NesHeader.VROMSize > 0 ) {
    int nPage ;
for (nPage = 0; nPage < 8; ++nPage )
      W.PPUBANK[ nPage ] = VROMPAGE( nPage );
    NESCore_Develop_Character_Data();
  }


 
}

/*-------------------------------------------------------------------*/
/*  Mapper 193 Write to SRAM Function                                */
/*-------------------------------------------------------------------*/
void Map193_Sram( word wAddr, byte byData )
{
  switch( wAddr ) {
  case	0x6000:
    W.PPUBANK[ 0 ] = VROMPAGE( ((byData&0xfc) + 0 ) % ( S.NesHeader.VROMSize << 3 ) );
    W.PPUBANK[ 1 ] = VROMPAGE( ((byData&0xfc) + 1 ) % ( S.NesHeader.VROMSize << 3 ) );
    W.PPUBANK[ 2 ] = VROMPAGE( ((byData&0xfc) + 2 ) % ( S.NesHeader.VROMSize << 3 ) );
    W.PPUBANK[ 3 ] = VROMPAGE( ((byData&0xfc) + 3 ) % ( S.NesHeader.VROMSize << 3 ) );
    NESCore_Develop_Character_Data();
    break;
  case	0x6001:
    W.PPUBANK[ 4 ] = VROMPAGE( ( byData + 0 ) % ( S.NesHeader.VROMSize << 3 ) );
    W.PPUBANK[ 5 ] = VROMPAGE( ( byData + 1 ) % ( S.NesHeader.VROMSize << 3 ) );
    NESCore_Develop_Character_Data();
    break;
  case	0x6002:
    W.PPUBANK[ 6 ] = VROMPAGE( ( byData + 0 ) % ( S.NesHeader.VROMSize << 3 ) );
    W.PPUBANK[ 7 ] = VROMPAGE( ( byData + 1 ) % ( S.NesHeader.VROMSize << 3 ) );
    NESCore_Develop_Character_Data();
    break;
  case	0x6003:
    W.ROMBANK0 = ROMPAGE( ((byData<<2) + 0 ) % ( S.NesHeader.ROMSize << 1 ) );
    W.ROMBANK1 = ROMPAGE( ((byData<<2) + 1 ) % ( S.NesHeader.ROMSize << 1 ) );
    W.ROMBANK2 = ROMPAGE( ((byData<<2) + 2 ) % ( S.NesHeader.ROMSize << 1 ) );
    W.ROMBANK3 = ROMPAGE( ((byData<<2) + 3 ) % ( S.NesHeader.ROMSize << 1 ) );
    break;
  }
}
