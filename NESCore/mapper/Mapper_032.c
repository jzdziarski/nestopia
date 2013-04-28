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

byte Map32_Saved;

void Map32_Init()
{
  /* Initialize Mapper */
  MapperInit = Map32_Init;

  /* Write to Mapper */
  MapperWrite = Map32_Write;

  /* Write to SRAM */
  MapperSram = Map0_Sram;

  /* Write to APU */
  MapperApu = Map0_Apu;

  /* Read from APU */
  MapperReadApu = Map0_ReadApu;

  /* Callback at VSync */
  MapperVSync = Map0_VSync;

  /* Callback at HSync */
  MapperHSync = Map0_HSync;

  /* Callback at PPU */
  MapperPPU = Map0_PPU;

  /* Callback at Rendering Screen ( 1:BG, 0:Sprite ) */
  MapperRenderScreen = Map0_RenderScreen;

  /* Initialize state flag */
  Map32_Saved = 0x00;

  /* Set SRAM Banks */
  W.SRAMBANK = S.SRAM;

  /* Set ROM Banks */
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

void Map32_Write( word wAddr, byte byData )
{
  switch ( wAddr & 0xf000 )
  {
    case 0x8000:
      /* Set ROM Banks */
      byData %= ( S.NesHeader.ROMSize << 1 );

      if ( Map32_Saved & 0x02 ) 
      {
        W.ROMBANK2 = ROMPAGE( byData );
      } else {
        W.ROMBANK0 = ROMPAGE( byData );
      }
      break;
      
    case 0x9000:
      Map32_Saved = byData;
      
      // Name Table Mirroring
      NESCore_Mirroring( byData & 0x01 );
      break;

    case 0xa000:
      /* Set ROM Banks */
      byData %= ( S.NesHeader.ROMSize << 1 );
      W.ROMBANK1 = ROMPAGE( byData );
      break;

    case 0xb000:
      /* Set PPU Banks */
      byData %= ( S.NesHeader.VROMSize << 3 );
      W.PPUBANK[ wAddr & 0x0007 ] = VROMPAGE( byData );
      NESCore_Develop_Character_Data();
      break;

    default:
      break;
  }
}
