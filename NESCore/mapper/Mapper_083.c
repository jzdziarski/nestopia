/*===================================================================*/
/*                                                                   */
/*                        Mapper 83 (Pirates)                        */
/*                                                                   */
/*===================================================================*/

byte Map83_Regs[3];
dword Map83_Chr_Bank;
dword Map83_IRQ_Cnt;
byte Map83_IRQ_Enabled;

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 83                                             */
/*-------------------------------------------------------------------*/
void Map83_Init()
{
  /* Initialize Mapper */
  MapperInit = Map83_Init;

  /* Write to Mapper */
  MapperWrite = Map83_Write;

  /* Write to SRAM */
  MapperSram = Map0_Sram;

  /* Write to APU */
  MapperApu = Map83_Apu;

  /* Read from APU */
  MapperReadApu = Map83_ReadApu;

  /* Callback at VSync */
  MapperVSync = Map0_VSync;

  /* Callback at HSync */
  MapperHSync = Map83_HSync;

  /* Callback at PPU */
  MapperPPU = Map0_PPU;

  /* Callback at Rendering Screen ( 1:BG, 0:Sprite ) */
  MapperRenderScreen = Map0_RenderScreen;

  /* Set SRAM Banks */
  W.SRAMBANK = S.SRAM;

  /* Initialize Registers */
  Map83_Regs[0] = Map83_Regs[1] = Map83_Regs[2] = 0;

  /* Set ROM Banks */
  if ( ( S.NesHeader.ROMSize << 1 ) >= 32 )
  {
    W.ROMBANK0 = ROMPAGE( 0 );
    W.ROMBANK1 = ROMPAGE( 1 );
    W.ROMBANK2 = ROMPAGE( 30 );
    W.ROMBANK3 = ROMPAGE( 31 );
    Map83_Regs[ 1 ] = 0x30;
  }
  else
  {
    W.ROMBANK0 = ROMPAGE( 0 );
    W.ROMBANK1 = ROMPAGE( 1 );
    W.ROMBANK2 = ROMLASTPAGE( 1 );
    W.ROMBANK3 = ROMLASTPAGE( 0 );
  }

  /* Set PPU Banks */
  if ( S.NesHeader.VROMSize > 0 )
  {
    int nPage ;
for (nPage = 0; nPage < 8; ++nPage )
      W.PPUBANK[ nPage ] = VROMPAGE( nPage );
    NESCore_Develop_Character_Data();
  }

  /* Initialize IRQ Registers */
  Map83_IRQ_Enabled = 0;
  Map83_IRQ_Cnt = 0;
  Map83_Chr_Bank = 0;


 
}

/*-------------------------------------------------------------------*/
/*  Mapper 83 Read from APU Function                                 */
/*-------------------------------------------------------------------*/
byte Map83_ReadApu( word wAddr )
{
  if ( ( wAddr & 0x5100 ) == 0x5100 )
  {
    return Map83_Regs[2];
  }
  else
  {
    return wAddr >> 8;
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 83 Write to APU Function                                  */
/*-------------------------------------------------------------------*/
void Map83_Apu( word wAddr, byte byData )
{
  switch(wAddr)
  {
    case 0x5101:
    case 0x5102:
    case 0x5103:
      Map83_Regs[2] = byData;
      break;
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 83 Write Function                                         */
/*-------------------------------------------------------------------*/
void Map83_Write( word wAddr, byte byData )
{
  /* Set ROM Banks */
  switch( wAddr )
  {
    case 0x8000:
    case 0xB000:
    case 0xB0FF:
    case 0xB1FF:
      Map83_Regs[0] = byData;
      Map83_Chr_Bank = (byData & 0x30) << 4;

      W.ROMBANK0 = ROMPAGE( (byData*2+0) % (S.NesHeader.ROMSize << 1) );
      W.ROMBANK1 = ROMPAGE( (byData*2+1) % (S.NesHeader.ROMSize << 1) );
      W.ROMBANK2 = ROMPAGE( (((byData&0x30)|0x0F)*2+0) % (S.NesHeader.ROMSize << 1) );
      W.ROMBANK3 = ROMPAGE( (((byData&0x30)|0x0F)*2+1) % (S.NesHeader.ROMSize << 1) );
      break;

    case 0x8100:
      if ( S.NesHeader.VROMSize <= 32 )
      {
	Map83_Regs[1] = byData;
      }
      if ((byData & 0x03) == 0x00)
      {
	NESCore_Mirroring( 1 );
      }
      else if((byData & 0x03) == 0x01)
      {
	NESCore_Mirroring( 0 );
      }
      else if((byData & 0x03) == 0x02)
      {
	NESCore_Mirroring( 3 );
      }
      else
      {
	NESCore_Mirroring( 2 );
      }
      break;

    case 0x8200:
      Map83_IRQ_Cnt = ( Map83_IRQ_Cnt & 0xFF00 ) | (dword)byData;
      break;

    case 0x8201:
      Map83_IRQ_Cnt = ( Map83_IRQ_Cnt & 0x00FF ) | ((dword)byData << 8);
      Map83_IRQ_Enabled = byData;
      break;

    case 0x8300:
      W.ROMBANK0 = ROMPAGE( byData % (S.NesHeader.ROMSize << 1) );	
      break;

    case 0x8301:
      W.ROMBANK1 = ROMPAGE( byData % (S.NesHeader.ROMSize << 1) );	
      break;

    case 0x8302:
      W.ROMBANK2 = ROMPAGE( byData % (S.NesHeader.ROMSize << 1) );	
      break;

    case 0x8310:
      if ((Map83_Regs[1] & 0x30) == 0x30)
      {
	W.PPUBANK[ 0 ] = VROMPAGE( (Map83_Chr_Bank^byData) % (S.NesHeader.VROMSize << 3) );
	NESCore_Develop_Character_Data();
      }
      else if((Map83_Regs[1] & 0x30) == 0x10 || (Map83_Regs[1] & 0x30) == 0x20)
      { 
	W.PPUBANK[ 0 ] = VROMPAGE( (((Map83_Chr_Bank^byData)*2)+0) % (S.NesHeader.VROMSize << 3) );
	W.PPUBANK[ 1 ] = VROMPAGE( (((Map83_Chr_Bank^byData)*2)+1) % (S.NesHeader.VROMSize << 3) );
	NESCore_Develop_Character_Data();
      }
      break;

    case 0x8311:
      if ((Map83_Regs[1] & 0x30) == 0x30)
      {
	W.PPUBANK[ 1 ] = VROMPAGE( (Map83_Chr_Bank^byData) % (S.NesHeader.VROMSize << 3) );
	NESCore_Develop_Character_Data();
      }
      else if((Map83_Regs[1] & 0x30) == 0x10 || (Map83_Regs[1] & 0x30) == 0x20)
      {
	W.PPUBANK[ 2 ] = VROMPAGE( (((Map83_Chr_Bank^byData)*2)+0) % (S.NesHeader.VROMSize << 3) );
	W.PPUBANK[ 3 ] = VROMPAGE( (((Map83_Chr_Bank^byData)*2)+1) % (S.NesHeader.VROMSize << 3) );
	NESCore_Develop_Character_Data();
      }
      break;

    case 0x8312:
      if ((Map83_Regs[1] & 0x30) == 0x30)
      {
	W.PPUBANK[ 2 ] = VROMPAGE( (Map83_Chr_Bank^byData) % (S.NesHeader.VROMSize << 3) );
	NESCore_Develop_Character_Data();
      }
      else if((Map83_Regs[1] & 0x30) == 0x10 || (Map83_Regs[1] & 0x30) == 0x20)
      {
	W.PPUBANK[ 4 ] = VROMPAGE( (((Map83_Chr_Bank^byData)*2)+0) % (S.NesHeader.VROMSize << 3) );
	W.PPUBANK[ 5 ] = VROMPAGE( (((Map83_Chr_Bank^byData)*2)+1) % (S.NesHeader.VROMSize << 3) );
	NESCore_Develop_Character_Data();
      }
      break;

    case 0x8313:
      if ((Map83_Regs[1] & 0x30) == 0x30)
      {
	W.PPUBANK[ 3 ] = VROMPAGE( (Map83_Chr_Bank^byData) % (S.NesHeader.VROMSize << 3) );
	NESCore_Develop_Character_Data();
      }
      else if((Map83_Regs[1] & 0x30) == 0x10 || (Map83_Regs[1] & 0x30) == 0x20)
      {
	W.PPUBANK[ 6 ] = VROMPAGE( (((Map83_Chr_Bank^byData)*2)+0) % (S.NesHeader.VROMSize << 3) );
	W.PPUBANK[ 7 ] = VROMPAGE( (((Map83_Chr_Bank^byData)*2)+1) % (S.NesHeader.VROMSize << 3) );
	NESCore_Develop_Character_Data();
      }
      break;

    case 0x8314:
      if ((Map83_Regs[1] & 0x30) == 0x30)
      { 
	W.PPUBANK[ 4 ] = VROMPAGE( (Map83_Chr_Bank^byData) % (S.NesHeader.VROMSize << 3) );
	NESCore_Develop_Character_Data();
      }
      else if((Map83_Regs[1] & 0x30) == 0x10 || (Map83_Regs[1] & 0x30) == 0x20)
      {
	W.PPUBANK[ 4 ] = VROMPAGE( (((Map83_Chr_Bank^byData)*2)+0) % (S.NesHeader.VROMSize << 3) );
	W.PPUBANK[ 5 ] = VROMPAGE( (((Map83_Chr_Bank^byData)*2)+1) % (S.NesHeader.VROMSize << 3) );
	NESCore_Develop_Character_Data();
      }
      break;

    case 0x8315:
      if ((Map83_Regs[1] & 0x30) == 0x30)
      {
	W.PPUBANK[ 5 ] = VROMPAGE( (Map83_Chr_Bank^byData) % (S.NesHeader.VROMSize << 3) );
	NESCore_Develop_Character_Data();
      }
      else if ((Map83_Regs[1] & 0x30) == 0x10 || (Map83_Regs[1] & 0x30) == 0x20)
      {
	W.PPUBANK[ 6 ] = VROMPAGE( (((Map83_Chr_Bank^byData)*2)+0) % (S.NesHeader.VROMSize << 3) );
	W.PPUBANK[ 7 ] = VROMPAGE( (((Map83_Chr_Bank^byData)*2)+1) % (S.NesHeader.VROMSize << 3) );
	NESCore_Develop_Character_Data();
      }
      break;

    case 0x8316:
      if ((Map83_Regs[1] & 0x30) == 0x30)
      {
	W.PPUBANK[ 6 ] = VROMPAGE( (Map83_Chr_Bank^byData) % (S.NesHeader.VROMSize << 3) );
	NESCore_Develop_Character_Data();
      }
      else if ((Map83_Regs[1] & 0x30) == 0x10 || (Map83_Regs[1] & 0x30) == 0x20)
      {
	W.PPUBANK[ 4 ] = VROMPAGE( (((Map83_Chr_Bank^byData)*2)+0) % (S.NesHeader.VROMSize << 3) );
	W.PPUBANK[ 5 ] = VROMPAGE( (((Map83_Chr_Bank^byData)*2)+1) % (S.NesHeader.VROMSize << 3) );
	NESCore_Develop_Character_Data();
      }
      break;

    case 0x8317:
      if ((Map83_Regs[1] & 0x30) == 0x30)
      {
	W.PPUBANK[ 7 ] = VROMPAGE( (Map83_Chr_Bank^byData) % (S.NesHeader.VROMSize << 3) );
	NESCore_Develop_Character_Data();
      }
      else if ( (Map83_Regs[1] & 0x30) == 0x10 || (Map83_Regs[1] & 0x30) == 0x20)
      {
	W.PPUBANK[ 6 ] = VROMPAGE( (((Map83_Chr_Bank^byData)*2)+0) % (S.NesHeader.VROMSize << 3) );
	W.PPUBANK[ 7 ] = VROMPAGE( (((Map83_Chr_Bank^byData)*2)+1) % (S.NesHeader.VROMSize << 3) );
	NESCore_Develop_Character_Data();
      }
      break;

    case 0x8318:
      W.ROMBANK0 = ROMPAGE( (((Map83_Regs[0]&0x30)|byData)*2+0) % (S.NesHeader.ROMSize << 1) );	
      W.ROMBANK1 = ROMPAGE( (((Map83_Regs[0]&0x30)|byData)*2+1) % (S.NesHeader.ROMSize << 1) );	
      break;
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 83 H-Sync Function                                        */
/*-------------------------------------------------------------------*/
void Map83_HSync()
{
  if (Map83_IRQ_Enabled)
  {
    if (Map83_IRQ_Cnt <= 114)
    {
      IRQ_REQ;
      Map83_IRQ_Enabled = 0;
    }
    else
    {
      Map83_IRQ_Cnt -= 114;
    }
  }
}
