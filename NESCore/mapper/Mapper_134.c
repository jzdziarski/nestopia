/*===================================================================*/
/*                                                                   */
/*                          Mapper 134                               */
/*                                                                   */
/*===================================================================*/

byte    Map134_Cmd, Map134_Prg, Map134_Chr;

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 134                                            */
/*-------------------------------------------------------------------*/
void Map134_Init()
{
  /* Initialize Mapper */
  MapperInit = Map134_Init;

  /* Write to Mapper */
  MapperWrite = Map0_Write;

  /* Write to SRAM */
  MapperSram = Map0_Sram;

  /* Write to APU */
  MapperApu = Map134_Apu;

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
/*  Mapper 134 Write to APU Function                                 */
/*-------------------------------------------------------------------*/
void Map134_Apu( word wAddr, byte byData )
{
  switch( wAddr & 0x4101 ) {
  case	0x4100:
    Map134_Cmd = byData & 0x07;
    break;
  case	0x4101:
    switch( Map134_Cmd ) {
    case 0:	
      Map134_Prg = 0;
      Map134_Chr = 3;
      break;
    case 4:
      Map134_Chr &= 0x3;
      Map134_Chr |= (byData & 0x07) << 2;
      break;
    case 5:
      Map134_Prg = byData & 0x07;
      break;
    case 6:
      Map134_Chr &= 0x1C;
      Map134_Chr |= byData & 0x3;
      break;
    case 7:
      if( byData & 0x01 ) NESCore_Mirroring( 0 );
      else		  NESCore_Mirroring( 1 );
      break;
    }
    break;
  }

  /* Set ROM Banks */
  W.ROMBANK0 = ROMPAGE( ((Map134_Prg<<2) + 0 ) % (S.NesHeader.ROMSize << 1) );
  W.ROMBANK1 = ROMPAGE( ((Map134_Prg<<2) + 1 ) % (S.NesHeader.ROMSize << 1) );
  W.ROMBANK2 = ROMPAGE( ((Map134_Prg<<2) + 2 ) % (S.NesHeader.ROMSize << 1) );
  W.ROMBANK3 = ROMPAGE( ((Map134_Prg<<2) + 3 ) % (S.NesHeader.ROMSize << 1) );
  
  /* Set PPU Banks */
  W.PPUBANK[ 0 ] = VROMPAGE( ((Map134_Chr<<3) + 0) % (S.NesHeader.VROMSize << 3) );
  W.PPUBANK[ 1 ] = VROMPAGE( ((Map134_Chr<<3) + 1) % (S.NesHeader.VROMSize << 3) );
  W.PPUBANK[ 2 ] = VROMPAGE( ((Map134_Chr<<3) + 2) % (S.NesHeader.VROMSize << 3) );
  W.PPUBANK[ 3 ] = VROMPAGE( ((Map134_Chr<<3) + 3) % (S.NesHeader.VROMSize << 3) );
  W.PPUBANK[ 4 ] = VROMPAGE( ((Map134_Chr<<3) + 4) % (S.NesHeader.VROMSize << 3) );
  W.PPUBANK[ 5 ] = VROMPAGE( ((Map134_Chr<<3) + 5) % (S.NesHeader.VROMSize << 3) );
  W.PPUBANK[ 6 ] = VROMPAGE( ((Map134_Chr<<3) + 6) % (S.NesHeader.VROMSize << 3) );
  W.PPUBANK[ 7 ] = VROMPAGE( ((Map134_Chr<<3) + 7) % (S.NesHeader.VROMSize << 3) );
  NESCore_Develop_Character_Data();

  //Map134_Wram[ wAddr & 0x1fff ] = byData;
}
