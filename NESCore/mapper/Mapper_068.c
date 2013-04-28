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

byte Map68_Regs[4];

void Map68_Init()
{
  MapperInit = Map68_Init;
  MapperWrite = Map68_Write;
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

  /* Initialize state flag */
  int i ;
  for (i = 0; i < 4; i++ )
      Map68_Regs[ i ] = 0x00;

}

void Map68_Write( word wAddr, byte byData )
{
  switch( wAddr )
  {
    case 0x8000:  
      /* Set PPU Banks */
      byData %= ( S.NesHeader.VROMSize << 2 );
      byData <<= 1;
      W.PPUBANK[ 0 ] = VROMPAGE( byData );
      W.PPUBANK[ 1 ] = VROMPAGE( byData + 1);
      NESCore_Develop_Character_Data();
      break;

    case 0x9000:  
      /* Set PPU Banks */
      byData %= ( S.NesHeader.VROMSize << 2 );
      byData <<= 1;
      W.PPUBANK[ 2 ] = VROMPAGE( byData );
      W.PPUBANK[ 3 ] = VROMPAGE( byData + 1);
      NESCore_Develop_Character_Data();
      break;

    case 0xA000:  
      /* Set PPU Banks */
      byData %= ( S.NesHeader.VROMSize << 2 );
      byData <<= 1;
      W.PPUBANK[ 4 ] = VROMPAGE( byData );
      W.PPUBANK[ 5 ] = VROMPAGE( byData + 1);
      NESCore_Develop_Character_Data();
      break;

    case 0xB000:  
      /* Set PPU Banks */
      byData %= ( S.NesHeader.VROMSize << 2 );
      byData <<= 1;
      W.PPUBANK[ 6 ] = VROMPAGE( byData );
      W.PPUBANK[ 7 ] = VROMPAGE( byData + 1);
      NESCore_Develop_Character_Data();
      break;

    case 0xC000:  
      Map68_Regs[ 2 ] = byData;
      Map68_SyncMirror();
      break;

    case 0xD000:  
      Map68_Regs[ 3 ] = byData;
      Map68_SyncMirror();
      break;

    case 0xE000:  
      Map68_Regs[ 0 ] = ( byData & 0x10 ) >> 4;
      Map68_Regs[ 1 ] = byData & 0x03;
      Map68_SyncMirror();
      break;

    case 0xF000:  
      /* Set ROM Banks */
      byData %= S.NesHeader.ROMSize;
      byData <<= 1;
      W.ROMBANK0 = ROMPAGE( byData );
      W.ROMBANK1 = ROMPAGE( byData + 1 );
      break;
  }
}

void Map68_SyncMirror( void )
{
  if ( Map68_Regs[ 0 ] )
  {
    switch( Map68_Regs[ 1 ] )
    {
      case 0x00:
        W.PPUBANK[  8 ] = VROMPAGE( Map68_Regs[ 2 ] + 0x80 );
        W.PPUBANK[  9 ] = VROMPAGE( Map68_Regs[ 3 ] + 0x80 );
        W.PPUBANK[ 10 ] = VROMPAGE( Map68_Regs[ 2 ] + 0x80 );
        W.PPUBANK[ 11 ] = VROMPAGE( Map68_Regs[ 3 ] + 0x80 );
        break;

      case 0x01:
        W.PPUBANK[  8 ] = VROMPAGE( Map68_Regs[ 2 ] + 0x80 );
        W.PPUBANK[  9 ] = VROMPAGE( Map68_Regs[ 2 ] + 0x80 );
        W.PPUBANK[ 10 ] = VROMPAGE( Map68_Regs[ 3 ] + 0x80 );
        W.PPUBANK[ 11 ] = VROMPAGE( Map68_Regs[ 3 ] + 0x80 );
        break;

      case 0x02:
        W.PPUBANK[  8 ] = VROMPAGE( Map68_Regs[ 2 ] + 0x80 );
        W.PPUBANK[  9 ] = VROMPAGE( Map68_Regs[ 2 ] + 0x80 );
        W.PPUBANK[ 10 ] = VROMPAGE( Map68_Regs[ 2 ] + 0x80 );
        W.PPUBANK[ 11 ] = VROMPAGE( Map68_Regs[ 2 ] + 0x80 );
        break;

      case 0x03:
        W.PPUBANK[  8 ] = VROMPAGE( Map68_Regs[ 3 ] + 0x80 );
        W.PPUBANK[  9 ] = VROMPAGE( Map68_Regs[ 3 ] + 0x80 );
        W.PPUBANK[ 10 ] = VROMPAGE( Map68_Regs[ 3 ] + 0x80 );
        W.PPUBANK[ 11 ] = VROMPAGE( Map68_Regs[ 3 ] + 0x80 );
        break;
    }
    NESCore_Develop_Character_Data();
  } 
  else 
  {
    NESCore_Mirroring( Map68_Regs[ 1 ] );
  }
}
