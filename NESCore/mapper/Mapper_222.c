/*===================================================================*/
/*                                                                   */
/*                          Mapper 222                               */
/*                                                                   */
/*===================================================================*/

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 222                                            */
/*-------------------------------------------------------------------*/
void Map222_Init()
{
  /* Initialize Mapper */
  MapperInit = Map222_Init;

  /* Write to Mapper */
  MapperWrite = Map222_Write;

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
  if ( S.NesHeader.VROMSize > 0 ) {
    int nPage ;
for (nPage = 0; nPage < 8; ++nPage )
      W.PPUBANK[ nPage ] = VROMPAGE( nPage );
    NESCore_Develop_Character_Data();
  }

  /* Set Mirroring */
  NESCore_Mirroring( 1 );


 
}

/*-------------------------------------------------------------------*/
/*  Mapper 222 Write Function                                        */
/*-------------------------------------------------------------------*/
void Map222_Write( word wAddr, byte byData )
{
  switch( wAddr & 0xF003 ) {
  case	0x8000:
    W.ROMBANK0 = ROMPAGE( byData % ( S.NesHeader.ROMSize << 1 ) );
    break;
  case	0xA000:
    W.ROMBANK1 = ROMPAGE( byData % ( S.NesHeader.ROMSize << 1 ) );
    break;
  case	0xB000:
    W.PPUBANK[ 0 ] = VROMPAGE( byData % ( S.NesHeader.VROMSize << 3 ) );
    NESCore_Develop_Character_Data();
    break;
  case	0xB002:
    W.PPUBANK[ 1 ] = VROMPAGE( byData % ( S.NesHeader.VROMSize << 3 ) );
    NESCore_Develop_Character_Data();
    break;
  case	0xC000:
    W.PPUBANK[ 2 ] = VROMPAGE( byData % ( S.NesHeader.VROMSize << 3 ) );
    NESCore_Develop_Character_Data();
    break;
  case	0xC002:
    W.PPUBANK[ 3 ] = VROMPAGE( byData % ( S.NesHeader.VROMSize << 3 ) );
    NESCore_Develop_Character_Data();
    break;
  case	0xD000:
    W.PPUBANK[ 4 ] = VROMPAGE( byData % ( S.NesHeader.VROMSize << 3 ) );
    NESCore_Develop_Character_Data();
    break;
  case	0xD002:
    W.PPUBANK[ 5 ] = VROMPAGE( byData % ( S.NesHeader.VROMSize << 3 ) );
    NESCore_Develop_Character_Data();
    break;
  case	0xE000:
    W.PPUBANK[ 6 ] = VROMPAGE( byData % ( S.NesHeader.VROMSize << 3 ) );
    NESCore_Develop_Character_Data();
    break;
  case	0xE002:
    W.PPUBANK[ 7 ] = VROMPAGE( byData % ( S.NesHeader.VROMSize << 3 ) );
    NESCore_Develop_Character_Data();
    break;
  }
}
