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

byte  Map65_IRQ_Enable;
dword Map65_IRQ_Cnt;
dword Map65_IRQ_Latch;

void Map65_Init()
{
  MapperInit = Map65_Init;
  MapperWrite = Map65_Write;
  MapperSram = Map0_Sram;
  MapperApu = Map0_Apu;
  MapperReadApu = Map0_ReadApu;
  MapperVSync = Map0_VSync;
  MapperHSync = Map65_HSync;
  MapperPPU = Map0_PPU;
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
}

void Map65_Write( word wAddr, byte byData )
{
  switch ( wAddr )
  {
    case 0x8000:
      W.ROMBANK0 = ROMPAGE( byData % ( S.NesHeader.ROMSize << 1) );
      break;

    case 0x9000:
      if ( byData & 0x40 )
      {
        NESCore_Mirroring( 1 );
      } else {
        NESCore_Mirroring( 0 );
      }
      break;

    case 0x9003:
      Map65_IRQ_Enable = byData & 0x80;
      break;

    case 0x9004:
      Map65_IRQ_Cnt = Map65_IRQ_Latch;
      break;

    case 0x9005:
      Map65_IRQ_Latch = ( Map65_IRQ_Latch & 0x00ff ) | ((dword)byData << 8 );
      break;

    case 0x9006:
      Map65_IRQ_Latch = ( Map65_IRQ_Latch & 0xff00 ) | (dword)byData;
      break;

    /* Set PPU Banks */
    case 0xb000:
      W.PPUBANK[ 0 ] = VROMPAGE( byData % ( S.NesHeader.VROMSize << 3 ) );
      NESCore_Develop_Character_Data();
      break;

    case 0xb001:
      W.PPUBANK[ 1 ] = VROMPAGE( byData % ( S.NesHeader.VROMSize << 3 ) );
      NESCore_Develop_Character_Data();
      break;

    case 0xb002:
      W.PPUBANK[ 2 ] = VROMPAGE( byData % ( S.NesHeader.VROMSize << 3 ) );
      NESCore_Develop_Character_Data();
      break;

    case 0xb003:
      W.PPUBANK[ 3 ] = VROMPAGE( byData % ( S.NesHeader.VROMSize << 3 ) );
      NESCore_Develop_Character_Data();
      break;

    case 0xb004:
      W.PPUBANK[ 4 ] = VROMPAGE( byData % ( S.NesHeader.VROMSize << 3 ) );
      NESCore_Develop_Character_Data();
      break;

    case 0xb005:
      W.PPUBANK[ 5 ] = VROMPAGE( byData % ( S.NesHeader.VROMSize << 3 ) );
      NESCore_Develop_Character_Data();
      break;

    case 0xb006:
      W.PPUBANK[ 6 ] = VROMPAGE( byData % ( S.NesHeader.VROMSize << 3 ) );
      NESCore_Develop_Character_Data();
      break;

    case 0xb007:
      W.PPUBANK[ 7 ] = VROMPAGE( byData % ( S.NesHeader.VROMSize << 3 ) );
      NESCore_Develop_Character_Data();
      break;

    /* Set ROM Banks */
    case 0xa000:
      W.ROMBANK1 = ROMPAGE( byData % ( S.NesHeader.ROMSize << 1) );
      break;

    case 0xc000:
      W.ROMBANK2 = ROMPAGE( byData % ( S.NesHeader.ROMSize << 1) );
      break;
  }
}

void Map65_HSync()
{
/*
 *  Callback at HSync
 *
 */
  if ( Map65_IRQ_Enable )
  {
    if ( Map65_IRQ_Cnt <= 113 )
    {
      IRQ_REQ;
      Map65_IRQ_Enable = 0;
      Map65_IRQ_Cnt = 0xffff;
    } else {
      Map65_IRQ_Cnt -= 113;
    }
  }
}
