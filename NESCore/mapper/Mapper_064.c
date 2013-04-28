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

byte Map64_Cmd;
byte Map64_Prg;
byte Map64_Chr;

void Map64_Init()
{
  MapperInit = Map64_Init;
  MapperWrite = Map64_Write;
  MapperSram = Map0_Sram;
  MapperApu = Map0_Apu;
  MapperReadApu = Map0_ReadApu;
  MapperVSync = Map0_VSync;
  MapperHSync = Map0_HSync;
  MapperPPU = Map0_PPU;
  MapperRenderScreen = Map0_RenderScreen;

  W.SRAMBANK = S.SRAM;
  W.ROMBANK0 = ROMLASTPAGE( 0 );
  W.ROMBANK1 = ROMLASTPAGE( 0 );
  W.ROMBANK2 = ROMLASTPAGE( 0 );
  W.ROMBANK3 = ROMLASTPAGE( 0 );

  /* Set PPU Banks */
  if ( S.NesHeader.VROMSize > 0 )
  {
    int nPage ;
    for (nPage = 0; nPage < 8; ++nPage )
        W.PPUBANK[ nPage ] = VROMPAGE( nPage );
    NESCore_Develop_Character_Data();
  }

  /* Initialize state flag */
  Map64_Cmd = 0x00;
  Map64_Prg = 0x00;
  Map64_Chr = 0x00;

}

void Map64_Write( word wAddr, byte byData )
{
  switch ( wAddr )
  {
    case 0x8000:
      /* Set state flag */
      Map64_Cmd = byData & 0x0f;
      Map64_Prg = ( byData & 0x40 ) >> 6;
      Map64_Chr = ( byData & 0x80 ) >> 7;
      break;

    case 0x8001:
      switch ( Map64_Cmd )
      {
        case 0x00:
          /* Set PPU Banks */
          byData %= ( S.NesHeader.VROMSize << 3 );
          if ( Map64_Chr )
          {
            W.PPUBANK[ 4 ] = VROMPAGE( byData );
            W.PPUBANK[ 5 ] = VROMPAGE( byData + 1 );      
          } else {
            W.PPUBANK[ 0 ] = VROMPAGE( byData );
            W.PPUBANK[ 1 ] = VROMPAGE( byData + 1 );  
          } 
          NESCore_Develop_Character_Data();
          break;

        case 0x01:
          /* Set PPU Banks */
          byData %= ( S.NesHeader.VROMSize << 3 );
          if ( Map64_Chr )
          {
            W.PPUBANK[ 6 ] = VROMPAGE( byData );
            W.PPUBANK[ 7 ] = VROMPAGE( byData + 1 );      
          } else {
            W.PPUBANK[ 2 ] = VROMPAGE( byData );
            W.PPUBANK[ 3 ] = VROMPAGE( byData + 1 );  
          } 
          NESCore_Develop_Character_Data();
          break;

        case 0x02:
          /* Set PPU Banks */
          byData %= ( S.NesHeader.VROMSize << 3 );
          if ( Map64_Chr )
          {
            W.PPUBANK[ 0 ] = VROMPAGE( byData );
          } else {
            W.PPUBANK[ 4 ] = VROMPAGE( byData );
          } 
          NESCore_Develop_Character_Data();
          break;

        case 0x03:
          /* Set PPU Banks */
          byData %= ( S.NesHeader.VROMSize << 3 );
          if ( Map64_Chr )
          {
            W.PPUBANK[ 1 ] = VROMPAGE( byData );
          } else {
            W.PPUBANK[ 5 ] = VROMPAGE( byData );
          } 
          NESCore_Develop_Character_Data();
          break;

        case 0x04:
          /* Set PPU Banks */
          byData %= ( S.NesHeader.VROMSize << 3 );
          if ( Map64_Chr )
          {
            W.PPUBANK[ 2 ] = VROMPAGE( byData );
          } else {
            W.PPUBANK[ 6 ] = VROMPAGE( byData );
          } 
          NESCore_Develop_Character_Data();
          break;

        case 0x05:
          /* Set PPU Banks */
          byData %= ( S.NesHeader.VROMSize << 3 );
          if ( Map64_Chr )
          {
            W.PPUBANK[ 3 ] = VROMPAGE( byData );
          } else {
            W.PPUBANK[ 7 ] = VROMPAGE( byData );
          } 
          NESCore_Develop_Character_Data();
          break;

        case 0x06:
          /* Set ROM Banks */
          byData %= ( S.NesHeader.ROMSize << 1 );
          if ( Map64_Prg )
          {
            W.ROMBANK1 = ROMPAGE( byData );
          } else {
            W.ROMBANK0 = ROMPAGE( byData );
          } 
          break;

        case 0x07:
          /* Set ROM Banks */
          byData %= ( S.NesHeader.ROMSize << 1 );
          if ( Map64_Prg )
          {
            W.ROMBANK2 = ROMPAGE( byData );
          } else {
            W.ROMBANK1 = ROMPAGE( byData );
          } 
          break;

        case 0x08:
          /* Set PPU Banks */
          byData %= ( S.NesHeader.VROMSize << 3 );
          W.PPUBANK[ 1 ] = VROMPAGE( byData );
          NESCore_Develop_Character_Data();
          break;

        case 0x09:
          /* Set PPU Banks */
          byData %= ( S.NesHeader.VROMSize << 3 );
          W.PPUBANK[ 3 ] = VROMPAGE( byData );
          NESCore_Develop_Character_Data();
          break;

        case 0x0f:
          /* Set ROM Banks */
          byData %= ( S.NesHeader.ROMSize << 1 );
          if ( Map64_Prg )
          {
            W.ROMBANK0 = ROMPAGE( byData );
          } else {
            W.ROMBANK2 = ROMPAGE( byData );
          } 
          break;
      }
      break;

    default:
      switch ( wAddr & 0xf000 )
      {
        case 0xa000:
          /* Name Table Mirroring */
          NESCore_Mirroring( byData & 0x01 );
          break;

        default:
          break;
      }
      break;
  }
}
