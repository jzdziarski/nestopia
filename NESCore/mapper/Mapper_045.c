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

byte  Map45_Regs[7];
dword Map45_P[4],Map45_Prg0,Map45_Prg1,Map45_Prg2,Map45_Prg3;
dword Map45_C[8],Map45_Chr0, Map45_Chr1,Map45_Chr2, Map45_Chr3;
dword Map45_Chr4, Map45_Chr5, Map45_Chr6, Map45_Chr7;

byte Map45_IRQ_Enable;
byte Map45_IRQ_Cnt;
byte Map45_IRQ_Latch;

void Map45_Init()
{
  MapperInit = Map45_Init;
  MapperWrite = Map45_Write;
  MapperSram = Map45_Sram;
  MapperApu = Map0_Apu;
  MapperReadApu = Map0_ReadApu;
  MapperVSync = Map0_VSync;
  MapperHSync = Map45_HSync;
  MapperPPU = Map0_PPU;

  /* Callback at Rendering Screen ( 1:BG, 0:Sprite ) */
  MapperRenderScreen = Map0_RenderScreen;

  /* Set SRAM Banks */
  W.SRAMBANK = S.SRAM;

  /* Set ROM Banks */
  Map45_Prg0 = 0;
  Map45_Prg1 = 1;
  Map45_Prg2 = S.NesHeader.ROMSize * 2 - 2;
  Map45_Prg3 = S.NesHeader.ROMSize * 2 - 1;

  W.ROMBANK0 = ROMPAGE( Map45_Prg0 );  Map45_P[0] = Map45_Prg0;
  W.ROMBANK1 = ROMPAGE( Map45_Prg1 );  Map45_P[1] = Map45_Prg1;
  W.ROMBANK2 = ROMPAGE( Map45_Prg2 );  Map45_P[2] = Map45_Prg2;
  W.ROMBANK3 = ROMPAGE( Map45_Prg3 );  Map45_P[3] = Map45_Prg3;
  
  /* Set PPU Banks */
  Map45_Chr0 = 0;  Map45_C[0] = Map45_Chr0;
  Map45_Chr1 = 1;  Map45_C[1] = Map45_Chr1;
  Map45_Chr2 = 2;  Map45_C[2] = Map45_Chr2;
  Map45_Chr3 = 3;  Map45_C[3] = Map45_Chr3;
  Map45_Chr4 = 4;  Map45_C[4] = Map45_Chr4;
  Map45_Chr5 = 5;  Map45_C[5] = Map45_Chr5;
  Map45_Chr6 = 6;  Map45_C[6] = Map45_Chr6;
  Map45_Chr7 = 7;  Map45_C[7] = Map45_Chr7;

  int nPage ;
  for (nPage = 0; nPage < 8; ++nPage ) {
      W.PPUBANK[ nPage ] = VROMPAGE( nPage );
  }
  NESCore_Develop_Character_Data();
  
  /* Initialize IRQ Registers */
  Map45_IRQ_Enable = 0;
  Map45_IRQ_Cnt = 0;
  Map45_IRQ_Latch = 0;

  Map45_Regs[0] = Map45_Regs[1] = Map45_Regs[2] = Map45_Regs[3] = 0;
  Map45_Regs[4] = Map45_Regs[5] = Map45_Regs[6] = 0;
}

void Map45_Sram( word wAddr, byte byData )
{
	if(wAddr == 0x6000)
	{
		Map45_Regs[Map45_Regs[5]] = byData;
		Map45_Regs[5]= (Map45_Regs[5]+1) & 0x03;
		Map45_Set_CPU_Bank4((byte)Map45_Prg0);
		Map45_Set_CPU_Bank5((byte)Map45_Prg1);
		Map45_Set_CPU_Bank6((byte)Map45_Prg2);
		Map45_Set_CPU_Bank7((byte)Map45_Prg3);
		Map45_Set_PPU_Banks();
  }
}

void Map45_Write( word wAddr, byte byData )
{
  dword swap;

	switch(wAddr & 0xE001) 
  {
    case 0x8000:
			if((byData & 0x40) != (Map45_Regs[6] & 0x40))
			{
				swap = Map45_Prg0; Map45_Prg0 = Map45_Prg2; Map45_Prg2 = swap;
				swap = Map45_P[0]; Map45_P[0] = Map45_P[2]; Map45_P[2] = swap;
        W.ROMBANK0 = ROMPAGE( Map45_P[0] % ( S.NesHeader.ROMSize << 1) );
        W.ROMBANK2 = ROMPAGE( Map45_P[2] % ( S.NesHeader.ROMSize << 1) );
			}
			if (S.NesHeader.ROMSize > 0)
			{
				if((byData & 0x80) != (Map45_Regs[6] & 0x80))
				{
					swap = Map45_Chr4; Map45_Chr4 = Map45_Chr0; Map45_Chr0 = swap;
					swap = Map45_Chr5; Map45_Chr5 = Map45_Chr1; Map45_Chr1 = swap;
					swap = Map45_Chr6; Map45_Chr6 = Map45_Chr2; Map45_Chr2 = swap;
					swap = Map45_Chr7; Map45_Chr7 = Map45_Chr3; Map45_Chr3 = swap;
					swap = Map45_C[4]; Map45_C[4] = Map45_C[0]; Map45_C[0] = swap;
					swap = Map45_C[5]; Map45_C[5] = Map45_C[1]; Map45_C[1] = swap;
					swap = Map45_C[6]; Map45_C[6] = Map45_C[2]; Map45_C[2] = swap;
					swap = Map45_C[7]; Map45_C[7] = Map45_C[3]; Map45_C[3] = swap;
					
          W.PPUBANK[ 0 ] = VROMPAGE( Map45_C[0] % ( S.NesHeader.VROMSize << 3 ) );
          W.PPUBANK[ 1 ] = VROMPAGE( Map45_C[1] % ( S.NesHeader.VROMSize << 3 ) );
          W.PPUBANK[ 2 ] = VROMPAGE( Map45_C[2] % ( S.NesHeader.VROMSize << 3 ) );
          W.PPUBANK[ 3 ] = VROMPAGE( Map45_C[3] % ( S.NesHeader.VROMSize << 3 ) );
          W.PPUBANK[ 4 ] = VROMPAGE( Map45_C[4] % ( S.NesHeader.VROMSize << 3 ) );
          W.PPUBANK[ 5 ] = VROMPAGE( Map45_C[5] % ( S.NesHeader.VROMSize << 3 ) );
          W.PPUBANK[ 6 ] = VROMPAGE( Map45_C[6] % ( S.NesHeader.VROMSize << 3 ) );
          W.PPUBANK[ 7 ] = VROMPAGE( Map45_C[7] % ( S.NesHeader.VROMSize << 3 ) );
          NESCore_Develop_Character_Data();
				}
			}
			Map45_Regs[6] = byData;
		  break;

	case 0x8001:
	  switch(Map45_Regs[6] & 0x07)
		{
			case 0x00:
				Map45_Chr0 = (byData & 0xFE)+0;
				Map45_Chr1 = (byData & 0xFE)+1;
				Map45_Set_PPU_Banks();
				break;

			case 0x01:
			  Map45_Chr2 = (byData & 0xFE)+0;
				Map45_Chr3 = (byData & 0xFE)+1;
				Map45_Set_PPU_Banks();
				break;

			case 0x02:
				Map45_Chr4 = byData;
				Map45_Set_PPU_Banks();
				break;

			case 0x03:
				Map45_Chr5 = byData;
				Map45_Set_PPU_Banks();
				break;

			case 0x04:
				Map45_Chr6 = byData;
				Map45_Set_PPU_Banks();
				break;

			case 0x05:
				Map45_Chr7 = byData;
				Map45_Set_PPU_Banks();
				break;

      case 0x06:
				if(Map45_Regs[6] & 0x40)
				{
				  Map45_Prg2 = byData & 0x3F;
					Map45_Set_CPU_Bank6(byData);
				}
				else
				{
					Map45_Prg0 = byData & 0x3F;
					Map45_Set_CPU_Bank4(byData);
				}
				break;

			case 0x07:
				Map45_Prg1 = byData & 0x3F;
				Map45_Set_CPU_Bank5(byData);
				break;
		}
		break;

	case 0xA000:
     if ( byData & 0x01 )
     {
       NESCore_Mirroring( 0 );
     } else {
       NESCore_Mirroring( 1 );
     }
		break;

	case 0xC000:
	  Map45_IRQ_Cnt = byData;
		break;

	case 0xC001:
		Map45_IRQ_Latch = byData;
		break;

	case 0xE000:
		Map45_IRQ_Enable = 0;
		break;

	case 0xE001:
		Map45_IRQ_Enable = 1;
		break;
	}
}

void Map45_HSync()
{
  if(Map45_IRQ_Enable)
  {
    if ( /* 0 <= S.PPU_Scanline && */ S.PPU_Scanline <= 239 )
    {
      if ( S.PPU_R1 & R1_SHOW_BG || S.PPU_R1 & R1_SHOW_SP )
			{
				if(!(Map45_IRQ_Cnt--))
				{
					Map45_IRQ_Cnt = Map45_IRQ_Latch;
          IRQ_REQ;
				}
			}
		}
	}
}

void Map45_Set_CPU_Bank4(byte byData)
{
	byData &= (Map45_Regs[3] & 0x3F) ^ 0xFF;
	byData &= 0x3F;
	byData |= Map45_Regs[1];
  W.ROMBANK0 = ROMPAGE( byData % ( S.NesHeader.ROMSize << 1) );
	Map45_P[0] = byData;
}

void Map45_Set_CPU_Bank5(byte byData)
{
	byData &= (Map45_Regs[3] & 0x3F) ^ 0xFF;
	byData &= 0x3F;
	byData |= Map45_Regs[1];
  W.ROMBANK1 = ROMPAGE( byData % ( S.NesHeader.ROMSize << 1) );
	Map45_P[1] = byData;
}

void Map45_Set_CPU_Bank6(byte byData)
{
	byData &= (Map45_Regs[3] & 0x3F) ^ 0xFF;
	byData &= 0x3F;
	byData |= Map45_Regs[1];
  W.ROMBANK2 = ROMPAGE( byData % ( S.NesHeader.ROMSize << 1) );
	Map45_P[2] = byData;
}

void Map45_Set_CPU_Bank7(byte byData)
{
	byData &= (Map45_Regs[3] & 0x3F) ^ 0xFF;
	byData &= 0x3F;
	byData |= Map45_Regs[1];
  W.ROMBANK3 = ROMPAGE( byData % ( S.NesHeader.ROMSize << 1) );
	Map45_P[3] = byData;
}

void Map45_Set_PPU_Banks()
{
	byte table[16] =
	    {
	        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	        0x01,0x03,0x07,0x0F,0x1F,0x3F,0x7F,0xFF
	    };
	Map45_C[0] = Map45_Chr0;
	Map45_C[1] = Map45_Chr1;
	Map45_C[2] = Map45_Chr2;
	Map45_C[3] = Map45_Chr3;
	Map45_C[4] = Map45_Chr4;
	Map45_C[5] = Map45_Chr5;
	Map45_C[6] = Map45_Chr6;
	Map45_C[7] = Map45_Chr7;
        byte i;
	for(i = 0; i < 8; i++)
	{
		Map45_C[i] &= table[Map45_Regs[2] & 0x0F];
		Map45_C[i] |= Map45_Regs[0] & 0xff;
		Map45_C[i] += (byte)(Map45_Regs[2] & 0x10) << 4;
	}
	if(Map45_Regs[6] & 0x80)
	{
    W.PPUBANK[ 0 ] = VROMPAGE( Map45_C[4] % ( S.NesHeader.VROMSize << 3 ) );
    W.PPUBANK[ 1 ] = VROMPAGE( Map45_C[5] % ( S.NesHeader.VROMSize << 3 ) );
    W.PPUBANK[ 2 ] = VROMPAGE( Map45_C[6] % ( S.NesHeader.VROMSize << 3 ) );
    W.PPUBANK[ 3 ] = VROMPAGE( Map45_C[7] % ( S.NesHeader.VROMSize << 3 ) );
    W.PPUBANK[ 4 ] = VROMPAGE( Map45_C[0] % ( S.NesHeader.VROMSize << 3 ) );
    W.PPUBANK[ 5 ] = VROMPAGE( Map45_C[1] % ( S.NesHeader.VROMSize << 3 ) );
    W.PPUBANK[ 6 ] = VROMPAGE( Map45_C[2] % ( S.NesHeader.VROMSize << 3 ) );
    W.PPUBANK[ 7 ] = VROMPAGE( Map45_C[3] % ( S.NesHeader.VROMSize << 3 ) );
    NESCore_Develop_Character_Data();
	}
	else
	{
    W.PPUBANK[ 0 ] = VROMPAGE( Map45_C[0] % ( S.NesHeader.VROMSize << 3 ) );
    W.PPUBANK[ 1 ] = VROMPAGE( Map45_C[1] % ( S.NesHeader.VROMSize << 3 ) );
    W.PPUBANK[ 2 ] = VROMPAGE( Map45_C[2] % ( S.NesHeader.VROMSize << 3 ) );
    W.PPUBANK[ 3 ] = VROMPAGE( Map45_C[3] % ( S.NesHeader.VROMSize << 3 ) );
    W.PPUBANK[ 4 ] = VROMPAGE( Map45_C[4] % ( S.NesHeader.VROMSize << 3 ) );
    W.PPUBANK[ 5 ] = VROMPAGE( Map45_C[5] % ( S.NesHeader.VROMSize << 3 ) );
    W.PPUBANK[ 6 ] = VROMPAGE( Map45_C[6] % ( S.NesHeader.VROMSize << 3 ) );
    W.PPUBANK[ 7 ] = VROMPAGE( Map45_C[7] % ( S.NesHeader.VROMSize << 3 ) );
    NESCore_Develop_Character_Data();
	}
}


