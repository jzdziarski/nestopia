/*===================================================================*/
/*                                                                   */
/*                 Mapper 246 : Phone Serm Berm                      */
/*                                                                   */
/*===================================================================*/

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 246                                            */
/*-------------------------------------------------------------------*/
void Map246_Init()
{
  /* Initialize Mapper */
  MapperInit = Map246_Init;

  /* Write to Mapper */
  MapperWrite = Map0_Write;

  /* Write to SRAM */
  MapperSram = Map246_Sram;

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


 
}

/*-------------------------------------------------------------------*/
/*  Mapper 246 Write to SRAM Function                                */
/*-------------------------------------------------------------------*/
void Map246_Sram( word wAddr, byte byData )
{
  if( wAddr>=0x6000 && wAddr<0x8000 ) {
    switch( wAddr ) {
    case 0x6000:
      W.ROMBANK0 = ROMPAGE(((byData<<2)+0) % (S.NesHeader.ROMSize<<1));
      break;
    case 0x6001:
      W.ROMBANK1 = ROMPAGE(((byData<<2)+1) % (S.NesHeader.ROMSize<<1));
      break;
    case 0x6002:
      W.ROMBANK2 = ROMPAGE(((byData<<2)+2) % (S.NesHeader.ROMSize<<1));
      break;
    case 0x6003: 
      W.ROMBANK3 = ROMPAGE(((byData<<2)+3) % (S.NesHeader.ROMSize<<1));
      break;
    case 0x6004:
      W.PPUBANK[ 0 ] = VROMPAGE(((byData<<1)+0) % (S.NesHeader.VROMSize<<3)); 
      W.PPUBANK[ 1 ] = VROMPAGE(((byData<<1)+1) % (S.NesHeader.VROMSize<<3)); 
      NESCore_Develop_Character_Data();
      break;
    case 0x6005:
      W.PPUBANK[ 2 ] = VROMPAGE(((byData<<1)+0) % (S.NesHeader.VROMSize<<3)); 
      W.PPUBANK[ 3 ] = VROMPAGE(((byData<<1)+1) % (S.NesHeader.VROMSize<<3)); 
      NESCore_Develop_Character_Data();
      break;
    case 0x6006:
      W.PPUBANK[ 4 ] = VROMPAGE(((byData<<1)+0) % (S.NesHeader.VROMSize<<3)); 
      W.PPUBANK[ 5 ] = VROMPAGE(((byData<<1)+1) % (S.NesHeader.VROMSize<<3)); 
      NESCore_Develop_Character_Data();
      break;
    case 0x6007:
      W.PPUBANK[ 6 ] = VROMPAGE(((byData<<1)+0) % (S.NesHeader.VROMSize<<3)); 
      W.PPUBANK[ 7 ] = VROMPAGE(((byData<<1)+1) % (S.NesHeader.VROMSize<<3)); 
      NESCore_Develop_Character_Data();
      break;
    default:
      W.SRAMBANK[wAddr&0x1FFF] = byData;
      break;
    }
  }
}
