/*===================================================================*/
/*                                                                   */
/*                   Mapper 96 : Bandai 74161                        */
/*                                                                   */
/*===================================================================*/

byte	Map96_Reg[2];

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 96                                             */
/*-------------------------------------------------------------------*/
void Map96_Init()
{
  /* Initialize Mapper */
  MapperInit = Map96_Init;

  /* Write to Mapper */
  MapperWrite = Map96_Write;

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
  MapperPPU = Map96_PPU;

  /* Callback at Rendering Screen ( 1:BG, 0:Sprite ) */
  MapperRenderScreen = Map0_RenderScreen;

  /* Set SRAM Banks */
  W.SRAMBANK = S.SRAM;

  /* Set Registers */
  Map96_Reg[0] = Map96_Reg[1] = 0;

  /* Set ROM Banks */
  W.ROMBANK0 = ROMPAGE( 0 );
  W.ROMBANK1 = ROMPAGE( 1 );
  W.ROMBANK2 = ROMPAGE( 2 );
  W.ROMBANK3 = ROMPAGE( 3 );

  /* Set PPU Banks */
  Map96_Set_Banks();
  NESCore_Mirroring( 3 );


 
}

/*-------------------------------------------------------------------*/
/*  Mapper 96 Write Function                                         */
/*-------------------------------------------------------------------*/
void Map96_Write( word wAddr, byte byData )
{
  W.ROMBANK0 = ROMPAGE((((byData & 0x03)<<2)+0) % (S.NesHeader.ROMSize<<1));
  W.ROMBANK1 = ROMPAGE((((byData & 0x03)<<2)+1) % (S.NesHeader.ROMSize<<1));
  W.ROMBANK2 = ROMPAGE((((byData & 0x03)<<2)+2) % (S.NesHeader.ROMSize<<1));
  W.ROMBANK3 = ROMPAGE((((byData & 0x03)<<2)+3) % (S.NesHeader.ROMSize<<1));
  
  Map96_Reg[0] = (byData & 0x04) >> 2;
  Map96_Set_Banks();
}

/*-------------------------------------------------------------------*/
/*  Mapper 96 PPU Function                                           */
/*-------------------------------------------------------------------*/
void Map96_PPU( word wAddr )
{
  if( (wAddr & 0xF000) == 0x2000 ) {
    Map96_Reg[1] = (wAddr>>8)&0x03;
    Map96_Set_Banks();
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 96 Set Banks Function                                     */
/*-------------------------------------------------------------------*/
void Map96_Set_Banks()
{
  W.PPUBANK[ 0 ] = CRAMPAGE(((Map96_Reg[0]*4+Map96_Reg[1])<<2)+0);
  W.PPUBANK[ 1 ] = CRAMPAGE(((Map96_Reg[0]*4+Map96_Reg[1])<<2)+1);
  W.PPUBANK[ 2 ] = CRAMPAGE(((Map96_Reg[0]*4+Map96_Reg[1])<<2)+2);
  W.PPUBANK[ 3 ] = CRAMPAGE(((Map96_Reg[0]*4+Map96_Reg[1])<<2)+3);
  W.PPUBANK[ 4 ] = CRAMPAGE(((Map96_Reg[0]*4+0x03)<<2)+0);
  W.PPUBANK[ 5 ] = CRAMPAGE(((Map96_Reg[0]*4+0x03)<<2)+1);
  W.PPUBANK[ 6 ] = CRAMPAGE(((Map96_Reg[0]*4+0x03)<<2)+2);
  W.PPUBANK[ 7 ] = CRAMPAGE(((Map96_Reg[0]*4+0x03)<<2)+3);
  NESCore_Develop_Character_Data();
}

