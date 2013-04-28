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

byte  Map74_Regs[ 8 ];
dword Map74_Rom_Bank;
dword Map74_Prg0, Map74_Prg1;
dword Map74_Chr01, Map74_Chr23;
dword Map74_Chr4, Map74_Chr5, Map74_Chr6, Map74_Chr7;

#define Map74_Chr_Swap()    ( Map74_Regs[ 0 ] & 0x80 )
#define Map74_Prg_Swap()    ( Map74_Regs[ 0 ] & 0x40 )

byte Map74_IRQ_Enable;
byte Map74_IRQ_Cnt;
byte Map74_IRQ_Latch;
byte Map74_IRQ_Request;
byte Map74_IRQ_Present;
byte Map74_IRQ_Present_Vbl;

void Map74_Init()
{
  MapperInit = Map74_Init;
  MapperWrite = Map74_Write;
  MapperSram = Map0_Sram;
  MapperApu = Map0_Apu;
  MapperReadApu = Map0_ReadApu;
  MapperVSync = Map0_VSync;
  MapperHSync = Map74_HSync;
  MapperPPU = Map0_PPU;
  MapperRenderScreen = Map0_RenderScreen;

  /* Set SRAM Banks */
  W.SRAMBANK = S.SRAM;

  /* Initialize State Registers */
  int nPage ;
  for (nPage = 0; nPage < 8; nPage++ )
      Map74_Regs[ nPage ] = 0x00;

  /* Set ROM Banks */
  Map74_Prg0 = 0;
  Map74_Prg1 = 1;
  Map74_Set_CPU_Banks();

  /* Set PPU Banks */
  if ( S.NesHeader.VROMSize > 0 )
  {
    Map74_Chr01 = 0;
    Map74_Chr23 = 2;
    Map74_Chr4  = 4;
    Map74_Chr5  = 5;
    Map74_Chr6  = 6;
    Map74_Chr7  = 7;
    Map74_Set_PPU_Banks();
  } else {
    Map74_Chr01 = Map74_Chr23 = 0;
    Map74_Chr4 = Map74_Chr5 = Map74_Chr6 = Map74_Chr7 = 0;
  }

  /* Initialize IRQ Registers */
  Map74_IRQ_Enable = 0;
  Map74_IRQ_Cnt = 0;
  Map74_IRQ_Latch = 0;
  Map74_IRQ_Request = 0;
  Map74_IRQ_Present = 0;
  Map74_IRQ_Present_Vbl = 0;

  /* VRAM Write Enabled */
  S.VRAMWriteEnable = 1;
}

void Map74_Write( word wAddr, byte byData )
{
  dword dwBankNum;

  switch ( wAddr & 0xe001 )
  {
    case 0x8000:
      Map74_Regs[ 0 ] = byData;
      Map74_Set_PPU_Banks();
      Map74_Set_CPU_Banks();
      break;

    case 0x8001:
      Map74_Regs[ 1 ] = byData;
      dwBankNum = Map74_Regs[ 1 ];

      switch ( Map74_Regs[ 0 ] & 0x07 )
      {
        /* Set PPU Banks */
        case 0x00:
          if ( S.NesHeader.VROMSize > 0 )
          {
            dwBankNum &= 0xfe;
            Map74_Chr01 = dwBankNum;
            Map74_Set_PPU_Banks();
          }
          break;

        case 0x01:
          if ( S.NesHeader.VROMSize > 0 )
          {
            dwBankNum &= 0xfe;
            Map74_Chr23 = dwBankNum;
            Map74_Set_PPU_Banks();
          }
          break;

        case 0x02:
          if ( S.NesHeader.VROMSize > 0 )
          {
            Map74_Chr4 = dwBankNum;
            Map74_Set_PPU_Banks();
          }
          break;

        case 0x03:
          if ( S.NesHeader.VROMSize > 0 )
          {
            Map74_Chr5 = dwBankNum;
            Map74_Set_PPU_Banks();
          }
          break;

        case 0x04:
          if ( S.NesHeader.VROMSize > 0 )
          {
            Map74_Chr6 = dwBankNum;
            Map74_Set_PPU_Banks();
          }
          break;

        case 0x05:
          if ( S.NesHeader.VROMSize > 0 )
          {
            Map74_Chr7 = dwBankNum;
            Map74_Set_PPU_Banks();
          }
          break;

        /* Set ROM Banks */
        case 0x06:
          Map74_Prg0 = dwBankNum;
          Map74_Set_CPU_Banks();
          break;

        case 0x07:
          Map74_Prg1 = dwBankNum;
          Map74_Set_CPU_Banks();
          break;
      }
      break;

    case 0xa000:
      Map74_Regs[ 2 ] = byData;

      if ( !S.ROM_FourScr )
      {
        if ( byData & 0x01 )
        {
          NESCore_Mirroring( 0 );
        } else {
          NESCore_Mirroring( 1 );
        }
      }
      break;

    case 0xa001:
      Map74_Regs[ 3 ] = byData;
      break;

    case 0xc000:
      Map74_Regs[ 4 ] = byData;
      Map74_IRQ_Latch = byData;
      break;

    case 0xc001:
      Map74_Regs[ 5 ] = byData;
      if ( S.PPU_Scanline < 240 )
      {
          Map74_IRQ_Cnt |= 0x80;
          Map74_IRQ_Present = 0xff;
      } else {
          Map74_IRQ_Cnt |= 0x80;
          Map74_IRQ_Present_Vbl = 0xff;
          Map74_IRQ_Present = 0;
      }
      break;

    case 0xe000:
      Map74_Regs[ 6 ] = byData;
      Map74_IRQ_Enable = 0;
			Map74_IRQ_Request = 0;
      break;

    case 0xe001:
      Map74_Regs[ 7 ] = byData;
      Map74_IRQ_Enable = 1;
			Map74_IRQ_Request = 0;
      break;
  }
}

void Map74_HSync()
{
  if ( ( /* 0 <= S.PPU_Scanline && */ S.PPU_Scanline <= 239 ) && 
       ( S.PPU_R1 & R1_SHOW_BG || S.PPU_R1 & R1_SHOW_SP ) )
  {
		if( Map74_IRQ_Present_Vbl ) {
			Map74_IRQ_Cnt = Map74_IRQ_Latch;
			Map74_IRQ_Present_Vbl = 0;
		}
		if( Map74_IRQ_Present ) {
			Map74_IRQ_Cnt = Map74_IRQ_Latch;
			Map74_IRQ_Present = 0;
		} else if( Map74_IRQ_Cnt > 0 ) {
			Map74_IRQ_Cnt--;
		}

		if( Map74_IRQ_Cnt == 0 ) {
			if( Map74_IRQ_Enable ) {
				Map74_IRQ_Request = 0xFF;
			}
			Map74_IRQ_Present = 0xFF;
		}
	}
	if( Map74_IRQ_Request  ) {
		IRQ_REQ;
	}
}

void Map74_Set_CPU_Banks()
{
  if ( Map74_Prg_Swap() )
  {
    W.ROMBANK0 = ROMLASTPAGE( 1 );
    W.ROMBANK1 = ROMPAGE( Map74_Prg1 % ( S.NesHeader.ROMSize << 1 ) );
    W.ROMBANK2 = ROMPAGE( Map74_Prg0 % ( S.NesHeader.ROMSize << 1 ) );
    W.ROMBANK3 = ROMLASTPAGE( 0 );
  } else {
    W.ROMBANK0 = ROMPAGE( Map74_Prg0 % ( S.NesHeader.ROMSize << 1 ) );
    W.ROMBANK1 = ROMPAGE( Map74_Prg1 % ( S.NesHeader.ROMSize << 1 ) );
    W.ROMBANK2 = ROMLASTPAGE( 1 );
    W.ROMBANK3 = ROMLASTPAGE( 0 );
  }
}

void Map74_Set_PPU_Banks()
{
  if ( S.NesHeader.VROMSize > 0 )
  {
    if ( Map74_Chr_Swap() )
    { 
      W.PPUBANK[ 0 ] = VROMPAGE( Map74_Chr4 % ( S.NesHeader.VROMSize << 3 ) );
      W.PPUBANK[ 1 ] = VROMPAGE( Map74_Chr5 % ( S.NesHeader.VROMSize << 3 ) );
      W.PPUBANK[ 2 ] = VROMPAGE( Map74_Chr6 % ( S.NesHeader.VROMSize << 3 ) );
      W.PPUBANK[ 3 ] = VROMPAGE( Map74_Chr7 % ( S.NesHeader.VROMSize << 3 ) );
      W.PPUBANK[ 4 ] = VROMPAGE( ( Map74_Chr01 + 0 ) % ( S.NesHeader.VROMSize << 3 ) );
      W.PPUBANK[ 5 ] = VROMPAGE( ( Map74_Chr01 + 1 ) % ( S.NesHeader.VROMSize << 3 ) );
      W.PPUBANK[ 6 ] = VROMPAGE( ( Map74_Chr23 + 0 ) % ( S.NesHeader.VROMSize << 3 ) );
      W.PPUBANK[ 7 ] = VROMPAGE( ( Map74_Chr23 + 1 ) % ( S.NesHeader.VROMSize << 3 ) );
      NESCore_Develop_Character_Data();
    } else {
      W.PPUBANK[ 0 ] = VROMPAGE( ( Map74_Chr01 + 0 ) % ( S.NesHeader.VROMSize << 3 ) );
      W.PPUBANK[ 1 ] = VROMPAGE( ( Map74_Chr01 + 1 ) % ( S.NesHeader.VROMSize << 3 ) );
      W.PPUBANK[ 2 ] = VROMPAGE( ( Map74_Chr23 + 0 ) % ( S.NesHeader.VROMSize << 3 ) );
      W.PPUBANK[ 3 ] = VROMPAGE( ( Map74_Chr23 + 1 ) % ( S.NesHeader.VROMSize << 3 ) );
      W.PPUBANK[ 4 ] = VROMPAGE( Map74_Chr4 % ( S.NesHeader.VROMSize << 3 ) );
      W.PPUBANK[ 5 ] = VROMPAGE( Map74_Chr5 % ( S.NesHeader.VROMSize << 3 ) );
      W.PPUBANK[ 6 ] = VROMPAGE( Map74_Chr6 % ( S.NesHeader.VROMSize << 3 ) );
      W.PPUBANK[ 7 ] = VROMPAGE( Map74_Chr7 % ( S.NesHeader.VROMSize << 3 ) );
      NESCore_Develop_Character_Data();
    }
  }
  else
  {
    if ( Map74_Chr_Swap() )
    { 
      W.PPUBANK[ 0 ] = CRAMPAGE( 0 );
      W.PPUBANK[ 1 ] = CRAMPAGE( 1 );
      W.PPUBANK[ 2 ] = CRAMPAGE( 2 );
      W.PPUBANK[ 3 ] = CRAMPAGE( 3 );
      W.PPUBANK[ 4 ] = CRAMPAGE( 4 );
      W.PPUBANK[ 5 ] = CRAMPAGE( 5 );
      W.PPUBANK[ 6 ] = CRAMPAGE( 6 );
      W.PPUBANK[ 7 ] = CRAMPAGE( 7 );
      NESCore_Develop_Character_Data();
    } else {
      W.PPUBANK[ 0 ] = CRAMPAGE( 0 );
      W.PPUBANK[ 1 ] = CRAMPAGE( 1 );
      W.PPUBANK[ 2 ] = CRAMPAGE( 2 );
      W.PPUBANK[ 3 ] = CRAMPAGE( 3 );
      W.PPUBANK[ 4 ] = CRAMPAGE( 4 );
      W.PPUBANK[ 5 ] = CRAMPAGE( 5 );
      W.PPUBANK[ 6 ] = CRAMPAGE( 6 );
      W.PPUBANK[ 7 ] = CRAMPAGE( 7 );
      NESCore_Develop_Character_Data();
    }
  }    
}

