/*===================================================================*/
/*                                                                   */
/*                      Mapper 243 (Pirates)                         */
/*                                                                   */
/*===================================================================*/

byte Map243_Regs[4];

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 243                                            */
/*-------------------------------------------------------------------*/
void Map243_Init()
{
  /* Initialize Mapper */
  MapperInit = Map243_Init;

  /* Write to Mapper */
  MapperWrite = Map0_Write;

  /* Write to SRAM */
  MapperSram = Map0_Sram;

  /* Write to APU */
  MapperApu = Map243_Apu;

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
  if ( S.NesHeader.VROMSize > 0 )
  {
    int nPage ;
for (nPage = 0; nPage < 8; ++nPage )
    {
      W.PPUBANK[ nPage ] = VROMPAGE( nPage );
    }
    NESCore_Develop_Character_Data();
  }

  /* Initialize state registers */
  Map243_Regs[0] = Map243_Regs[1] = Map243_Regs[2] = Map243_Regs[3] = 0;


 
}

/*-------------------------------------------------------------------*/
/*  Mapper 243 Write to Apu Function                                 */
/*-------------------------------------------------------------------*/
void Map243_Apu( word wAddr, byte byData )
{
  if ( wAddr == 0x4100 )
  {
    Map243_Regs[0] = byData;
  }
  else if ( wAddr == 0x4101 )
  {
    switch ( Map243_Regs[0] & 0x07 )
    {
      case 0x02:
        Map243_Regs[1] = byData & 0x01;
        break;

      case 0x00:
      case 0x04:
      case 0x07:
        Map243_Regs[2] = ( byData & 0x01 ) << 1;
        break;

      /* Set ROM Banks */
      case 0x05:
        W.ROMBANK0 = ROMPAGE( ( byData * 4 + 0 ) % ( S.NesHeader.ROMSize << 1 ) );
        W.ROMBANK1 = ROMPAGE( ( byData * 4 + 1 ) % ( S.NesHeader.ROMSize << 1 ) );
        W.ROMBANK2 = ROMPAGE( ( byData * 4 + 2 ) % ( S.NesHeader.ROMSize << 1 ) );
        W.ROMBANK3 = ROMPAGE( ( byData * 4 + 3 ) % ( S.NesHeader.ROMSize << 1 ) );
        break;

      case 0x06:
        Map243_Regs[3] = ( byData & 0x03 ) << 2;
        break;
    }

    /* Set PPU Banks */
    if ( ( S.NesHeader.VROMSize << 3 ) <= 64 )
    {
      byte chr_bank = ( Map243_Regs[2] + Map243_Regs[3] ) >> 1;

      W.PPUBANK[ 0 ] = VROMPAGE( ( chr_bank * 8 + 0 ) % ( S.NesHeader.VROMSize << 3 ) );
      W.PPUBANK[ 1 ] = VROMPAGE( ( chr_bank * 8 + 1 ) % ( S.NesHeader.VROMSize << 3 ) );
      W.PPUBANK[ 2 ] = VROMPAGE( ( chr_bank * 8 + 2 ) % ( S.NesHeader.VROMSize << 3 ) );
      W.PPUBANK[ 3 ] = VROMPAGE( ( chr_bank * 8 + 3 ) % ( S.NesHeader.VROMSize << 3 ) );
      W.PPUBANK[ 4 ] = VROMPAGE( ( chr_bank * 8 + 4 ) % ( S.NesHeader.VROMSize << 3 ) );
      W.PPUBANK[ 5 ] = VROMPAGE( ( chr_bank * 8 + 5 ) % ( S.NesHeader.VROMSize << 3 ) );
      W.PPUBANK[ 6 ] = VROMPAGE( ( chr_bank * 8 + 6 ) % ( S.NesHeader.VROMSize << 3 ) );
      W.PPUBANK[ 7 ] = VROMPAGE( ( chr_bank * 8 + 7 ) % ( S.NesHeader.VROMSize << 3 ) );
      NESCore_Develop_Character_Data();
    }
    else
    {
      byte chr_bank = Map243_Regs[1] + Map243_Regs[2] + Map243_Regs[3];

      W.PPUBANK[ 0 ] = VROMPAGE( ( chr_bank * 8 + 0 ) % ( S.NesHeader.VROMSize << 3 ) );
      W.PPUBANK[ 1 ] = VROMPAGE( ( chr_bank * 8 + 1 ) % ( S.NesHeader.VROMSize << 3 ) );
      W.PPUBANK[ 2 ] = VROMPAGE( ( chr_bank * 8 + 2 ) % ( S.NesHeader.VROMSize << 3 ) );
      W.PPUBANK[ 3 ] = VROMPAGE( ( chr_bank * 8 + 3 ) % ( S.NesHeader.VROMSize << 3 ) );
      W.PPUBANK[ 4 ] = VROMPAGE( ( chr_bank * 8 + 4 ) % ( S.NesHeader.VROMSize << 3 ) );
      W.PPUBANK[ 5 ] = VROMPAGE( ( chr_bank * 8 + 5 ) % ( S.NesHeader.VROMSize << 3 ) );
      W.PPUBANK[ 6 ] = VROMPAGE( ( chr_bank * 8 + 6 ) % ( S.NesHeader.VROMSize << 3 ) );
      W.PPUBANK[ 7 ] = VROMPAGE( ( chr_bank * 8 + 7 ) % ( S.NesHeader.VROMSize << 3 ) );
      NESCore_Develop_Character_Data();
    }
  }
}
