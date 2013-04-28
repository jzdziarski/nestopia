/*===================================================================*/
/*                                                                   */
/*                      Mapper 182  (Pirates)                        */
/*                                                                   */
/*===================================================================*/

byte Map182_Regs[1];
byte Map182_IRQ_Enable;
byte Map182_IRQ_Cnt;

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 182                                            */
/*-------------------------------------------------------------------*/
void Map182_Init()
{
  /* Initialize Mapper */
  MapperInit = Map182_Init;

  /* Write to Mapper */
  MapperWrite = Map182_Write;

  /* Write to SRAM */
  MapperSram = Map0_Sram;

  /* Write to APU */
  MapperApu = Map0_Apu;

  /* Read from APU */
  MapperReadApu = Map0_ReadApu;

  /* Callback at VSync */
  MapperVSync = Map0_VSync;

  /* Callback at HSync */
  MapperHSync = Map182_HSync;

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
  Map182_Regs[0] = 0;
  Map182_IRQ_Enable = 0;
  Map182_IRQ_Cnt = 0;


 
}

/*-------------------------------------------------------------------*/
/*  Mapper 182 Write Function                                        */
/*-------------------------------------------------------------------*/
void Map182_Write( word wAddr, byte byData )
{
  switch( wAddr & 0xF003 )
  {
    /* Name Table Mirroring */
    case 0x8001:
      if ( byData & 0x01 )
      {
        NESCore_Mirroring( 0 );   /* Horizontal */
      }
      else
      {
        NESCore_Mirroring( 1 );   /* Vertical */
      }
      break;

    case 0xA000:
      Map182_Regs[0] = byData & 0x07;
      break;

    case 0xC000:
      switch( Map182_Regs[0] )
      { 
        /* Set PPU Banks */ 
        case 0x00:
          W.PPUBANK[ 0 ] = VROMPAGE( ( ( byData & 0xFE ) + 0 ) % ( S.NesHeader.VROMSize << 3 ) );
          W.PPUBANK[ 1 ] = VROMPAGE( ( ( byData & 0xFE ) + 1 ) % ( S.NesHeader.VROMSize << 3 ) );
          NESCore_Develop_Character_Data();
          break;

        case 0x01:
          W.PPUBANK[ 5 ] = VROMPAGE( byData % ( S.NesHeader.VROMSize << 3 ) );
          NESCore_Develop_Character_Data();
          break;

        case 0x02:
          W.PPUBANK[ 2 ] = VROMPAGE( ( ( byData & 0xFE ) + 0 ) % ( S.NesHeader.VROMSize << 3 ) );
          W.PPUBANK[ 3 ] = VROMPAGE( ( ( byData & 0xFE ) + 1 ) % ( S.NesHeader.VROMSize << 3 ) );
          NESCore_Develop_Character_Data();
          break;

        case 0x03:
          W.PPUBANK[ 7 ] = VROMPAGE( byData % ( S.NesHeader.VROMSize << 3 ) );
          NESCore_Develop_Character_Data();
          break;

        /* Set ROM Banks */ 
        case 0x04:
          W.ROMBANK0 = ROMPAGE( byData % ( S.NesHeader.ROMSize << 1 ) );
          break;

        case 0x05:
          W.ROMBANK1 = ROMPAGE( byData % ( S.NesHeader.ROMSize << 1 ) );
          break;

        /* Set PPU Banks */ 
        case 0x06:
          W.PPUBANK[ 4 ] = VROMPAGE( byData % ( S.NesHeader.VROMSize << 3 ) );
          NESCore_Develop_Character_Data();
          break;

        case 0x07:
          W.PPUBANK[ 6 ] = VROMPAGE( byData % ( S.NesHeader.VROMSize << 3 ) );
          NESCore_Develop_Character_Data();
          break;
      }
      break;

   case 0xE003:
     Map182_IRQ_Cnt = byData;
     Map182_IRQ_Enable = byData;
     break;
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 182 H-Sync Function                                       */
/*-------------------------------------------------------------------*/
void Map182_HSync()
{
/*
 *  Callback at HSync
 *
 */
  if ( Map182_IRQ_Enable )
  {
    if ( /* 0 <= S.PPU_Scanline && */ S.PPU_Scanline <= 240 )
    {
      if ( S.PPU_R1 & R1_SHOW_BG || S.PPU_R1 & R1_SHOW_SP )
      {
        if ( !( --Map182_IRQ_Cnt ) )
        {
          Map182_IRQ_Cnt = 0;
          Map182_IRQ_Enable = 0;
          IRQ_REQ;
        }
      }
    }
  }
}
