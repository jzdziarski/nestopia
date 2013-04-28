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

byte  Map17_IRQ_Enable;
dword Map17_IRQ_Cnt;
dword Map17_IRQ_Latch;

void Map17_Init()
{
  MapperInit = Map17_Init;
  MapperWrite = Map0_Write;
  MapperSram = Map0_Sram;
  MapperApu = Map17_Apu;
  MapperReadApu = Map0_ReadApu;
  MapperVSync = Map0_VSync;
  MapperHSync = Map17_HSync;
  MapperPPU = Map0_PPU;
  W.SRAMBANK = S.SRAM;

  /* Callback at Rendering Screen ( 1:BG, 0:Sprite ) */
  MapperRenderScreen = Map0_RenderScreen;

  W.ROMBANK0 = ROMPAGE( 0 );
  W.ROMBANK1 = ROMPAGE( 1 );
  W.ROMBANK2 = ROMLASTPAGE( 1 );
  W.ROMBANK3 = ROMLASTPAGE( 0 );

  if ( S.NesHeader.VROMSize > 0 )
  {
    int nPage ;
    for (nPage = 0; nPage < 8; ++nPage )
        W.PPUBANK[ nPage ] = VROMPAGE( nPage );
    NESCore_Develop_Character_Data();
  }

  Map17_IRQ_Enable = 0;
  Map17_IRQ_Cnt = 0;
  Map17_IRQ_Latch = 0;

}

void Map17_Apu( word wAddr, byte bData )
{
  switch ( wAddr )
  {
    case 0x42fe:
      if ( ( bData & 0x10 ) == 0 )
      {
        NESCore_Mirroring( 3 );
      } else {
        NESCore_Mirroring( 2 );
      }
      break;

    case 0x42ff:
      if ( ( bData & 0x10 ) == 0 )
      {
        NESCore_Mirroring( 1 );
      } else {
        NESCore_Mirroring( 0 );
      }
      break;

    case 0x4501:
      Map17_IRQ_Enable = 0;
      break;

    case 0x4502:
      Map17_IRQ_Latch = ( Map17_IRQ_Latch & 0xff00 ) | bData;
      break;

    case 0x4503:
      Map17_IRQ_Latch = ( Map17_IRQ_Latch & 0x00ff ) | ( (dword)bData << 8 );
      Map17_IRQ_Cnt = Map17_IRQ_Latch;
      Map17_IRQ_Enable = 1;
      break;

    case 0x4504:
      bData %= ( S.NesHeader.ROMSize << 1 );
      W.ROMBANK0 = ROMPAGE( bData );
      break;

    case 0x4505:
      bData %= ( S.NesHeader.ROMSize << 1 );
      W.ROMBANK1 = ROMPAGE( bData );
      break;

    case 0x4506:
      bData %= ( S.NesHeader.ROMSize << 1 );
      W.ROMBANK2 = ROMPAGE( bData );
      break;

    case 0x4507:
      bData %= ( S.NesHeader.ROMSize << 1 );
      W.ROMBANK3 = ROMPAGE( bData );
      break;

    case 0x4510:
      bData %= ( S.NesHeader.VROMSize << 3 );
      W.PPUBANK[ 0 ] = VROMPAGE( bData );
      NESCore_Develop_Character_Data();
      break;

    case 0x4511:
      bData %= ( S.NesHeader.VROMSize << 3 );
      W.PPUBANK[ 1 ] = VROMPAGE( bData );
      NESCore_Develop_Character_Data();
      break;

    case 0x4512:
      bData %= ( S.NesHeader.VROMSize << 3 );
      W.PPUBANK[ 2 ] = VROMPAGE( bData );
      NESCore_Develop_Character_Data();
      break;

    case 0x4513:
      bData %= ( S.NesHeader.VROMSize << 3 );
      W.PPUBANK[ 3 ] = VROMPAGE( bData );
      NESCore_Develop_Character_Data();
      break;

    case 0x4514:
      bData %= ( S.NesHeader.VROMSize << 3 );
      W.PPUBANK[ 4 ] = VROMPAGE( bData );
      NESCore_Develop_Character_Data();
      break;

    case 0x4515:
      bData %= ( S.NesHeader.VROMSize << 3 );
      W.PPUBANK[ 5 ] = VROMPAGE( bData );
      NESCore_Develop_Character_Data();
      break;

    case 0x4516:
      bData %= ( S.NesHeader.VROMSize << 3 );
      W.PPUBANK[ 6 ] = VROMPAGE( bData );
      NESCore_Develop_Character_Data();
      break;

    case 0x4517:
      bData %= ( S.NesHeader.VROMSize << 3 );
      W.PPUBANK[ 7 ] = VROMPAGE( bData );
      NESCore_Develop_Character_Data();
      break;
  }
}

void Map17_HSync()
{
  if ( Map17_IRQ_Enable )
  {
    if ( Map17_IRQ_Cnt >= 0xffff - 113 )
    {
      IRQ_REQ;
      Map17_IRQ_Cnt = 0;
      Map17_IRQ_Enable = 0;
    } else {
      Map17_IRQ_Cnt += 113;
    }
  }
}
