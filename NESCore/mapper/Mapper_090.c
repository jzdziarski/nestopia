/*===================================================================*/
/*                                                                   */
/*                        Mapper 90 (PC-JY-??)                       */
/*                                                                   */
/*===================================================================*/

byte Map90_Prg_Reg[ 4 ];
byte Map90_Chr_Low_Reg[ 8 ];
byte Map90_Chr_High_Reg[ 8 ];
byte Map90_Nam_Low_Reg[ 4 ];
byte Map90_Nam_High_Reg[ 4 ];

byte Map90_Prg_Bank_Size;
byte Map90_Prg_Bank_6000;
byte Map90_Prg_Bank_E000;
byte Map90_Chr_Bank_Size;
byte Map90_Mirror_Mode;
byte Map90_Mirror_Type;

dword Map90_Value1;
dword Map90_Value2;

byte Map90_IRQ_Enable;
byte Map90_IRQ_Cnt;
byte Map90_IRQ_Latch;

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 90                                             */
/*-------------------------------------------------------------------*/
void Map90_Init()
{
  /* Initialize Mapper */
  MapperInit = Map90_Init;

  /* Write to Mapper */
  MapperWrite = Map90_Write;

  /* Write to SRAM */
  MapperSram = Map0_Sram;

  /* Write to APU */
  MapperApu = Map90_Apu;

  /* Read from APU */
  MapperReadApu = Map90_ReadApu;

  /* Callback at VSync */
  MapperVSync = Map0_VSync;

  /* Callback at HSync */
  MapperHSync = Map90_HSync;

  /* Callback at PPU */
  MapperPPU = Map0_PPU;

  /* Callback at Rendering Screen ( 1:BG, 0:Sprite ) */
  MapperRenderScreen = Map0_RenderScreen;

  /* Set SRAM Banks */
  W.SRAMBANK = S.SRAM;

  /* Set ROM Banks */
  W.ROMBANK0 = ROMLASTPAGE( 3 );
  W.ROMBANK1 = ROMLASTPAGE( 2 );
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
  Map90_IRQ_Cnt = 0;
  Map90_IRQ_Latch = 0;
  Map90_IRQ_Enable = 0;

  byte byPage;
  for ( byPage = 0; byPage < 4; byPage++ )
  {
    Map90_Prg_Reg[ byPage ] = ( S.NesHeader.ROMSize << 1 ) - 4 + byPage;
    Map90_Nam_Low_Reg[ byPage ] = 0;
    Map90_Nam_High_Reg[ byPage ] = 0;
    Map90_Chr_Low_Reg[ byPage ] = byPage;
    Map90_Chr_High_Reg[ byPage ] = 0;
    Map90_Chr_Low_Reg[ byPage + 4 ] = byPage + 4;
    Map90_Chr_High_Reg[ byPage + 4 ] = 0;
  }


 
}

/*-------------------------------------------------------------------*/
/*  Mapper 90 Write to APU Function                                  */
/*-------------------------------------------------------------------*/
void Map90_Apu( word wAddr, byte byData )
{
  switch ( wAddr )
  {
    case 0x5000:
      Map90_Value1 = byData;
      break;

    case 0x5001:
      Map90_Value2 = byData;
      break;
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 90 Read from APU Function                                 */
/*-------------------------------------------------------------------*/
byte Map90_ReadApu( word wAddr )
{
  if ( wAddr == 0x5000 )
  {
    return ( byte )( ( Map90_Value1 * Map90_Value2 ) & 0x00ff );
  } else {
    return ( byte )( wAddr >> 8 );
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 90 Write Function                                         */
/*-------------------------------------------------------------------*/
void Map90_Write( word wAddr, byte byData )
{
  switch ( wAddr )
  {
    /* Set ROM Banks */
    case 0x8000:
    case 0x8001:
    case 0x8002:
    case 0x8003:
      Map90_Prg_Reg[ wAddr & 0x03 ] = byData;
      Map90_Sync_Prg_Banks();
      break;
    
    case 0x9000:
    case 0x9001:
    case 0x9002:
    case 0x9003:
    case 0x9004:
    case 0x9005:
    case 0x9006:
    case 0x9007:
      Map90_Chr_Low_Reg[ wAddr & 0x07 ] = byData;
      Map90_Sync_Chr_Banks();
      break;

    case 0xa000:
    case 0xa001:
    case 0xa002:
    case 0xa003:
    case 0xa004:
    case 0xa005:
    case 0xa006:
    case 0xa007:
      Map90_Chr_High_Reg[ wAddr & 0x07 ] = byData;
      Map90_Sync_Chr_Banks();
      break;

    case 0xb000:
    case 0xb001:
    case 0xb002:
    case 0xb003:
      Map90_Nam_Low_Reg[ wAddr & 0x03 ] = byData;
      Map90_Sync_Mirror();
      break;

    case 0xb004:
    case 0xb005:
    case 0xb006:
    case 0xb007:
      Map90_Nam_High_Reg[ wAddr & 0x03 ] = byData;
      Map90_Sync_Mirror();
      break;

    case 0xc002:
      Map90_IRQ_Enable = 0;
      break;

    case 0xc003:
    case 0xc004:
      if ( Map90_IRQ_Enable == 0 )
      {
        Map90_IRQ_Enable = 1;
        Map90_IRQ_Cnt = Map90_IRQ_Latch;
      }
      break;

    case 0xc005:
      Map90_IRQ_Cnt = byData;
      Map90_IRQ_Latch = byData;
      break;

    case 0xd000:
      Map90_Prg_Bank_6000 = byData & 0x80;
      Map90_Prg_Bank_E000 = byData & 0x04;
      Map90_Prg_Bank_Size = byData & 0x03;
      Map90_Chr_Bank_Size = ( byData & 0x18 ) >> 3;
      Map90_Mirror_Mode = byData & 0x20;
      
      Map90_Sync_Prg_Banks();
      Map90_Sync_Chr_Banks();
      Map90_Sync_Mirror();
      break;

    case 0xd001:
      Map90_Mirror_Type = byData & 0x03;
      Map90_Sync_Mirror();
      break;

    case 0xd003:
      /* Bank Page*/
      break;
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 90 H-Sync Function                                        */
/*-------------------------------------------------------------------*/
void Map90_HSync()
{
/*
 *  Callback at HSync
 *
 */
  if ( /* 0 <= S.PPU_Scanline && */ S.PPU_Scanline <= 239 )
  {
    if ( S.PPU_R1 & R1_SHOW_BG || S.PPU_R1 & R1_SHOW_SP )
    {
      if ( Map90_IRQ_Cnt == 0 )
      {
        if ( Map90_IRQ_Enable )
        {
          IRQ_REQ;
        }
        Map90_IRQ_Latch = 0;
        Map90_IRQ_Enable = 0;
      } else {
        Map90_IRQ_Cnt--;
      }
    }
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 90 Sync Mirror Function                                   */
/*-------------------------------------------------------------------*/
void Map90_Sync_Mirror( void )
{
  byte byPage;
  dword dwNamBank[ 4 ];

  for ( byPage = 0; byPage < 4; byPage++ )
  {
    dwNamBank[ byPage ] = 
      ( (dword)Map90_Nam_High_Reg[ byPage ] << 8 ) | (dword)Map90_Nam_Low_Reg[ byPage ];
  }

  if ( Map90_Mirror_Mode )
  {
    for ( byPage = 0; byPage < 4; byPage++ )
    {
      if ( !Map90_Nam_High_Reg[ byPage ] && ( Map90_Nam_Low_Reg[ byPage ] == byPage ) )
      {
        Map90_Mirror_Mode = 0;
      }
    }

    if ( Map90_Mirror_Mode )
    {
        W.PPUBANK[ NAME_TABLE0 ] = VROMPAGE( dwNamBank[ 0 ] % ( S.NesHeader.VROMSize << 3 ) );
        W.PPUBANK[ NAME_TABLE1 ] = VROMPAGE( dwNamBank[ 1 ] % ( S.NesHeader.VROMSize << 3 ) );
        W.PPUBANK[ NAME_TABLE2 ] = VROMPAGE( dwNamBank[ 2 ] % ( S.NesHeader.VROMSize << 3 ) );
        W.PPUBANK[ NAME_TABLE3 ] = VROMPAGE( dwNamBank[ 3 ] % ( S.NesHeader.VROMSize << 3 ) );
    }
  } else {
    switch ( Map90_Mirror_Type )
    {
      case 0x00:
        NESCore_Mirroring( 1 );
        break;

      case 0x01:
        NESCore_Mirroring( 0 );
        break;

      default:
        NESCore_Mirroring( 3 );
        break;
    }
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 90 Sync Char Banks Function                               */
/*-------------------------------------------------------------------*/
void Map90_Sync_Chr_Banks( void )
{
  byte byPage;
  dword dwChrBank[ 8 ];

  for ( byPage = 0; byPage < 8; byPage++ )
  {
    dwChrBank[ byPage ] =
      ( (dword)Map90_Chr_High_Reg[ byPage ] << 8 ) | (dword)Map90_Chr_Low_Reg[ byPage ];
  }

  switch ( Map90_Chr_Bank_Size )
  {
    case 0:
      W.PPUBANK[ 0 ] = VROMPAGE( ( ( dwChrBank[ 0 ] << 3 ) + 0 ) % ( S.NesHeader.VROMSize << 3 ) );
      W.PPUBANK[ 1 ] = VROMPAGE( ( ( dwChrBank[ 0 ] << 3 ) + 1 ) % ( S.NesHeader.VROMSize << 3 ) );
      W.PPUBANK[ 2 ] = VROMPAGE( ( ( dwChrBank[ 0 ] << 3 ) + 2 ) % ( S.NesHeader.VROMSize << 3 ) );
      W.PPUBANK[ 3 ] = VROMPAGE( ( ( dwChrBank[ 0 ] << 3 ) + 3 ) % ( S.NesHeader.VROMSize << 3 ) );
      W.PPUBANK[ 4 ] = VROMPAGE( ( ( dwChrBank[ 0 ] << 3 ) + 4 ) % ( S.NesHeader.VROMSize << 3 ) );
      W.PPUBANK[ 5 ] = VROMPAGE( ( ( dwChrBank[ 0 ] << 3 ) + 5 ) % ( S.NesHeader.VROMSize << 3 ) );
      W.PPUBANK[ 6 ] = VROMPAGE( ( ( dwChrBank[ 0 ] << 3 ) + 6 ) % ( S.NesHeader.VROMSize << 3 ) );
      W.PPUBANK[ 7 ] = VROMPAGE( ( ( dwChrBank[ 0 ] << 3 ) + 7 ) % ( S.NesHeader.VROMSize << 3 ) );
      NESCore_Develop_Character_Data();
      break;

    case 1:
      W.PPUBANK[ 0 ] = VROMPAGE( ( ( dwChrBank[ 0 ] << 2 ) + 0 ) % ( S.NesHeader.VROMSize << 3 ) );
      W.PPUBANK[ 1 ] = VROMPAGE( ( ( dwChrBank[ 0 ] << 2 ) + 1 ) % ( S.NesHeader.VROMSize << 3 ) );
      W.PPUBANK[ 2 ] = VROMPAGE( ( ( dwChrBank[ 0 ] << 2 ) + 2 ) % ( S.NesHeader.VROMSize << 3 ) );
      W.PPUBANK[ 3 ] = VROMPAGE( ( ( dwChrBank[ 0 ] << 2 ) + 3 ) % ( S.NesHeader.VROMSize << 3 ) );
      W.PPUBANK[ 4 ] = VROMPAGE( ( ( dwChrBank[ 4 ] << 2 ) + 0 ) % ( S.NesHeader.VROMSize << 3 ) );
      W.PPUBANK[ 5 ] = VROMPAGE( ( ( dwChrBank[ 4 ] << 2 ) + 1 ) % ( S.NesHeader.VROMSize << 3 ) );
      W.PPUBANK[ 6 ] = VROMPAGE( ( ( dwChrBank[ 4 ] << 2 ) + 2 ) % ( S.NesHeader.VROMSize << 3 ) );
      W.PPUBANK[ 7 ] = VROMPAGE( ( ( dwChrBank[ 4 ] << 2 ) + 3 ) % ( S.NesHeader.VROMSize << 3 ) );
      NESCore_Develop_Character_Data();
      break;

    case 2:
      W.PPUBANK[ 0 ] = VROMPAGE( ( ( dwChrBank[ 0 ] << 1 ) + 0 ) % ( S.NesHeader.VROMSize << 3 ) );
      W.PPUBANK[ 1 ] = VROMPAGE( ( ( dwChrBank[ 0 ] << 1 ) + 1 ) % ( S.NesHeader.VROMSize << 3 ) );
      W.PPUBANK[ 2 ] = VROMPAGE( ( ( dwChrBank[ 2 ] << 1 ) + 0 ) % ( S.NesHeader.VROMSize << 3 ) );
      W.PPUBANK[ 3 ] = VROMPAGE( ( ( dwChrBank[ 2 ] << 1 ) + 1 ) % ( S.NesHeader.VROMSize << 3 ) );
      W.PPUBANK[ 4 ] = VROMPAGE( ( ( dwChrBank[ 4 ] << 1 ) + 0 ) % ( S.NesHeader.VROMSize << 3 ) );
      W.PPUBANK[ 5 ] = VROMPAGE( ( ( dwChrBank[ 4 ] << 1 ) + 1 ) % ( S.NesHeader.VROMSize << 3 ) );
      W.PPUBANK[ 6 ] = VROMPAGE( ( ( dwChrBank[ 6 ] << 1 ) + 0 ) % ( S.NesHeader.VROMSize << 3 ) );
      W.PPUBANK[ 7 ] = VROMPAGE( ( ( dwChrBank[ 6 ] << 1 ) + 1 ) % ( S.NesHeader.VROMSize << 3 ) );
      NESCore_Develop_Character_Data();
      break;

    default:
      W.PPUBANK[ 0 ] = VROMPAGE( dwChrBank[ 0 ] % ( S.NesHeader.VROMSize << 3 ) );
      W.PPUBANK[ 1 ] = VROMPAGE( dwChrBank[ 1 ] % ( S.NesHeader.VROMSize << 3 ) );
      W.PPUBANK[ 2 ] = VROMPAGE( dwChrBank[ 2 ] % ( S.NesHeader.VROMSize << 3 ) );
      W.PPUBANK[ 3 ] = VROMPAGE( dwChrBank[ 3 ] % ( S.NesHeader.VROMSize << 3 ) );
      W.PPUBANK[ 4 ] = VROMPAGE( dwChrBank[ 4 ] % ( S.NesHeader.VROMSize << 3 ) );
      W.PPUBANK[ 5 ] = VROMPAGE( dwChrBank[ 5 ] % ( S.NesHeader.VROMSize << 3 ) );
      W.PPUBANK[ 6 ] = VROMPAGE( dwChrBank[ 6 ] % ( S.NesHeader.VROMSize << 3 ) );
      W.PPUBANK[ 7 ] = VROMPAGE( dwChrBank[ 7 ] % ( S.NesHeader.VROMSize << 3 ) );
      NESCore_Develop_Character_Data();
      break;
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 90 Sync Program Banks Function                            */
/*-------------------------------------------------------------------*/
void Map90_Sync_Prg_Banks( void )
{
  switch ( Map90_Prg_Bank_Size )
  {
    case 0:
      W.ROMBANK0 = ROMLASTPAGE( 3 );
      W.ROMBANK1 = ROMLASTPAGE( 2 );
      W.ROMBANK2 = ROMLASTPAGE( 1 );
      W.ROMBANK3 = ROMLASTPAGE( 0 );      
      break;

    case 1:
      W.ROMBANK0 = ROMPAGE( ( ( Map90_Prg_Reg[ 1 ] << 1 ) + 0 ) % ( S.NesHeader.ROMSize << 1 ) );
      W.ROMBANK1 = ROMPAGE( ( ( Map90_Prg_Reg[ 1 ] << 1 ) + 1 ) % ( S.NesHeader.ROMSize << 1 ) );
      W.ROMBANK2 = ROMLASTPAGE( 1 );
      W.ROMBANK3 = ROMLASTPAGE( 0 );      
      break;

    case 2:
      if ( Map90_Prg_Bank_E000 )
      {
        W.ROMBANK0 = ROMPAGE( Map90_Prg_Reg[ 0 ] % ( S.NesHeader.ROMSize << 1 ) );
        W.ROMBANK1 = ROMPAGE( Map90_Prg_Reg[ 1 ] % ( S.NesHeader.ROMSize << 1 ) );
        W.ROMBANK2 = ROMPAGE( Map90_Prg_Reg[ 2 ] % ( S.NesHeader.ROMSize << 1 ) );
        W.ROMBANK3 = ROMPAGE( Map90_Prg_Reg[ 3 ] % ( S.NesHeader.ROMSize << 1 ) );   
      } else {
        if ( Map90_Prg_Bank_6000 )
        {
          W.SRAMBANK = ROMPAGE( Map90_Prg_Reg[ 3 ] % ( S.NesHeader.ROMSize << 1 ) );
        }
        W.ROMBANK0 = ROMPAGE( Map90_Prg_Reg[ 0 ] % ( S.NesHeader.ROMSize << 1 ) );
        W.ROMBANK1 = ROMPAGE( Map90_Prg_Reg[ 1 ] % ( S.NesHeader.ROMSize << 1 ) );
        W.ROMBANK2 = ROMPAGE( Map90_Prg_Reg[ 2 ] % ( S.NesHeader.ROMSize << 1 ) );
        W.ROMBANK3 = ROMLASTPAGE( 0 );  
      }
      break;

    default:
      /* 8k in reverse mode? */
      W.ROMBANK0 = ROMPAGE( Map90_Prg_Reg[ 3 ] % ( S.NesHeader.ROMSize << 1 ) );
      W.ROMBANK1 = ROMPAGE( Map90_Prg_Reg[ 2 ] % ( S.NesHeader.ROMSize << 1 ) );
      W.ROMBANK2 = ROMPAGE( Map90_Prg_Reg[ 1 ] % ( S.NesHeader.ROMSize << 1 ) );
      W.ROMBANK3 = ROMPAGE( Map90_Prg_Reg[ 0 ] % ( S.NesHeader.ROMSize << 1 ) ); 
      break;
  }
}
