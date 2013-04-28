/*===================================================================*/
/*                                                                   */
/*                 Mapper 183 : Gimmick (Bootleg)                    */
/*                                                                   */
/*===================================================================*/

byte	Map183_Reg[8];
byte	Map183_IRQ_Enable;
int	Map183_IRQ_Counter;

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 183                                            */
/*-------------------------------------------------------------------*/
void Map183_Init()
{
  /* Initialize Mapper */
  MapperInit = Map183_Init;

  /* Write to Mapper */
  MapperWrite = Map183_Write;

  /* Write to SRAM */
  MapperSram = Map0_Sram;

  /* Write to APU */
  MapperApu = Map0_Apu;

  /* Read from APU */
  MapperReadApu = Map0_ReadApu;

  /* Callback at VSync */
  MapperVSync = Map0_VSync;

  /* Callback at HSync */
  MapperHSync = Map183_HSync;

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
  if ( S.NesHeader.VROMSize > 0 ) {
    int nPage ;
for (nPage = 0; nPage < 8; ++nPage )
      W.PPUBANK[ nPage ] = VROMPAGE( nPage );
    NESCore_Develop_Character_Data();
  }

  /* Initialize Registers */
  int i ;
for (i = 0; i < 8; i++ ) {
    Map183_Reg[i] = i;
  }
  Map183_IRQ_Enable = 0;
  Map183_IRQ_Counter = 0;


 
}

/*-------------------------------------------------------------------*/
/*  Mapper 183 Write Function                                        */
/*-------------------------------------------------------------------*/
void Map183_Write( word wAddr, byte byData )
{
  switch( wAddr ) {
  case	0x8800:
    W.ROMBANK0 = ROMPAGE( byData % (S.NesHeader.ROMSize << 1) );
    break;
  case	0xA800:
    W.ROMBANK1 = ROMPAGE( byData % (S.NesHeader.ROMSize << 1) );
    break;
  case	0xA000:
    W.ROMBANK2 = ROMPAGE( byData % (S.NesHeader.ROMSize << 1) );
    break;
    
  case	0xB000:
    Map183_Reg[0] = (Map183_Reg[0]&0xF0)|(byData&0x0F);
    W.PPUBANK[ 0 ] = VROMPAGE( Map183_Reg[0] % (S.NesHeader.VROMSize << 3) );
    NESCore_Develop_Character_Data();
    break;
  case	0xB004:
    Map183_Reg[0] = (Map183_Reg[0]&0x0F)|((byData&0x0F)<<4);
    W.PPUBANK[ 0 ] = VROMPAGE( Map183_Reg[0] % (S.NesHeader.VROMSize << 3) );
    NESCore_Develop_Character_Data();
    break;
  case	0xB008:
    Map183_Reg[1] = (Map183_Reg[1]&0xF0)|(byData&0x0F);
    W.PPUBANK[ 1 ] = VROMPAGE( Map183_Reg[1] % (S.NesHeader.VROMSize << 3) );
    NESCore_Develop_Character_Data();
    break;
  case	0xB00C:
    Map183_Reg[1] = (Map183_Reg[1]&0x0F)|((byData&0x0F)<<4);
    W.PPUBANK[ 1 ] = VROMPAGE( Map183_Reg[1] % (S.NesHeader.VROMSize << 3) );
    NESCore_Develop_Character_Data();
    break;
    
  case	0xC000:
    Map183_Reg[2] = (Map183_Reg[2]&0xF0)|(byData&0x0F);
    W.PPUBANK[ 2 ] = VROMPAGE( Map183_Reg[2] % (S.NesHeader.VROMSize << 3) );
    NESCore_Develop_Character_Data();
    break;
  case	0xC004:
    Map183_Reg[2] = (Map183_Reg[2]&0x0F)|((byData&0x0F)<<4);
    W.PPUBANK[ 2 ] = VROMPAGE( Map183_Reg[2] % (S.NesHeader.VROMSize << 3) );
    NESCore_Develop_Character_Data();
    break;
  case	0xC008:
    Map183_Reg[3] = (Map183_Reg[3]&0xF0)|(byData&0x0F);
    W.PPUBANK[ 3 ] = VROMPAGE( Map183_Reg[3] % (S.NesHeader.VROMSize << 3) );
    NESCore_Develop_Character_Data();
    break;
  case	0xC00C:
    Map183_Reg[3] = (Map183_Reg[3]&0x0F)|((byData&0x0F)<<4);
    W.PPUBANK[ 3 ] = VROMPAGE( Map183_Reg[3] % (S.NesHeader.VROMSize << 3) );
    NESCore_Develop_Character_Data();
    break;
    
  case	0xD000:
    Map183_Reg[4] = (Map183_Reg[4]&0xF0)|(byData&0x0F);
    W.PPUBANK[ 4 ] = VROMPAGE( Map183_Reg[4] % (S.NesHeader.VROMSize << 3) );
    NESCore_Develop_Character_Data();
    break;
  case	0xD004:
    Map183_Reg[4] = (Map183_Reg[4]&0x0F)|((byData&0x0F)<<4);
    W.PPUBANK[ 4 ] = VROMPAGE( Map183_Reg[4] % (S.NesHeader.VROMSize << 3) );
    NESCore_Develop_Character_Data();
    break;
  case	0xD008:
    Map183_Reg[5] = (Map183_Reg[5]&0xF0)|(byData&0x0F);
    W.PPUBANK[ 5 ] = VROMPAGE( Map183_Reg[5] % (S.NesHeader.VROMSize << 3) );
    NESCore_Develop_Character_Data();
    break;
  case	0xD00C:
    Map183_Reg[5] = (Map183_Reg[5]&0x0F)|((byData&0x0F)<<4);
    W.PPUBANK[ 5 ] = VROMPAGE( Map183_Reg[5] % (S.NesHeader.VROMSize << 3) );
    NESCore_Develop_Character_Data();
    break;
    
  case	0xE000:
    Map183_Reg[6] = (Map183_Reg[6]&0xF0)|(byData&0x0F);
    W.PPUBANK[ 6 ] = VROMPAGE( Map183_Reg[6] % (S.NesHeader.VROMSize << 3) );
    NESCore_Develop_Character_Data();
    break;
  case	0xE004:
    Map183_Reg[6] = (Map183_Reg[6]&0x0F)|((byData&0x0F)<<4);
    W.PPUBANK[ 6 ] = VROMPAGE( Map183_Reg[6] % (S.NesHeader.VROMSize << 3) );
    NESCore_Develop_Character_Data();
    break;
  case	0xE008:
    Map183_Reg[7] = (Map183_Reg[3]&0xF0)|(byData&0x0F);
    W.PPUBANK[ 7 ] = VROMPAGE( Map183_Reg[7] % (S.NesHeader.VROMSize << 3) );
    NESCore_Develop_Character_Data();
    break;
  case	0xE00C:
    Map183_Reg[7] = (Map183_Reg[3]&0x0F)|((byData&0x0F)<<4);
    W.PPUBANK[ 7 ] = VROMPAGE( Map183_Reg[7] % (S.NesHeader.VROMSize << 3) );
    NESCore_Develop_Character_Data();
    break;
    
  case	0x9008:
    if( byData == 1 ) {
      int i ;
for (i = 0; i < 8; i++ ) {
	Map183_Reg[i] = i;
      }
      /* Set ROM Banks */
      W.ROMBANK0 = ROMPAGE( 0 );
      W.ROMBANK1 = ROMPAGE( 1 );
      W.ROMBANK2 = ROMLASTPAGE( 1 );
      W.ROMBANK3 = ROMLASTPAGE( 0 );

      /* Set PPU Banks */
      if ( S.NesHeader.VROMSize > 0 ) {
	W.PPUBANK[ 0 ] = VROMPAGE( 0 );
	W.PPUBANK[ 1 ] = VROMPAGE( 1 );
	W.PPUBANK[ 2 ] = VROMPAGE( 2 );
	W.PPUBANK[ 3 ] = VROMPAGE( 3 );
	W.PPUBANK[ 4 ] = VROMPAGE( 4 );
	W.PPUBANK[ 5 ] = VROMPAGE( 5 );
	W.PPUBANK[ 6 ] = VROMPAGE( 6 );
	W.PPUBANK[ 7 ] = VROMPAGE( 7 );
	NESCore_Develop_Character_Data();
      }
    }
    break;
    
  case	0x9800:
    if( byData == 0 )      NESCore_Mirroring( 1 );
    else if( byData == 1 ) NESCore_Mirroring( 0 );
    else if( byData == 2 ) NESCore_Mirroring( 2 );
    else if( byData == 3 ) NESCore_Mirroring( 3 );
    break;
    
  case	0xF000:
    Map183_IRQ_Counter = (Map183_IRQ_Counter&0xFF00)|byData;
    break;
  case	0xF004:
    Map183_IRQ_Counter = (Map183_IRQ_Counter&0x00FF)|(byData<<8);
    break;
  case	0xF008:
    Map183_IRQ_Enable = byData;
    break;
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 183 H-Sync Function                                       */
/*-------------------------------------------------------------------*/
void Map183_HSync()
{
  if( Map183_IRQ_Enable & 0x02 ) {
    if( Map183_IRQ_Counter <= 113 ) {
      Map183_IRQ_Counter = 0;
      IRQ_REQ;
    } else {
      Map183_IRQ_Counter -= 113;
    }
  }
}
