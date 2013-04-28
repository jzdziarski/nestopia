/*===================================================================*/
/*                                                                   */
/*                          Mapper 251                               */
/*                                                                   */
/*===================================================================*/

byte	Map251_Reg[11];
byte	Map251_Breg[4];

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 251                                            */
/*-------------------------------------------------------------------*/
void Map251_Init()
{
  /* Initialize Mapper */
  MapperInit = Map251_Init;

  /* Write to Mapper */
  MapperWrite = Map251_Write;

  /* Write to SRAM */
  MapperSram = Map251_Sram;

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
  W.ROMBANK2 = ROMLASTPAGE( 1 );
  W.ROMBANK3 = ROMLASTPAGE( 0 );

  /* Set Registers */
  NESCore_Mirroring( 1 );

  int	i;
  for( i = 0; i < 11; i++ )
    Map251_Reg[i] = 0;
  for( i = 0; i < 4; i++ )
    Map251_Breg[i] = 0;


 
}

/*-------------------------------------------------------------------*/
/*  Mapper 251 Write Function                                        */
/*-------------------------------------------------------------------*/
void Map251_Write( word wAddr, byte byData )
{
  switch( wAddr & 0xE001 ) {
  case	0x8000:
    Map251_Reg[8] = byData;
    Map251_Set_Banks();
    break;
  case	0x8001:
    Map251_Reg[Map251_Reg[8]&0x07] = byData;
    Map251_Set_Banks();
    break;
  case	0xA001:
    if( byData & 0x80 ) {
      Map251_Reg[ 9] = 1;
      Map251_Reg[10] = 0;
    } else {
      Map251_Reg[ 9] = 0;
    }
    break;
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 251 Write to SRAM Function                                */
/*-------------------------------------------------------------------*/
void Map251_Sram( word wAddr, byte byData )
{
  if( (wAddr & 0xE001) == 0x6000 ) {
    if( Map251_Reg[9] ) {
      Map251_Breg[Map251_Reg[10]++] = byData;
      if( Map251_Reg[10] == 4 ) {
	Map251_Reg[10] = 0;
	Map251_Set_Banks();
      }
    }
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 251 Set Banks Function                                    */
/*-------------------------------------------------------------------*/
void Map251_Set_Banks()
{
  int	nChr[6];
  int	nPrg[4];

  int i ;
for (i = 0; i < 6; i++ ) {
    nChr[i] = (Map251_Reg[i]|(Map251_Breg[1]<<4)) & ((Map251_Breg[2]<<4)|0x0F);
  }

  if( Map251_Reg[8] & 0x80 ) {
    W.PPUBANK[ 0 ] = VROMPAGE(nChr[2] % (S.NesHeader.VROMSize<<3));
    W.PPUBANK[ 1 ] = VROMPAGE(nChr[3] % (S.NesHeader.VROMSize<<3));
    W.PPUBANK[ 2 ] = VROMPAGE(nChr[4] % (S.NesHeader.VROMSize<<3));
    W.PPUBANK[ 3 ] = VROMPAGE(nChr[5] % (S.NesHeader.VROMSize<<3));
    W.PPUBANK[ 4 ] = VROMPAGE(nChr[0] % (S.NesHeader.VROMSize<<3));
    W.PPUBANK[ 5 ] = VROMPAGE((nChr[0]+1) % (S.NesHeader.VROMSize<<3));
    W.PPUBANK[ 6 ] = VROMPAGE(nChr[1] % (S.NesHeader.VROMSize<<3));
    W.PPUBANK[ 7 ] = VROMPAGE((nChr[1]+1) % (S.NesHeader.VROMSize<<3));
    NESCore_Develop_Character_Data();
  } else {
    W.PPUBANK[ 0 ] = VROMPAGE(nChr[0] % (S.NesHeader.VROMSize<<3));
    W.PPUBANK[ 1 ] = VROMPAGE((nChr[0]+1) % (S.NesHeader.VROMSize<<3));
    W.PPUBANK[ 2 ] = VROMPAGE(nChr[1] % (S.NesHeader.VROMSize<<3));
    W.PPUBANK[ 3 ] = VROMPAGE((nChr[1]+1) % (S.NesHeader.VROMSize<<3));
    W.PPUBANK[ 4 ] = VROMPAGE(nChr[2] % (S.NesHeader.VROMSize<<3));
    W.PPUBANK[ 5 ] = VROMPAGE(nChr[3] % (S.NesHeader.VROMSize<<3));
    W.PPUBANK[ 6 ] = VROMPAGE(nChr[4] % (S.NesHeader.VROMSize<<3));
    W.PPUBANK[ 7 ] = VROMPAGE(nChr[5] % (S.NesHeader.VROMSize<<3));
    NESCore_Develop_Character_Data();
  }

  nPrg[0] = (Map251_Reg[6]&((Map251_Breg[3]&0x3F)^0x3F))|(Map251_Breg[1]);
  nPrg[1] = (Map251_Reg[7]&((Map251_Breg[3]&0x3F)^0x3F))|(Map251_Breg[1]);
  nPrg[2] = nPrg[3] =((Map251_Breg[3]&0x3F)^0x3F)|(Map251_Breg[1]);
  nPrg[2] &= (S.NesHeader.ROMSize<<1)-1;

  if( Map251_Reg[8] & 0x40 ) {
    W.ROMBANK0 = ROMPAGE(nPrg[2] % (S.NesHeader.ROMSize<<1));
    W.ROMBANK1 = ROMPAGE(nPrg[1] % (S.NesHeader.ROMSize<<1));
    W.ROMBANK2 = ROMPAGE(nPrg[0] % (S.NesHeader.ROMSize<<1));
    W.ROMBANK3 = ROMPAGE(nPrg[3] % (S.NesHeader.ROMSize<<1));
  } else { 
    W.ROMBANK0 = ROMPAGE(nPrg[0] % (S.NesHeader.ROMSize<<1));
    W.ROMBANK1 = ROMPAGE(nPrg[1] % (S.NesHeader.ROMSize<<1));
    W.ROMBANK2 = ROMPAGE(nPrg[2] % (S.NesHeader.ROMSize<<1));
    W.ROMBANK3 = ROMPAGE(nPrg[3] % (S.NesHeader.ROMSize<<1));
  }
}
