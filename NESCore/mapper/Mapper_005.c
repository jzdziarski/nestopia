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

/* MMC5 */

struct Map5_State {
    byte Map5_Wram[ 0x2000 * 8 ];
    byte Map5_Ex_Ram[ 0x400 ]; 
    byte Map5_Ex_Vram[ 0x400 ];
    byte Map5_Ex_Nam[ 0x400 ];

    byte Map5_Prg_Reg[ 8 ];
    byte Map5_Wram_Reg[ 8 ];
    byte Map5_Chr_Reg[ 8 ][ 2 ];

    byte Map5_IRQEnable;
    byte Map5_IRQStatus;
    byte Map5_IRQLine;

    dword Map5_Value0;
    dword Map5_Value1;

    byte Map5_Wram_Protect0;
    byte Map5_Wram_Protect1;
    byte Map5_Prg_Size;
    byte Map5_Chr_Size;
    byte Map5_Gfx_Mode;
};

struct Map5_State MS5;

void Map5_Init()
{
  int nPage;
  MapperInit = Map5_Init;
  MapperWrite = Map5_Write;
  MapperSram = Map0_Sram;
  MapperApu = Map5_Apu;
  MapperReadApu = Map5_ReadApu;
  MapperVSync = Map0_VSync;
  MapperHSync = Map5_HSync;
  MapperPPU = Map0_PPU;
  MapperRenderScreen = Map5_RenderScreen;

  W.SRAMBANK = S.SRAM;
  W.ROMBANK0 = ROMLASTPAGE( 0 );
  W.ROMBANK1 = ROMLASTPAGE( 0 );
  W.ROMBANK2 = ROMLASTPAGE( 0 );
  W.ROMBANK3 = ROMLASTPAGE( 0 );

  for ( nPage = 0; nPage < 8; ++nPage )
      W.PPUBANK[ nPage ] = VROMPAGE( nPage );
  NESCore_Develop_Character_Data();

  /* Initialize State Registers */
  for ( nPage = 4; nPage < 8; ++nPage )
  {
    MS5.Map5_Prg_Reg[ nPage ] = ( S.NesHeader.ROMSize << 1 ) - 1;
    MS5.Map5_Wram_Reg[ nPage ] = 0xff;
  }
  MS5.Map5_Wram_Reg[ 3 ] = 0xff;

  byte byPage;
  for ( byPage = 4; byPage < 8; ++byPage )
  {
    MS5.Map5_Chr_Reg[ byPage ][ 0 ] = byPage;
    MS5.Map5_Chr_Reg[ byPage ][ 1 ] = ( byPage & 0x03 ) + 4;
  }

  memset( MS5.Map5_Wram, 0x00, sizeof( MS5.Map5_Wram ) );
  memset( MS5.Map5_Ex_Ram, 0x00, sizeof( MS5.Map5_Ex_Ram ) );
  memset( MS5.Map5_Ex_Vram, 0x00, sizeof( MS5.Map5_Ex_Vram ) );
  memset( MS5.Map5_Ex_Nam, 0x00, sizeof( MS5.Map5_Ex_Nam ) );

  MS5.Map5_Prg_Size = 3;
  MS5.Map5_Wram_Protect0 = 0;
  MS5.Map5_Wram_Protect1 = 0;
  MS5.Map5_Chr_Size = 3;
  MS5.Map5_Gfx_Mode = 0;

  MS5.Map5_IRQEnable = 0;
  MS5.Map5_IRQStatus = 0;
  MS5.Map5_IRQLine = 0;

  M.state = &MS5;
  M.size = sizeof(MS5);
}

byte Map5_ReadApu( word wAddr )
{
  byte byRet = (byte)( wAddr >> 8 );

  switch ( wAddr )
  {
    case 0x5204:
      byRet = MS5.Map5_IRQStatus;
      MS5.Map5_IRQStatus = 0;
      break;

    case 0x5205:
      byRet = (byte)( ( MS5.Map5_Value0 * MS5.Map5_Value1 ) & 0x00ff );
      break;

    case 0x5206:
      byRet = (byte)( ( ( MS5.Map5_Value0 * MS5.Map5_Value1 ) & 0xff00 ) >> 8 );
      break;

    default:
      if ( 0x5c00 <= wAddr && wAddr <= 0x5fff )
      {
        byRet = MS5.Map5_Ex_Ram[ wAddr - 0x5c00 ];
      }
      break;
  }
  return byRet;
}

void Map5_Apu( word wAddr, byte byData )
{
  int nPage;

  switch ( wAddr )
  {
    case 0x5100:
      MS5.Map5_Prg_Size = byData & 0x03;
      break;

    case 0x5101:
      MS5.Map5_Chr_Size = byData & 0x03;
      break;

    case 0x5102:
      MS5.Map5_Wram_Protect0 = byData & 0x03;
      break;

    case 0x5103:
      MS5.Map5_Wram_Protect1 = byData & 0x03;
      break;

    case 0x5104:
      MS5.Map5_Gfx_Mode = byData & 0x03;
      break;

    case 0x5105:
      for ( nPage = 0; nPage < 4; nPage++ )
      {
        byte byNamReg;
        
        byNamReg = byData & 0x03;
        byData = byData >> 2;

        switch ( byNamReg )
        {
          case 0:
#if 1
            W.PPUBANK[ nPage + 8 ] = VRAMPAGE( 0 );
#else
            W.PPUBANK[ nPage + 8 ] = CRAMPAGE( 8 );
#endif
            break;
          case 1:
#if 1
            W.PPUBANK[ nPage + 8 ] = VRAMPAGE( 1 );
#else
            W.PPUBANK[ nPage + 8 ] = CRAMPAGE( 9 );
#endif
            break;
          case 2:
            W.PPUBANK[ nPage + 8 ] = MS5.Map5_Ex_Vram;
            break;
          case 3:
            W.PPUBANK[ nPage + 8 ] = MS5.Map5_Ex_Nam;
            break;
        }
      }
      break;

    case 0x5106:
      memset( MS5.Map5_Ex_Nam, byData, 0x3c0 );
      break;

    case 0x5107:
      byData &= 0x03;
      byData = byData | ( byData << 2 ) | ( byData << 4 ) | ( byData << 6 );
      memset( &( MS5.Map5_Ex_Nam[ 0x3c0 ] ), byData, 0x400 - 0x3c0 );
      break;

    case 0x5113:
      MS5.Map5_Wram_Reg[ 3 ] = byData & 0x07;
      W.SRAMBANK = Map5_ROMPAGE( byData & 0x07 );
      break;

    case 0x5114:
    case 0x5115:
    case 0x5116:
    case 0x5117:
      MS5.Map5_Prg_Reg[ wAddr & 0x07 ] = byData;
      Map5_Sync_Prg_Banks();
      break;

    case 0x5120:
    case 0x5121:
    case 0x5122:
    case 0x5123:
    case 0x5124:
    case 0x5125:
    case 0x5126:
    case 0x5127:
      MS5.Map5_Chr_Reg[ wAddr & 0x07 ][ 0 ] = byData;
      Map5_Sync_Prg_Banks();
      break;

    case 0x5128:
    case 0x5129:
    case 0x512a:
    case 0x512b:
      MS5.Map5_Chr_Reg[ ( wAddr & 0x03 ) + 0 ][ 1 ] = byData;
      MS5.Map5_Chr_Reg[ ( wAddr & 0x03 ) + 4 ][ 1 ] = byData;
      break;

    case 0x5200:
    case 0x5201:
    case 0x5202:
      /* Nothing to do */
      break;

    case 0x5203:
      if ( MS5.Map5_IRQLine >= 0x40 )
      {
        MS5.Map5_IRQLine = byData;
      } else {
        MS5.Map5_IRQLine += byData;
      }
      break;

    case 0x5204:
      MS5.Map5_IRQEnable = byData;
      break;

    case 0x5205:
      MS5.Map5_Value0 = byData;
      break;

    case 0x5206:
      MS5.Map5_Value1 = byData;
      break;

    default:
      if ( 0x5000 <= wAddr && wAddr <= 0x5015 )
      {
        /* Extra Sound */
      } else 
      if ( 0x5c00 <= wAddr && wAddr <= 0x5fff )
      {
        switch ( MS5.Map5_Gfx_Mode )
        {
          case 0:
            MS5.Map5_Ex_Vram[ wAddr - 0x5c00 ] = byData;
            break;
          case 2:
            MS5.Map5_Ex_Ram[ wAddr - 0x5c00 ] = byData;
            break;
        }
      }
      break;
  }
}

void Map5_Sram( word wAddr, byte byData )
{
  if ( MS5.Map5_Wram_Protect0 == 0x02 && MS5.Map5_Wram_Protect1 == 0x01 )
  {
    if ( MS5.Map5_Wram_Reg[ 3 ] != 0xff )
    {
      MS5.Map5_Wram[ 0x2000 * MS5.Map5_Wram_Reg[ 3 ] + ( wAddr - 0x6000) ] = byData;
    }
  }
}

void Map5_Write( word wAddr, byte byData )
{
  if ( MS5.Map5_Wram_Protect0 == 0x02 && MS5.Map5_Wram_Protect1 == 0x01 )
  {
    switch ( wAddr & 0xe000 )
    {
      case 0x8000:      /* $8000-$9fff */
        if ( MS5.Map5_Wram_Reg[ 4 ] != 0xff )
        {
          MS5.Map5_Wram[ 0x2000 * MS5.Map5_Wram_Reg[ 4 ] + ( wAddr - 0x8000) ] = byData;
        }
        break;

      case 0xa000:      /* $a000-$bfff */
        if ( MS5.Map5_Wram_Reg[ 5 ] != 0xff )
        {
          MS5.Map5_Wram[ 0x2000 * MS5.Map5_Wram_Reg[ 5 ] + ( wAddr - 0xa000) ] = byData;
        }
        break;

      case 0xc000:      /* $c000-$dfff */
        if ( MS5.Map5_Wram_Reg[ 6 ] != 0xff )
        {
          MS5.Map5_Wram[ 0x2000 * MS5.Map5_Wram_Reg[ 6 ] + ( wAddr - 0xc000) ] = byData;
        }
        break;
    }
  }
}

void Map5_HSync()
{
  if ( S.PPU_Scanline <= 240 )
  {
    if ( S.PPU_Scanline == MS5.Map5_IRQLine )
    {
      MS5.Map5_IRQStatus |= 0x80;

      if ( MS5.Map5_IRQEnable && MS5.Map5_IRQLine < 0xf0 )
      {
        IRQ_REQ;
      }
      if ( MS5.Map5_IRQLine >= 0x40 )
      {
        MS5.Map5_IRQEnable = 0;
      }
    }
  } else {
    MS5.Map5_IRQStatus |= 0x40;
  }
}

void Map5_RenderScreen( byte byMode )
{
  dword dwPage[ 8 ];

  switch ( MS5.Map5_Chr_Size )
  {
    case 0:
      dwPage[ 7 ] = ( (dword)MS5.Map5_Chr_Reg[7][byMode] << 3 ) % ( S.NesHeader.VROMSize << 3 );

      W.PPUBANK[ 0 ] = VROMPAGE( dwPage[ 7 ] + 0 );
      W.PPUBANK[ 1 ] = VROMPAGE( dwPage[ 7 ] + 1 );
      W.PPUBANK[ 2 ] = VROMPAGE( dwPage[ 7 ] + 2 );
      W.PPUBANK[ 3 ] = VROMPAGE( dwPage[ 7 ] + 3 );
      W.PPUBANK[ 4 ] = VROMPAGE( dwPage[ 7 ] + 4 );
      W.PPUBANK[ 5 ] = VROMPAGE( dwPage[ 7 ] + 5 );
      W.PPUBANK[ 6 ] = VROMPAGE( dwPage[ 7 ] + 6 );
      W.PPUBANK[ 7 ] = VROMPAGE( dwPage[ 7 ] + 7 );
      NESCore_Develop_Character_Data();
      break;

    case 1:
      dwPage[ 3 ] = ( (dword)MS5.Map5_Chr_Reg[3][byMode] << 2 ) % ( S.NesHeader.VROMSize << 3 );
      dwPage[ 7 ] = ( (dword)MS5.Map5_Chr_Reg[7][byMode] << 2 ) % ( S.NesHeader.VROMSize << 3 );

      W.PPUBANK[ 0 ] = VROMPAGE( dwPage[ 3 ] + 0 );
      W.PPUBANK[ 1 ] = VROMPAGE( dwPage[ 3 ] + 1 );
      W.PPUBANK[ 2 ] = VROMPAGE( dwPage[ 3 ] + 2 );
      W.PPUBANK[ 3 ] = VROMPAGE( dwPage[ 3 ] + 3 );
      W.PPUBANK[ 4 ] = VROMPAGE( dwPage[ 7 ] + 0 );
      W.PPUBANK[ 5 ] = VROMPAGE( dwPage[ 7 ] + 1 );
      W.PPUBANK[ 6 ] = VROMPAGE( dwPage[ 7 ] + 2 );
      W.PPUBANK[ 7 ] = VROMPAGE( dwPage[ 7 ] + 3 );
     NESCore_Develop_Character_Data();
      break;

    case 2:
      dwPage[ 1 ] = ( (dword)MS5.Map5_Chr_Reg[1][byMode] << 1 ) % ( S.NesHeader.VROMSize << 3 );
      dwPage[ 3 ] = ( (dword)MS5.Map5_Chr_Reg[3][byMode] << 1 ) % ( S.NesHeader.VROMSize << 3 );
      dwPage[ 5 ] = ( (dword)MS5.Map5_Chr_Reg[5][byMode] << 1 ) % ( S.NesHeader.VROMSize << 3 );
      dwPage[ 7 ] = ( (dword)MS5.Map5_Chr_Reg[7][byMode] << 1 ) % ( S.NesHeader.VROMSize << 3 );

      W.PPUBANK[ 0 ] = VROMPAGE( dwPage[ 1 ] + 0 );
      W.PPUBANK[ 1 ] = VROMPAGE( dwPage[ 1 ] + 1 );
      W.PPUBANK[ 2 ] = VROMPAGE( dwPage[ 3 ] + 0 );
      W.PPUBANK[ 3 ] = VROMPAGE( dwPage[ 3 ] + 1 );
      W.PPUBANK[ 4 ] = VROMPAGE( dwPage[ 5 ] + 0 );
      W.PPUBANK[ 5 ] = VROMPAGE( dwPage[ 5 ] + 1 );
      W.PPUBANK[ 6 ] = VROMPAGE( dwPage[ 7 ] + 0 );
      W.PPUBANK[ 7 ] = VROMPAGE( dwPage[ 7 ] + 1 );
      NESCore_Develop_Character_Data();
      break;

    default:
      dwPage[ 0 ] = (dword)MS5.Map5_Chr_Reg[0][byMode] % ( S.NesHeader.VROMSize << 3 );
      dwPage[ 1 ] = (dword)MS5.Map5_Chr_Reg[1][byMode] % ( S.NesHeader.VROMSize << 3 );
      dwPage[ 2 ] = (dword)MS5.Map5_Chr_Reg[2][byMode] % ( S.NesHeader.VROMSize << 3 );
      dwPage[ 3 ] = (dword)MS5.Map5_Chr_Reg[3][byMode] % ( S.NesHeader.VROMSize << 3 );
      dwPage[ 4 ] = (dword)MS5.Map5_Chr_Reg[4][byMode] % ( S.NesHeader.VROMSize << 3 );
      dwPage[ 5 ] = (dword)MS5.Map5_Chr_Reg[5][byMode] % ( S.NesHeader.VROMSize << 3 );
      dwPage[ 6 ] = (dword)MS5.Map5_Chr_Reg[6][byMode] % ( S.NesHeader.VROMSize << 3 );
      dwPage[ 7 ] = (dword)MS5.Map5_Chr_Reg[7][byMode] % ( S.NesHeader.VROMSize << 3 );

      W.PPUBANK[ 0 ] = VROMPAGE( dwPage[ 0 ] );
      W.PPUBANK[ 1 ] = VROMPAGE( dwPage[ 1 ] );
      W.PPUBANK[ 2 ] = VROMPAGE( dwPage[ 2 ] );
      W.PPUBANK[ 3 ] = VROMPAGE( dwPage[ 3 ] );
      W.PPUBANK[ 4 ] = VROMPAGE( dwPage[ 4 ] );
      W.PPUBANK[ 5 ] = VROMPAGE( dwPage[ 5 ] );
      W.PPUBANK[ 6 ] = VROMPAGE( dwPage[ 6 ] );
      W.PPUBANK[ 7 ] = VROMPAGE( dwPage[ 7 ] );
      NESCore_Develop_Character_Data();
      break;
  }
}

void Map5_Sync_Prg_Banks( void )
{
  switch( MS5.Map5_Prg_Size )
  {
    case 0:
      MS5.Map5_Wram_Reg[ 4 ] = 0xff;
      MS5.Map5_Wram_Reg[ 5 ] = 0xff;
      MS5.Map5_Wram_Reg[ 6 ] = 0xff;

      W.ROMBANK0 = ROMPAGE( ( (MS5.Map5_Prg_Reg[7] & 0x7c) + 0 ) % ( S.NesHeader.ROMSize << 1 ) );
      W.ROMBANK1 = ROMPAGE( ( (MS5.Map5_Prg_Reg[7] & 0x7c) + 1 ) % ( S.NesHeader.ROMSize << 1 ) );
      W.ROMBANK2 = ROMPAGE( ( (MS5.Map5_Prg_Reg[7] & 0x7c) + 2 ) % ( S.NesHeader.ROMSize << 1 ) );
      W.ROMBANK3 = ROMPAGE( ( (MS5.Map5_Prg_Reg[7] & 0x7c) + 3 ) % ( S.NesHeader.ROMSize << 1 ) );
      break;

    case 1:
      if ( MS5.Map5_Prg_Reg[ 5 ] & 0x80 )
      {
        MS5.Map5_Wram_Reg[ 4 ] = 0xff;
        MS5.Map5_Wram_Reg[ 5 ] = 0xff;
        W.ROMBANK0 = ROMPAGE( ( (MS5.Map5_Prg_Reg[7] & 0x7e) + 0 ) % ( S.NesHeader.ROMSize << 1 ) );
        W.ROMBANK1 = ROMPAGE( ( (MS5.Map5_Prg_Reg[7] & 0x7e) + 1 ) % ( S.NesHeader.ROMSize << 1 ) );
      } else {
        MS5.Map5_Wram_Reg[ 4 ] = ( MS5.Map5_Prg_Reg[ 5 ] & 0x06 ) + 0;
        MS5.Map5_Wram_Reg[ 5 ] = ( MS5.Map5_Prg_Reg[ 5 ] & 0x06 ) + 1;
        W.ROMBANK0 = Map5_ROMPAGE( MS5.Map5_Wram_Reg[ 4 ] );
        W.ROMBANK1 = Map5_ROMPAGE( MS5.Map5_Wram_Reg[ 5 ] );
      }

      MS5.Map5_Wram_Reg[ 6 ] = 0xff;
      W.ROMBANK2 = ROMPAGE( ( (MS5.Map5_Prg_Reg[7] & 0x7e) + 0 ) % ( S.NesHeader.ROMSize << 1 ) );
      W.ROMBANK3 = ROMPAGE( ( (MS5.Map5_Prg_Reg[7] & 0x7e) + 1 ) % ( S.NesHeader.ROMSize << 1 ) );
      break;

    case 2:
      if ( MS5.Map5_Prg_Reg[ 5 ] & 0x80 )
      {
        MS5.Map5_Wram_Reg[ 4 ] = 0xff;
        MS5.Map5_Wram_Reg[ 5 ] = 0xff;
        W.ROMBANK0 = ROMPAGE( ( (MS5.Map5_Prg_Reg[5] & 0x7e) + 0 ) % ( S.NesHeader.ROMSize << 1 ) );
        W.ROMBANK1 = ROMPAGE( ( (MS5.Map5_Prg_Reg[5] & 0x7e) + 1 ) % ( S.NesHeader.ROMSize << 1 ) );
      } else {
        MS5.Map5_Wram_Reg[ 4 ] = ( MS5.Map5_Prg_Reg[ 5 ] & 0x06 ) + 0;
        MS5.Map5_Wram_Reg[ 5 ] = ( MS5.Map5_Prg_Reg[ 5 ] & 0x06 ) + 1;
        W.ROMBANK0 = Map5_ROMPAGE( MS5.Map5_Wram_Reg[ 4 ] );
        W.ROMBANK1 = Map5_ROMPAGE( MS5.Map5_Wram_Reg[ 5 ] );
      }

      if ( MS5.Map5_Prg_Reg[ 6 ] & 0x80 )
      {
        MS5.Map5_Wram_Reg[ 6 ] = 0xff;
        W.ROMBANK2 = ROMPAGE( (MS5.Map5_Prg_Reg[6] & 0x7f) % ( S.NesHeader.ROMSize << 1 ) );
      } else {
        MS5.Map5_Wram_Reg[ 6 ] = MS5.Map5_Prg_Reg[ 6 ] & 0x07;
        W.ROMBANK2 = Map5_ROMPAGE( MS5.Map5_Wram_Reg[ 6 ] );
      }

      W.ROMBANK3 = ROMPAGE( (MS5.Map5_Prg_Reg[7] & 0x7f) % ( S.NesHeader.ROMSize << 1 ) );
      break;

    default:
      if ( MS5.Map5_Prg_Reg[ 4 ] & 0x80 )
      {
        MS5.Map5_Wram_Reg[ 4 ] = 0xff;
        W.ROMBANK0 = ROMPAGE( (MS5.Map5_Prg_Reg[4] & 0x7f) % ( S.NesHeader.ROMSize << 1 ) );
      } else {
        MS5.Map5_Wram_Reg[ 4 ] = MS5.Map5_Prg_Reg[ 4 ] & 0x07;
        W.ROMBANK0 = Map5_ROMPAGE( MS5.Map5_Wram_Reg[ 4 ] );
      }

      if ( MS5.Map5_Prg_Reg[ 5 ] & 0x80 )
      {
        MS5.Map5_Wram_Reg[ 5 ] = 0xff;
        W.ROMBANK1 = ROMPAGE( (MS5.Map5_Prg_Reg[5] & 0x7f) % ( S.NesHeader.ROMSize << 1 ) );
      } else {
        MS5.Map5_Wram_Reg[ 5 ] = MS5.Map5_Prg_Reg[ 5 ] & 0x07;
        W.ROMBANK1 = Map5_ROMPAGE( MS5.Map5_Wram_Reg[ 5 ] );
      }

      if ( MS5.Map5_Prg_Reg[ 6 ] & 0x80 )
      {
        MS5.Map5_Wram_Reg[ 6 ] = 0xff;
        W.ROMBANK2 = ROMPAGE( (MS5.Map5_Prg_Reg[6] & 0x7f) % ( S.NesHeader.ROMSize << 1 ) );
      } else {
        MS5.Map5_Wram_Reg[ 6 ] = MS5.Map5_Prg_Reg[ 6 ] & 0x07;
        W.ROMBANK2 = Map5_ROMPAGE( MS5.Map5_Wram_Reg[ 6 ] );
      }

      W.ROMBANK3 = ROMPAGE( (MS5.Map5_Prg_Reg[7] & 0x7f) % ( S.NesHeader.ROMSize << 1 ) );
      break;
  }
}

void Map5_Restore() {
  Map5_Sync_Prg_Banks();
}
