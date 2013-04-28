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

void Map22_Init()
{
  MapperInit = Map22_Init;
  MapperWrite = Map22_Write;
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

}

void Map22_Write( word wAddr, byte byData )
{
  switch ( wAddr )
  {
    case 0x8000:
      /* Set ROM Banks */
      byData %= ( S.NesHeader.ROMSize << 1 );
      W.ROMBANK0 = ROMPAGE( byData );
      break;

    case 0x9000:
      /* Name Table Mirroring */
      switch ( byData & 0x03 )
      {
        case 0:
          NESCore_Mirroring( 1 );   /* Vertical */
          break;
        case 1:
          NESCore_Mirroring( 0 );   /* Horizontal */
          break;
        case 2:
          NESCore_Mirroring( 2 );   /* One Screen 0x2000 */
          break;
        case 3:
          NESCore_Mirroring( 3 );   /* One Screen 0x2400 */
          break;
      }
      break;

    case 0xa000:
      /* Set ROM Banks */
      byData %= ( S.NesHeader.ROMSize << 1 );
      W.ROMBANK1 = ROMPAGE( byData );
      break;
    
    case 0xb000:
      /* Set PPU Banks */
      byData >>= 1;
      byData %= ( S.NesHeader.VROMSize << 3 );
      W.PPUBANK[ 0 ] = VROMPAGE( byData );
      NESCore_Develop_Character_Data();
      break;

    case 0xb001:
      /* Set PPU Banks */
      byData >>= 1;
      byData %= ( S.NesHeader.VROMSize << 3 );
      W.PPUBANK[ 1 ] = VROMPAGE( byData );
      NESCore_Develop_Character_Data();
      break;
    
    case 0xc000:
      /* Set PPU Banks */
      byData >>= 1;
      byData %= ( S.NesHeader.VROMSize << 3 );
      W.PPUBANK[ 2 ] = VROMPAGE( byData );
      NESCore_Develop_Character_Data();
      break;

    case 0xc001:
      /* Set PPU Banks */
      byData >>= 1;
      byData %= ( S.NesHeader.VROMSize << 3 );
      W.PPUBANK[ 3 ] = VROMPAGE( byData );
      NESCore_Develop_Character_Data();
      break;
          
    case 0xd000:
      /* Set PPU Banks */
      byData >>= 1;
      byData %= ( S.NesHeader.VROMSize << 3 );
      W.PPUBANK[ 4 ] = VROMPAGE( byData );
      NESCore_Develop_Character_Data();
      break;

    case 0xd001:
      /* Set PPU Banks */
      byData >>= 1;
      byData %= ( S.NesHeader.VROMSize << 3 );
      W.PPUBANK[ 5 ] = VROMPAGE( byData );
      NESCore_Develop_Character_Data();
      break;
          
    case 0xe000:
      /* Set PPU Banks */
      byData >>= 1;
      byData %= ( S.NesHeader.VROMSize << 3 );
      W.PPUBANK[ 6 ] = VROMPAGE( byData );
      NESCore_Develop_Character_Data();
      break;

    case 0xe001:
      /* Set PPU Banks */
      byData >>= 1;
      byData %= ( S.NesHeader.VROMSize << 3 );
      W.PPUBANK[ 7 ] = VROMPAGE( byData );
      NESCore_Develop_Character_Data();
      break; 
  }
}
