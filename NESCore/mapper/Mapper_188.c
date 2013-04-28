/*===================================================================*/
/*                                                                   */
/*                        Mapper 188 (Bandai)                        */
/*                                                                   */
/*===================================================================*/

byte Map188_Dummy[ 0x2000 ];

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 188                                            */
/*-------------------------------------------------------------------*/
void Map188_Init()
{
  /* Initialize Mapper */
  MapperInit = Map188_Init;

  /* Write to Mapper */
  MapperWrite = Map188_Write;

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
  W.SRAMBANK = Map188_Dummy;

  /* Set ROM Banks */
  if ( ( S.NesHeader.ROMSize << 1 ) > 16 )
  {
    W.ROMBANK0 = ROMPAGE( 0 );
    W.ROMBANK1 = ROMPAGE( 1 );
    W.ROMBANK2 = ROMPAGE( 14 );
    W.ROMBANK3 = ROMPAGE( 15 );
  } else {
    W.ROMBANK0 = ROMPAGE( 0 );
    W.ROMBANK1 = ROMPAGE( 1 );
    W.ROMBANK2 = ROMLASTPAGE( 1 );
    W.ROMBANK3 = ROMLASTPAGE( 0 );
  }

  /* Magic Code */
  Map188_Dummy[ 0 ] = 0x03;


 
}

/*-------------------------------------------------------------------*/
/*  Mapper 188 Write Function                                        */
/*-------------------------------------------------------------------*/
void Map188_Write( word wAddr, byte byData )
{
  /* Set ROM Banks */
  if ( byData )
  {
    if ( byData & 0x10 )
    {
      byData = ( byData & 0x07 ) << 1;
      W.ROMBANK0 = ROMPAGE( ( byData + 0 ) % ( S.NesHeader.ROMSize << 1 ) );
      W.ROMBANK1 = ROMPAGE( ( byData + 1 ) % ( S.NesHeader.ROMSize << 1 ) );
    } else {
      byData <<= 1;
      W.ROMBANK0 = ROMPAGE( ( byData + 16 ) % ( S.NesHeader.ROMSize << 1 ) );
      W.ROMBANK1 = ROMPAGE( ( byData + 17 ) % ( S.NesHeader.ROMSize << 1 ) );
    }
  } 
  else 
  {
    if ( ( S.NesHeader.ROMSize << 1 ) == 0x10 )
    {
      W.ROMBANK0 = ROMPAGE( 14 );
      W.ROMBANK1 = ROMPAGE( 15 );
    } else {
      W.ROMBANK0 = ROMPAGE( 16 );
      W.ROMBANK1 = ROMPAGE( 17 );
    }
  }
}
