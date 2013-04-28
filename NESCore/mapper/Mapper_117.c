/*===================================================================*/
/*                                                                   */
/*                    Mapper 117 (PC-Future)                         */
/*                                                                   */
/*===================================================================*/

byte Map117_IRQ_Line;
byte Map117_IRQ_Enable1;
byte Map117_IRQ_Enable2;

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 117                                            */
/*-------------------------------------------------------------------*/
void Map117_Init()
{
  /* Initialize Mapper */
  MapperInit = Map117_Init;

  /* Write to Mapper */
  MapperWrite = Map117_Write;

  /* Write to SRAM */
  MapperSram = Map0_Sram;

  /* Write to APU */
  MapperApu = Map0_Apu;

  /* Read from APU */
  MapperReadApu = Map0_ReadApu;

  /* Callback at VSync */
  MapperVSync = Map0_VSync;

  /* Callback at HSync */
  MapperHSync = Map117_HSync;

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

  /* Initialize IRQ Registers */
  Map117_IRQ_Line = 0;
  Map117_IRQ_Enable1 = 0;
  Map117_IRQ_Enable2 = 1;


 
}

/*-------------------------------------------------------------------*/
/*  Mapper 117 Write Function                                        */
/*-------------------------------------------------------------------*/
void Map117_Write( word wAddr, byte byData )
{
  switch ( wAddr )
  {
  
    /* Set ROM Banks */
    case 0x8000:
      byData %= ( S.NesHeader.ROMSize << 1 );
      W.ROMBANK0 = ROMPAGE( byData );
      break;

    case 0x8001:
      byData %= ( S.NesHeader.ROMSize << 1 );
      W.ROMBANK1 = ROMPAGE( byData );
      break;

    case 0x8002:
      byData %= ( S.NesHeader.ROMSize << 1 );
      W.ROMBANK2 = ROMPAGE( byData );
      break;

    /* Set PPU Banks */
    case 0xA000:
      byData %= ( S.NesHeader.VROMSize << 3 );
      W.PPUBANK[ 0 ] = VROMPAGE( byData );
      NESCore_Develop_Character_Data();
      break;

    case 0xA001:
      byData %= ( S.NesHeader.VROMSize << 3 );
      W.PPUBANK[ 1 ] = VROMPAGE( byData );
      NESCore_Develop_Character_Data();
      break;

    case 0xA002:
      byData %= ( S.NesHeader.VROMSize << 3 );
      W.PPUBANK[ 2 ] = VROMPAGE( byData );
      NESCore_Develop_Character_Data();
      break;

    case 0xA003:
      byData %= ( S.NesHeader.VROMSize << 3 );
      W.PPUBANK[ 3 ] = VROMPAGE( byData );
      NESCore_Develop_Character_Data();
      break;

    case 0xA004:
      byData %= ( S.NesHeader.VROMSize << 3 );
      W.PPUBANK[ 4 ] = VROMPAGE( byData );
      NESCore_Develop_Character_Data();
      break;

    case 0xA005:
      byData %= ( S.NesHeader.VROMSize << 3 );
      W.PPUBANK[ 5 ] = VROMPAGE( byData );
      NESCore_Develop_Character_Data();
      break;

    case 0xA006:
      byData %= ( S.NesHeader.VROMSize << 3 );
      W.PPUBANK[ 6 ] = VROMPAGE( byData );
      NESCore_Develop_Character_Data();
      break;

    case 0xA007:
      byData %= ( S.NesHeader.VROMSize << 3 );
      W.PPUBANK[ 7 ] = VROMPAGE( byData );
      NESCore_Develop_Character_Data();
      break;

    case 0xc001:
    case 0xc002:
    case 0xc003:
      Map117_IRQ_Enable1 = Map117_IRQ_Line = byData;
      break;

    case 0xe000:
      Map117_IRQ_Enable2 = byData & 0x01;
      break;
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 117 H-Sync Function                                       */
/*-------------------------------------------------------------------*/
void Map117_HSync()
{
  if ( Map117_IRQ_Enable1 && Map117_IRQ_Enable2 )
  {
    if ( Map117_IRQ_Line == S.PPU_Scanline )
    {
      Map117_IRQ_Enable1 = 0x00;
      IRQ_REQ;
    }
  }
}
