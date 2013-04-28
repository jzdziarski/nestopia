/*

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public License
 as published by the Free Software Foundation; version 3
 of the License.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

byte	Map119_Reg[8];
byte	Map119_Prg0, Map119_Prg1;
byte	Map119_Chr01, Map119_Chr23, Map119_Chr4;
byte    Map119_Chr5,  Map119_Chr6,  Map119_Chr7;
byte	Map119_WeSram;

byte	Map119_IRQ_Enable;
byte	Map119_IRQ_Counter;
byte	Map119_IRQ_Latch;

void Map119_Init()
{
  MapperInit = Map119_Init;
  MapperWrite = Map119_Write;
  MapperSram = Map0_Sram;
  MapperApu = Map0_Apu;
  MapperReadApu = Map0_ReadApu;
  MapperVSync = Map0_VSync;
  MapperHSync = Map119_HSync;
  MapperPPU = Map0_PPU;
  MapperRenderScreen = Map0_RenderScreen;

  W.SRAMBANK = S.SRAM;

  memset(&Map119_Reg, 0, sizeof(Map119_Reg));

  Map119_Prg0 = 0;
  Map119_Prg1 = 1;
  Map119_Set_CPU_Banks();

  Map119_Chr01 = 0;
  Map119_Chr23 = 2;
  Map119_Chr4  = 4;
  Map119_Chr5  = 5;
  Map119_Chr6  = 6;
  Map119_Chr7  = 7;
  Map119_Set_PPU_Banks();

  Map119_WeSram = 0;
  Map119_IRQ_Enable = 0;
  Map119_IRQ_Counter = 0;
  Map119_IRQ_Latch = 0;
}

void Map119_Write( word wAddr, byte byData )
{
  switch( wAddr & 0xE001 ) {
  case	0x8000:
    Map119_Reg[0] = byData;
    Map119_Set_CPU_Banks();
    Map119_Set_PPU_Banks();
    break;
  case	0x8001:
    Map119_Reg[1] = byData;
    
    switch( Map119_Reg[0] & 0x07 ) {
    case	0x00:
      if( S.NesHeader.VROMSize > 0 ) {
	Map119_Chr01 = byData & 0xFE;
	Map119_Set_PPU_Banks();
      }
      break;
    case	0x01:
      if( S.NesHeader.VROMSize > 0 ) {
	Map119_Chr23 = byData & 0xFE;
	Map119_Set_PPU_Banks();
      }
      break;
    case	0x02:
      if( S.NesHeader.VROMSize > 0 ) {
	Map119_Chr4 = byData;
	Map119_Set_PPU_Banks();
      }
      break;
    case	0x03:
      if( S.NesHeader.VROMSize > 0 ) {
	Map119_Chr5 = byData;
	Map119_Set_PPU_Banks();
      }
      break;
    case	0x04:
      if( S.NesHeader.VROMSize > 0 ) {
	Map119_Chr6 = byData;
	Map119_Set_PPU_Banks();
      }
      break;
    case	0x05:
      if( S.NesHeader.VROMSize > 0 ) {
	Map119_Chr7 = byData;
	Map119_Set_PPU_Banks();
      }
      break;
    case	0x06:
      Map119_Prg0 = byData;
      Map119_Set_CPU_Banks();
      break;
    case	0x07:
      Map119_Prg1 = byData;
      Map119_Set_CPU_Banks();
      break;
    }
    break;
  case	0xA000:
    Map119_Reg[2] = byData;
    if (!S.ROM_FourScr) {
      if (byData & 0x01) NESCore_Mirroring( 0 );
      else		 NESCore_Mirroring( 1 );
    }
    break;
  case	0xA001:
    Map119_Reg[3] = byData;
    break;
  case	0xC000:
    Map119_Reg[4] = byData;
    Map119_IRQ_Counter = byData;
    break;
  case	0xC001:
    Map119_Reg[5] = byData;
    Map119_IRQ_Latch = byData;
    break;
  case	0xE000:
    Map119_Reg[6] = byData;
    Map119_IRQ_Enable = 0;
    Map119_IRQ_Counter = Map119_IRQ_Latch;
    break;
  case	0xE001:
    Map119_Reg[7] = byData;
    Map119_IRQ_Enable = 1;
    break;
  }
}

void Map119_HSync()
{
  if ((S.PPU_Scanline <= 239)) {
    if (S.PPU_R1 & R1_SHOW_BG || S.PPU_R1 & R1_SHOW_SP) {
      if (Map119_IRQ_Enable) {
	if (!(Map119_IRQ_Counter--) ) {
	  Map119_IRQ_Counter = Map119_IRQ_Latch;
	  IRQ_REQ;
	}
      }
    }
  }
}

void Map119_Set_CPU_Banks()
{
  if( Map119_Reg[0] & 0x40 ) {
    W.ROMBANK0 = ROMLASTPAGE( 1 );
    W.ROMBANK1 = ROMPAGE( Map119_Prg1 % ( S.NesHeader.ROMSize << 1 ) );
    W.ROMBANK2 = ROMPAGE( Map119_Prg0 % ( S.NesHeader.ROMSize << 1 ) );
    W.ROMBANK3 = ROMLASTPAGE( 0 );
  } else {
    W.ROMBANK0 = ROMPAGE( Map119_Prg0 % ( S.NesHeader.ROMSize << 1 ) );
    W.ROMBANK1 = ROMPAGE( Map119_Prg1 % ( S.NesHeader.ROMSize << 1 ) );
    W.ROMBANK2 = ROMLASTPAGE( 1 );
    W.ROMBANK3 = ROMLASTPAGE( 0 );
  }
}

void Map119_Set_PPU_Banks()
{
  if (Map119_Reg[0]&0x80) {
    if (Map119_Chr4 & 0x40)
         W.PPUBANK[ 0 ] = CRAMPAGE(Map119_Chr4&0x07);
    else
         W.PPUBANK[ 0 ] = VROMPAGE(Map119_Chr4 % (S.NesHeader.ROMSize<<1));
    if (Map119_Chr5 & 0x40)
         W.PPUBANK[ 1 ] = CRAMPAGE(Map119_Chr5&0x07);
    else
         W.PPUBANK[ 1 ] = VROMPAGE(Map119_Chr5 % (S.NesHeader.ROMSize<<1));
    if (Map119_Chr6 & 0x40)
         W.PPUBANK[ 2 ] = CRAMPAGE(Map119_Chr6&0x07);
    else
         W.PPUBANK[ 2 ] = VROMPAGE(Map119_Chr6 % (S.NesHeader.ROMSize<<1));
    if (Map119_Chr7 & 0x40)
         W.PPUBANK[ 3 ] = CRAMPAGE(Map119_Chr7&0x07);
    else
         W.PPUBANK[ 3 ] = VROMPAGE(Map119_Chr7 % (S.NesHeader.ROMSize<<1));
    if ((Map119_Chr01+0) & 0x40)
        W.PPUBANK[ 4 ] = CRAMPAGE((Map119_Chr01+0)&0x07);
    else
        W.PPUBANK[ 4 ] = VROMPAGE((Map119_Chr01+0) % (S.NesHeader.VROMSize<<3));
    if ((Map119_Chr01+1) & 0x40)
        W.PPUBANK[ 5 ] = CRAMPAGE((Map119_Chr01+1)&0x07);
    else
        W.PPUBANK[ 5 ] = VROMPAGE((Map119_Chr01+1) % (S.NesHeader.VROMSize<<3));
    if ((Map119_Chr23+0) & 0x40)
        W.PPUBANK[ 6 ] = CRAMPAGE((Map119_Chr23+0)&0x07);
    else
        W.PPUBANK[ 6 ] = VROMPAGE((Map119_Chr23+0) % (S.NesHeader.VROMSize<<3));
    if ((Map119_Chr23+1) & 0x40)
        W.PPUBANK[ 7 ] = CRAMPAGE((Map119_Chr23+1)&0x07);
    else
        W.PPUBANK[ 7 ] = VROMPAGE((Map119_Chr23+1) % (S.NesHeader.VROMSize<<3));
  } 
  else 
  {
    if ((Map119_Chr01+0) & 0x40)
        W.PPUBANK[ 0 ] = CRAMPAGE((Map119_Chr01+0)&0x07);
    else
        W.PPUBANK[ 0 ] = VROMPAGE((Map119_Chr01+0) % (S.NesHeader.VROMSize<<3));
    if ((Map119_Chr01+1) & 0x40)
        W.PPUBANK[ 1 ] = CRAMPAGE((Map119_Chr01+1)&0x07);
    else
        W.PPUBANK[ 1 ] = VROMPAGE((Map119_Chr01+1) % (S.NesHeader.VROMSize<<3));
    if ((Map119_Chr23+0) & 0x40)
        W.PPUBANK[ 2 ] = CRAMPAGE((Map119_Chr23+0)&0x07);
    else
        W.PPUBANK[ 2 ] = VROMPAGE((Map119_Chr23+0) % (S.NesHeader.VROMSize<<3));
    if ((Map119_Chr23+1) & 0x40)
        W.PPUBANK[ 3 ] = CRAMPAGE((Map119_Chr23+1)&0x07);
    else
        W.PPUBANK[ 3 ] = VROMPAGE((Map119_Chr23+1) % (S.NesHeader.VROMSize<<3));
    if (Map119_Chr4 & 0x40)
        W.PPUBANK[ 4 ] = CRAMPAGE(Map119_Chr4&0x07);
    else
        W.PPUBANK[ 4 ] = VROMPAGE(Map119_Chr4 % (S.NesHeader.VROMSize<<3));
    if (Map119_Chr5 & 0x40)
        W.PPUBANK[ 5 ] = CRAMPAGE(Map119_Chr5&0x07);
    else 
        W.PPUBANK[ 5 ] = VROMPAGE(Map119_Chr5 % (S.NesHeader.VROMSize<<3));
    if (Map119_Chr6 & 0x40)
        W.PPUBANK[ 6 ] = CRAMPAGE(Map119_Chr6&0x07);
    else
        W.PPUBANK[ 6 ] = VROMPAGE(Map119_Chr6 % (S.NesHeader.VROMSize<<3));
    if (Map119_Chr7 & 0x40)
        W.PPUBANK[ 7 ] = CRAMPAGE(Map119_Chr7&0x07);
    else
        W.PPUBANK[ 7 ] = VROMPAGE(Map119_Chr7 % (S.NesHeader.VROMSize<<3));
  }
  NESCore_Develop_Character_Data();
}

