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

byte Map6_IRQ_Enable;
dword Map6_IRQ_Cnt;
byte Map6_Chr_Ram[ 0x2000 * 4 ];

void Map6_Init()
{
  int nPage;

  MapperInit = Map6_Init;
  MapperWrite = Map6_Write;
  MapperSram = Map0_Sram;
  MapperApu = Map6_Apu;
  MapperReadApu = Map0_ReadApu;
  MapperVSync = Map0_VSync;
  MapperHSync = Map6_HSync;
  MapperPPU = Map0_PPU;
  MapperRenderScreen = Map0_RenderScreen;

  /* Set SRAM Banks */
  W.SRAMBANK = S.SRAM;
  W.ROMBANK0 = ROMPAGE( 0 );
  W.ROMBANK1 = ROMPAGE( 1 );
  W.ROMBANK2 = ROMPAGE( 14 );
  W.ROMBANK3 = ROMPAGE( 15 );

  /* Set PPU Banks */
  if (S.NesHeader.VROMSize > 0)
  {
    for (nPage = 0; nPage < 8; ++nPage)
    {
      W.PPUBANK[ nPage ] = VROMPAGE( nPage );
    }
    NESCore_Develop_Character_Data();
  }
  else
  {
    for ( nPage = 0; nPage < 8; ++nPage )
    {
      W.PPUBANK[ nPage ] = Map6_VROMPAGE( nPage );
    }
    NESCore_Develop_Character_Data();
  }
}

void Map6_Apu( word wAddr, byte bData )
{
  switch ( wAddr )
  {
    /* Name Table Mirroring */
    case 0x42fe:
      if ( bData & 0x10 )
      {
        NESCore_Mirroring( 2 );
      } else {
        NESCore_Mirroring( 3 );
      }
      break;

    case 0x42ff:
      if ( bData & 0x10 )
      {
        NESCore_Mirroring( 0 );
      } else {
        NESCore_Mirroring( 1 );
      }
      break;

    case 0x4501:
      Map6_IRQ_Enable = 0;
      break;

    case 0x4502:
      Map6_IRQ_Cnt = ( Map6_IRQ_Cnt & 0xff00 ) | (dword)bData;
      break;

    case 0x4503:
      Map6_IRQ_Cnt = ( Map6_IRQ_Cnt & 0x00ff ) | ( (dword)bData << 8 );
      Map6_IRQ_Enable = 1;
      break;
  }
}

void Map6_Write( word wAddr, byte bData )
{
  byte byPrgBank = ( bData & 0x3c ) >> 2;
  byte byChrBank = bData & 0x03;

  /* Set ROM Banks */
  byPrgBank <<= 1;
  byPrgBank %= ( S.NesHeader.ROMSize << 1 );

  W.ROMBANK0 = ROMPAGE( byPrgBank );
  W.ROMBANK1 = ROMPAGE( byPrgBank + 1 );

  /* Set PPU Banks */
  W.PPUBANK[ 0 ] = &Map6_Chr_Ram[ byChrBank * 0x2000 + 0 * 0x400 ];
  W.PPUBANK[ 1 ] = &Map6_Chr_Ram[ byChrBank * 0x2000 + 1 * 0x400 ];
  W.PPUBANK[ 2 ] = &Map6_Chr_Ram[ byChrBank * 0x2000 + 2 * 0x400 ];
  W.PPUBANK[ 3 ] = &Map6_Chr_Ram[ byChrBank * 0x2000 + 3 * 0x400 ];
  W.PPUBANK[ 4 ] = &Map6_Chr_Ram[ byChrBank * 0x2000 + 4 * 0x400 ];
  W.PPUBANK[ 5 ] = &Map6_Chr_Ram[ byChrBank * 0x2000 + 5 * 0x400 ];
  W.PPUBANK[ 6 ] = &Map6_Chr_Ram[ byChrBank * 0x2000 + 6 * 0x400 ];
  W.PPUBANK[ 7 ] = &Map6_Chr_Ram[ byChrBank * 0x2000 + 7 * 0x400 ];
  NESCore_Develop_Character_Data();
}

void Map6_HSync()
{
  if ( Map6_IRQ_Enable )
  {
    Map6_IRQ_Cnt += 133;
    if ( Map6_IRQ_Cnt >= 0xffff )
    {
      IRQ_REQ;
      Map6_IRQ_Cnt = 0;
    }
  }
}
