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

void Map66_Init()
{
  int nPage;

  MapperInit = Map66_Init;
  MapperWrite = Map66_Write;
  MapperSram = Map66_Write;
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

  /* Set PPU Banks */
  if ( S.NesHeader.VROMSize > 0 )
  {
    for ( nPage = 0; nPage < 8; ++nPage )
        W.PPUBANK[ nPage ] = VROMPAGE( nPage );
    NESCore_Develop_Character_Data();
  }
}

void Map66_Write( word wAddr, byte byData )
{
  byte byRom;
  byte byVRom;

  byRom  = ( byData >> 4 ) & 0x0F;
  byVRom = byData & 0x0F;

  /* Set ROM Banks */
  byRom <<= 1;
  byRom %= S.NesHeader.ROMSize;
  byRom <<= 1;

  W.ROMBANK0 = ROMPAGE( byRom );
  W.ROMBANK1 = ROMPAGE( byRom + 1 );
  W.ROMBANK2 = ROMPAGE( byRom + 2 );
  W.ROMBANK3 = ROMPAGE( byRom + 3 );

  /* Set PPU Banks */
  byVRom <<= 3;
  byVRom %= ( S.NesHeader.VROMSize << 3 );

  W.PPUBANK[ 0 ] = VROMPAGE( byVRom );
  W.PPUBANK[ 1 ] = VROMPAGE( byVRom + 1 );
  W.PPUBANK[ 2 ] = VROMPAGE( byVRom + 2 );
  W.PPUBANK[ 3 ] = VROMPAGE( byVRom + 3 );
  W.PPUBANK[ 4 ] = VROMPAGE( byVRom + 4 );
  W.PPUBANK[ 5 ] = VROMPAGE( byVRom + 5 );
  W.PPUBANK[ 6 ] = VROMPAGE( byVRom + 6 );
  W.PPUBANK[ 7 ] = VROMPAGE( byVRom + 7 );
  NESCore_Develop_Character_Data();
}
