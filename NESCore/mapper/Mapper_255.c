/*===================================================================*/
/*                                                                   */
/*                     Mapper 255 : 110-in-1                         */
/*                                                                   */
/*===================================================================*/

byte    Map255_Reg[4];

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 255                                            */
/*-------------------------------------------------------------------*/
void Map255_Init()
{
  /* Initialize Mapper */
  MapperInit = Map255_Init;

  /* Write to Mapper */
  MapperWrite = Map255_Write;

  /* Write to SRAM */
  MapperSram = Map0_Sram;

  /* Write to APU */
  MapperApu = Map255_Apu;

  /* Read from APU */
  MapperReadApu = Map255_ReadApu;

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

  /* Set PPU Banks */
  if ( S.NesHeader.VROMSize > 0 ) {
    int nPage ;
for (nPage = 0; nPage < 8; ++nPage )
      W.PPUBANK[ nPage ] = VROMPAGE( nPage );
    NESCore_Develop_Character_Data();
  }

  /* Set Registers */
  NESCore_Mirroring( 1 );

  Map255_Reg[0] = 0;
  Map255_Reg[1] = 0;
  Map255_Reg[2] = 0;
  Map255_Reg[3] = 0;


 
}

/*-------------------------------------------------------------------*/
/*  Mapper 255 Write Function                                        */
/*-------------------------------------------------------------------*/
void Map255_Write( word wAddr, byte byData )
{
  byte	byPrg = (wAddr & 0x0F80)>>7;
  int	nChr = (wAddr & 0x003F);
  int	nBank = (wAddr & 0x4000)>>14;

  if( wAddr & 0x2000 ) {
    NESCore_Mirroring( 0 );
  } else {
    NESCore_Mirroring( 1 );
  }

  if( wAddr & 0x1000 ) {
    if( wAddr & 0x0040 ) {
      W.ROMBANK0 = ROMPAGE((0x80*nBank+byPrg*4+2) % (S.NesHeader.ROMSize<<1));
      W.ROMBANK1 = ROMPAGE((0x80*nBank+byPrg*4+3) % (S.NesHeader.ROMSize<<1));
      W.ROMBANK2 = ROMPAGE((0x80*nBank+byPrg*4+2) % (S.NesHeader.ROMSize<<1));
      W.ROMBANK3 = ROMPAGE((0x80*nBank+byPrg*4+3) % (S.NesHeader.ROMSize<<1));
    } else {
      W.ROMBANK0 = ROMPAGE((0x80*nBank+byPrg*4+0) % (S.NesHeader.ROMSize<<1));
      W.ROMBANK1 = ROMPAGE((0x80*nBank+byPrg*4+1) % (S.NesHeader.ROMSize<<1));
      W.ROMBANK2 = ROMPAGE((0x80*nBank+byPrg*4+0) % (S.NesHeader.ROMSize<<1));
      W.ROMBANK3 = ROMPAGE((0x80*nBank+byPrg*4+1) % (S.NesHeader.ROMSize<<1));
    }
  } else {
    W.ROMBANK0 = ROMPAGE((0x80*nBank+byPrg*4+0) % (S.NesHeader.ROMSize<<1));
    W.ROMBANK1 = ROMPAGE((0x80*nBank+byPrg*4+1) % (S.NesHeader.ROMSize<<1));
    W.ROMBANK2 = ROMPAGE((0x80*nBank+byPrg*4+2) % (S.NesHeader.ROMSize<<1));
    W.ROMBANK3 = ROMPAGE((0x80*nBank+byPrg*4+3) % (S.NesHeader.ROMSize<<1));
  }
  
  W.PPUBANK[ 0 ] = VROMPAGE((0x200*nBank+nChr*8+0) % (S.NesHeader.VROMSize<<3));
  W.PPUBANK[ 1 ] = VROMPAGE((0x200*nBank+nChr*8+1) % (S.NesHeader.VROMSize<<3));
  W.PPUBANK[ 2 ] = VROMPAGE((0x200*nBank+nChr*8+2) % (S.NesHeader.VROMSize<<3));
  W.PPUBANK[ 3 ] = VROMPAGE((0x200*nBank+nChr*8+3) % (S.NesHeader.VROMSize<<3));
  W.PPUBANK[ 4 ] = VROMPAGE((0x200*nBank+nChr*8+4) % (S.NesHeader.VROMSize<<3));
  W.PPUBANK[ 5 ] = VROMPAGE((0x200*nBank+nChr*8+5) % (S.NesHeader.VROMSize<<3));
  W.PPUBANK[ 6 ] = VROMPAGE((0x200*nBank+nChr*8+6) % (S.NesHeader.VROMSize<<3));
  W.PPUBANK[ 7 ] = VROMPAGE((0x200*nBank+nChr*8+7) % (S.NesHeader.VROMSize<<3));
}

/*-------------------------------------------------------------------*/
/*  Mapper 255 Write to APU Function                                 */
/*-------------------------------------------------------------------*/
void Map255_Apu( word wAddr, byte byData )
{
  if( wAddr >= 0x5800 ) {
    Map255_Reg[wAddr&0x0003] = byData & 0x0F;
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 255 Read from APU Function                                */
/*-------------------------------------------------------------------*/
byte Map255_ReadApu( word wAddr )
{
  if( wAddr >= 0x5800 ) {
    return	Map255_Reg[wAddr&0x0003] & 0x0F;
  } else {
    return	wAddr>>8;
  }
}
