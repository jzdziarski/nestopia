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

void Map70_Init()
{
  MapperInit = Map70_Init;
  MapperWrite = Map70_Write;
  MapperSram = Map0_Sram;
  MapperApu = Map0_Apu;
  MapperReadApu = Map0_ReadApu;
  MapperVSync = Map0_VSync;
  MapperHSync = Map0_HSync;
  MapperPPU = Map0_PPU;

  /* Callback at Rendering Screen ( 1:BG, 0:Sprite ) */
  MapperRenderScreen = Map0_RenderScreen;

  W.SRAMBANK = S.SRAM;
  W.ROMBANK0 = ROMPAGE( 0 );
  W.ROMBANK1 = ROMPAGE( 1 );
  W.ROMBANK2 = ROMLASTPAGE( 1 );
  W.ROMBANK3 = ROMLASTPAGE( 0 );

}

void Map70_Write( word wAddr, byte byData )
{
  byte byChrBank = byData & 0x0f;
  byte byPrgBank = ( byData & 0x70 ) >> 4;

  /* Set ROM Banks */
  byPrgBank <<= 1;
  byPrgBank %= ( S.NesHeader.ROMSize << 1 );

  W.ROMBANK0 = ROMPAGE( byPrgBank );
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

  /* Name Table Mirroring */
  if ( byData & 0x80 )
  {
    NESCore_Mirroring( 2 );
  } else {
    NESCore_Mirroring( 3 );
  }
}
