/*===================================================================*/
/*                                                                   */
/*                   Mapper 202 (150-in-1)                           */
/*                                                                   */
/*===================================================================*/

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 202                                            */
/*-------------------------------------------------------------------*/
void Map202_Init()
{
  /* Initialize Mapper */
  MapperInit = Map202_Init;

  /* Write to Mapper */
  MapperWrite = Map202_Write;

  /* Write to SRAM */
  MapperSram = Map0_Sram;

  /* Write to APU */
  MapperApu = Map202_Apu;

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
  W.ROMBANK0 = ROMPAGE( 12 );
  W.ROMBANK1 = ROMPAGE( 13 );
  W.ROMBANK2 = ROMPAGE( 14 );
  W.ROMBANK3 = ROMPAGE( 15 );

  /* Set PPU Banks */
  if ( S.NesHeader.VROMSize > 0 ) {
    int nPage ;
for (nPage = 0; nPage < 8; ++nPage )
      W.PPUBANK[ nPage ] = VROMPAGE( nPage );
    NESCore_Develop_Character_Data();
  }


 
}

/*-------------------------------------------------------------------*/
/*  Mapper 202 Write to APU Function                                 */
/*-------------------------------------------------------------------*/
void Map202_Apu( word wAddr, byte byData )
{
	Map202_WriteSub( wAddr, byData );
}

/*-------------------------------------------------------------------*/
/*  Mapper 202 Write Function                                        */
/*-------------------------------------------------------------------*/
void Map202_Write( word wAddr, byte byData )
{
	Map202_WriteSub( wAddr, byData );
}

/*-------------------------------------------------------------------*/
/*  Mapper 202 Write Sub Function                                    */
/*-------------------------------------------------------------------*/
void Map202_WriteSub( word wAddr, byte byData )
{
	int bank = (wAddr>>1) & 0x07;

	/* Set ROM Banks */
	W.ROMBANK0 = ROMPAGE(((bank<<1)+0) % (S.NesHeader.ROMSize<<1));
	W.ROMBANK1 = ROMPAGE(((bank<<1)+1) % (S.NesHeader.ROMSize<<1));

	if ((wAddr&0x0c) == 0x0c) {
		W.ROMBANK2 = ROMPAGE((((bank+1)<<1)+0) % (S.NesHeader.ROMSize<<1));
		W.ROMBANK3 = ROMPAGE((((bank+1)<<1)+1) % (S.NesHeader.ROMSize<<1));
	} else {		
		W.ROMBANK2 = ROMPAGE((((bank+0)<<1)+0) % (S.NesHeader.ROMSize<<1));
		W.ROMBANK3 = ROMPAGE((((bank+0)<<1)+1) % (S.NesHeader.ROMSize<<1));
	}	

	/* Set PPU Banks */
	W.PPUBANK[0] = VROMPAGE(((bank<<3)+0) % (S.NesHeader.VROMSize<<3));
	W.PPUBANK[1] = VROMPAGE(((bank<<3)+1) % (S.NesHeader.VROMSize<<3));
	W.PPUBANK[2] = VROMPAGE(((bank<<3)+2) % (S.NesHeader.VROMSize<<3));
	W.PPUBANK[3] = VROMPAGE(((bank<<3)+3) % (S.NesHeader.VROMSize<<3));
	W.PPUBANK[4] = VROMPAGE(((bank<<3)+4) % (S.NesHeader.VROMSize<<3));
	W.PPUBANK[5] = VROMPAGE(((bank<<3)+5) % (S.NesHeader.VROMSize<<3));
	W.PPUBANK[6] = VROMPAGE(((bank<<3)+6) % (S.NesHeader.VROMSize<<3));
	W.PPUBANK[7] = VROMPAGE(((bank<<3)+7) % (S.NesHeader.VROMSize<<3));
	NESCore_Develop_Character_Data();

	if ( wAddr & 0x01 ) {
		NESCore_Mirroring( 0 );
	} else {
		NESCore_Mirroring( 1 );
	}
}

