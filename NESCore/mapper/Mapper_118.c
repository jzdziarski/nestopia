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

/* MMC3 */

#include <string.h>

struct Map118_State {
    byte  Map118_Regs[8];
    dword Map118_Rom_Bank;
    dword Map118_Prg0, Map118_Prg1;
    dword Map118_PPU[6];
    byte Map118_IRQ_Enabled;
    byte Map118_IRQ_Counter;
    byte Map118_IRQ_Latch;
};

struct Map118_State MS118;

#define Map118_Chr_Swap() (MS118.Map118_Regs[ 0 ] & 0x80)
#define Map118_Prg_Swap() (MS118.Map118_Regs[ 0 ] & 0x40)

void Map118_Init()
{
  MapperInit     = Map118_Init;
  MapperWrite    = Map118_Write;
  MapperSram     = Map0_Sram;
  MapperApu      = Map0_Apu;
  MapperReadApu  = Map0_ReadApu;
  MapperVSync    = Map0_VSync;
  MapperHSync    = Map118_HSync;
  MapperPPU      = Map0_PPU;
  MapperRenderScreen = Map0_RenderScreen;
  MapperRestore = Map118_Restore;
  W.SRAMBANK = S.SRAM;

  /* Initialize State Registers */
  memset(&MS118.Map118_Regs, 0, sizeof(MS118.Map118_Regs));

  /* Set ROM Banks */
  MS118.Map118_Prg0 = 0;
  MS118.Map118_Prg1 = 1;
  Map118_Set_CPU_Banks();

  /* Set PPU Banks */
  if ( S.NesHeader.VROMSize > 0 )
  {
     W.PPUBANK[0] = VROMPAGE(0);
	 W.PPUBANK[1] = VROMPAGE(1);
	 W.PPUBANK[2] = VROMPAGE(2);
	 W.PPUBANK[3] = VROMPAGE(3);
	 W.PPUBANK[4] = VROMPAGE(4);
	 W.PPUBANK[5] = VROMPAGE(5);
	 W.PPUBANK[6] = VROMPAGE(6);
	 W.PPUBANK[7] = VROMPAGE(7);

  } else {
    memset(&MS118.Map118_PPU, 0, sizeof(MS118.Map118_PPU));
  }

  M.state = &MS118;
  M.size = sizeof(MS118);
}

void Map118_Write( word wAddr, byte bData )
{
  dword dwBankNum;

  switch ( wAddr & 0xe001 )
  {
    case 0x8000:
      MS118.Map118_Regs[ 0 ] = bData;

      Map118_Set_CPU_Banks();

      break;

    case 0x8001:
      MS118.Map118_Regs[ 1 ] = bData;
      dwBankNum = MS118.Map118_Regs[ 1 ];
      if ((MS118.Map118_Regs[0] & 0x07) < 6)
      {
          if (bData & 0x80)
              NESCore_Mirroring(3);
          else
              NESCore_Mirroring(2);
      }

      switch ( MS118.Map118_Regs[ 0 ] & 0x07 )
      {
        /* Set PPU Banks */
        case 0x00:
          if ( S.NesHeader.VROMSize > 0 )
          {
          dwBankNum &= 0xfe;
          MS118.Map118_PPU[0] = dwBankNum;
    if ( Map118_Chr_Swap() )
    {
        W.PPUBANK[4] = VROMPAGE((MS118.Map118_PPU[0]  ) % (S.NesHeader.VROMSize << 3));
        W.PPUBANK[5] = VROMPAGE((MS118.Map118_PPU[0]+1) % (S.NesHeader.VROMSize << 3));
    } else {
        W.PPUBANK[0] = VROMPAGE((MS118.Map118_PPU[0]  ) % (S.NesHeader.VROMSize << 3));
        W.PPUBANK[1] = VROMPAGE((MS118.Map118_PPU[0]+1) % (S.NesHeader.VROMSize << 3));
    }
    NESCore_Develop_Character_Data();
          }
          break;

        case 0x01:
          if ( S.NesHeader.VROMSize > 0 )
          {
          dwBankNum &= 0xfe;
          MS118.Map118_PPU[1] = dwBankNum;

    if ( Map118_Chr_Swap() )
    {
        W.PPUBANK[6] = VROMPAGE((MS118.Map118_PPU[1]  ) % (S.NesHeader.VROMSize << 3));
        W.PPUBANK[7] = VROMPAGE((MS118.Map118_PPU[1]+1) % (S.NesHeader.VROMSize << 3));
    } else {
        W.PPUBANK[2] = VROMPAGE((MS118.Map118_PPU[1]  ) % (S.NesHeader.VROMSize << 3));
        W.PPUBANK[3] = VROMPAGE((MS118.Map118_PPU[1]+1) % (S.NesHeader.VROMSize << 3));
    }
    NESCore_Develop_Character_Data();
          }
          break;

        case 0x02:
          if ( S.NesHeader.VROMSize > 0 )
          {
          MS118.Map118_PPU[2] = dwBankNum;
    if ( Map118_Chr_Swap() )
    {
        W.PPUBANK[0] = VROMPAGE( MS118.Map118_PPU[2] % ( S.NesHeader.VROMSize << 3 ) );
    } else {
        W.PPUBANK[4] = VROMPAGE( MS118.Map118_PPU[2] % ( S.NesHeader.VROMSize << 3 ) );
    }
    NESCore_Develop_Character_Data();
          }
          break;

        case 0x03:
          if ( S.NesHeader.VROMSize > 0 )
          {
          MS118.Map118_PPU[3] = dwBankNum;
    if ( Map118_Chr_Swap() )
    {
        W.PPUBANK[1] = VROMPAGE( MS118.Map118_PPU[3] % ( S.NesHeader.VROMSize << 3 ) );
    } else {
        W.PPUBANK[5] = VROMPAGE( MS118.Map118_PPU[3] % ( S.NesHeader.VROMSize << 3 ) );
    }
    NESCore_Develop_Character_Data();
          }
          break;

        case 0x04:
          if ( S.NesHeader.VROMSize > 0 )
          {
          MS118.Map118_PPU[4] = dwBankNum;
    if ( Map118_Chr_Swap() )
    {
        W.PPUBANK[2] = VROMPAGE( MS118.Map118_PPU[4] % ( S.NesHeader.VROMSize << 3 ) );
    } else {
        W.PPUBANK[6] = VROMPAGE( MS118.Map118_PPU[4] % ( S.NesHeader.VROMSize << 3 ) );
    }
    NESCore_Develop_Character_Data();
          }
          break;

        case 0x05:
          if ( S.NesHeader.VROMSize > 0 )
          {
          MS118.Map118_PPU[5] = dwBankNum;
    if ( Map118_Chr_Swap() )
    {
        W.PPUBANK[3] = VROMPAGE( MS118.Map118_PPU[5] % ( S.NesHeader.VROMSize << 3 ) );
    } else {
        W.PPUBANK[7] = VROMPAGE( MS118.Map118_PPU[5] % ( S.NesHeader.VROMSize << 3 ) );
    }
    NESCore_Develop_Character_Data();
          }
          break;

        /* Set ROM Banks */
        case 0x06:
          MS118.Map118_Prg0 = dwBankNum;
          Map118_Set_CPU_Banks();
          break;

        case 0x07:
          MS118.Map118_Prg1 = dwBankNum;
          Map118_Set_CPU_Banks();
          break;
      }
      break;

    case 0xe000: /* ACK IRQ */
      MS118.Map118_IRQ_Enabled = 0;
      break;

    case 0xe001: /* Enable IRQ */
      MS118.Map118_IRQ_Enabled = 1;
      break;
  }
  
  if (wAddr >= 0xc000 && wAddr <= 0xDFFF) {
     if (wAddr % 2 == 0) {
	    MS118.Map118_IRQ_Latch = bData;
	 }
	 else {
		MS118.Map118_IRQ_Counter = MS118.Map118_IRQ_Latch;
	 }
  }
}

void Map118_HSync()
{
  int oldCounter = MS118.Map118_IRQ_Counter;
  if (S.PPU_Scanline < 240
       && ((S.PPU_R1 & R1_SHOW_BG) || (S.PPU_R1 & R1_SHOW_SP)))
  {
      if (MS118.Map118_IRQ_Counter != 0) {
          MS118.Map118_IRQ_Counter--;
      } else {
          MS118.Map118_IRQ_Counter = MS118.Map118_IRQ_Latch;
      } 
	  if ((oldCounter > 0) && (MS118.Map118_IRQ_Counter == 0) && (MS118.Map118_IRQ_Enabled)) {
	     IRQ_REQ;
	  }
  }
}

void Map118_Set_CPU_Banks()
{

  if (Map118_Prg_Swap())
  {
    W.ROMBANK0 = ROMLASTPAGE( 1 );
    W.ROMBANK1 = ROMPAGE( MS118.Map118_Prg1 % ( S.NesHeader.ROMSize << 1 ) );
    W.ROMBANK2 = ROMPAGE( MS118.Map118_Prg0 % ( S.NesHeader.ROMSize << 1 ) );
    W.ROMBANK3 = ROMLASTPAGE( 0 );
  } else {
    W.ROMBANK0 = ROMPAGE( MS118.Map118_Prg0 % ( S.NesHeader.ROMSize << 1 ) );
    W.ROMBANK1 = ROMPAGE( MS118.Map118_Prg1 % ( S.NesHeader.ROMSize << 1 ) );
    W.ROMBANK2 = ROMLASTPAGE( 1 );
    W.ROMBANK3 = ROMLASTPAGE( 0 );
  }
}

void Map118_Set_PPU_Banks()
{
  if ( S.NesHeader.VROMSize > 0 )
  {
    if ( Map118_Chr_Swap() )
    { 
        W.PPUBANK[0] = VROMPAGE( MS118.Map118_PPU[2] % ( S.NesHeader.VROMSize << 3 ) );
        W.PPUBANK[1] = VROMPAGE( MS118.Map118_PPU[3] % ( S.NesHeader.VROMSize << 3 ) );
        W.PPUBANK[2] = VROMPAGE( MS118.Map118_PPU[4] % ( S.NesHeader.VROMSize << 3 ) );
        W.PPUBANK[3] = VROMPAGE( MS118.Map118_PPU[5] % ( S.NesHeader.VROMSize << 3 ) );
        W.PPUBANK[4] = VROMPAGE((MS118.Map118_PPU[0]  ) % (S.NesHeader.VROMSize << 3));
        W.PPUBANK[5] = VROMPAGE((MS118.Map118_PPU[0]+1) % (S.NesHeader.VROMSize << 3));
        W.PPUBANK[6] = VROMPAGE((MS118.Map118_PPU[1]  ) % (S.NesHeader.VROMSize << 3));
        W.PPUBANK[7] = VROMPAGE((MS118.Map118_PPU[1]+1) % (S.NesHeader.VROMSize << 3));
    } else {
        W.PPUBANK[4] = VROMPAGE( MS118.Map118_PPU[2] % ( S.NesHeader.VROMSize << 3 ) );
        W.PPUBANK[5] = VROMPAGE( MS118.Map118_PPU[3] % ( S.NesHeader.VROMSize << 3 ) );
        W.PPUBANK[6] = VROMPAGE( MS118.Map118_PPU[4] % ( S.NesHeader.VROMSize << 3 ) );
        W.PPUBANK[7] = VROMPAGE( MS118.Map118_PPU[5] % ( S.NesHeader.VROMSize << 3 ) );
        W.PPUBANK[0] = VROMPAGE((MS118.Map118_PPU[0]  ) % (S.NesHeader.VROMSize << 3));
        W.PPUBANK[1] = VROMPAGE((MS118.Map118_PPU[0]+1) % (S.NesHeader.VROMSize << 3));
        W.PPUBANK[2] = VROMPAGE((MS118.Map118_PPU[1]  ) % (S.NesHeader.VROMSize << 3));
        W.PPUBANK[3] = VROMPAGE((MS118.Map118_PPU[1]+1) % (S.NesHeader.VROMSize << 3));
    }
    NESCore_Develop_Character_Data();
  }
  else 
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
  }    
}

void Map118_Restore()
{
    Map118_Set_PPU_Banks();
}

