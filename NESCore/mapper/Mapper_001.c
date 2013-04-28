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

/* MMC1 */

struct Map1_State {
    byte  Map1_Regs[4];
    dword Map1_Cnt;
    byte  Map1_Latch;
    word  Map1_Last_Write_Addr;
    byte  Map1_Size;
    dword Map1_256k_base;
    dword Map1_Swap;

    dword Map1_bank1;
    dword Map1_bank2;
    dword Map1_bank3;
    dword Map1_bank4;

    dword Map1_HI1;
    dword Map1_HI2;
};

struct Map1_State MS1;

enum Map1_Size_t
{
  Map1_SMALL,
  Map1_512K,
  Map1_1024K
};

void Map1_Init()
{
  dword size_in_K;
  MapperInit = Map1_Init;
  MapperWrite = Map1_Write;
  MapperSram = Map0_Sram;
  MapperApu = Map0_Apu;
  MapperReadApu = Map0_ReadApu;
  MapperVSync = Map0_VSync;
  MapperHSync = Map0_HSync;
  MapperPPU = Map0_PPU;
  MapperRenderScreen = Map0_RenderScreen;
  MapperRestore = Map1_Restore;

  W.SRAMBANK = S.SRAM;

  MS1.Map1_Cnt = 0;
  MS1.Map1_Latch = 0x00;
  MS1.Map1_Regs[ 0 ] = 0x0c;
  MS1.Map1_Regs[ 1 ] = 0x00;
  MS1.Map1_Regs[ 2 ] = 0x00;
  MS1.Map1_Regs[ 3 ] = 0x00;

  size_in_K = (S.NesHeader.ROMSize << 1) * 8;

  if (size_in_K == 1024)
    MS1.Map1_Size = Map1_1024K;
  else if (size_in_K == 512)
    MS1.Map1_Size = Map1_512K;
  else
    MS1.Map1_Size = Map1_SMALL;

  MS1.Map1_256k_base = 0x00;
  MS1.Map1_Swap = 0;

  if( MS1.Map1_Size == Map1_SMALL )
  {
    /* Set two high pages to last two banks */
    MS1.Map1_HI1 = ( S.NesHeader.ROMSize << 1 ) - 2;
    MS1.Map1_HI2 = ( S.NesHeader.ROMSize << 1 ) - 1;
  }
  else
  {
    /* Set two high pages to last two banks of current 256K Region */
    MS1.Map1_HI1 = ( 256 / 8 ) - 2;
    MS1.Map1_HI2 = ( 256 / 8 ) - 1;
  }

  MS1.Map1_bank1 = 0;
  MS1.Map1_bank2 = 1;
  MS1.Map1_bank3 = MS1.Map1_HI1;
  MS1.Map1_bank4 = MS1.Map1_HI2;

  Map1_set_ROM_banks();
  M.state = &MS1;
  M.size = sizeof(MS1);
}

void Map1_set_ROM_banks()
{
  /* 31 = 256 / 8 - 1 */

  W.ROMBANK0 = ROMPAGE(((MS1.Map1_256k_base << 5) + (MS1.Map1_bank1 & (31))) % ( S.NesHeader.ROMSize << 1 ) );  
  W.ROMBANK1 = ROMPAGE(((MS1.Map1_256k_base << 5) + (MS1.Map1_bank2 & (31))) % ( S.NesHeader.ROMSize << 1 ) );
  W.ROMBANK2 = ROMPAGE(((MS1.Map1_256k_base << 5) + (MS1.Map1_bank3 & (31))) % ( S.NesHeader.ROMSize << 1 ) );
  W.ROMBANK3 = ROMPAGE(((MS1.Map1_256k_base << 5) + (MS1.Map1_bank4 & (31))) % ( S.NesHeader.ROMSize << 1 ) ); 
}

void Map1_Write(word wAddr, byte bData)
{
  dword dwRegNum;

/*
  This doesn't make sense. We don't want to reset if the write address changes,
  we just write to the last address provided.

  if ((wAddr & 0x6000) != (MS1.Map1_Last_Write_Addr & 0x6000))
  {
    MS1.Map1_Cnt = 0;
    MS1.Map1_Latch = 0x00;
  }
  MS1.Map1_Last_Write_Addr = wAddr;
*/

  if ( bData & 0x80 )
  {
    MS1.Map1_Cnt = 0;
    MS1.Map1_Latch = 0x00;
    return;
  }

  if ( bData & 0x01 ) MS1.Map1_Latch |= ( 1 << MS1.Map1_Cnt );
  MS1.Map1_Cnt++;
  if( MS1.Map1_Cnt < 5 ) return;

  dwRegNum = ( wAddr & 0x7FFF ) >> 13;
  MS1.Map1_Regs[ dwRegNum ] = MS1.Map1_Latch;

  MS1.Map1_Cnt = 0;
  MS1.Map1_Latch = 0x00;

  switch( dwRegNum )
  {
    case 0:
      {
        /* Set mirroring */

        if (MS1.Map1_Regs[0] & 0x02)
        {
          if(MS1.Map1_Regs[0] & 0x01)
            NESCore_Mirroring( 0 );
          else
            NESCore_Mirroring( 1 );
        }
        else
        {
          /* One-screen mirroring */

          if (MS1.Map1_Regs[0] & 0x01)
            NESCore_Mirroring( 2 );
          else
            NESCore_Mirroring( 3 );
        }
      }
      break;

    case 1:
      {
        byte bankNum = MS1.Map1_Regs[1];

        if (MS1.Map1_Size == Map1_1024K)
        {
          if (MS1.Map1_Regs[0] & 0x10)
          {
            if (MS1.Map1_Swap)
            {
              MS1.Map1_256k_base = (MS1.Map1_Regs[1] & 0x10) >> 4;
              if (MS1.Map1_Regs[0] & 0x08)
                  MS1.Map1_256k_base |= ((MS1.Map1_Regs[2] & 0x10) >> 3);
              Map1_set_ROM_banks();
              MS1.Map1_Swap = 0;
            }
            else {
              MS1.Map1_Swap = 1;
            }
          }
          else
          {
            /* Use first or fourth 256K Banks */
            MS1.Map1_256k_base = (MS1.Map1_Regs[1] & 0x10) ? 3 : 0;
            Map1_set_ROM_banks();
          }
        }
        else if ((MS1.Map1_Size == Map1_512K) && (!S.NesHeader.VROMSize))
        {
          MS1.Map1_256k_base = (MS1.Map1_Regs[1] & 0x10) >> 4;
          Map1_set_ROM_banks();
        }
        else if (S.NesHeader.VROMSize)
        {
          /* Set W.VROM Bank at $0000 */
          if (MS1.Map1_Regs[0] & 0x10)
          {
            /* Swap 4K */
            bankNum <<= 2;
            W.PPUBANK[ 0 ]= VROMPAGE((bankNum+0) % (S.NesHeader.VROMSize << 3));
            W.PPUBANK[ 1 ]= VROMPAGE((bankNum+1) % (S.NesHeader.VROMSize << 3));
            W.PPUBANK[ 2 ]= VROMPAGE((bankNum+2) % (S.NesHeader.VROMSize << 3));
            W.PPUBANK[ 3 ]= VROMPAGE((bankNum+3) % (S.NesHeader.VROMSize << 3));
            NESCore_Develop_Character_Data();
          }
          else
          {
            /* Swap 8k */
            bankNum <<= 2;
            W.PPUBANK[ 0 ]= VROMPAGE((bankNum+0) % (S.NesHeader.VROMSize << 3));
            W.PPUBANK[ 1 ]= VROMPAGE((bankNum+1) % (S.NesHeader.VROMSize << 3));
            W.PPUBANK[ 2 ]= VROMPAGE((bankNum+2) % (S.NesHeader.VROMSize << 3));
            W.PPUBANK[ 3 ]= VROMPAGE((bankNum+3) % (S.NesHeader.VROMSize << 3));
            W.PPUBANK[ 4 ]= VROMPAGE((bankNum+4) % (S.NesHeader.VROMSize << 3));
            W.PPUBANK[ 5 ]= VROMPAGE((bankNum+5) % (S.NesHeader.VROMSize << 3));
            W.PPUBANK[ 6 ]= VROMPAGE((bankNum+6) % (S.NesHeader.VROMSize << 3));
            W.PPUBANK[ 7 ]= VROMPAGE((bankNum+7) % (S.NesHeader.VROMSize << 3));
            NESCore_Develop_Character_Data();
          }
        }
      }
      break;

    case 2:
      {
        byte bankNum = MS1.Map1_Regs[2];

        if ((MS1.Map1_Size == Map1_1024K) && (MS1.Map1_Regs[0] & 0x08))
        {
          if (MS1.Map1_Swap)
          {
            MS1.Map1_256k_base =  (MS1.Map1_Regs[1] & 0x10) >> 4;
            MS1.Map1_256k_base |= ((MS1.Map1_Regs[2] & 0x10) >> 3);
            Map1_set_ROM_banks();
            MS1.Map1_Swap = 0;
          }
          else
          {
            MS1.Map1_Swap = 1;
          }
        }

        if (!S.NesHeader.VROMSize) 
        {
          if (MS1.Map1_Regs[ 0 ] & 0x10)
          {
            bankNum <<= 2;
            W.PPUBANK[ 4 ] = CRAMPAGE(bankNum  );
            W.PPUBANK[ 5 ] = CRAMPAGE(bankNum+1);
            W.PPUBANK[ 6 ] = CRAMPAGE(bankNum+2);
            W.PPUBANK[ 7 ] = CRAMPAGE(bankNum+3);
            NESCore_Develop_Character_Data();
            break;
          }
        }

        /* Set 4K W.VROM Bank at $1000 */
        if(MS1.Map1_Regs[0] & 0x10)
        {
          /* Swap 4K */
          bankNum <<= 2;
          W.PPUBANK[4] = VROMPAGE((bankNum+0) % (S.NesHeader.VROMSize << 3));
          W.PPUBANK[5] = VROMPAGE((bankNum+1) % (S.NesHeader.VROMSize << 3));
          W.PPUBANK[6] = VROMPAGE((bankNum+2) % (S.NesHeader.VROMSize << 3));
          W.PPUBANK[7] = VROMPAGE((bankNum+3) % (S.NesHeader.VROMSize << 3));
          NESCore_Develop_Character_Data();
        }
      }
      break;

    case 3:
      {
        byte bankNum = MS1.Map1_Regs[3];

        /* Set ROM Bank */
        if (MS1.Map1_Regs[0] & 0x08)
        {
          /* 16K ROM */
          bankNum <<= 1;

          if (MS1.Map1_Regs[0] & 0x04)
          {
            /* 16K ROM at $8000 */
            MS1.Map1_bank1 = bankNum;
            MS1.Map1_bank2 = bankNum+1;
            MS1.Map1_bank3 = MS1.Map1_HI1;
            MS1.Map1_bank4 = MS1.Map1_HI2;
          }
          else
          {
            /* 16K ROM at $C0000 */
            if (MS1.Map1_Size == Map1_SMALL)
            {
              MS1.Map1_bank1 = 0;
              MS1.Map1_bank2 = 1;
              MS1.Map1_bank3 = bankNum;
              MS1.Map1_bank4 = bankNum+1;
            }
          }
        }
        else
        {
          /* 32K ROM at $8000 */
          bankNum <<= 1;

          MS1.Map1_bank1 = bankNum;
          MS1.Map1_bank2 = bankNum+1;
          if (MS1.Map1_Size == Map1_SMALL)
          {
            MS1.Map1_bank3 = bankNum+2;
            MS1.Map1_bank4 = bankNum+3;
          }
        }
        Map1_set_ROM_banks();
      }
      break;
   }
}

void Map1_Restore() { 

  Map1_set_ROM_banks();
}

