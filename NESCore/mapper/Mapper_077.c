/*===================================================================*/
/*                                                                   */
/*                Mapper 77  (Irem Early Mapper #0)                  */
/*                                                                   */
/*===================================================================*/

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 77                                             */
/*-------------------------------------------------------------------*/
void Map77_Init()
{
  /* Initialize Mapper */
  MapperInit = Map77_Init;

  /* Write to Mapper */
  MapperWrite = Map77_Write;

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

  /* VRAM Write Enabled */
  S.VRAMWriteEnable = 1;
}

/*-------------------------------------------------------------------*/
/*  Mapper 77 Write Function                                         */
/*-------------------------------------------------------------------*/
void Map77_Write( word wAddr, byte byData )
{
  byte byRomBank = byData & 0x07;
  byte byChrBank = ( byData & 0xf0 ) >> 4;

  /* Set ROM Banks */
  byRomBank <<= 2;
  byRomBank %= ( S.NesHeader.ROMSize << 1 );

  W.ROMBANK0 = ROMPAGE( byRomBank );
  W.ROMBANK1 = ROMPAGE( byRomBank + 1 );
  W.ROMBANK2 = ROMPAGE( byRomBank + 2 );
  W.ROMBANK3 = ROMPAGE( byRomBank + 3 );

  /* Set PPU Banks */
  byChrBank <<= 1;
  byChrBank %= ( S.NesHeader.VROMSize << 3 );

  W.PPUBANK[ 0 ] = VROMPAGE( byChrBank );
  W.PPUBANK[ 1 ] = VROMPAGE( byChrBank + 1 );
  NESCore_Develop_Character_Data();
}
