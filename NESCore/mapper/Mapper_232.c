/*===================================================================*/
/*                                                                   */
/*                   Mapper 232 : Quattro Games                      */
/*                                                                   */
/*===================================================================*/

byte Map232_Regs[2];

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 232                                            */
/*-------------------------------------------------------------------*/
void Map232_Init()
{
  /* Initialize Mapper */
  MapperInit = Map232_Init;

  /* Write to Mapper */
  MapperWrite = Map232_Write;

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
  W.ROMBANK2 = ROMLASTPAGE( 1 );
  W.ROMBANK3 = ROMLASTPAGE( 0 );

  /* Initialize Registers */
  Map232_Regs[0] = 0x0C;
  Map232_Regs[1] = 0x00;
  

 
}

/*-------------------------------------------------------------------*/
/*  Mapper 232 Write Function                                        */
/*-------------------------------------------------------------------*/
void Map232_Write( word wAddr, byte byData )
{
  if ( wAddr == 0x9000 ) {
    Map232_Regs[0] = (byData & 0x18)>>1;
  } else if ( 0xA000 <= wAddr /* && wAddr <= 0xFFFF */ ) {
    Map232_Regs[1] = byData & 0x03;
  }
  
  W.ROMBANK0= ROMPAGE((((Map232_Regs[0]|Map232_Regs[1])<<1)+0) % (S.NesHeader.ROMSize<<1));
  W.ROMBANK1= ROMPAGE((((Map232_Regs[0]|Map232_Regs[1])<<1)+1) % (S.NesHeader.ROMSize<<1));
  W.ROMBANK2= ROMPAGE((((Map232_Regs[0]|0x03)<<1)+0) % (S.NesHeader.ROMSize<<1));
  W.ROMBANK3= ROMPAGE((((Map232_Regs[0]|0x03)<<1)+1) % (S.NesHeader.ROMSize<<1));
}
