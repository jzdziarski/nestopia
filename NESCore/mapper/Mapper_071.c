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

void Map71_Init()
{
  MapperInit = Map71_Init;
  MapperWrite = Map71_Write;
  MapperSram = Map0_Sram;
  MapperApu = Map0_Apu;
  MapperReadApu = Map0_ReadApu;
  MapperVSync = Map0_VSync;
  MapperHSync = Map0_HSync;
  MapperPPU = Map0_PPU;
  MapperRenderScreen = Map0_RenderScreen;

  /* Set SRAM Banks */
  W.SRAMBANK = S.SRAM;

  /* Set ROM Banks */
  W.ROMBANK0 = ROMPAGE( 0 );
  W.ROMBANK1 = ROMPAGE( 1 );
  W.ROMBANK2 = ROMLASTPAGE( 1 );
  W.ROMBANK3 = ROMLASTPAGE( 0 );
}

void Map71_Write( word wAddr, byte byData )
{
  switch ( wAddr & 0xf000 )
  {
    case 0x9000:
      if ( byData & 0x10 )
      {
        NESCore_Mirroring( 2 );
      } else {
        NESCore_Mirroring( 3 );
      }
      break;

    /* Set ROM Banks */
    case 0xc000:
    case 0xd000:
    case 0xe000:
    case 0xf000:
      W.ROMBANK0 = ROMPAGE( ( ( byData << 1 ) + 0 ) % ( S.NesHeader.ROMSize << 1 ) );
      W.ROMBANK1 = ROMPAGE( ( ( byData << 1 ) + 1 ) % ( S.NesHeader.ROMSize << 1 ) );
      break;
  }
}
