/*===================================================================*/
/*                                                                   */
/*                       Mapper 88 (Namco 118)                       */
/*                                                                   */
/*===================================================================*/

byte  Map88_Regs[ 1 ];

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 88                                             */
/*-------------------------------------------------------------------*/
void Map88_Init()
{
  /* Initialize Mapper */
  MapperInit = Map88_Init;

  /* Write to Mapper */
  MapperWrite = Map88_Write;

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
/*  Mapper 88 Write Function                                         */
/*-------------------------------------------------------------------*/
void Map88_Write( word wAddr, byte byData )
{
  switch ( wAddr )
  {
    case 0x8000:
      Map88_Regs[ 0 ] = byData;
      break;

    case 0x8001:
      switch ( Map88_Regs[ 0 ] & 0x07 )
      { 
        case 0x00:
          W.PPUBANK[ 0 ] = VROMPAGE( ( ( byData & 0xfe ) + 0 ) % ( S.NesHeader.VROMSize << 3 ) );
          W.PPUBANK[ 1 ] = VROMPAGE( ( ( byData & 0xfe ) + 1 ) % ( S.NesHeader.VROMSize << 3 ) );
          NESCore_Develop_Character_Data();
          break;

        case 0x01:
          W.PPUBANK[ 2 ] = VROMPAGE( ( ( byData & 0xfe ) + 0 ) % ( S.NesHeader.VROMSize << 3 ) );
          W.PPUBANK[ 3 ] = VROMPAGE( ( ( byData & 0xfe ) + 1 ) % ( S.NesHeader.VROMSize << 3 ) );
          NESCore_Develop_Character_Data();
          break;

        case 0x02:
          W.PPUBANK[ 4 ] = VROMPAGE( ( byData + 0x40 ) % ( S.NesHeader.VROMSize << 3 ) );
          NESCore_Develop_Character_Data();
          break;

        case 0x03:
          W.PPUBANK[ 5 ] = VROMPAGE( ( byData + 0x40 ) % ( S.NesHeader.VROMSize << 3 ) );
          NESCore_Develop_Character_Data();
          break;

        case 0x04:
          W.PPUBANK[ 6 ] = VROMPAGE( ( byData + 0x40 ) % ( S.NesHeader.VROMSize << 3 ) );
          NESCore_Develop_Character_Data();
          break;

        case 0x05:
          W.PPUBANK[ 7 ] = VROMPAGE( ( byData + 0x40 ) % ( S.NesHeader.VROMSize << 3 ) );
          NESCore_Develop_Character_Data();
          break;

        case 0x06:
          W.ROMBANK0 = ROMPAGE( byData % ( S.NesHeader.ROMSize << 1 ) );
          break;

        case 0x07:
          W.ROMBANK1 = ROMPAGE( byData % ( S.NesHeader.ROMSize << 1 ) );
          break;
      }
      break;

    case 0xc000:
      if ( byData ) 
      {
        NESCore_Mirroring( 2 );
      } else {
        NESCore_Mirroring( 3 );
      }
      break;
  }
}
