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

void Map8_Init()
{
  int nPage;

  MapperInit = Map8_Init;
  MapperWrite = Map8_Write;
  MapperSram = Map0_Sram;
  MapperApu = Map0_Apu;
  MapperReadApu = Map0_ReadApu;
  MapperVSync = Map0_VSync;
  MapperHSync = Map0_HSync;
  MapperPPU = Map0_PPU;
  MapperRenderScreen = Map0_RenderScreen;

  W.SRAMBANK = S.SRAM;
  W.ROMBANK0 = ROMPAGE( 0 );
  W.ROMBANK1 = ROMPAGE( 1 );
  W.ROMBANK2 = ROMPAGE( 2 );
  W.ROMBANK3 = ROMPAGE( 3 );

  if ( S.NesHeader.VROMSize > 0 )
  {
    for ( nPage = 0; nPage < 8; ++nPage )
        W.PPUBANK[ nPage ] = VROMPAGE( nPage );
    NESCore_Develop_Character_Data();
  }
}

void Map8_Write( word wAddr, byte bData )
{
  byte byPrgBank = ( bData & 0xf8 ) >> 3;
  byte byChrBank = bData & 0x07;

  /* Set ROM Banks */
  byPrgBank <<= 1;
  byPrgBank %= ( S.NesHeader.ROMSize << 1 );

  W.ROMBANK0 = ROMPAGE( byPrgBank + 0 );
  W.ROMBANK1 = ROMPAGE( byPrgBank + 1 );

  /* Set PPU Banks */
  byChrBank <<= 3;
  byChrBank %= ( S.NesHeader.VROMSize << 3 );

  W.PPUBANK[ 0 ] = VROMPAGE( byChrBank + 0 );
  W.PPUBANK[ 1 ] = VROMPAGE( byChrBank + 1 );
  W.PPUBANK[ 2 ] = VROMPAGE( byChrBank + 2 );
  W.PPUBANK[ 3 ] = VROMPAGE( byChrBank + 3 );
  W.PPUBANK[ 4 ] = VROMPAGE( byChrBank + 4 );
  W.PPUBANK[ 5 ] = VROMPAGE( byChrBank + 5 );
  W.PPUBANK[ 6 ] = VROMPAGE( byChrBank + 6 );
  W.PPUBANK[ 7 ] = VROMPAGE( byChrBank + 7 );
  NESCore_Develop_Character_Data();
}
