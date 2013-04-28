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

byte Map46_Regs[ 4 ];

void Map46_Init()
{
  MapperInit = Map46_Init;
  MapperWrite = Map46_Write;
  MapperSram = Map46_Sram;
  MapperApu = Map0_Apu;
  MapperReadApu = Map0_ReadApu;
  MapperVSync = Map0_VSync;
  MapperHSync = Map0_HSync;
  MapperPPU = Map0_PPU;
  MapperRenderScreen = Map0_RenderScreen;

  W.SRAMBANK = S.SRAM;
  Map46_Regs[ 0 ] = Map46_Regs[ 1 ] = Map46_Regs[ 2 ] = Map46_Regs[ 3 ] = 0;
  Map46_Set_ROM_Banks();

  /* Name Table Mirroring */
  NESCore_Mirroring( 1 );
}

void Map46_Sram( word wAddr, byte byData )
{
  /* Set ROM Banks */
  Map46_Regs[ 0 ] = byData & 0x0f;
  Map46_Regs[ 1 ] = ( byData & 0xf0 ) >> 4;
  Map46_Set_ROM_Banks();
}

void Map46_Write( word wAddr, byte byData )
{
  /* Set ROM Banks */
  Map46_Regs[ 2 ] = byData & 0x01;
  Map46_Regs[ 3 ] = ( byData & 0x70 ) >> 4;
  Map46_Set_ROM_Banks();
}

void Map46_Set_ROM_Banks()
{
  /* Set ROM Banks */
  W.ROMBANK0 = ROMPAGE( ( ( Map46_Regs[ 0 ] << 3 ) + ( Map46_Regs[ 2 ] << 2 ) + 0 ) % ( S.NesHeader.ROMSize << 1 ) );  
  W.ROMBANK1 = ROMPAGE( ( ( Map46_Regs[ 0 ] << 3 ) + ( Map46_Regs[ 2 ] << 2 ) + 1 ) % ( S.NesHeader.ROMSize << 1 ) );
  W.ROMBANK2 = ROMPAGE( ( ( Map46_Regs[ 0 ] << 3 ) + ( Map46_Regs[ 2 ] << 2 ) + 2 ) % ( S.NesHeader.ROMSize << 1 ) );
  W.ROMBANK3 = ROMPAGE( ( ( Map46_Regs[ 0 ] << 3 ) + ( Map46_Regs[ 2 ] << 2 ) + 3 ) % ( S.NesHeader.ROMSize << 1 ) ); 

  /* Set PPU Banks */
  W.PPUBANK[ 0 ] = VROMPAGE( ( ( Map46_Regs[ 1 ] << 6 ) + ( Map46_Regs[ 3 ] << 3 ) + 0 ) % ( S.NesHeader.VROMSize << 3 ) ); 
  W.PPUBANK[ 1 ] = VROMPAGE( ( ( Map46_Regs[ 1 ] << 6 ) + ( Map46_Regs[ 3 ] << 3 ) + 1 ) % ( S.NesHeader.VROMSize << 3 ) ); 
  W.PPUBANK[ 2 ] = VROMPAGE( ( ( Map46_Regs[ 1 ] << 6 ) + ( Map46_Regs[ 3 ] << 3 ) + 2 ) % ( S.NesHeader.VROMSize << 3 ) ); 
  W.PPUBANK[ 3 ] = VROMPAGE( ( ( Map46_Regs[ 1 ] << 6 ) + ( Map46_Regs[ 3 ] << 3 ) + 3 ) % ( S.NesHeader.VROMSize << 3 ) ); 
  W.PPUBANK[ 4 ] = VROMPAGE( ( ( Map46_Regs[ 1 ] << 6 ) + ( Map46_Regs[ 3 ] << 3 ) + 4 ) % ( S.NesHeader.VROMSize << 3 ) ); 
  W.PPUBANK[ 5 ] = VROMPAGE( ( ( Map46_Regs[ 1 ] << 6 ) + ( Map46_Regs[ 3 ] << 3 ) + 5 ) % ( S.NesHeader.VROMSize << 3 ) ); 
  W.PPUBANK[ 6 ] = VROMPAGE( ( ( Map46_Regs[ 1 ] << 6 ) + ( Map46_Regs[ 3 ] << 3 ) + 6 ) % ( S.NesHeader.VROMSize << 3 ) ); 
  W.PPUBANK[ 7 ] = VROMPAGE( ( ( Map46_Regs[ 1 ] << 6 ) + ( Map46_Regs[ 3 ] << 3 ) + 7 ) % ( S.NesHeader.VROMSize << 3 ) ); 
  NESCore_Develop_Character_Data();
}
