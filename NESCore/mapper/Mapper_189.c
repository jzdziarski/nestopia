/*===================================================================*/
/*                                                                   */
/*                       Mapper 189 (Pirates)                        */
/*                                                                   */
/*===================================================================*/

byte Map189_Regs[ 1 ];
byte Map189_IRQ_Cnt;
byte Map189_IRQ_Latch;
byte Map189_IRQ_Enable;

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 189                                            */
/*-------------------------------------------------------------------*/
void Map189_Init()
{
  /* Initialize Mapper */
  MapperInit = Map189_Init;

  /* Write to Mapper */
  MapperWrite = Map189_Write;

  /* Write to SRAM */
  MapperSram = Map0_Sram;

  /* Write to APU */
  MapperApu = Map189_Apu;

  /* Read from APU */
  MapperReadApu = Map0_ReadApu;

  /* Callback at VSync */
  MapperVSync = Map0_VSync;

  /* Callback at HSync */
  MapperHSync = Map189_HSync;

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
    {
      W.PPUBANK[ nPage ] = VROMPAGE( nPage );
    }
    NESCore_Develop_Character_Data();
  }

  /* Initialize IRQ registers */
  Map189_IRQ_Cnt = 0;
  Map189_IRQ_Latch = 0;
  Map189_IRQ_Enable = 0;


 
}

/*-------------------------------------------------------------------*/
/*  Mapper 189 Write to Apu Function                                 */
/*-------------------------------------------------------------------*/
void Map189_Apu( word wAddr, byte byData )
{
  if ( wAddr >= 0x4100 && wAddr <= 0x41FF )
  {
    byData = ( byData & 0x30 ) >> 4;
    W.ROMBANK0 = ROMPAGE( ( byData * 4 + 0 ) % ( S.NesHeader.ROMSize << 1 ) );
    W.ROMBANK1 = ROMPAGE( ( byData * 4 + 1 ) % ( S.NesHeader.ROMSize << 1 ) );
    W.ROMBANK2 = ROMPAGE( ( byData * 4 + 2 ) % ( S.NesHeader.ROMSize << 1 ) );
    W.ROMBANK3 = ROMPAGE( ( byData * 4 + 3 ) % ( S.NesHeader.ROMSize << 1 ) );
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 189 Write Function                                        */
/*-------------------------------------------------------------------*/
void Map189_Write( word wAddr, byte byData )
{
  /* Set ROM Banks */
  switch( wAddr )
  {
    case 0x8000:
      Map189_Regs[0] = byData;
      break;

    case 0x8001:
      switch( Map189_Regs[0] )
      {
        case 0x40:
	  W.PPUBANK[ 0 ] = VROMPAGE( ( byData + 0 ) % ( S.NesHeader.VROMSize << 3 ) );
	  W.PPUBANK[ 1 ] = VROMPAGE( ( byData + 1 ) % ( S.NesHeader.VROMSize << 3 ) );
	  NESCore_Develop_Character_Data();
	  break;

        case 0x41:
	  W.PPUBANK[ 2 ] = VROMPAGE( ( byData + 0 ) % ( S.NesHeader.VROMSize << 3 ) );
	  W.PPUBANK[ 3 ] = VROMPAGE( ( byData + 1 ) % ( S.NesHeader.VROMSize << 3 ) );
	  NESCore_Develop_Character_Data();
	  break;

        case 0x42:
	  W.PPUBANK[ 4 ] = VROMPAGE( byData % ( S.NesHeader.VROMSize << 3 ) );
	  NESCore_Develop_Character_Data();
	  break;

        case 0x43:
	  W.PPUBANK[ 5 ] = VROMPAGE( byData % ( S.NesHeader.VROMSize << 3 ) );
	  NESCore_Develop_Character_Data();
	  break;

        case 0x44:
	  W.PPUBANK[ 6 ] = VROMPAGE( byData % ( S.NesHeader.VROMSize << 3 ) );
	  NESCore_Develop_Character_Data();
	  break;

        case 0x45:
	  W.PPUBANK[ 7 ] = VROMPAGE( byData % ( S.NesHeader.VROMSize << 3 ) );
	  NESCore_Develop_Character_Data();
	  break;

        case 0x46:
	  W.ROMBANK2 = ROMPAGE( byData % ( S.NesHeader.ROMSize << 1 ) );
	  break;  

        case 0x47:
	  W.ROMBANK1 = ROMPAGE( byData % ( S.NesHeader.ROMSize << 1 ) );
	  break;  
      }
      break;

    case 0xC000:
      Map189_IRQ_Cnt = byData;
      break;

    case 0xC001:
      Map189_IRQ_Latch = byData;
      break;

    case 0xE000:
      Map189_IRQ_Enable = 0;
      break;

    case 0xE001:
      Map189_IRQ_Enable = 1;
      break;
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 189 H-Sync Function                                       */
/*-------------------------------------------------------------------*/
void Map189_HSync()
{
/*
 *  Callback at HSync
 *
 */
  if ( Map189_IRQ_Enable )
  {
    if ( /* 0 <= S.PPU_Scanline && */ S.PPU_Scanline <= 239 )
    {
      if ( S.PPU_R1 & R1_SHOW_BG || S.PPU_R1 & R1_SHOW_SP )
      {
        if ( !( --Map189_IRQ_Cnt ) )
        {
          Map189_IRQ_Cnt = Map189_IRQ_Latch;
          IRQ_REQ;
        }
      }
    }
  }
}
