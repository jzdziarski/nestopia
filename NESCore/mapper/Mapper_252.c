/*===================================================================*/
/*                                                                   */
/*              Mapper 252 : Sangokushi - Chuugen no hasha           */
/*                                                                   */
/*===================================================================*/

byte	Map252_Reg[9];
byte	Map252_IRQ_Enable;
byte	Map252_IRQ_Counter;
byte	Map252_IRQ_Latch;
byte	Map252_IRQ_Occur;
int	Map252_IRQ_Clock;

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 252                                            */
/*-------------------------------------------------------------------*/
void Map252_Init()
{
  /* Initialize Mapper */
  MapperInit = Map252_Init;

  /* Write to Mapper */
  MapperWrite = Map252_Write;

  /* Write to SRAM */
  MapperSram = Map0_Sram;

  /* Write to APU */
  MapperApu = Map0_Apu;

  /* Read from APU */
  MapperReadApu = Map0_ReadApu;

  /* Callback at VSync */
  MapperVSync = Map0_VSync;

  /* Callback at HSync */
  MapperHSync = Map252_HSync;

  /* Callback at PPU */
  MapperPPU = Map0_PPU;

  /* Callback at Rendering Screen ( 1:BG, 0:Sprite ) */
  MapperRenderScreen = Map0_RenderScreen;

  /* Set SRAM Banks */
  W.SRAMBANK = S.SRAM;

  /* Set Registers */
  int i ;
for (i = 0; i < 9; i++ ) {
    Map252_Reg[i] = i;
  }

  Map252_IRQ_Enable = 0;
  Map252_IRQ_Counter = 0;
  Map252_IRQ_Latch = 0;
  Map252_IRQ_Clock = 0;
  Map252_IRQ_Occur = 0;

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


 
}

/*-------------------------------------------------------------------*/
/*  Mapper 252 Write Function                                        */
/*-------------------------------------------------------------------*/
void Map252_Write( word wAddr, byte byData )
{
  if( (wAddr & 0xF000) == 0x8000 ) {
    W.ROMBANK0 = ROMPAGE( byData % (S.NesHeader.ROMSize<<1));
    return;
  }
  if( (wAddr & 0xF000) == 0xA000 ) {
    W.ROMBANK1 = ROMPAGE( byData % (S.NesHeader.ROMSize<<1));
    return;
  }
  switch( wAddr & 0xF00C ) {
  case 0xB000:
    Map252_Reg[0] = (Map252_Reg[0] & 0xF0) | (byData & 0x0F);
    W.PPUBANK[ 0 ] = VROMPAGE(Map252_Reg[0] % (S.NesHeader.VROMSize<<3));
    NESCore_Develop_Character_Data();
    break;	
  case 0xB004:
    Map252_Reg[0] = (Map252_Reg[0] & 0x0F) | ((byData & 0x0F) << 4);
    W.PPUBANK[ 0 ] = VROMPAGE(Map252_Reg[0] % (S.NesHeader.VROMSize<<3));
    NESCore_Develop_Character_Data();
    break;
  case 0xB008:
    Map252_Reg[1] = (Map252_Reg[1] & 0xF0) | (byData & 0x0F);
    W.PPUBANK[ 1 ] = VROMPAGE(Map252_Reg[1] % (S.NesHeader.VROMSize<<3));
    NESCore_Develop_Character_Data();
    break;
  case 0xB00C:
    Map252_Reg[1] = (Map252_Reg[1] & 0x0F) | ((byData & 0x0F) << 4);
    W.PPUBANK[ 1 ] = VROMPAGE(Map252_Reg[1] % (S.NesHeader.VROMSize<<3));
    NESCore_Develop_Character_Data();
    break;
    
  case 0xC000:
    Map252_Reg[2] = (Map252_Reg[2] & 0xF0) | (byData & 0x0F);
    W.PPUBANK[ 2 ] = VROMPAGE(Map252_Reg[2] % (S.NesHeader.VROMSize<<3));
    NESCore_Develop_Character_Data();
    break;
  case 0xC004:
    Map252_Reg[2] = (Map252_Reg[2] & 0x0F) | ((byData & 0x0F) << 4);
    W.PPUBANK[ 2 ] = VROMPAGE(Map252_Reg[2] % (S.NesHeader.VROMSize<<3));
    NESCore_Develop_Character_Data();
    break;
  case 0xC008:
    Map252_Reg[3] = (Map252_Reg[3] & 0xF0) | (byData & 0x0F);
    W.PPUBANK[ 3 ] = VROMPAGE(Map252_Reg[3] % (S.NesHeader.VROMSize<<3));
    NESCore_Develop_Character_Data();
    break;
  case 0xC00C:
    Map252_Reg[3] = (Map252_Reg[3] & 0x0F) | ((byData & 0x0F) << 4);
    W.PPUBANK[ 3 ] = VROMPAGE(Map252_Reg[3] % (S.NesHeader.VROMSize<<3));
    NESCore_Develop_Character_Data();
    break;
    
  case 0xD000:
    Map252_Reg[4] = (Map252_Reg[4] & 0xF0) | (byData & 0x0F);
    W.PPUBANK[ 4 ] = VROMPAGE(Map252_Reg[4] % (S.NesHeader.VROMSize<<3));
    NESCore_Develop_Character_Data();
    break;
  case 0xD004:
    Map252_Reg[4] = (Map252_Reg[4] & 0x0F) | ((byData & 0x0F) << 4);
    W.PPUBANK[ 4 ] = VROMPAGE(Map252_Reg[4] % (S.NesHeader.VROMSize<<3));
    NESCore_Develop_Character_Data();
    break;
  case 0xD008:
    Map252_Reg[5] = (Map252_Reg[5] & 0xF0) | (byData & 0x0F);
    W.PPUBANK[ 5 ] = VROMPAGE(Map252_Reg[5] % (S.NesHeader.VROMSize<<3));
    NESCore_Develop_Character_Data();
    break;
  case 0xD00C:
    Map252_Reg[5] = (Map252_Reg[5] & 0x0F) | ((byData & 0x0F) << 4);
    W.PPUBANK[ 5 ] = VROMPAGE(Map252_Reg[5] % (S.NesHeader.VROMSize<<3));
    NESCore_Develop_Character_Data();
    break;
    
  case 0xE000:
    Map252_Reg[6] = (Map252_Reg[6] & 0xF0) | (byData & 0x0F);
    W.PPUBANK[ 6 ] = VROMPAGE(Map252_Reg[6] % (S.NesHeader.VROMSize<<3));
    NESCore_Develop_Character_Data();
    break;
  case 0xE004:
    Map252_Reg[6] = (Map252_Reg[6] & 0x0F) | ((byData & 0x0F) << 4);
    W.PPUBANK[ 6 ] = VROMPAGE(Map252_Reg[6] % (S.NesHeader.VROMSize<<3));
    NESCore_Develop_Character_Data();
    break;
  case 0xE008:
    Map252_Reg[7] = (Map252_Reg[7] & 0xF0) | (byData & 0x0F);
    W.PPUBANK[ 7 ] = VROMPAGE(Map252_Reg[7] % (S.NesHeader.VROMSize<<3));
    NESCore_Develop_Character_Data();
    break;
  case 0xE00C:
    Map252_Reg[7] = (Map252_Reg[7] & 0x0F) | ((byData & 0x0F) << 4);
    W.PPUBANK[ 7 ] = VROMPAGE(Map252_Reg[7] % (S.NesHeader.VROMSize<<3));
    NESCore_Develop_Character_Data();
    break;
    
  case 0xF000:
    Map252_IRQ_Latch = (Map252_IRQ_Latch & 0xF0) | (byData & 0x0F);
    Map252_IRQ_Occur = 0;
    break;
  case 0xF004:
    Map252_IRQ_Latch = (Map252_IRQ_Latch & 0x0F) | ((byData & 0x0F) << 4);
    Map252_IRQ_Occur = 0;
    break;
    
  case 0xF008:
    Map252_IRQ_Enable = byData & 0x03;
    if( Map252_IRQ_Enable & 0x02 ) {
      Map252_IRQ_Counter = Map252_IRQ_Latch;
      Map252_IRQ_Clock = 0;
    }
    Map252_IRQ_Occur = 0;
    break;
    
  case 0xF00C:
    Map252_IRQ_Enable = (Map252_IRQ_Enable & 0x01) * 3;
    Map252_IRQ_Occur = 0;
    break;
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 252 H-Sync Function                                       */
/*-------------------------------------------------------------------*/
void Map252_HSync()
{
  if( Map252_IRQ_Enable & 0x02 ) {
    if( Map252_IRQ_Counter == 0xFF ) {
      Map252_IRQ_Occur = 0xFF;
      Map252_IRQ_Counter = Map252_IRQ_Latch;
      Map252_IRQ_Enable = (Map252_IRQ_Enable & 0x01) * 3;
    } else {
      Map252_IRQ_Counter++;
    }
  }
  if( Map252_IRQ_Occur ) {
    IRQ_REQ;
  }
}

