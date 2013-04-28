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

struct Map4_State {
    byte  Regs[8];
    dword Prg0, Prg1;
    dword PPU[6];
    byte IRQ_Enabled;
    byte IRQ_Counter;
    byte IRQ_Latch;
};

struct Map4_State MS4;

#define Map4_Chr_Swap() (MS4.Regs[ 0 ] & 0x80)
#define Map4_Prg_Swap() (MS4.Regs[ 0 ] & 0x40)
#define Map4_Vsz (S.NesHeader.VROMSize <<3)

void Map4_Init()
{
  MapperInit     = Map4_Init;
  MapperWrite    = Map4_Write;
  MapperSram     = Map0_Sram;
  MapperApu      = Map0_Apu;
  MapperReadApu  = Map0_ReadApu;
  MapperVSync    = Map0_VSync;
  MapperHSync    = Map4_HSync;
  MapperPPU      = Map0_PPU;
  MapperRenderScreen = Map0_RenderScreen;
  MapperRestore = Map4_Restore;
  W.SRAMBANK = S.SRAM;

  /* Initialize State Registers */
  memset(&MS4.Regs, 0, sizeof(MS4.Regs));

  /* Set ROM Banks */
  MS4.Prg0 = 0;
  MS4.Prg1 = 1;
  Map4_Set_CPU_Banks();

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
    memset(&MS4.PPU, 0, sizeof(MS4.PPU));
  }

  M.state = &MS4;
  M.size = sizeof(MS4);
}

void Map4_Write( word wAddr, byte bData )
{
  dword dwBankNum;

  switch ( wAddr & 0xe001 )
  {
    case 0x8000:
      MS4.Regs[ 0 ] = bData;

      Map4_Set_CPU_Banks();

      break;

    case 0x8001:
      MS4.Regs[ 1 ] = bData;
      dwBankNum = MS4.Regs[ 1 ];

      switch ( MS4.Regs[ 0 ] & 0x07 )
      {
        /* Set PPU Banks */
        case 0x00:
          if ( S.NesHeader.VROMSize > 0 )
          {
              dwBankNum &= 0xfe;
              MS4.PPU[0] = dwBankNum;
              if ( Map4_Chr_Swap() )
              {
                  W.PPUBANK[4] = VROMPAGE((MS4.PPU[0]  ) % (Map4_Vsz));
                  W.PPUBANK[5] = VROMPAGE((MS4.PPU[0]+1) % (Map4_Vsz));
              } else {
                  W.PPUBANK[0] = VROMPAGE((MS4.PPU[0]  ) % (Map4_Vsz));
                  W.PPUBANK[1] = VROMPAGE((MS4.PPU[0]+1) % (Map4_Vsz));
              }
              NESCore_Develop_Character_Data();
          }
          break;

        case 0x01:
          if ( S.NesHeader.VROMSize > 0 )
          {
              dwBankNum &= 0xfe;
              MS4.PPU[1] = dwBankNum;
              if ( Map4_Chr_Swap() )
              {
                  W.PPUBANK[6] = VROMPAGE((MS4.PPU[1]  ) % (Map4_Vsz));
                  W.PPUBANK[7] = VROMPAGE((MS4.PPU[1]+1) % (Map4_Vsz));
              } else {
                  W.PPUBANK[2] = VROMPAGE((MS4.PPU[1]  ) % (Map4_Vsz));
                  W.PPUBANK[3] = VROMPAGE((MS4.PPU[1]+1) % (Map4_Vsz));
              }
              NESCore_Develop_Character_Data();
          }
          break;

        case 0x02:
          if ( S.NesHeader.VROMSize > 0 )
          {
              MS4.PPU[2] = dwBankNum;
              if ( Map4_Chr_Swap() )
                  W.PPUBANK[0] = VROMPAGE( MS4.PPU[2] % ( Map4_Vsz ) );
              else
                  W.PPUBANK[4] = VROMPAGE( MS4.PPU[2] % ( Map4_Vsz ) );
              NESCore_Develop_Character_Data();
          }
          break;

        case 0x03:
          if ( S.NesHeader.VROMSize > 0 )
          {
              MS4.PPU[3] = dwBankNum;
              if ( Map4_Chr_Swap() )
                 W.PPUBANK[1] = VROMPAGE( MS4.PPU[3] % ( Map4_Vsz ) );
              else
                 W.PPUBANK[5] = VROMPAGE( MS4.PPU[3] % ( Map4_Vsz ) );
              NESCore_Develop_Character_Data();
          }
          break;

        case 0x04:
          if ( S.NesHeader.VROMSize > 0 )
          {
              MS4.PPU[4] = dwBankNum;
              if ( Map4_Chr_Swap() )
                  W.PPUBANK[2] = VROMPAGE( MS4.PPU[4] % ( Map4_Vsz ) );
              else
                  W.PPUBANK[6] = VROMPAGE( MS4.PPU[4] % ( Map4_Vsz ) );
              NESCore_Develop_Character_Data();
          }
          break;

        case 0x05:
          if ( S.NesHeader.VROMSize > 0 )
          {
              MS4.PPU[5] = dwBankNum;
              if ( Map4_Chr_Swap() )
                  W.PPUBANK[3] = VROMPAGE( MS4.PPU[5] % ( Map4_Vsz ) );
              else
                  W.PPUBANK[7] = VROMPAGE( MS4.PPU[5] % ( Map4_Vsz ) );
              NESCore_Develop_Character_Data();
          }
          break;

        /* Set ROM Banks */
        case 0x06:
          MS4.Prg0 = dwBankNum;
          Map4_Set_CPU_Banks();
          break;

        case 0x07:
          MS4.Prg1 = dwBankNum;
          Map4_Set_CPU_Banks();
          break;
      }
      break;

    case 0xa000:
      MS4.Regs[ 2 ] = bData & 1;
      if (!S.ROM_FourScr)
      {
        if ( bData & 0x01 )
        {
            NESCore_Mirroring( 0 );
        } else {
            NESCore_Mirroring( 1 );
        }
      }
      break;

    case 0xa001:
      MS4.Regs[ 3 ] = bData;
      break;
 
    case 0xe000: /* ACK IRQ */
      MS4.IRQ_Enabled = 0;
      break;

    case 0xe001: /* Enable IRQ */
      MS4.IRQ_Enabled = 1;
      break;
  }
  
  /* Old IRQ Technique */ 
  /*
  if (wAddr >= 0xc000 && wAddr <= 0xdfff) {
     if (wAddr % 2 == 0) {
         MS4.IRQ_Counter = bData;
     }
     else {
         MS4.IRQ_Latch = bData;
     }
  }
  */

  if (wAddr >= 0xc000 && wAddr <= 0xdfff) {
     if (wAddr % 2 == 0) {
         MS4.IRQ_Latch = bData;
     }
     else {
         MS4.IRQ_Counter = MS4.IRQ_Latch;
     }
  }
}

/* Old IRQ Technique */

/*
void Map4_HSync()
{
  if ( MS4.IRQ_Enabled
       && S.PPU_Scanline < 240
       && ((S.PPU_R1 & R1_SHOW_BG) || (S.PPU_R1 & R1_SHOW_SP)))
  {
      if (MS4.IRQ_Counter != 0) {
          MS4.IRQ_Counter--;
      } else {
          MS4.IRQ_Counter = MS4.IRQ_Latch;
          IRQ_REQ;
      }
  }
}
*/

void Map4_HSync()
{
  int oldCounter = MS4.IRQ_Counter;
  if (    S.PPU_Scanline < 240
     && ((S.PPU_R1 & R1_SHOW_BG) || (S.PPU_R1 & R1_SHOW_SP)))
  {
      if (MS4.IRQ_Counter != 0) {
          MS4.IRQ_Counter--;
      } else {
          MS4.IRQ_Counter = MS4.IRQ_Latch;
      } 

      if (   (oldCounter > 0) 
          && (MS4.IRQ_Counter == 0) 
          && (MS4.IRQ_Enabled) ) 
      {
          IRQ_REQ;
      }
  }
}

void Map4_Set_CPU_Banks()
{

  if (Map4_Prg_Swap())
  {
    W.ROMBANK0 = ROMLASTPAGE( 1 );
    W.ROMBANK1 = ROMPAGE( MS4.Prg1 % ( S.NesHeader.ROMSize << 1 ) );
    W.ROMBANK2 = ROMPAGE( MS4.Prg0 % ( S.NesHeader.ROMSize << 1 ) );
    W.ROMBANK3 = ROMLASTPAGE( 0 );
  } else {
    W.ROMBANK0 = ROMPAGE( MS4.Prg0 % ( S.NesHeader.ROMSize << 1 ) );
    W.ROMBANK1 = ROMPAGE( MS4.Prg1 % ( S.NesHeader.ROMSize << 1 ) );
    W.ROMBANK2 = ROMLASTPAGE( 1 );
    W.ROMBANK3 = ROMLASTPAGE( 0 );
  }
}

void Map4_Set_PPU_Banks()
{
  if ( S.NesHeader.VROMSize > 0 )
  {
    if ( Map4_Chr_Swap() )
    { 
      W.PPUBANK[0] = VROMPAGE(MS4.PPU[2] % (Map4_Vsz));
      W.PPUBANK[1] = VROMPAGE(MS4.PPU[3] % (Map4_Vsz));
      W.PPUBANK[2] = VROMPAGE(MS4.PPU[4] % (Map4_Vsz));
      W.PPUBANK[3] = VROMPAGE(MS4.PPU[5] % (Map4_Vsz));
      W.PPUBANK[4] = VROMPAGE((MS4.PPU[0]  ) % (Map4_Vsz));
      W.PPUBANK[5] = VROMPAGE((MS4.PPU[0]+1) % (Map4_Vsz));
      W.PPUBANK[6] = VROMPAGE((MS4.PPU[1]  ) % (Map4_Vsz));
      W.PPUBANK[7] = VROMPAGE((MS4.PPU[1]+1) % (Map4_Vsz));
    } else {
      W.PPUBANK[4] = VROMPAGE(MS4.PPU[2] % (Map4_Vsz));
      W.PPUBANK[5] = VROMPAGE(MS4.PPU[3] % (Map4_Vsz));
      W.PPUBANK[6] = VROMPAGE(MS4.PPU[4] % (Map4_Vsz));
      W.PPUBANK[7] = VROMPAGE(MS4.PPU[5] % (Map4_Vsz));
      W.PPUBANK[0] = VROMPAGE((MS4.PPU[0]  ) % (Map4_Vsz));
      W.PPUBANK[1] = VROMPAGE((MS4.PPU[0]+1) % (Map4_Vsz));
      W.PPUBANK[2] = VROMPAGE((MS4.PPU[1]  ) % (Map4_Vsz));
      W.PPUBANK[3] = VROMPAGE((MS4.PPU[1]+1) % (Map4_Vsz));
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

void Map4_Restore()
{ }

