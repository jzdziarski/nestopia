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

void Map60_Init()
{
  MapperInit = Map60_Init;
  MapperWrite = Map60_Write;
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

/*-------------------------------------------------------------------*/
/*  Mapper 60 Write Function                                         */
/*-------------------------------------------------------------------*/
void Map60_Write( word wAddr, byte byData )
{
  if( wAddr & 0x80 ) {
    W.ROMBANK0 = ROMPAGE((((wAddr&0x70)>>3)+0) % (S.NesHeader.ROMSize<<1));
    W.ROMBANK1 = ROMPAGE((((wAddr&0x70)>>3)+1) % (S.NesHeader.ROMSize<<1));
    W.ROMBANK2 = ROMPAGE((((wAddr&0x70)>>3)+0) % (S.NesHeader.ROMSize<<1));
    W.ROMBANK3 = ROMPAGE((((wAddr&0x70)>>3)+1) % (S.NesHeader.ROMSize<<1));
  } else {
    W.ROMBANK0 = ROMPAGE((((wAddr&0x70)>>3)+0) % (S.NesHeader.ROMSize<<1));
    W.ROMBANK1 = ROMPAGE((((wAddr&0x70)>>3)+1) % (S.NesHeader.ROMSize<<1));
    W.ROMBANK2 = ROMPAGE((((wAddr&0x70)>>3)+2) % (S.NesHeader.ROMSize<<1));
    W.ROMBANK3 = ROMPAGE((((wAddr&0x70)>>3)+3) % (S.NesHeader.ROMSize<<1));
  }
  

  W.PPUBANK[ 0 ] = VROMPAGE((((wAddr&0x07)<<3)+0) % (S.NesHeader.VROMSize<<3));
  W.PPUBANK[ 1 ] = VROMPAGE((((wAddr&0x07)<<3)+1) % (S.NesHeader.VROMSize<<3));
  W.PPUBANK[ 2 ] = VROMPAGE((((wAddr&0x07)<<3)+2) % (S.NesHeader.VROMSize<<3));
  W.PPUBANK[ 3 ] = VROMPAGE((((wAddr&0x07)<<3)+3) % (S.NesHeader.VROMSize<<3));
  W.PPUBANK[ 4 ] = VROMPAGE((((wAddr&0x07)<<3)+4) % (S.NesHeader.VROMSize<<3));
  W.PPUBANK[ 5 ] = VROMPAGE((((wAddr&0x07)<<3)+5) % (S.NesHeader.VROMSize<<3));
  W.PPUBANK[ 6 ] = VROMPAGE((((wAddr&0x07)<<3)+6) % (S.NesHeader.VROMSize<<3));
  W.PPUBANK[ 7 ] = VROMPAGE((((wAddr&0x07)<<3)+7) % (S.NesHeader.VROMSize<<3));
  NESCore_Develop_Character_Data();
  
  if( byData & 0x08 ) NESCore_Mirroring( 0 );
  else		      NESCore_Mirroring( 1 );
}
