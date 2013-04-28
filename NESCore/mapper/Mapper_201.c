/*===================================================================*/
/*                                                                   */
/*                   Mapper 201 (21-in-1)                            */
/*                                                                   */
/*===================================================================*/

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 201                                            */
/*-------------------------------------------------------------------*/
void Map201_Init()
{
  /* Initialize Mapper */
  MapperInit = Map201_Init;

  /* Write to Mapper */
  MapperWrite = Map201_Write;

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
  W.ROMBANK2 = ROMPAGE( 0 );
  W.ROMBANK3 = ROMPAGE( 1 );

  /* Set PPU Banks */
  if ( S.NesHeader.VROMSize > 0 ) {
    int nPage ;
for (nPage = 0; nPage < 8; ++nPage )
      W.PPUBANK[ nPage ] = VROMPAGE( nPage );
    NESCore_Develop_Character_Data();
  }


 
}

/*-------------------------------------------------------------------*/
/*  Mapper 201 Write Function                                        */
/*-------------------------------------------------------------------*/
void Map201_Write( word wAddr, byte byData )
{
	byte byBank = (byte)wAddr & 0x03;
	if (!(wAddr&0x08) )
		byBank = 0;

	/* Set ROM Banks */
  W.ROMBANK0 = ROMPAGE(((byBank<<2)+0) % (S.NesHeader.ROMSize<<1));
  W.ROMBANK1 = ROMPAGE(((byBank<<2)+1) % (S.NesHeader.ROMSize<<1));
  W.ROMBANK2 = ROMPAGE(((byBank<<2)+2) % (S.NesHeader.ROMSize<<1));
  W.ROMBANK3 = ROMPAGE(((byBank<<2)+3) % (S.NesHeader.ROMSize<<1));

	/* Set PPU Banks */
	W.PPUBANK[0] = VROMPAGE(((byBank<<3)+0) % (S.NesHeader.VROMSize<<3));
	W.PPUBANK[1] = VROMPAGE(((byBank<<3)+1) % (S.NesHeader.VROMSize<<3));
	W.PPUBANK[2] = VROMPAGE(((byBank<<3)+2) % (S.NesHeader.VROMSize<<3));
	W.PPUBANK[3] = VROMPAGE(((byBank<<3)+3) % (S.NesHeader.VROMSize<<3));
	W.PPUBANK[4] = VROMPAGE(((byBank<<3)+4) % (S.NesHeader.VROMSize<<3));
	W.PPUBANK[5] = VROMPAGE(((byBank<<3)+5) % (S.NesHeader.VROMSize<<3));
	W.PPUBANK[6] = VROMPAGE(((byBank<<3)+6) % (S.NesHeader.VROMSize<<3));
	W.PPUBANK[7] = VROMPAGE(((byBank<<3)+7) % (S.NesHeader.VROMSize<<3));
	NESCore_Develop_Character_Data();
}

