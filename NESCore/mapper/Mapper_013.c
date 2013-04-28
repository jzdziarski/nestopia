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

void Map13_Init()
{
  MapperInit = Map13_Init;
  MapperWrite = Map13_Write;
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
  W.PPUBANK[ 0 ] = CRAMPAGE( 0 );
  W.PPUBANK[ 1 ] = CRAMPAGE( 1 );
  W.PPUBANK[ 2 ] = CRAMPAGE( 2 );
  W.PPUBANK[ 3 ] = CRAMPAGE( 3 );
  W.PPUBANK[ 4 ] = CRAMPAGE( 0 );
  W.PPUBANK[ 5 ] = CRAMPAGE( 1 );
  W.PPUBANK[ 6 ] = CRAMPAGE( 2 );
  W.PPUBANK[ 7 ] = CRAMPAGE( 3 );
  NESCore_Develop_Character_Data();
}

/*-------------------------------------------------------------------*/
/*  Mapper 13 Write Function                                         */
/*-------------------------------------------------------------------*/
void Map13_Write( word wAddr, byte bData )
{
  W.ROMBANK0 = ROMPAGE((((bData&0x30)>>2)+0) % (S.NesHeader.ROMSize<<1));
  W.ROMBANK1 = ROMPAGE((((bData&0x30)>>2)+1) % (S.NesHeader.ROMSize<<1));
  W.ROMBANK2 = ROMPAGE((((bData&0x30)>>2)+2) % (S.NesHeader.ROMSize<<1));
  W.ROMBANK3 = ROMPAGE((((bData&0x30)>>2)+3) % (S.NesHeader.ROMSize<<1));

  W.PPUBANK[ 4 ] = CRAMPAGE(((bData&0x03)<<2)+0);
  W.PPUBANK[ 5 ] = CRAMPAGE(((bData&0x03)<<2)+1);
  W.PPUBANK[ 6 ] = CRAMPAGE(((bData&0x03)<<2)+2);
  W.PPUBANK[ 7 ] = CRAMPAGE(((bData&0x03)<<2)+3);
  NESCore_Develop_Character_Data();
}
