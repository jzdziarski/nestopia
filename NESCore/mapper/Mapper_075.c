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

byte Map75_Regs[ 2 ];

void Map75_Init()
{
  MapperInit = Map75_Init;
  MapperWrite = Map75_Write;
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

  /* Set PPU Banks */
  if ( S.NesHeader.VROMSize > 0 )
  {
    int nPage ;
    for (nPage = 0; nPage < 8; ++nPage )
        W.PPUBANK[ nPage ] = VROMPAGE( nPage );
    NESCore_Develop_Character_Data();
  }

  /* Initialize State Flag */
  Map75_Regs[ 0 ] = 0;
  Map75_Regs[ 1 ] = 1;
}

void Map75_Write( word wAddr, byte byData )
{
  switch ( wAddr & 0xf000 )
  {
    /* Set ROM Banks */
    case 0x8000:
      byData %= ( S.NesHeader.ROMSize << 1 );
      W.ROMBANK0 = ROMPAGE( byData );
      break;

    case 0x9000:
      /* Set Mirroring */
      if ( byData & 0x01 )
      {
        NESCore_Mirroring( 0 );
      } else {
        NESCore_Mirroring( 1 );
      }

      /* Set PPU Banks */
      Map75_Regs[ 0 ] = ( Map75_Regs[ 0 ] & 0x0f ) | ( ( byData & 0x02 ) << 3 );
      W.PPUBANK[ 0 ] = VROMPAGE( ( Map75_Regs[ 0 ] << 2 ) + 0 );
      W.PPUBANK[ 1 ] = VROMPAGE( ( Map75_Regs[ 0 ] << 2 ) + 1 );
      W.PPUBANK[ 2 ] = VROMPAGE( ( Map75_Regs[ 0 ] << 2 ) + 2 );
      W.PPUBANK[ 3 ] = VROMPAGE( ( Map75_Regs[ 0 ] << 2 ) + 3 );

      Map75_Regs[ 1 ] = ( Map75_Regs[ 1 ] & 0x0f ) | ( ( byData & 0x04 ) << 2 );
      W.PPUBANK[ 4 ] = VROMPAGE( ( Map75_Regs[ 1 ] << 2 ) + 0 );
      W.PPUBANK[ 5 ] = VROMPAGE( ( Map75_Regs[ 1 ] << 2 ) + 1 );
      W.PPUBANK[ 6 ] = VROMPAGE( ( Map75_Regs[ 1 ] << 2 ) + 2 );
      W.PPUBANK[ 7 ] = VROMPAGE( ( Map75_Regs[ 1 ] << 2 ) + 3 );
      NESCore_Develop_Character_Data();
      break;

    /* Set ROM Banks */
    case 0xA000:
      byData %= ( S.NesHeader.ROMSize << 1 );
      W.ROMBANK1 = ROMPAGE( byData );
      break;

    /* Set ROM Banks */
    case 0xC000:
      byData %= ( S.NesHeader.ROMSize << 1 );
      W.ROMBANK2 = ROMPAGE( byData );
      break;

    case 0xE000:
      /* Set PPU Banks */
      Map75_Regs[ 0 ] = ( Map75_Regs[ 0 ] & 0x10 ) | ( byData & 0x0f );
      W.PPUBANK[ 0 ] = VROMPAGE( ( Map75_Regs[ 0 ] << 2 ) + 0 );
      W.PPUBANK[ 1 ] = VROMPAGE( ( Map75_Regs[ 0 ] << 2 ) + 1 );
      W.PPUBANK[ 2 ] = VROMPAGE( ( Map75_Regs[ 0 ] << 2 ) + 2 );
      W.PPUBANK[ 3 ] = VROMPAGE( ( Map75_Regs[ 0 ] << 2 ) + 3 );
      NESCore_Develop_Character_Data();
      break;

    case 0xF000:
      /* Set PPU Banks */
      Map75_Regs[ 1 ] = ( Map75_Regs[ 1 ] & 0x10 ) | ( byData & 0x0f );
      W.PPUBANK[ 4 ] = VROMPAGE( ( Map75_Regs[ 1 ] << 2 ) + 0 );
      W.PPUBANK[ 5 ] = VROMPAGE( ( Map75_Regs[ 1 ] << 2 ) + 1 );
      W.PPUBANK[ 6 ] = VROMPAGE( ( Map75_Regs[ 1 ] << 2 ) + 2 );
      W.PPUBANK[ 7 ] = VROMPAGE( ( Map75_Regs[ 1 ] << 2 ) + 3 );
      NESCore_Develop_Character_Data();
      break;
  }
}
