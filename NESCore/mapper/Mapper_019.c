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

byte  Map19_Chr_Ram[ 0x2000 ];
byte  Map19_Regs[ 3 ];

byte  Map19_IRQ_Enable;
dword Map19_IRQ_Cnt;

void Map19_Init()
{
  MapperInit = Map19_Init;
  MapperWrite = Map19_Write;
  MapperSram = Map0_Sram;
  MapperApu = Map19_Apu;
  MapperReadApu = Map19_ReadApu;
  MapperVSync = Map0_VSync;
  MapperHSync = Map19_HSync;
  MapperPPU = Map0_PPU;

  /* Callback at Rendering Screen ( 1:BG, 0:Sprite ) */
  MapperRenderScreen = Map0_RenderScreen;

  /* Set SRAM Banks */
  W.SRAMBANK = S.SRAM;

  /* Set ROM Banks */
  W.ROMBANK0 = ROMPAGE( 0 );
  W.ROMBANK1 = ROMPAGE( 1 );
  W.ROMBANK2 = ROMLASTPAGE( 1 );
  W.ROMBANK3 = ROMLASTPAGE( 0 );

  /* Set PPU Banks */
  if ( S.NesHeader.VROMSize > 0 )
  {
    dword dwLastPage = (dword) S.NesHeader.VROMSize << 3;
    W.PPUBANK[ 0 ] = VROMPAGE(dwLastPage - 8);
    W.PPUBANK[ 1 ] = VROMPAGE(dwLastPage - 7);
    W.PPUBANK[ 2 ] = VROMPAGE(dwLastPage - 6);
    W.PPUBANK[ 3 ] = VROMPAGE(dwLastPage - 5);
    W.PPUBANK[ 4 ] = VROMPAGE(dwLastPage - 4);
    W.PPUBANK[ 5 ] = VROMPAGE(dwLastPage - 3);
    W.PPUBANK[ 6 ] = VROMPAGE(dwLastPage - 2);
    W.PPUBANK[ 7 ] = VROMPAGE(dwLastPage - 1);
    NESCore_Develop_Character_Data();
  }

  /* Initialize State Register */
  Map19_Regs[ 0 ] = 0x00;
  Map19_Regs[ 1 ] = 0x00;
  Map19_Regs[ 2 ] = 0x00;
}

void Map19_Write( word wAddr, byte byData )
{
  /* Set PPU Banks */
  switch ( wAddr & 0xf800 )
  {
    case 0x8000:  /* $8000-87ff */
      if ( byData < 0xe0 || Map19_Regs[ 0 ] == 1 )
      {
        byData %= ( S.NesHeader.VROMSize << 3 );
        W.PPUBANK[ 0 ] = VROMPAGE( byData );
      } else {
        W.PPUBANK[ 0 ] = Map19_VROMPAGE( 0 );
      }
      NESCore_Develop_Character_Data();
      break;

    case 0x8800:  /* $8800-8fff */
      if ( byData < 0xe0 || Map19_Regs[ 0 ] == 1 )
      {
        byData %= ( S.NesHeader.VROMSize << 3 );
        W.PPUBANK[ 1 ] = VROMPAGE( byData );
      } else {
        W.PPUBANK[ 1 ] = Map19_VROMPAGE( 1 );
      }
      NESCore_Develop_Character_Data();
      break;

    case 0x9000:  /* $9000-97ff */
      if ( byData < 0xe0 || Map19_Regs[ 0 ] == 1 )
      {
        byData %= ( S.NesHeader.VROMSize << 3 );
        W.PPUBANK[ 2 ] = VROMPAGE( byData );
      } else {
        W.PPUBANK[ 2 ] = Map19_VROMPAGE( 2 );
      }
      NESCore_Develop_Character_Data();
      break;

    case 0x9800:  /* $9800-9fff */
      if ( byData < 0xe0 || Map19_Regs[ 0 ] == 1 )
      {
        byData %= ( S.NesHeader.VROMSize << 3 );
        W.PPUBANK[ 3 ] = VROMPAGE( byData );
      } else {
        W.PPUBANK[ 3 ] = Map19_VROMPAGE( 3 );
      }
      NESCore_Develop_Character_Data();
      break;

    case 0xa000:  /* $a000-a7ff */
      if ( byData < 0xe0 || Map19_Regs[ 0 ] == 1 )
      {
        byData %= ( S.NesHeader.VROMSize << 3 );
        W.PPUBANK[ 4 ] = VROMPAGE( byData );
      } else {
        W.PPUBANK[ 4 ] = Map19_VROMPAGE( 4 );
      }
      NESCore_Develop_Character_Data();
      break;

    case 0xa800:  /* $a800-afff */
      if ( byData < 0xe0 || Map19_Regs[ 0 ] == 1 )
      {
        byData %= ( S.NesHeader.VROMSize << 3 );
        W.PPUBANK[ 5 ] = VROMPAGE( byData );
      } else {
        W.PPUBANK[ 5 ] = Map19_VROMPAGE( 5 );
      }
      NESCore_Develop_Character_Data();
      break;

    case 0xb000:  /* $b000-b7ff */
      if ( byData < 0xe0 || Map19_Regs[ 0 ] == 1 )
      {
        byData %= ( S.NesHeader.VROMSize << 3 );
        W.PPUBANK[ 6 ] = VROMPAGE( byData );
      } else {
        W.PPUBANK[ 6 ] = Map19_VROMPAGE( 6 );
      }
      NESCore_Develop_Character_Data();
      break;

    case 0xb800:  /* $b800-bfff */
      if ( byData < 0xe0 || Map19_Regs[ 0 ] == 1 )
      {
        byData %= ( S.NesHeader.VROMSize << 3 );
        W.PPUBANK[ 7 ] = VROMPAGE( byData );
      } else {
        W.PPUBANK[ 7 ] = Map19_VROMPAGE( 7 );
      }
      NESCore_Develop_Character_Data();
      break;

    case 0xc000:  /* $c000-c7ff */
      if ( byData < 0xe0 || Map19_Regs[ 0 ] == 1 )
      {
        byData %= ( S.NesHeader.VROMSize << 3 );
        W.PPUBANK[ NAME_TABLE0 ] = VROMPAGE( byData );
      } else {
        W.PPUBANK[ NAME_TABLE0 ] = VRAMPAGE( byData & 0x01 );
      }
      break;

    case 0xc800:  /* $c800-cfff */
      if ( byData < 0xe0 || Map19_Regs[ 0 ] == 1 )
      {
        byData %= ( S.NesHeader.VROMSize << 3 );
        W.PPUBANK[ NAME_TABLE1 ] = VROMPAGE( byData );
      } else {
        W.PPUBANK[ NAME_TABLE1 ] = VRAMPAGE( byData & 0x01 );
      }
      break;

    case 0xd000:  /* $d000-d7ff */
      if ( byData < 0xe0 || Map19_Regs[ 0 ] == 1 )
      {
        byData %= ( S.NesHeader.VROMSize << 3 );
        W.PPUBANK[ NAME_TABLE2 ] = VROMPAGE( byData );
      } else {
        W.PPUBANK[ NAME_TABLE2 ] = VRAMPAGE( byData & 0x01 );
      }
      break;

    case 0xd800:  /* $d800-dfff */
      if ( byData < 0xe0 || Map19_Regs[ 0 ] == 1 )
      {
        byData %= ( S.NesHeader.VROMSize << 3 );
        W.PPUBANK[ NAME_TABLE3 ] = VROMPAGE( byData );
      } else {
        W.PPUBANK[ NAME_TABLE3 ] = VRAMPAGE( byData & 0x01 );
      }
      break;

    case 0xe000:  /* $e000-e7ff */
      byData &= 0x3f;
      byData %= ( S.NesHeader.ROMSize << 1 );
      W.ROMBANK0 = ROMPAGE( byData );
      break;

    case 0xe800:  /* $e800-efff */
      Map19_Regs[ 0 ] = ( byData & 0x40 ) >> 6;
      Map19_Regs[ 1 ] = ( byData & 0x80 ) >> 7;

      byData &= 0x3f;
      byData %= ( S.NesHeader.ROMSize << 1 );
      W.ROMBANK1 = ROMPAGE( byData );
      break;

    case 0xf000:  /* $f000-f7ff */
      byData &= 0x3f;
      byData %= ( S.NesHeader.ROMSize << 1 );
      W.ROMBANK2 = ROMPAGE( byData );
      break;

    case 0xf800:  /* $e800-efff */
      if ( wAddr == 0xf800 )
      {
        // Extra Sound
      }
      break;    
  }
}

void Map19_Apu( word wAddr, byte byData )
{
  switch ( wAddr & 0xf800 )
  {
    case 0x4800:
      if ( wAddr == 0x4800 )
      {
        // Extra Sound
      }
      break;

    case 0x5000:  /* $5000-57ff */
      Map19_IRQ_Cnt = ( Map19_IRQ_Cnt & 0xff00 ) | byData;
      break;

    case 0x5800:  /* $5800-5fff */
      Map19_IRQ_Cnt = ( Map19_IRQ_Cnt & 0x00ff ) | ( (dword)( byData & 0x7f ) << 8 );
      Map19_IRQ_Enable = ( byData & 0x80 ) >> 7;
      break;
  }
}

byte Map19_ReadApu( word wAddr )
{
  switch ( wAddr & 0xf800 )
  {
    case 0x4800:
      if ( wAddr == 0x4800 )
      {
        // Extra Sound
      }
      return (byte)( wAddr >> 8 );

    case 0x5000:  /* $5000-57ff */
      return (byte)(Map19_IRQ_Cnt & 0x00ff );

    case 0x5800:  /* $5800-5fff */
      return (byte)( (Map19_IRQ_Cnt & 0x7f00) >> 8 );

    default:
      return (byte)( wAddr >> 8 );
  }
}

void Map19_HSync()
{
  byte Map19_IRQ_Timing = 113;

  if ( Map19_IRQ_Enable )
  {
    if ( Map19_IRQ_Cnt >= (dword)(0x7fff - Map19_IRQ_Timing) )
    {
      Map19_IRQ_Cnt = 0x7fff;
      IRQ_REQ;
    } else {
      Map19_IRQ_Cnt += Map19_IRQ_Timing;
    }
  }
}
