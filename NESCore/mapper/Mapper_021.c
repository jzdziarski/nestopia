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

byte Map21_Regs[ 10 ];
byte Map21_IRQ_Enable;
byte Map21_IRQ_Cnt;
byte Map21_IRQ_Latch;

void Map21_Init()
{
  MapperInit = Map21_Init;
  MapperWrite = Map21_Write;
  MapperSram = Map0_Sram;
  MapperApu = Map0_Apu;
  MapperReadApu = Map0_ReadApu;
  MapperVSync = Map0_VSync;
  MapperHSync = Map21_HSync;
  MapperPPU = Map0_PPU;
  MapperRenderScreen = Map0_RenderScreen;

  W.SRAMBANK = S.SRAM;
  W.ROMBANK0 = ROMPAGE( 0 );
  W.ROMBANK1 = ROMPAGE( 1 );
  W.ROMBANK2 = ROMLASTPAGE( 1 );
  W.ROMBANK3 = ROMLASTPAGE( 0 );

  /* Initialize State Registers */
  int nPage ;
  for (nPage = 0; nPage < 8; nPage++ )
      Map21_Regs[ nPage ] = nPage;
  Map21_Regs[ 8 ] = 0;

  Map21_IRQ_Enable = 0;
  Map21_IRQ_Cnt = 0;
  Map21_IRQ_Latch = 0;
}

void Map21_Write( word wAddr, byte byData )
{
  switch ( wAddr & 0xf00f )
  {
    /* Set ROM Banks */
    case 0x8000:
      if ( Map21_Regs[ 8 ] & 0x02 )
      {
        byData %= ( S.NesHeader.ROMSize << 1 );
        W.ROMBANK2 = ROMPAGE( byData );
      } else {
        byData %= ( S.NesHeader.ROMSize << 1 );
        W.ROMBANK0 = ROMPAGE( byData );
      }
      break;

    case 0xa000:
      byData %= ( S.NesHeader.ROMSize << 1 );
      W.ROMBANK1 = ROMPAGE( byData );
      break;

    /* Name Table Mirroring */
    case 0x9000:
      switch ( byData & 0x03 )
      {
        case 0:
          NESCore_Mirroring( 1 );
          break;

        case 1:
          NESCore_Mirroring( 0 );
          break;
       
        case 2:
          NESCore_Mirroring( 3 );
          break;

        case 3:
          NESCore_Mirroring( 2 );
          break; 
      }
      break;

    case 0x9002:
      Map21_Regs[ 8 ] = byData;
      break;

    case 0xb000:
      Map21_Regs[ 0 ] = ( Map21_Regs[ 0 ] & 0xf0 ) | ( byData & 0x0f );
      W.PPUBANK[ 0 ] = VROMPAGE( Map21_Regs[ 0 ] % ( S.NesHeader.VROMSize << 3 ) );
      NESCore_Develop_Character_Data();
      break;

    case 0xb002:
      Map21_Regs[ 0 ] = ( Map21_Regs[ 0 ] & 0x0f ) | ( ( byData & 0x0f ) << 4 );
      W.PPUBANK[ 0 ] = VROMPAGE( Map21_Regs[ 0 ] % ( S.NesHeader.VROMSize << 3 ) );
      NESCore_Develop_Character_Data();
      break;

    case 0xb001:
    case 0xb004:
      Map21_Regs[ 1 ] = ( Map21_Regs[ 1 ] & 0xf0 ) | ( byData & 0x0f );
      W.PPUBANK[ 1 ] = VROMPAGE( Map21_Regs[ 1 ] % ( S.NesHeader.VROMSize << 3 ) );
      NESCore_Develop_Character_Data();
      break;

    case 0xb003:
    case 0xb006:
      Map21_Regs[ 1 ] = ( Map21_Regs[ 1 ] & 0x0f ) | ( ( byData & 0x0f ) << 4 );
      W.PPUBANK[ 1 ] = VROMPAGE( Map21_Regs[ 1 ] % ( S.NesHeader.VROMSize << 3 ) );
      NESCore_Develop_Character_Data();
      break;

    case 0xc000:
      Map21_Regs[ 2 ] = ( Map21_Regs[ 2 ] & 0xf0 ) | ( byData & 0x0f );
      W.PPUBANK[ 2 ] = VROMPAGE( Map21_Regs[ 2 ] % ( S.NesHeader.VROMSize << 3 ) );
      NESCore_Develop_Character_Data();
      break;

    case 0xc002:
      Map21_Regs[ 2 ] = ( Map21_Regs[ 2 ] & 0x0f ) | ( ( byData & 0x0f ) << 4 );
      W.PPUBANK[ 2 ] = VROMPAGE( Map21_Regs[ 2 ] % ( S.NesHeader.VROMSize << 3 ) );
      NESCore_Develop_Character_Data();
      break;

    case 0xc001:
    case 0xc004:
      Map21_Regs[ 3 ] = ( Map21_Regs[ 3 ] & 0xf0 ) | ( byData & 0x0f );
      W.PPUBANK[ 3 ] = VROMPAGE( Map21_Regs[ 3 ] % ( S.NesHeader.VROMSize << 3 ) );
      NESCore_Develop_Character_Data();
      break;

    case 0xc003:
    case 0xc006:
      Map21_Regs[ 3 ] = ( Map21_Regs[ 3 ] & 0x0f ) | ( ( byData & 0x0f ) << 4 );
      W.PPUBANK[ 3 ] = VROMPAGE( Map21_Regs[ 3 ] % ( S.NesHeader.VROMSize << 3 ) );
      NESCore_Develop_Character_Data();
      break;

    case 0xd000:
      Map21_Regs[ 4 ] = ( Map21_Regs[ 4 ] & 0xf0 ) | ( byData & 0x0f );
      W.PPUBANK[ 4 ] = VROMPAGE( Map21_Regs[ 4 ] % ( S.NesHeader.VROMSize << 3 ) );
      NESCore_Develop_Character_Data();
      break;

    case 0xd002:
      Map21_Regs[ 4 ] = ( Map21_Regs[ 4 ] & 0x0f ) | ( ( byData & 0x0f ) << 4 );
      W.PPUBANK[ 4 ] = VROMPAGE( Map21_Regs[ 4 ] % ( S.NesHeader.VROMSize << 3 ) );
      NESCore_Develop_Character_Data();
      break;

    case 0xd001:
    case 0xd004:
      Map21_Regs[ 5 ] = ( Map21_Regs[ 5 ] & 0xf0 ) | ( byData & 0x0f );
      W.PPUBANK[ 5 ] = VROMPAGE( Map21_Regs[ 5 ] % ( S.NesHeader.VROMSize << 3 ) );
      NESCore_Develop_Character_Data();
      break;

    case 0xd003:
    case 0xd006:
      Map21_Regs[ 5 ] = ( Map21_Regs[ 5 ] & 0x0f ) | ( ( byData & 0x0f ) << 4 );
      W.PPUBANK[ 5 ] = VROMPAGE( Map21_Regs[ 5 ] % ( S.NesHeader.VROMSize << 3 ) );
      NESCore_Develop_Character_Data();
      break;

    case 0xe000:
      Map21_Regs[ 6 ] = ( Map21_Regs[ 6 ] & 0xf0 ) | ( byData & 0x0f );
      W.PPUBANK[ 6 ] = VROMPAGE( Map21_Regs[ 6 ] % ( S.NesHeader.VROMSize << 3 ) );
      NESCore_Develop_Character_Data();
      break;

    case 0xe002:
      Map21_Regs[ 6 ] = ( Map21_Regs[ 6 ] & 0x0f ) | ( ( byData & 0x0f ) << 4 );
      W.PPUBANK[ 6 ] = VROMPAGE( Map21_Regs[ 6 ] % ( S.NesHeader.VROMSize << 3 ) );
      NESCore_Develop_Character_Data();
      break;

    case 0xe001:
    case 0xe004:
      Map21_Regs[ 7 ] = ( Map21_Regs[ 7 ] & 0xf0 ) | ( byData & 0x0f );
      W.PPUBANK[ 7 ] = VROMPAGE( Map21_Regs[ 7 ] % ( S.NesHeader.VROMSize << 3 ) );
      NESCore_Develop_Character_Data();
      break;

    case 0xe003:
    case 0xe006:
      Map21_Regs[ 7 ] = ( Map21_Regs[ 7 ] & 0x0f ) | ( ( byData & 0x0f ) << 4 );
      W.PPUBANK[ 7 ] = VROMPAGE( Map21_Regs[ 7 ] % ( S.NesHeader.VROMSize << 3 ) );
      NESCore_Develop_Character_Data();
      break;

    case 0xf000:
      Map21_IRQ_Latch = ( Map21_IRQ_Latch & 0xf0 ) | ( byData & 0x0f );
      break;

    case 0xf002:
      Map21_IRQ_Latch = ( Map21_IRQ_Latch & 0x0f ) | ( ( byData & 0x0f ) << 4 );
      break;

    case 0xf003:
      if ( Map21_IRQ_Enable & 0x01 )
      {
        Map21_IRQ_Enable |= 0x02;
      } else {
        Map21_IRQ_Enable &= 0x01;
      }
      break;

    case 0xf004:
      Map21_IRQ_Enable = byData & 0x03;
      if ( Map21_IRQ_Enable & 0x02 )
      {
        Map21_IRQ_Cnt = Map21_IRQ_Latch;
      }
      break;
  }
}

void Map21_HSync()
{
  if ( Map21_IRQ_Enable & 0x02 )
  {
    if ( Map21_IRQ_Cnt == 0xff )
    {
      Map21_IRQ_Cnt = Map21_IRQ_Latch;
      
      if ( Map21_IRQ_Enable & 0x01 )
      {
        Map21_IRQ_Enable |= 0x02;
      } else {
        Map21_IRQ_Enable &= 0x01;
      }
      IRQ_REQ;
    } else {
      Map21_IRQ_Cnt++;
    }
  }
}
