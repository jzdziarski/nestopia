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

byte Map33_Regs[ 8 ];
byte Map33_Switch;

byte Map33_IRQ_Enable;
byte Map33_IRQ_Cnt;

void Map33_Init()
{
  MapperInit = Map33_Init;
  MapperWrite = Map33_Write;
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
    {
        W.PPUBANK[ nPage ] = VROMPAGE( nPage );
        Map33_Regs[ nPage ] = nPage;
    }
    NESCore_Develop_Character_Data();
  } 
  else 
  {
    int nPage ;
    for (nPage = 0; nPage < 8; ++nPage )
        Map33_Regs[ nPage ] = 0;
  }

  Map33_Switch = 0;
  Map33_IRQ_Enable = 0;
  Map33_IRQ_Cnt = 0;
}

void Map33_Write( word wAddr, byte byData )
{
  /* Set ROM Banks */
  switch ( wAddr )
  {
    case 0x8000:
      byData %= ( S.NesHeader.ROMSize << 1 );
      W.ROMBANK0 = ROMPAGE( byData );
      break;

    case 0x8001:
      byData %= ( S.NesHeader.ROMSize << 1 );
      W.ROMBANK1 = ROMPAGE( byData );
      break;

    case 0x8002:
      Map33_Regs[ 0 ] = byData * 2;
      Map33_Regs[ 1 ] = byData * 2 + 1;

      W.PPUBANK[ 0 ] = VROMPAGE( Map33_Regs[ 0 ] % ( S.NesHeader.VROMSize << 3 ) );
      W.PPUBANK[ 1 ] = VROMPAGE( Map33_Regs[ 1 ] % ( S.NesHeader.VROMSize << 3 ) );
      NESCore_Develop_Character_Data();
      break;

    case 0x8003:
      Map33_Regs[ 2 ] = byData * 2;
      Map33_Regs[ 3 ] = byData * 2 + 1;

      W.PPUBANK[ 2 ] = VROMPAGE( Map33_Regs[ 2 ] % ( S.NesHeader.VROMSize << 3 ) );
      W.PPUBANK[ 3 ] = VROMPAGE( Map33_Regs[ 3 ] % ( S.NesHeader.VROMSize << 3 ) );
      NESCore_Develop_Character_Data();
      break;

    case 0xa000:
      Map33_Regs[ 4 ] = byData;
      W.PPUBANK[ 4 ] = VROMPAGE( Map33_Regs[ 4 ] % ( S.NesHeader.VROMSize << 3 ) );
      NESCore_Develop_Character_Data();
      break;

    case 0xa001:
      Map33_Regs[ 5 ] = byData;
      W.PPUBANK[ 5 ] = VROMPAGE( Map33_Regs[ 5 ] % ( S.NesHeader.VROMSize << 3 ) );
      NESCore_Develop_Character_Data();
      break;

    case 0xa002:
      Map33_Regs[ 6 ] = byData;
      W.PPUBANK[ 6 ] = VROMPAGE( Map33_Regs[ 6 ] % ( S.NesHeader.VROMSize << 3 ) );
      NESCore_Develop_Character_Data();
      break;

    case 0xa003:
      Map33_Regs[ 7 ] = byData;
      W.PPUBANK[ 7 ] = VROMPAGE( Map33_Regs[ 7 ] % ( S.NesHeader.VROMSize << 3 ) );
      NESCore_Develop_Character_Data();
      break;

    case 0xc000:
      Map33_IRQ_Cnt = byData;
      break;

    case 0xc001:
    case 0xc002:
    case 0xe001:
    case 0xe002:
      if ( Map33_IRQ_Cnt == byData )
      {
        Map33_IRQ_Enable = 0xff;
      } else {
        Map33_IRQ_Enable = byData;
      }
      break;

    case 0xe000:
      if ( byData & 0x40 )
      {
        NESCore_Mirroring( 0 );
      } else {
        NESCore_Mirroring( 1 );
      }
      break;
  }
}

void Map33_HSync()
{
  if ( Map33_IRQ_Enable )
  {
    if ( Map33_IRQ_Enable == 0xff )
    {
      if ( S.PPU_Scanline == (word)( 0xff - Map33_IRQ_Cnt ) )
      {
        IRQ_REQ;
        Map33_IRQ_Cnt = 0;
        Map33_IRQ_Enable = 0;
      }
    } else {
      if ( Map33_IRQ_Cnt == 0xff )
      {
        IRQ_REQ;
        Map33_IRQ_Cnt = 0;
        Map33_IRQ_Enable = 0;
      } else {
        Map33_IRQ_Cnt++;
      }
    }
  }
}
