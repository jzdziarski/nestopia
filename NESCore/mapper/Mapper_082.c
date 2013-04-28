/*===================================================================*/
/*                                                                   */
/*                        Mapper 82 (Taito X1-17)                    */
/*                                                                   */
/*===================================================================*/

byte Map82_Regs[ 1 ];

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 82                                             */
/*-------------------------------------------------------------------*/
void Map82_Init()
{
  /* Initialize Mapper */
  MapperInit = Map82_Init;

  /* Write to Mapper */
  MapperWrite = Map0_Write;

  /* Write to SRAM */
  MapperSram = Map82_Sram;

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

  /* Set PPU Banks */
  if ( S.NesHeader.VROMSize > 0 )
  {
    int nPage ;
for (nPage = 0; nPage < 8; ++nPage )
      W.PPUBANK[ nPage ] = VROMPAGE( nPage );
    NESCore_Develop_Character_Data();
  }

  /* Name Table Mirroring */
  NESCore_Mirroring( 1 );


 
}

/*-------------------------------------------------------------------*/
/*  Mapper 82 Write to SRAM Function                                 */
/*-------------------------------------------------------------------*/
void Map82_Sram( word wAddr, byte byData )
{
  switch ( wAddr )
  {
    /* Set PPU Banks */
    case 0x7ef0:
      byData &= 0xfe;
      byData %= ( S.NesHeader.VROMSize << 3 );

      if ( Map82_Regs[ 0 ] )
      {
        W.PPUBANK[ 4 ] = VROMPAGE( byData );
        W.PPUBANK[ 5 ] = VROMPAGE( byData + 1 );
      } else {
        W.PPUBANK[ 0 ] = VROMPAGE( byData );
        W.PPUBANK[ 1 ] = VROMPAGE( byData + 1 );
      }
      NESCore_Develop_Character_Data();
      break;

    case 0x7ef1:
      byData &= 0xfe;
      byData %= ( S.NesHeader.VROMSize << 3 );

      if ( Map82_Regs[ 0 ] )
      {
        W.PPUBANK[ 6 ] = VROMPAGE( byData );
        W.PPUBANK[ 7 ] = VROMPAGE( byData + 1 );
      } else {
        W.PPUBANK[ 2 ] = VROMPAGE( byData );
        W.PPUBANK[ 3 ] = VROMPAGE( byData + 1 );
      }
      NESCore_Develop_Character_Data();
      break;
  
    case 0x7ef2:
      byData %= ( S.NesHeader.VROMSize << 3 );
      
      if ( !Map82_Regs[ 0 ] )
      {
        W.PPUBANK[ 4 ] = VROMPAGE( byData );
      } else {
        W.PPUBANK[ 0 ] = VROMPAGE( byData );
      }
      NESCore_Develop_Character_Data();
      break;    
      
    case 0x7ef3:
      byData %= ( S.NesHeader.VROMSize << 3 );
      
      if ( !Map82_Regs[ 0 ] )
      {
        W.PPUBANK[ 5 ] = VROMPAGE( byData );
      } else {
        W.PPUBANK[ 1 ] = VROMPAGE( byData );
      }
      NESCore_Develop_Character_Data();
      break;  

    case 0x7ef4:
      byData %= ( S.NesHeader.VROMSize << 3 );
      
      if ( !Map82_Regs[ 0 ] )
      {
        W.PPUBANK[ 6 ] = VROMPAGE( byData );
      } else {
        W.PPUBANK[ 2 ] = VROMPAGE( byData );
      }
      NESCore_Develop_Character_Data();
      break;  

    case 0x7ef5:
      byData %= ( S.NesHeader.VROMSize << 3 );
      
      if ( !Map82_Regs[ 0 ] )
      {
        W.PPUBANK[ 7 ] = VROMPAGE( byData );
      } else {
        W.PPUBANK[ 3 ] = VROMPAGE( byData );
      }
      NESCore_Develop_Character_Data();
      break;  

    /* Name Table Mirroring */
    case 0x7ef6:
      Map82_Regs[ 0 ] = byData & 0x02;
      
      if ( byData & 0x01 )
      {
        NESCore_Mirroring( 1 );
      } else {
        NESCore_Mirroring( 0 );
      }

    /* Set ROM Banks */
    case 0x7efa:
      byData >>= 2;
      byData %= ( S.NesHeader.ROMSize << 1 );
      W.ROMBANK0 = ROMPAGE( byData );
      break;

    case 0x7efb:
      byData >>= 2;
      byData %= ( S.NesHeader.ROMSize << 1 );
      W.ROMBANK1 = ROMPAGE( byData );
      break;

    case 0x7efc:
      byData >>= 2;
      byData %= ( S.NesHeader.ROMSize << 1 );
      W.ROMBANK2 = ROMPAGE( byData );
      break;
  }
}
