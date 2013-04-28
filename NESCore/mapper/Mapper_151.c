/*===================================================================*/
/*                                                                   */
/*                   Mapper 151 (VSUnisystem)                        */
/*                                                                   */
/*===================================================================*/

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 151                                            */
/*-------------------------------------------------------------------*/
void Map151_Init()
{
  /* Initialize Mapper */
  MapperInit = Map151_Init;

  /* Write to Mapper */
  MapperWrite = Map151_Write;

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

  /* Set PPU Banks */
  if ( S.NesHeader.VROMSize > 0 )
  {
    int nPage ;
for (nPage = 0; nPage < 8; ++nPage )
      W.PPUBANK[ nPage ] = VROMPAGE( nPage );
    NESCore_Develop_Character_Data();
  }


 
}

/*-------------------------------------------------------------------*/
/*  Mapper 151 Write Function                                        */
/*-------------------------------------------------------------------*/
void Map151_Write( word wAddr, byte byData )
{
  /* Set ROM Banks */
  switch( wAddr & 0xF000 )
  {
    case 0x8000:
      W.ROMBANK0 = ROMPAGE( byData % ( S.NesHeader.ROMSize << 1 ) );
      break;

    case 0xA000:
      W.ROMBANK1 = ROMPAGE( byData % ( S.NesHeader.ROMSize << 1 ) );
      break;

    case 0xC000:
      W.ROMBANK2 = ROMPAGE( byData % ( S.NesHeader.ROMSize << 1 ) );
      break;

    case 0xE000:
      W.PPUBANK[ 0 ] = VROMPAGE( ( byData*4+0 ) % ( S.NesHeader.VROMSize << 3 ) );
      W.PPUBANK[ 1 ] = VROMPAGE( ( byData*4+1 ) % ( S.NesHeader.VROMSize << 3 ) );
      W.PPUBANK[ 2 ] = VROMPAGE( ( byData*4+2 ) % ( S.NesHeader.VROMSize << 3 ) );
      W.PPUBANK[ 3 ] = VROMPAGE( ( byData*4+3 ) % ( S.NesHeader.VROMSize << 3 ) );
      NESCore_Develop_Character_Data();
      break;

    case 0xF000:
      W.PPUBANK[ 4 ] = VROMPAGE( ( byData*4+0 ) % ( S.NesHeader.VROMSize << 3 ) );
      W.PPUBANK[ 5 ] = VROMPAGE( ( byData*4+1 ) % ( S.NesHeader.VROMSize << 3 ) );
      W.PPUBANK[ 6 ] = VROMPAGE( ( byData*4+2 ) % ( S.NesHeader.VROMSize << 3 ) );
      W.PPUBANK[ 7 ] = VROMPAGE( ( byData*4+3 ) % ( S.NesHeader.VROMSize << 3 ) );
      NESCore_Develop_Character_Data();
      break;
  }
}
