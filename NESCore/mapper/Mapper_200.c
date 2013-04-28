/*===================================================================*/
/*                                                                   */
/*                   Mapper 200 (1200-in-1)                          */
/*                                                                   */
/*===================================================================*/

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 200                                            */
/*-------------------------------------------------------------------*/
void Map200_Init()
{
  /* Initialize Mapper */
  MapperInit = Map200_Init;

  /* Write to Mapper */
  MapperWrite = Map200_Write;

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
/*  Mapper 200 Write Function                                        */
/*-------------------------------------------------------------------*/
void Map200_Write( word wAddr, byte byData )
{
	/* Set ROM Banks */
  W.ROMBANK0 = ROMPAGE((((wAddr&0x07)<<1)+0) % (S.NesHeader.ROMSize<<1));
  W.ROMBANK1 = ROMPAGE((((wAddr&0x07)<<1)+1) % (S.NesHeader.ROMSize<<1));
  W.ROMBANK2 = ROMPAGE((((wAddr&0x07)<<1)+0) % (S.NesHeader.ROMSize<<1));
  W.ROMBANK3 = ROMPAGE((((wAddr&0x07)<<1)+1) % (S.NesHeader.ROMSize<<1));

	/* Set PPU Banks */
	W.PPUBANK[0] = VROMPAGE((((wAddr&0x07)<<3)+0) % (S.NesHeader.VROMSize<<3));
	W.PPUBANK[1] = VROMPAGE((((wAddr&0x07)<<3)+1) % (S.NesHeader.VROMSize<<3));
	W.PPUBANK[2] = VROMPAGE((((wAddr&0x07)<<3)+2) % (S.NesHeader.VROMSize<<3));
	W.PPUBANK[3] = VROMPAGE((((wAddr&0x07)<<3)+3) % (S.NesHeader.VROMSize<<3));
	W.PPUBANK[4] = VROMPAGE((((wAddr&0x07)<<3)+4) % (S.NesHeader.VROMSize<<3));
	W.PPUBANK[5] = VROMPAGE((((wAddr&0x07)<<3)+5) % (S.NesHeader.VROMSize<<3));
	W.PPUBANK[6] = VROMPAGE((((wAddr&0x07)<<3)+6) % (S.NesHeader.VROMSize<<3));
	W.PPUBANK[7] = VROMPAGE((((wAddr&0x07)<<3)+7) % (S.NesHeader.VROMSize<<3));
	NESCore_Develop_Character_Data();

	if ( wAddr & 0x01 ) {
		NESCore_Mirroring( 1 );
	} else {
		NESCore_Mirroring( 0 );
	}
}
