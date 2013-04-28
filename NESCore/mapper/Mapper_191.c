/*===================================================================*/
/*                                                                   */
/* Mapper 191 : SACHEN Super Cartridge Xin1 (Ver.1-9), Q-BOY Support */
/*                                                                   */
/*===================================================================*/

byte	Map191_Reg[8];
byte	Map191_Prg0, Map191_Prg1;
byte	Map191_Chr0, Map191_Chr1, Map191_Chr2, Map191_Chr3;
byte	Map191_Highbank;

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 191                                            */
/*-------------------------------------------------------------------*/
void Map191_Init()
{
  /* Initialize Mapper */
  MapperInit = Map191_Init;

  /* Write to Mapper */
  MapperWrite = Map0_Write;

  /* Write to SRAM */
  MapperSram = Map0_Sram;

  /* Write to APU */
  MapperApu = Map191_Apu;

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

  /* Initialize Registers */
  int i ;
for (i = 0; i < 8; i++ ) {
    Map191_Reg[i] = 0x00;
  }

  /* Set ROM Banks */
  Map191_Prg0 = 0;
  // Map191_Prg1 = 1;
  Map191_Set_CPU_Banks();

  /* Set PPU Banks */
  Map191_Chr0 = 0;
  Map191_Chr1 = 0;
  Map191_Chr2 = 0;
  Map191_Chr3 = 0;
  Map191_Highbank = 0;
  Map191_Set_PPU_Banks();


 
}

/*-------------------------------------------------------------------*/
/*  Mapper 191 Write to APU Function                                 */
/*-------------------------------------------------------------------*/
void Map191_Apu( word wAddr, byte byData )
{
  switch( wAddr ) {
  case	0x4100:
    Map191_Reg[0]=byData;
    break;
  case	0x4101:
    Map191_Reg[1]=byData;
    switch( Map191_Reg[0] ) {
    case	0:
      Map191_Chr0=byData&7;
      Map191_Set_PPU_Banks();
      break;
    case	1:
      Map191_Chr1=byData&7;
      Map191_Set_PPU_Banks();
      break;
    case	2:
      Map191_Chr2=byData&7;
      Map191_Set_PPU_Banks();
      break;
    case	3:
      Map191_Chr3=byData&7;
      Map191_Set_PPU_Banks();
      break;
    case	4:
      Map191_Highbank=byData&7;
      Map191_Set_PPU_Banks();
      break;
    case	5:
      Map191_Prg0=byData&7;
      Map191_Set_CPU_Banks();
      break;
    case	7:
      if( byData & 0x02 ) NESCore_Mirroring( 0 );
      else		  NESCore_Mirroring( 1 );
      break;
    }
    break;
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 191 Set CPU Banks Function                                */
/*-------------------------------------------------------------------*/
void	Map191_Set_CPU_Banks()
{
  W.ROMBANK0 = ROMPAGE( ((Map191_Prg0<<2) + 0 ) % ( S.NesHeader.ROMSize << 1 ) );
  W.ROMBANK1 = ROMPAGE( ((Map191_Prg0<<2) + 1 ) % ( S.NesHeader.ROMSize << 1 ) );
  W.ROMBANK2 = ROMPAGE( ((Map191_Prg0<<2) + 2 ) % ( S.NesHeader.ROMSize << 1 ) );
  W.ROMBANK3 = ROMPAGE( ((Map191_Prg0<<2) + 3 ) % ( S.NesHeader.ROMSize << 1 ) );
};

/*-------------------------------------------------------------------*/
/*  Mapper 191 Set PPU Banks Function                                */
/*-------------------------------------------------------------------*/
void	Map191_Set_PPU_Banks()
{
  if ( S.NesHeader.VROMSize > 0 ) {
    W.PPUBANK[ 0 ] = VROMPAGE( ((((Map191_Highbank<<3)+Map191_Chr0)<<2)+0 ) % ( S.NesHeader.VROMSize << 3 ) );
    W.PPUBANK[ 1 ] = VROMPAGE( ((((Map191_Highbank<<3)+Map191_Chr0)<<2)+1 ) % ( S.NesHeader.VROMSize << 3 ) );
    W.PPUBANK[ 2 ] = VROMPAGE( ((((Map191_Highbank<<3)+Map191_Chr1)<<2)+2 ) % ( S.NesHeader.VROMSize << 3 ) );
    W.PPUBANK[ 3 ] = VROMPAGE( ((((Map191_Highbank<<3)+Map191_Chr1)<<2)+3 ) % ( S.NesHeader.VROMSize << 3 ) );
    W.PPUBANK[ 4 ] = VROMPAGE( ((((Map191_Highbank<<3)+Map191_Chr2)<<2)+0 ) % ( S.NesHeader.VROMSize << 3 ) );
    W.PPUBANK[ 5 ] = VROMPAGE( ((((Map191_Highbank<<3)+Map191_Chr2)<<2)+1 ) % ( S.NesHeader.VROMSize << 3 ) );
    W.PPUBANK[ 6 ] = VROMPAGE( ((((Map191_Highbank<<3)+Map191_Chr3)<<2)+2 ) % ( S.NesHeader.VROMSize << 3 ) );
    W.PPUBANK[ 7 ] = VROMPAGE( ((((Map191_Highbank<<3)+Map191_Chr3)<<2)+3 ) % ( S.NesHeader.VROMSize << 3 ) );
    NESCore_Develop_Character_Data();
  }
}
