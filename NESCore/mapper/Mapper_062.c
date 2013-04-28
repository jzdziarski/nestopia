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

void Map62_Init()
{
  MapperInit = Map62_Init;
  MapperWrite = Map62_Write;
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
  W.ROMBANK2 = ROMPAGE( 2 );
  W.ROMBANK3 = ROMPAGE( 3 );

  /* Set PPU Banks */
  if ( S.NesHeader.VROMSize > 0 ) {
    int nPage ;
    for (nPage = 0; nPage < 8; ++nPage )
        W.PPUBANK[ nPage ] = VROMPAGE( nPage );
    NESCore_Develop_Character_Data();
  }
}

void Map62_Write( word wAddr, byte byData )
{
  switch( wAddr & 0xFF00 ) {
  case	0x8100:
    W.ROMBANK0 = ROMPAGE((byData+0) % (S.NesHeader.ROMSize<<1));
    W.ROMBANK1 = ROMPAGE((byData+1) % (S.NesHeader.ROMSize<<1));
    break;
  case	0x8500:
    W.ROMBANK0 = ROMPAGE(byData % (S.NesHeader.ROMSize<<1));
    break;
  case	0x8700:
    W.ROMBANK1 = ROMPAGE(byData % (S.NesHeader.ROMSize<<1));
    break;

  default:
    W.PPUBANK[ 0 ] = VROMPAGE((byData+0) % (S.NesHeader.VROMSize<<3));
    W.PPUBANK[ 1 ] = VROMPAGE((byData+1) % (S.NesHeader.VROMSize<<3));
    W.PPUBANK[ 2 ] = VROMPAGE((byData+2) % (S.NesHeader.VROMSize<<3));
    W.PPUBANK[ 3 ] = VROMPAGE((byData+3) % (S.NesHeader.VROMSize<<3));
    W.PPUBANK[ 4 ] = VROMPAGE((byData+4) % (S.NesHeader.VROMSize<<3));
    W.PPUBANK[ 5 ] = VROMPAGE((byData+5) % (S.NesHeader.VROMSize<<3));
    W.PPUBANK[ 6 ] = VROMPAGE((byData+6) % (S.NesHeader.VROMSize<<3));
    W.PPUBANK[ 7 ] = VROMPAGE((byData+7) % (S.NesHeader.VROMSize<<3));
    NESCore_Develop_Character_Data();
    break;
  }
}
