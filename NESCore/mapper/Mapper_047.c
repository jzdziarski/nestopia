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

byte  Map47_Regs[ 8 ];
dword Map47_Rom_Bank;
dword Map47_Prg0, Map47_Prg1;
dword Map47_Chr01, Map47_Chr23;
dword Map47_Chr4, Map47_Chr5, Map47_Chr6, Map47_Chr7;

#define Map47_Chr_Swap()    ( Map47_Regs[ 0 ] & 0x80 )
#define Map47_Prg_Swap()    ( Map47_Regs[ 0 ] & 0x40 )

byte Map47_IRQ_Enable;
byte Map47_IRQ_Cnt;
byte Map47_IRQ_Latch;

void Map47_Init()
{
  MapperInit = Map47_Init;
  MapperWrite = Map47_Write;
  MapperSram = Map47_Sram;
  MapperApu = Map0_Apu;
  MapperReadApu = Map0_ReadApu;
  MapperVSync = Map0_VSync;
  MapperHSync = Map47_HSync;
  MapperPPU = Map0_PPU;
  MapperRenderScreen = Map0_RenderScreen;

  /* Set SRAM Banks */
  W.SRAMBANK = S.SRAM;

  /* Initialize State Registers */
  int nPage ;
  for (nPage = 0; nPage < 8; nPage++ )
      Map47_Regs[ nPage ] = 0x00;

  /* Set ROM Banks */
  Map47_Rom_Bank = 0;
  Map47_Prg0 = 0;
  Map47_Prg1 = 1;
  Map47_Set_CPU_Banks();

  /* Set PPU Banks */
  if ( S.NesHeader.VROMSize > 0 )
  {
    Map47_Chr01 = 0;
    Map47_Chr23 = 2;
    Map47_Chr4  = 4;
    Map47_Chr5  = 5;
    Map47_Chr6  = 6;
    Map47_Chr7  = 7;
    Map47_Set_PPU_Banks();
  } else {
    Map47_Chr01 = Map47_Chr23 = 0;
    Map47_Chr4 = Map47_Chr5 = Map47_Chr6 = Map47_Chr7 = 0;
  }

  /* Initialize IRQ Registers */
  Map47_IRQ_Enable = 0;
  Map47_IRQ_Cnt = 0;
  Map47_IRQ_Latch = 0;
}

void Map47_Sram( word wAddr, byte byData )
{
  switch ( wAddr )
  {
    case 0x6000:
      Map47_Rom_Bank = byData & 0x01;
      Map47_Set_CPU_Banks();
      Map47_Set_PPU_Banks();
      break;
  }
}

void Map47_Write( word wAddr, byte byData )
{
  dword dwBankNum;

  switch ( wAddr & 0xe001 )
  {
    case 0x8000:
      Map47_Regs[ 0 ] = byData;
      Map47_Set_PPU_Banks();
      Map47_Set_CPU_Banks();
      break;

    case 0x8001:
      Map47_Regs[ 1 ] = byData;
      dwBankNum = Map47_Regs[ 1 ];

      switch ( Map47_Regs[ 0 ] & 0x07 )
      {
        /* Set PPU Banks */
        case 0x00:
          if ( S.NesHeader.VROMSize > 0 )
          {
            dwBankNum &= 0xfe;
            Map47_Chr01 = dwBankNum;
            Map47_Set_PPU_Banks();
          }
          break;

        case 0x01:
          if ( S.NesHeader.VROMSize > 0 )
          {
            dwBankNum &= 0xfe;
            Map47_Chr23 = dwBankNum;
            Map47_Set_PPU_Banks();
          }
          break;

        case 0x02:
          if ( S.NesHeader.VROMSize > 0 )
          {
            Map47_Chr4 = dwBankNum;
            Map47_Set_PPU_Banks();
          }
          break;

        case 0x03:
          if ( S.NesHeader.VROMSize > 0 )
          {
            Map47_Chr5 = dwBankNum;
            Map47_Set_PPU_Banks();
          }
          break;

        case 0x04:
          if ( S.NesHeader.VROMSize > 0 )
          {
            Map47_Chr6 = dwBankNum;
            Map47_Set_PPU_Banks();
          }
          break;

        case 0x05:
          if ( S.NesHeader.VROMSize > 0 )
          {
            Map47_Chr7 = dwBankNum;
            Map47_Set_PPU_Banks();
          }
          break;

        /* Set ROM Banks */
        case 0x06:
          Map47_Prg0 = dwBankNum;
          Map47_Set_CPU_Banks();
          break;

        case 0x07:
          Map47_Prg1 = dwBankNum;
          Map47_Set_CPU_Banks();
          break;
      }
      break;

    case 0xa000:
      Map47_Regs[ 3 ] = byData;
      break;

    case 0xc000:
      Map47_Regs[ 4 ] = byData;
      Map47_IRQ_Cnt = Map47_Regs[ 4 ];
      break;

    case 0xc001:
      Map47_Regs[ 5 ] = byData;
      Map47_IRQ_Latch = Map47_Regs[ 5 ];
      break;

    case 0xe000:
      Map47_Regs[ 6 ] = byData;
      Map47_IRQ_Enable = 0;
      break;

    case 0xe001:
      Map47_Regs[ 7 ] = byData;
      Map47_IRQ_Enable = 1;
      break;
  }
}

void Map47_HSync()
{
  if ( Map47_IRQ_Enable )
  {
    if ( /* 0 <= S.PPU_Scanline && */ S.PPU_Scanline <= 239 )
    {
      if ( S.PPU_R1 & R1_SHOW_BG || S.PPU_R1 & R1_SHOW_SP )
      {
        if ( !( --Map47_IRQ_Cnt ) )
        {
          Map47_IRQ_Cnt = Map47_IRQ_Latch;
          IRQ_REQ;
        }
      }
    }
  }
}

void Map47_Set_CPU_Banks()
{
  if ( Map47_Prg_Swap() )
  {
    W.ROMBANK0 = ROMPAGE( ( ( Map47_Rom_Bank << 4 ) + 14 ) % ( S.NesHeader.ROMSize << 1 ) );
    W.ROMBANK1 = ROMPAGE( ( ( Map47_Rom_Bank << 4 ) + Map47_Prg1 ) % ( S.NesHeader.ROMSize << 1 ) );
    W.ROMBANK2 = ROMPAGE( ( ( Map47_Rom_Bank << 4 ) + Map47_Prg0 ) % ( S.NesHeader.ROMSize << 1 ) );
    W.ROMBANK3 = ROMPAGE( ( ( Map47_Rom_Bank << 4 ) + 15 ) % ( S.NesHeader.ROMSize << 1 ) );
  } else {
    W.ROMBANK0 = ROMPAGE( ( ( Map47_Rom_Bank << 4 ) + Map47_Prg0 ) % ( S.NesHeader.ROMSize << 1 ) );
    W.ROMBANK1 = ROMPAGE( ( ( Map47_Rom_Bank << 4 ) + Map47_Prg1 ) % ( S.NesHeader.ROMSize << 1 ) );
    W.ROMBANK2 = ROMPAGE( ( ( Map47_Rom_Bank << 4 ) + 14 ) % ( S.NesHeader.ROMSize << 1 ) );
    W.ROMBANK3 = ROMPAGE( ( ( Map47_Rom_Bank << 4 ) + 15 ) % ( S.NesHeader.ROMSize << 1 ) );
  }
}

void Map47_Set_PPU_Banks()
{
  if ( S.NesHeader.VROMSize > 0 )
  {
    if ( Map47_Chr_Swap() )
    { 
      W.PPUBANK[ 0 ] = VROMPAGE( ( ( Map47_Rom_Bank << 7 ) + Map47_Chr4 ) % ( S.NesHeader.VROMSize << 3 ) );
      W.PPUBANK[ 1 ] = VROMPAGE( ( ( Map47_Rom_Bank << 7 ) + Map47_Chr5 ) % ( S.NesHeader.VROMSize << 3 ) );
      W.PPUBANK[ 2 ] = VROMPAGE( ( ( Map47_Rom_Bank << 7 ) + Map47_Chr6 ) % ( S.NesHeader.VROMSize << 3 ) );
      W.PPUBANK[ 3 ] = VROMPAGE( ( ( Map47_Rom_Bank << 7 ) + Map47_Chr7 ) % ( S.NesHeader.VROMSize << 3 ) );
      W.PPUBANK[ 4 ] = VROMPAGE( ( ( Map47_Rom_Bank << 7 ) + ( Map47_Chr01 + 0 ) ) % ( S.NesHeader.VROMSize << 3 ) );
      W.PPUBANK[ 5 ] = VROMPAGE( ( ( Map47_Rom_Bank << 7 ) + ( Map47_Chr01 + 1 ) ) % ( S.NesHeader.VROMSize << 3 ) );
      W.PPUBANK[ 6 ] = VROMPAGE( ( ( Map47_Rom_Bank << 7 ) + ( Map47_Chr23 + 0 ) ) % ( S.NesHeader.VROMSize << 3 ) );
      W.PPUBANK[ 7 ] = VROMPAGE( ( ( Map47_Rom_Bank << 7 ) + ( Map47_Chr23 + 1 ) ) % ( S.NesHeader.VROMSize << 3 ) );
      NESCore_Develop_Character_Data();
    } else {
      W.PPUBANK[ 0 ] = VROMPAGE( ( ( Map47_Rom_Bank << 7 ) + ( Map47_Chr01 + 0 ) ) % ( S.NesHeader.VROMSize << 3 ) );
      W.PPUBANK[ 1 ] = VROMPAGE( ( ( Map47_Rom_Bank << 7 ) + ( Map47_Chr01 + 1 ) ) % ( S.NesHeader.VROMSize << 3 ) );
      W.PPUBANK[ 2 ] = VROMPAGE( ( ( Map47_Rom_Bank << 7 ) + ( Map47_Chr23 + 0 ) ) % ( S.NesHeader.VROMSize << 3 ) );
      W.PPUBANK[ 3 ] = VROMPAGE( ( ( Map47_Rom_Bank << 7 ) + ( Map47_Chr23 + 1 ) ) % ( S.NesHeader.VROMSize << 3 ) );
      W.PPUBANK[ 4 ] = VROMPAGE( ( ( Map47_Rom_Bank << 7 ) + Map47_Chr4 ) % ( S.NesHeader.VROMSize << 3 ) );
      W.PPUBANK[ 5 ] = VROMPAGE( ( ( Map47_Rom_Bank << 7 ) + Map47_Chr5 ) % ( S.NesHeader.VROMSize << 3 ) );
      W.PPUBANK[ 6 ] = VROMPAGE( ( ( Map47_Rom_Bank << 7 ) + Map47_Chr6 ) % ( S.NesHeader.VROMSize << 3 ) );
      W.PPUBANK[ 7 ] = VROMPAGE( ( ( Map47_Rom_Bank << 7 ) + Map47_Chr7 ) % ( S.NesHeader.VROMSize << 3 ) );
      NESCore_Develop_Character_Data();
    }
  }
}
