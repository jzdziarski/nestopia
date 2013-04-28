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

byte	Map57_Reg;

void Map57_Init()
{
  MapperInit = Map57_Init;
  MapperWrite = Map57_Write;
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
  W.ROMBANK2 = ROMPAGE( 0 );
  W.ROMBANK3 = ROMPAGE( 1 );

  /* Set PPU Banks */
  if ( S.NesHeader.VROMSize > 0 ) {
    int nPage ;
    for (nPage = 0; nPage < 8; ++nPage )
        W.PPUBANK[ nPage ] = VROMPAGE( nPage );
    NESCore_Develop_Character_Data();
  }

  /* Set Registers */
  Map57_Reg = 0;
}

void Map57_Write( word wAddr, byte byData )
{
  byte byChr;

  switch( wAddr ) {
  case	0x8000:
  case	0x8001:
  case	0x8002:
  case	0x8003:
    if( byData & 0x40 ) {
      byChr = (byData&0x03)+((Map57_Reg&0x10)>>1)+(Map57_Reg&0x07);

      W.PPUBANK[ 0 ] = VROMPAGE(((byChr<<3)+0) % (S.NesHeader.VROMSize<<3));
      W.PPUBANK[ 1 ] = VROMPAGE(((byChr<<3)+1) % (S.NesHeader.VROMSize<<3));
      W.PPUBANK[ 2 ] = VROMPAGE(((byChr<<3)+2) % (S.NesHeader.VROMSize<<3));
      W.PPUBANK[ 3 ] = VROMPAGE(((byChr<<3)+3) % (S.NesHeader.VROMSize<<3));
      W.PPUBANK[ 4 ] = VROMPAGE(((byChr<<3)+4) % (S.NesHeader.VROMSize<<3));
      W.PPUBANK[ 5 ] = VROMPAGE(((byChr<<3)+5) % (S.NesHeader.VROMSize<<3));
      W.PPUBANK[ 6 ] = VROMPAGE(((byChr<<3)+6) % (S.NesHeader.VROMSize<<3));
      W.PPUBANK[ 7 ] = VROMPAGE(((byChr<<3)+7) % (S.NesHeader.VROMSize<<3));
      NESCore_Develop_Character_Data();
    }
    break;
  case	0x8800:
    Map57_Reg = byData;
    
    if( byData & 0x80 ) {
      W.ROMBANK0 = ROMPAGE((((byData & 0x40)>>6)*4+8+0) % (S.NesHeader.ROMSize<<1));
      W.ROMBANK1 = ROMPAGE((((byData & 0x40)>>6)*4+8+1) % (S.NesHeader.ROMSize<<1));
      W.ROMBANK2 = ROMPAGE((((byData & 0x40)>>6)*4+8+2) % (S.NesHeader.ROMSize<<1));
      W.ROMBANK3 = ROMPAGE((((byData & 0x40)>>6)*4+8+3) % (S.NesHeader.ROMSize<<1));
    } else {
      W.ROMBANK0 = ROMPAGE((((byData & 0x60)>>5)*2+0) % (S.NesHeader.ROMSize<<1));
      W.ROMBANK1 = ROMPAGE((((byData & 0x60)>>5)*2+1) % (S.NesHeader.ROMSize<<1));
      W.ROMBANK2 = ROMPAGE((((byData & 0x60)>>5)*2+0) % (S.NesHeader.ROMSize<<1));
      W.ROMBANK3 = ROMPAGE((((byData & 0x60)>>5)*2+1) % (S.NesHeader.ROMSize<<1));
    }
    
    byChr = (byData&0x07)+((byData&0x10)>>1);
    
    W.PPUBANK[ 0 ] = VROMPAGE(((byChr<<3)+0) % (S.NesHeader.VROMSize<<3));
    W.PPUBANK[ 1 ] = VROMPAGE(((byChr<<3)+1) % (S.NesHeader.VROMSize<<3));
    W.PPUBANK[ 2 ] = VROMPAGE(((byChr<<3)+2) % (S.NesHeader.VROMSize<<3));
    W.PPUBANK[ 3 ] = VROMPAGE(((byChr<<3)+3) % (S.NesHeader.VROMSize<<3));
    W.PPUBANK[ 4 ] = VROMPAGE(((byChr<<3)+4) % (S.NesHeader.VROMSize<<3));
    W.PPUBANK[ 5 ] = VROMPAGE(((byChr<<3)+5) % (S.NesHeader.VROMSize<<3));
    W.PPUBANK[ 6 ] = VROMPAGE(((byChr<<3)+6) % (S.NesHeader.VROMSize<<3));
    W.PPUBANK[ 7 ] = VROMPAGE(((byChr<<3)+7) % (S.NesHeader.VROMSize<<3));
    NESCore_Develop_Character_Data();

    if( byData & 0x08 ) NESCore_Mirroring( 0 );
    else		NESCore_Mirroring( 1 );
    
    break;
  }
}

