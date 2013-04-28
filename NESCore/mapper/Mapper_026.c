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

byte Map26_IRQ_Enable;
byte Map26_IRQ_Cnt;
byte Map26_IRQ_Latch;

void Map26_Init()
{
  MapperInit = Map26_Init;
  MapperWrite = Map26_Write;
  MapperSram = Map0_Sram;
  MapperApu = Map0_Apu;
  MapperReadApu = Map0_ReadApu;
  MapperVSync = Map0_VSync;
  MapperHSync = Map26_HSync;
  MapperPPU = Map0_PPU;

  /* Callback at Rendering Screen ( 1:BG, 0:Sprite ) */
  MapperRenderScreen = Map0_RenderScreen;

  W.SRAMBANK = S.SRAM;
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

  Map26_IRQ_Enable = 0;
  Map26_IRQ_Cnt = 0;
}

void Map26_Write( word wAddr, byte byData )
{
  switch ( wAddr )
  {
    /* Set ROM Banks */
    case 0x8000:
      byData <<= 1;      
      byData %= ( S.NesHeader.ROMSize << 1 );
      
      W.ROMBANK0 = ROMPAGE( byData + 0 );
      W.ROMBANK1 = ROMPAGE( byData + 1 );
      break;

    /* Name Table Mirroring */
    case 0xb003:  
      switch ( byData & 0x7f )
      {
        case 0x08:
        case 0x2c:
          NESCore_Mirroring( 2 );   /* One Screen 0x2400 */
          break;
        case 0x20:
          NESCore_Mirroring( 1 );   /* Vertical */
          break;
        case 0x24:
          NESCore_Mirroring( 0 );   /* Horizontal */
          break;
        case 0x28:
          NESCore_Mirroring( 3 );   /* One Screen 0x2000 */
          break;
      }
      break;

    case 0xc000:
      byData %= ( S.NesHeader.ROMSize << 1 );
      W.ROMBANK2 = ROMPAGE( byData );      
      NESCore_Develop_Character_Data();
      break;

    case 0xd000:
      byData %= ( S.NesHeader.VROMSize << 3 );
      W.PPUBANK[ 0 ] = VROMPAGE( byData );
      NESCore_Develop_Character_Data();
      break;

    case 0xd001:
      byData %= ( S.NesHeader.VROMSize << 3 );
      W.PPUBANK[ 2 ] = VROMPAGE( byData );
      NESCore_Develop_Character_Data();
      break;

    case 0xd002:
      byData %= ( S.NesHeader.VROMSize << 3 );
      W.PPUBANK[ 1 ] = VROMPAGE( byData );
      NESCore_Develop_Character_Data();
      break;

    case 0xd003:
      byData %= ( S.NesHeader.VROMSize << 3 );
      W.PPUBANK[ 3 ] = VROMPAGE( byData );
      NESCore_Develop_Character_Data();
      break;

    case 0xe000:
      byData %= ( S.NesHeader.VROMSize << 3 );
      W.PPUBANK[ 4 ] = VROMPAGE( byData );
      NESCore_Develop_Character_Data();
      break;

    case 0xe001:
      byData %= ( S.NesHeader.VROMSize << 3 );
      W.PPUBANK[ 6 ] = VROMPAGE( byData );
      NESCore_Develop_Character_Data();
      break;

    case 0xe002:
      byData %= ( S.NesHeader.VROMSize << 3 );
      W.PPUBANK[ 5 ] = VROMPAGE( byData );
      NESCore_Develop_Character_Data();
      break;

    case 0xe003:
      byData %= ( S.NesHeader.VROMSize << 3 );
      W.PPUBANK[ 7 ] = VROMPAGE( byData );
      NESCore_Develop_Character_Data();
      break;

    /* Set IRQ Registers */
    case 0xf000:
      Map26_IRQ_Latch = byData;
      break;

    case 0xf001:
      Map26_IRQ_Enable = byData & 0x01;
      break;

    case 0xf002:
      Map26_IRQ_Enable = byData & 0x03;

      if ( Map26_IRQ_Enable & 0x02 )
      {
        Map26_IRQ_Cnt = Map26_IRQ_Latch;
      }
      break;
  }
}

void Map26_HSync()
{
  if ( Map26_IRQ_Enable & 0x03 )
  {
    if ( Map26_IRQ_Cnt >= 0xfe )
    {
      IRQ_REQ;
      Map26_IRQ_Cnt = Map26_IRQ_Latch;
      Map26_IRQ_Enable = 0;
    } else {
      Map26_IRQ_Cnt++;
    }
  }
}
