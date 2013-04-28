/*===================================================================*/
/*                                                                   */
/*                         Mapper 185  (Tecmo)                       */
/*                                                                   */
/*===================================================================*/

byte Map185_Dummy_Chr_Rom[ 0x400 ];

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 185                                            */
/*-------------------------------------------------------------------*/
void Map185_Init()
{
  /* Initialize Mapper */
  MapperInit = Map185_Init;

  /* Write to Mapper */
  MapperWrite = Map185_Write;

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
  W.ROMBANK2 = ROMPAGE( 2 );
  W.ROMBANK3 = ROMPAGE( 3 );

  /* Initialize Dummy W.VROM */
  int nPage ;
for (nPage = 0; nPage < 0x400; nPage++ )
  {
    Map185_Dummy_Chr_Rom[ nPage ] = 0xff;
  }


 
}

/*-------------------------------------------------------------------*/
/*  Mapper 185 Write Function                                        */
/*-------------------------------------------------------------------*/
void Map185_Write( word wAddr, byte byData )
{
  /* Set PPU Banks */ 
  if ( byData & 0x03 )
  {
    W.PPUBANK[ 0 ] = VROMPAGE( 0 );
    W.PPUBANK[ 1 ] = VROMPAGE( 1 );
    W.PPUBANK[ 2 ] = VROMPAGE( 2 );
    W.PPUBANK[ 3 ] = VROMPAGE( 3 );
    W.PPUBANK[ 4 ] = VROMPAGE( 4 );
    W.PPUBANK[ 5 ] = VROMPAGE( 5 );
    W.PPUBANK[ 6 ] = VROMPAGE( 6 );
    W.PPUBANK[ 7 ] = VROMPAGE( 7 );
    NESCore_Develop_Character_Data();
  } else {
    W.PPUBANK[ 0 ] = Map185_Dummy_Chr_Rom;
    W.PPUBANK[ 1 ] = Map185_Dummy_Chr_Rom;
    W.PPUBANK[ 2 ] = Map185_Dummy_Chr_Rom;
    W.PPUBANK[ 3 ] = Map185_Dummy_Chr_Rom;
    W.PPUBANK[ 4 ] = Map185_Dummy_Chr_Rom;
    W.PPUBANK[ 5 ] = Map185_Dummy_Chr_Rom;
    W.PPUBANK[ 6 ] = Map185_Dummy_Chr_Rom;
    W.PPUBANK[ 7 ] = Map185_Dummy_Chr_Rom;
    NESCore_Develop_Character_Data();
  }
}
