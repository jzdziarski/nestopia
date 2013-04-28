/*===================================================================*/
/*                                                                   */
/*                     Mapper 234 : Maxi-15                          */
/*                                                                   */
/*===================================================================*/

byte	Map234_Reg[2];

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 234                                            */
/*-------------------------------------------------------------------*/
void Map234_Init()
{
  /* Initialize Mapper */
  MapperInit = Map234_Init;

  /* Write to Mapper */
  MapperWrite = Map234_Write;

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

  /* Set Registers */
  Map234_Reg[0] = 0;
  Map234_Reg[1] = 0;


 
}

/*-------------------------------------------------------------------*/
/*  Mapper 234 Write Function                                        */
/*-------------------------------------------------------------------*/
void Map234_Write( word wAddr, byte byData )
{
  if( wAddr >= 0xFF80 && wAddr <= 0xFF9F ) {
    if( !Map234_Reg[0] ) {
      Map234_Reg[0] = byData;
      Map234_Set_Banks();
    }
  }
  
  if( wAddr >= 0xFFE8 && wAddr <= 0xFFF7 ) {
    Map234_Reg[1] = byData;
    Map234_Set_Banks();
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 234 Set Banks Function                                    */
/*-------------------------------------------------------------------*/
void Map234_Set_Banks()
{
  byte byPrg, byChr;

  if( Map234_Reg[0] & 0x80 ) {
    NESCore_Mirroring( 0 );
  } else {
    NESCore_Mirroring( 1 );
  }
  if( Map234_Reg[0] & 0x40 ) {
    byPrg = (Map234_Reg[0]&0x0E)|(Map234_Reg[1]&0x01);
    byChr = ((Map234_Reg[0]&0x0E)<<2)|((Map234_Reg[1]>>4)&0x07);
  } else {
    byPrg = Map234_Reg[0]&0x0F;
    byChr = ((Map234_Reg[0]&0x0F)<<2)|((Map234_Reg[1]>>4)&0x03);
  }
  
  /* Set ROM Banks */
  W.ROMBANK0 = ROMPAGE(((byPrg<<2)+0) % (S.NesHeader.ROMSize<<1));
  W.ROMBANK1 = ROMPAGE(((byPrg<<2)+1) % (S.NesHeader.ROMSize<<1));
  W.ROMBANK2 = ROMPAGE(((byPrg<<2)+2) % (S.NesHeader.ROMSize<<1));
  W.ROMBANK3 = ROMPAGE(((byPrg<<2)+3) % (S.NesHeader.ROMSize<<1));

  /* Set PPU Banks */
  W.PPUBANK[ 0 ] = VROMPAGE(((byChr<<3)+0) % (S.NesHeader.VROMSize<<3));
  W.PPUBANK[ 1 ] = VROMPAGE(((byChr<<3)+1) % (S.NesHeader.VROMSize<<3));
  W.PPUBANK[ 2 ] = VROMPAGE(((byChr<<3)+2) % (S.NesHeader.VROMSize<<3));
  W.PPUBANK[ 3 ] = VROMPAGE(((byChr<<3)+3) % (S.NesHeader.VROMSize<<3));
  W.PPUBANK[ 4 ] = VROMPAGE(((byChr<<3)+4) % (S.NesHeader.VROMSize<<3));
  W.PPUBANK[ 5 ] = VROMPAGE(((byChr<<3)+5) % (S.NesHeader.VROMSize<<3));
  W.PPUBANK[ 6 ] = VROMPAGE(((byChr<<3)+6) % (S.NesHeader.VROMSize<<3));
  W.PPUBANK[ 7 ] = VROMPAGE(((byChr<<3)+7) % (S.NesHeader.VROMSize<<3));
  NESCore_Develop_Character_Data();
}


