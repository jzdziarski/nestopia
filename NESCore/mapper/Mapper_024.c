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

byte Map24_IRQ_Count;
byte Map24_IRQ_State;
byte Map24_IRQ_Latch;

void Map24_Init()
{
  MapperInit = Map24_Init;
  MapperWrite = Map24_Write;
  MapperSram = Map0_Sram;
  MapperApu = Map0_Apu;
  MapperReadApu = Map0_ReadApu;
  MapperVSync = Map0_VSync;
  MapperHSync = Map24_HSync;
  MapperPPU = Map0_PPU;

  /* Callback at Rendering Screen ( 1:BG, 0:Sprite ) */
  MapperRenderScreen = Map0_RenderScreen;

  W.SRAMBANK = S.SRAM;
  W.ROMBANK0 = ROMPAGE( 0 );
  W.ROMBANK1 = ROMPAGE( 1 );
  W.ROMBANK2 = ROMLASTPAGE( 1 );
  W.ROMBANK3 = ROMLASTPAGE( 0 );
}

void Map24_Write( word wAddr, byte byData )
{
  switch ( wAddr )
  {
    case 0x8000:
      /* Set ROM Banks */
      W.ROMBANK0 = ROMPAGE( ( byData + 0 ) % ( S.NesHeader.ROMSize << 1) );
      W.ROMBANK1 = ROMPAGE( ( byData + 1 ) % ( S.NesHeader.ROMSize << 1) );
      break;

    case 0xb003:
      /* Name Table Mirroring */
      switch ( byData & 0x0c )
      {
        case 0x00:
          NESCore_Mirroring( 1 );   /* Vertical */
          break;
        case 0x04:
          NESCore_Mirroring( 0 );   /* Horizontal */
          break;
        case 0x08:
          NESCore_Mirroring( 3 );   /* One Screen 0x2000 */
          break;
        case 0x0c:
          NESCore_Mirroring( 2 );   /* One Screen 0x2400 */
          break;
      }
      break;

	  case 0xC000:
      W.ROMBANK2 = ROMPAGE( byData % ( S.NesHeader.ROMSize << 1) );
		  break;

	  case 0xD000:
      W.PPUBANK[ 0 ] = VROMPAGE( byData % ( S.NesHeader.VROMSize << 3 ) );
      NESCore_Develop_Character_Data();
      break;

	  case 0xD001:
      W.PPUBANK[ 1 ] = VROMPAGE( byData % ( S.NesHeader.VROMSize << 3 ) );
      NESCore_Develop_Character_Data();
      break;

	  case 0xD002:
      W.PPUBANK[ 2 ] = VROMPAGE( byData % ( S.NesHeader.VROMSize << 3 ) );
      NESCore_Develop_Character_Data();
      break;

	  case 0xD003:
      W.PPUBANK[ 3 ] = VROMPAGE( byData % ( S.NesHeader.VROMSize << 3 ) );
      NESCore_Develop_Character_Data();
      break;

	  case 0xE000:
      W.PPUBANK[ 4 ] = VROMPAGE( byData % ( S.NesHeader.VROMSize << 3 ) );
      NESCore_Develop_Character_Data();
      break;

	  case 0xE001:
      W.PPUBANK[ 5 ] = VROMPAGE( byData % ( S.NesHeader.VROMSize << 3 ) );
      NESCore_Develop_Character_Data();
      break;

	  case 0xE002:
      W.PPUBANK[ 6 ] = VROMPAGE( byData % ( S.NesHeader.VROMSize << 3 ) );
      NESCore_Develop_Character_Data();
      break;

	  case 0xE003:
      W.PPUBANK[ 7 ] = VROMPAGE( byData % ( S.NesHeader.VROMSize << 3 ) );
      NESCore_Develop_Character_Data();
      break;

	  case 0xF000:
			Map24_IRQ_Latch = byData;
	  	break;

	  case 0xF001:
			Map24_IRQ_State = byData & 0x03;
			if(Map24_IRQ_State & 0x02)
			{
				Map24_IRQ_Count = Map24_IRQ_Latch;
			}
		  break;

	  case 0xF002:
			if(Map24_IRQ_State & 0x01)
			{
				Map24_IRQ_State |= 0x02;
			}
			else
			{
				Map24_IRQ_State &= 0x01;
			}
		break;
  }
}

void Map24_HSync()
{
	if(Map24_IRQ_State & 0x02)
	{
	  if(Map24_IRQ_Count == 0xFF)
		{
			IRQ_REQ;
			Map24_IRQ_Count = Map24_IRQ_Latch;
		}
		else
		{
			Map24_IRQ_Count++;
		}
	}
}
