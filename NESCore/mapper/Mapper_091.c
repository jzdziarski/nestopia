/*===================================================================*/
/*                                                                   */
/*                        Mapper 91 (Pirates)                        */
/*                                                                   */
/*===================================================================*/

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 91                                             */
/*-------------------------------------------------------------------*/
void Map91_Init()
{
  /* Initialize Mapper */
  MapperInit = Map91_Init;

  /* Write to Mapper */
  MapperWrite = Map0_Write;

  /* Write to SRAM */
  MapperSram = Map91_Sram;

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
  W.ROMBANK0 = ROMLASTPAGE( 1 );
  W.ROMBANK1 = ROMLASTPAGE( 0 );
  W.ROMBANK2 = ROMLASTPAGE( 1 );
  W.ROMBANK3 = ROMLASTPAGE( 0 );

  /* Set PPU Banks */
  if ( S.NesHeader.VROMSize > 0 )
  {
    int nPage ;
for (nPage = 0; nPage < 8; ++nPage )
      W.PPUBANK[ nPage ] = VROMPAGE( nPage );
    NESCore_Develop_Character_Data();
  }

  /* Set Name Table Mirroring */
  NESCore_Mirroring( 1 );


 
}

/*-------------------------------------------------------------------*/
/*  Mapper 91 Write to Sram Function                                 */
/*-------------------------------------------------------------------*/
void Map91_Sram( word wAddr, byte byData )
{
  switch( wAddr & 0xF00F)
  {
    /* Set PPU Banks */
    case 0x6000:
      W.PPUBANK[ 0 ] = VROMPAGE( (byData*2+0) % ( S.NesHeader.VROMSize << 3 ) );
      W.PPUBANK[ 1 ] = VROMPAGE( (byData*2+1) % ( S.NesHeader.VROMSize << 3 ) );
      NESCore_Develop_Character_Data();
      break;

    case 0x6001:
      W.PPUBANK[ 2 ] = VROMPAGE( (byData*2+0) % ( S.NesHeader.VROMSize << 3 ) );
      W.PPUBANK[ 3 ] = VROMPAGE( (byData*2+1) % ( S.NesHeader.VROMSize << 3 ) );
      NESCore_Develop_Character_Data();
      break;

    case 0x6002:
      W.PPUBANK[ 4 ] = VROMPAGE( (byData*2+0) % ( S.NesHeader.VROMSize << 3 ) );
      W.PPUBANK[ 5 ] = VROMPAGE( (byData*2+1) % ( S.NesHeader.VROMSize << 3 ) );
      NESCore_Develop_Character_Data();
      break;

    case 0x6003:
      W.PPUBANK[ 6 ] = VROMPAGE( (byData*2+0) % ( S.NesHeader.VROMSize << 3 ) );
      W.PPUBANK[ 7 ] = VROMPAGE( (byData*2+1) % ( S.NesHeader.VROMSize << 3 ) );
      NESCore_Develop_Character_Data();
      break;

    /* Set CPU Banks */
    case 0x7000:
      W.ROMBANK0 = ROMPAGE( byData % ( S.NesHeader.ROMSize << 1 ) ); 
      break;

    case 0x7001:
      W.ROMBANK1 = ROMPAGE( byData % ( S.NesHeader.ROMSize << 1 ) ); 
      break;
  }
}
