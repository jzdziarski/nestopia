/*===================================================================*/
/*                                                                   */
/*        Mapper 105 : Nintendo World Championship                   */
/*                                                                   */
/*===================================================================*/

byte	Map105_Init_State;
byte	Map105_Write_Count;
byte	Map105_Bits;
byte	Map105_Reg[4];

byte	Map105_IRQ_Enable;
int	Map105_IRQ_Counter;

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 105                                            */
/*-------------------------------------------------------------------*/
void Map105_Init()
{
  /* Initialize Mapper */
  MapperInit = Map105_Init;

  /* Write to Mapper */
  MapperWrite = Map105_Write;

  /* Write to SRAM */
  MapperSram = Map0_Sram;

  /* Write to APU */
  MapperApu = Map0_Apu;

  /* Read from APU */
  MapperReadApu = Map0_ReadApu;

  /* Callback at VSync */
  MapperVSync = Map0_VSync;

  /* Callback at HSync */
  MapperHSync = Map105_HSync;

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
  Map105_Reg[0] = 0x0C;
  Map105_Reg[1] = 0x00;
  Map105_Reg[2] = 0x00;
  Map105_Reg[3] = 0x10;
  
  Map105_Bits = 0;
  Map105_Write_Count = 0;
  
  Map105_IRQ_Counter = 0;
  Map105_IRQ_Enable = 0;
  Map105_Init_State = 0;


 
}

/*-------------------------------------------------------------------*/
/*  Mapper 105 Write Function                                        */
/*-------------------------------------------------------------------*/
void Map105_Write( word wAddr, byte byData )
{
  word reg_num = (wAddr & 0x7FFF) >> 13;

  if( byData & 0x80 ) {
    Map105_Bits = Map105_Write_Count = 0;
    if( reg_num == 0 ) {
      Map105_Reg[reg_num] |= 0x0C;
    }
  } else {
    Map105_Bits |= (byData & 1) << Map105_Write_Count++;
    if( Map105_Write_Count == 5) {
      Map105_Reg[reg_num] = Map105_Bits & 0x1F;
      Map105_Bits = Map105_Write_Count = 0;
    }
  }

  if( Map105_Reg[0] & 0x02 ) {
    if( Map105_Reg[0] & 0x01 ) {
      NESCore_Mirroring( 0 );
    } else {
      NESCore_Mirroring( 1 );
    }
  } else {
    if( Map105_Reg[0] & 0x01 ) {
      NESCore_Mirroring( 3 );
    } else {
      NESCore_Mirroring( 2 );
    }
  }
  
  switch( Map105_Init_State ) {
  case 0:
  case 1:
    Map105_Init_State++;
    break;

  case 2:
    if(Map105_Reg[1] & 0x08) {
      if (Map105_Reg[0] & 0x08) {
	if (Map105_Reg[0] & 0x04) {
	  W.ROMBANK0 = ROMPAGE( ((Map105_Reg[3] & 0x07) * 2 + 16) % ( S.NesHeader.ROMSize << 1 ) );
	  W.ROMBANK1 = ROMPAGE( ((Map105_Reg[3] & 0x07) * 2 + 17) % ( S.NesHeader.ROMSize << 1 ) );
	  W.ROMBANK2 = ROMPAGE( 30 % ( S.NesHeader.ROMSize << 1 ) );
	  W.ROMBANK3 = ROMPAGE( 31 % ( S.NesHeader.ROMSize << 1 ) );
	} else {
	  W.ROMBANK0 = ROMPAGE( 16 % ( S.NesHeader.ROMSize << 1 ) );
	  W.ROMBANK1 = ROMPAGE( 17 % ( S.NesHeader.ROMSize << 1 ) );
	  W.ROMBANK2 = ROMPAGE( ((Map105_Reg[3] & 0x07) * 2 + 16) % ( S.NesHeader.ROMSize << 1 ) );
	  W.ROMBANK3 = ROMPAGE( ((Map105_Reg[3] & 0x07) * 2 + 17) % ( S.NesHeader.ROMSize << 1 ) );
	}
      } else {
	W.ROMBANK0 = ROMPAGE( ((Map105_Reg[3] & 0x06) * 2 + 16) % ( S.NesHeader.ROMSize << 1 ) );
	W.ROMBANK1 = ROMPAGE( ((Map105_Reg[3] & 0x06) * 2 + 17) % ( S.NesHeader.ROMSize << 1 ) );
	W.ROMBANK2 = ROMPAGE( ((Map105_Reg[3] & 0x06) * 2 + 18) % ( S.NesHeader.ROMSize << 1 ) );
	W.ROMBANK3 = ROMPAGE( ((Map105_Reg[3] & 0x06) * 2 + 19) % ( S.NesHeader.ROMSize << 1 ) );
      }
    } else {
      W.ROMBANK0 = ROMPAGE( ((Map105_Reg[1] & 0x06) * 2 + 0) % ( S.NesHeader.ROMSize << 1 ) );
      W.ROMBANK1 = ROMPAGE( ((Map105_Reg[1] & 0x06) * 2 + 1) % ( S.NesHeader.ROMSize << 1 ) );
      W.ROMBANK2 = ROMPAGE( ((Map105_Reg[1] & 0x06) * 2 + 2) % ( S.NesHeader.ROMSize << 1 ) );
      W.ROMBANK3 = ROMPAGE( ((Map105_Reg[1] & 0x06) * 2 + 3) % ( S.NesHeader.ROMSize << 1 ) );
    }
    
    if( Map105_Reg[1] & 0x10 ) {
      Map105_IRQ_Counter = 0;
      Map105_IRQ_Enable = 0;
    } else {
      Map105_IRQ_Enable = 1;
    }
    break;

  default:
    break;
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 105 H-Sync Function                                       */
/*-------------------------------------------------------------------*/
void Map105_HSync()
{
  if( !S.PPU_Scanline ) {
    if( Map105_IRQ_Enable ) {
      Map105_IRQ_Counter += 29781;
    }
    if( ((Map105_IRQ_Counter | 0x21FFFFFF) & 0x3E000000) == 0x3E000000 ) {
      IRQ_REQ;
    }
  }
}

