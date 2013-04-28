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

byte  Map49_Regs[ 3 ];
dword Map49_Prg0, Map49_Prg1;
dword Map49_Chr01, Map49_Chr23;
dword Map49_Chr4, Map49_Chr5, Map49_Chr6, Map49_Chr7;

byte Map49_IRQ_Enable;
byte Map49_IRQ_Cnt;
byte Map49_IRQ_Latch;

void Map49_Init()
{
  MapperInit = Map49_Init;
  MapperWrite = Map49_Write;
  MapperSram = Map49_Sram;
  MapperApu = Map0_Apu;
  MapperReadApu = Map0_ReadApu;
  MapperVSync = Map0_VSync;
  MapperHSync = Map49_HSync;
  MapperPPU = Map0_PPU;

  /* Callback at Rendering Screen ( 1:BG, 0:Sprite ) */
  MapperRenderScreen = Map0_RenderScreen;

  W.SRAMBANK = S.SRAM;
  Map49_Prg0 = 0;
  Map49_Prg1 = 1;
  W.ROMBANK0 = ROMPAGE( 0 );
  W.ROMBANK1 = ROMPAGE( 1 );
  W.ROMBANK2 = ROMPAGE( 2 );
  W.ROMBANK3 = ROMPAGE( 3 );
  
  /* Set PPU Banks */
  Map49_Chr01 = 0;
  Map49_Chr23 = 2;
  Map49_Chr4  = 4;
  Map49_Chr5  = 5;
  Map49_Chr6  = 6;
  Map49_Chr7  = 7;

  int nPage ;
  for (nPage = 0; nPage < 8; ++nPage )
      W.PPUBANK[ nPage ] = VROMPAGE( nPage );
  NESCore_Develop_Character_Data();
  
  /* Initialize IRQ Registers */
  Map49_Regs[ 0 ] = Map49_Regs[ 1 ] = Map49_Regs[ 2 ] = 0;
  Map49_IRQ_Enable = 0;
  Map49_IRQ_Cnt = 0;
  Map49_IRQ_Latch = 0;
}

void Map49_Sram( word wAddr, byte byData )
{
  if ( Map49_Regs[ 2 ] & 0x80 )
  {
    Map49_Regs[ 1 ] = byData;
    Map49_Set_CPU_Banks();
    Map49_Set_PPU_Banks();
  }
}

void Map49_Write( word wAddr, byte byData )
{
  switch ( wAddr & 0xe001 )
  {
    case 0x8000:
      if ( ( byData & 0x40 ) != ( Map49_Regs[ 0 ] & 0x40 ) )
      {
        Map49_Set_CPU_Banks();
      }
      if ( ( byData & 0x80 ) != ( Map49_Regs[ 0 ] & 0x80 ) )
      {
        Map49_Regs[ 0 ] = byData;
        Map49_Set_PPU_Banks();
      }
      Map49_Regs[ 0 ] = byData;
      break;

    case 0x8001:
      switch ( Map49_Regs[ 0 ] & 0x07 )
      {
        /* Set PPU Banks */
        case 0x00:
          Map49_Chr01 = byData & 0xfe;
          Map49_Set_PPU_Banks();
          break;

        case 0x01:
          Map49_Chr23 = byData & 0xfe;
          Map49_Set_PPU_Banks();
          break;

        case 0x02:
          Map49_Chr4 = byData;
          Map49_Set_PPU_Banks();
          break;

        case 0x03:
          Map49_Chr5 = byData;
          Map49_Set_PPU_Banks();
          break;

        case 0x04:
          Map49_Chr6 = byData;
          Map49_Set_PPU_Banks();
          break;

        case 0x05:
          Map49_Chr7 = byData;
          Map49_Set_PPU_Banks();
          break;

        /* Set ROM Banks */
        case 0x06:
          Map49_Prg0 = byData;
          Map49_Set_CPU_Banks();
          break;

        case 0x07:
          Map49_Prg1 = byData;
          Map49_Set_CPU_Banks();
          break;
      }
      break;

    case 0xa000:
      if ( !S.ROM_FourScr )
      {
        if ( byData & 0x01 )
        {
          NESCore_Mirroring( 0 );
        } else {
          NESCore_Mirroring( 1 );
        }
      }
      break;

    case 0xa001:
      Map49_Regs[ 2 ] = byData;
      break;

    case 0xc000:
      Map49_IRQ_Cnt = byData;
      break;

    case 0xc001:
      Map49_IRQ_Latch = byData;
      break;

    case 0xe000:
      Map49_IRQ_Enable = 0;
      break;

    case 0xe001:
      Map49_IRQ_Enable = 1;
      break;
  }
}

void Map49_HSync()
{
  if ( Map49_IRQ_Enable )
  {
    if ( /* 0 <= S.PPU_Scanline && */ S.PPU_Scanline <= 239 )
    {
      if ( S.PPU_R1 & R1_SHOW_BG || S.PPU_R1 & R1_SHOW_SP )
      {
        if ( !( Map49_IRQ_Cnt-- ) )
        {
          Map49_IRQ_Cnt = Map49_IRQ_Latch;
          IRQ_REQ;
        }
      }
    }
  }
}

void Map49_Set_CPU_Banks()
{
  dword dwBank0, dwBank1, dwBank2, dwBank3;

  if ( Map49_Regs[ 1 ] & 0x01 )
  {
    if ( Map49_Regs[ 0 ] & 0x40 )
    {
      dwBank0 = ( ( ( S.NesHeader.ROMSize << 1 ) - 1 ) & 0x0e ) | ( ( Map49_Regs[ 1 ] & 0xc0 ) >> 2 );
      dwBank1 = ( Map49_Prg1 & 0x0f ) | ( ( Map49_Regs[ 1 ] & 0xc0 ) >> 2 );
      dwBank2 = ( Map49_Prg0 & 0x0f ) | ( ( Map49_Regs[ 1 ] & 0xc0 ) >> 2 );
      dwBank3 = ( ( ( S.NesHeader.ROMSize << 1 ) - 1 ) & 0x0f ) | ( ( Map49_Regs[ 1 ] & 0xc0 ) >> 2 );
    } else {
      dwBank0 = ( Map49_Prg0 & 0x0f ) | ( ( Map49_Regs[ 1 ] & 0xc0 ) >> 2 );
      dwBank1 = ( Map49_Prg1 & 0x0f ) | ( ( Map49_Regs[ 1 ] & 0xc0 ) >> 2 );
      dwBank2 = ( ( ( S.NesHeader.ROMSize << 1 ) - 1 ) & 0x0e ) | ( ( Map49_Regs[ 1 ] & 0xc0 ) >> 2 );
      dwBank3 = ( ( ( S.NesHeader.ROMSize << 1 ) - 1 ) & 0x0f ) | ( ( Map49_Regs[ 1 ] & 0xc0 ) >> 2 );
    }
  } else {
    dwBank0 = ( ( Map49_Regs[ 1 ] & 0x70 ) >> 2 ) | 0;
    dwBank1 = ( ( Map49_Regs[ 1 ] & 0x70 ) >> 2 ) | 1;
    dwBank2 = ( ( Map49_Regs[ 1 ] & 0x70 ) >> 2 ) | 2;
    dwBank3 = ( ( Map49_Regs[ 1 ] & 0x70 ) >> 2 ) | 3;
  }

  /* Set ROM Banks */ 
  W.ROMBANK0 = ROMPAGE( dwBank0 % ( S.NesHeader.ROMSize << 1) );
  W.ROMBANK1 = ROMPAGE( dwBank1 % ( S.NesHeader.ROMSize << 1) );
  W.ROMBANK2 = ROMPAGE( dwBank2 % ( S.NesHeader.ROMSize << 1) );
  W.ROMBANK3 = ROMPAGE( dwBank3 % ( S.NesHeader.ROMSize << 1) );
}

void Map49_Set_PPU_Banks()
{
  Map49_Chr01 = ( Map49_Chr01 & 0x7f ) | ( ( Map49_Regs[ 1 ] & 0xc0 ) << 1 );
  Map49_Chr23 = ( Map49_Chr23 & 0x7f ) | ( ( Map49_Regs[ 1 ] & 0xc0 ) << 1 );
  Map49_Chr4 = ( Map49_Chr4 & 0x7f ) | ( ( Map49_Regs[ 1 ] & 0xc0 ) << 1 );
  Map49_Chr5 = ( Map49_Chr5 & 0x7f ) | ( ( Map49_Regs[ 1 ] & 0xc0 ) << 1 );
  Map49_Chr6 = ( Map49_Chr6 & 0x7f ) | ( ( Map49_Regs[ 1 ] & 0xc0 ) << 1 );
  Map49_Chr7 = ( Map49_Chr7 & 0x7f ) | ( ( Map49_Regs[ 1 ] & 0xc0 ) << 1 );

  /* Set PPU Banks */ 
  if ( Map49_Regs[ 0 ] & 0x80 )
  { 
    W.PPUBANK[ 0 ] = VROMPAGE( Map49_Chr4 % ( S.NesHeader.VROMSize << 3 ) );
    W.PPUBANK[ 1 ] = VROMPAGE( Map49_Chr5 % ( S.NesHeader.VROMSize << 3 ) );
    W.PPUBANK[ 2 ] = VROMPAGE( Map49_Chr6 % ( S.NesHeader.VROMSize << 3 ) );
    W.PPUBANK[ 3 ] = VROMPAGE( Map49_Chr7 % ( S.NesHeader.VROMSize << 3 ) );
    W.PPUBANK[ 4 ] = VROMPAGE( ( Map49_Chr01 + 0 ) % ( S.NesHeader.VROMSize << 3 ) );
    W.PPUBANK[ 5 ] = VROMPAGE( ( Map49_Chr01 + 1 ) % ( S.NesHeader.VROMSize << 3 ) );
    W.PPUBANK[ 6 ] = VROMPAGE( ( Map49_Chr23 + 0 ) % ( S.NesHeader.VROMSize << 3 ) );
    W.PPUBANK[ 7 ] = VROMPAGE( ( Map49_Chr23 + 1 ) % ( S.NesHeader.VROMSize << 3 ) );
    NESCore_Develop_Character_Data();
  } else {
    W.PPUBANK[ 0 ] = VROMPAGE( ( Map49_Chr01 + 0 ) % ( S.NesHeader.VROMSize << 3 ) );
    W.PPUBANK[ 1 ] = VROMPAGE( ( Map49_Chr01 + 1 ) % ( S.NesHeader.VROMSize << 3 ) );
    W.PPUBANK[ 2 ] = VROMPAGE( ( Map49_Chr23 + 0 ) % ( S.NesHeader.VROMSize << 3 ) );
    W.PPUBANK[ 3 ] = VROMPAGE( ( Map49_Chr23 + 1 ) % ( S.NesHeader.VROMSize << 3 ) );
    W.PPUBANK[ 4 ] = VROMPAGE( Map49_Chr4 % ( S.NesHeader.VROMSize << 3 ) );
    W.PPUBANK[ 5 ] = VROMPAGE( Map49_Chr5 % ( S.NesHeader.VROMSize << 3 ) );
    W.PPUBANK[ 6 ] = VROMPAGE( Map49_Chr6 % ( S.NesHeader.VROMSize << 3 ) );
    W.PPUBANK[ 7 ] = VROMPAGE( Map49_Chr7 % ( S.NesHeader.VROMSize << 3 ) );
    NESCore_Develop_Character_Data();
  }
}
