/*===================================================================*/
/*                                                                   */
/*                    Mapper 85 (Konami VRC7)                        */
/*                                                                   */
/*===================================================================*/

byte Map85_Chr_Ram[ 0x100 * 0x400 ];
byte Map85_Regs[ 1 ];
byte Map85_IRQ_Enable;
byte Map85_IRQ_Cnt;
byte Map85_IRQ_Latch;

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 85                                             */
/*-------------------------------------------------------------------*/
void Map85_Init()
{
  /* Initialize Mapper */
  MapperInit = Map85_Init;

  /* Write to Mapper */
  MapperWrite = Map85_Write;

  /* Write to SRAM */
  MapperSram = Map0_Sram;

  /* Write to APU */
  MapperApu = Map0_Apu;

  /* Read from APU */
  MapperReadApu = Map0_ReadApu;

  /* Callback at VSync */
  MapperVSync = Map0_VSync;

  /* Callback at HSync */
  MapperHSync = Map85_HSync;

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
  W.PPUBANK[ 0 ] = Map85_VROMPAGE( 0 );
  W.PPUBANK[ 1 ] = Map85_VROMPAGE( 0 );
  W.PPUBANK[ 2 ] = Map85_VROMPAGE( 0 );
  W.PPUBANK[ 3 ] = Map85_VROMPAGE( 0 );
  W.PPUBANK[ 4 ] = Map85_VROMPAGE( 0 );
  W.PPUBANK[ 5 ] = Map85_VROMPAGE( 0 );
  W.PPUBANK[ 6 ] = Map85_VROMPAGE( 0 );
  W.PPUBANK[ 7 ] = Map85_VROMPAGE( 0 );
  NESCore_Develop_Character_Data();

  /* Initialize State Registers */
  Map85_Regs[ 0 ] = 0;
  Map85_IRQ_Enable = 0;
  Map85_IRQ_Cnt = 0;
  Map85_IRQ_Latch = 0;


 
}

/*-------------------------------------------------------------------*/
/*  Mapper 85 Write Function                                         */
/*-------------------------------------------------------------------*/
void Map85_Write( word wAddr, byte byData )
{
  switch( wAddr & 0xf030 )
  {
    case 0x8000:
      byData %= ( S.NesHeader.ROMSize << 1 );
      W.ROMBANK0 = ROMPAGE( byData );
      break;

    case 0x8010:
      byData %= ( S.NesHeader.ROMSize << 1 );
      W.ROMBANK1 = ROMPAGE( byData );
      break;

    case 0x9000:
      byData %= ( S.NesHeader.ROMSize << 1 );
      W.ROMBANK2 = ROMPAGE( byData );
      break;

    case 0x9010:
    case 0x9030:
      /* Extra Sound */

    case 0xa000:
      W.PPUBANK[ 0 ] = Map85_VROMPAGE( byData );
      NESCore_Develop_Character_Data();
      break;

    case 0xa010:
      W.PPUBANK[ 1 ] = Map85_VROMPAGE( byData );
      NESCore_Develop_Character_Data();
      break;

    case 0xb000:
      W.PPUBANK[ 2 ] = Map85_VROMPAGE( byData );
      NESCore_Develop_Character_Data();
      break;

    case 0xb010:
      W.PPUBANK[ 3 ] = Map85_VROMPAGE( byData );
      NESCore_Develop_Character_Data();
      break;

    case 0xc000:
      W.PPUBANK[ 4 ] = Map85_VROMPAGE( byData );
      NESCore_Develop_Character_Data();
      break;

    case 0xc010:
      W.PPUBANK[ 5 ] = Map85_VROMPAGE( byData );
      NESCore_Develop_Character_Data();
      break;

    case 0xd000:
      W.PPUBANK[ 6 ] = Map85_VROMPAGE( byData );
      NESCore_Develop_Character_Data();
      break;

    case 0xd010:
      W.PPUBANK[ 7 ] = Map85_VROMPAGE( byData );
      NESCore_Develop_Character_Data();
      break;

    /* Name Table Mirroring */
    case 0xe000:
      switch ( byData & 0x03 )
      {
        case 0x00:
          NESCore_Mirroring( 1 );
          break;
        case 0x01:
          NESCore_Mirroring( 0 );
          break;
        case 0x02:
          NESCore_Mirroring( 3 );
          break;
        case 0x03:
          NESCore_Mirroring( 2 );
          break;
      }
      break;

    case 0xe010:
      Map85_IRQ_Latch = 0xff - byData;
      break;

    case 0xf000:
      Map85_Regs[ 0 ] = byData & 0x01;
      Map85_IRQ_Enable = ( byData & 0x02 ) >> 1;
      Map85_IRQ_Cnt = Map85_IRQ_Latch;
      break;

    case 0xf010:
      Map85_IRQ_Enable = Map85_Regs[ 0 ];
      Map85_IRQ_Cnt = Map85_IRQ_Latch;
      break;
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 85 H-Sync Function                                        */
/*-------------------------------------------------------------------*/
void Map85_HSync()
{
  if ( Map85_IRQ_Enable )
  {
    if ( Map85_IRQ_Cnt == 0 )
    {
      IRQ_REQ;
      Map85_IRQ_Enable = 0;
    } else {
      Map85_IRQ_Cnt--;
    }
  }
}
