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

byte  Map73_IRQ_Enable;
dword Map73_IRQ_Cnt;

void Map73_Init()
{
  MapperInit = Map73_Init;
  MapperWrite = Map73_Write;
  MapperSram = Map0_Sram;
  MapperApu = Map0_Apu;
  MapperReadApu = Map0_ReadApu;
  MapperVSync = Map0_VSync;
  MapperHSync = Map73_HSync;
  MapperPPU = Map0_PPU;
  MapperRenderScreen = Map0_RenderScreen;

  /* Set SRAM Banks */
  W.SRAMBANK = S.SRAM;

  /* Set ROM Banks */
  W.ROMBANK0 = ROMPAGE( 0 );
  W.ROMBANK1 = ROMPAGE( 1 );
  W.ROMBANK2 = ROMLASTPAGE( 1 );
  W.ROMBANK3 = ROMLASTPAGE( 0 );

  /* Initialize IRQ Registers */
  Map73_IRQ_Enable = 0;
  Map73_IRQ_Cnt = 0;  
}

void Map73_Write( word wAddr, byte byData )
{
  switch ( wAddr )
  {
    case 0x8000:
      Map73_IRQ_Cnt = ( Map73_IRQ_Cnt & 0xfff0 ) | ( byData & 0x0f );
      break;

    case 0x9000:
      Map73_IRQ_Cnt = ( Map73_IRQ_Cnt & 0xff0f ) | ( ( byData & 0x0f ) << 4 );
      break;

    case 0xa000:
      Map73_IRQ_Cnt = ( Map73_IRQ_Cnt & 0xf0ff ) | ( ( byData & 0x0f ) << 8 );
      break;

    case 0xb000:
      Map73_IRQ_Cnt = ( Map73_IRQ_Cnt & 0x0fff ) | ( ( byData & 0x0f ) << 12 );
      break;

    case 0xc000:
      Map73_IRQ_Enable = byData;
      break;

    /* Set ROM Banks */
    case 0xf000:
      byData <<= 1;
      byData %= ( S.NesHeader.ROMSize << 1 );
      W.ROMBANK0 = ROMPAGE( byData );
      W.ROMBANK1 = ROMPAGE( byData + 1 );
      break;
  }
}

void Map73_HSync()
{
  if ( Map73_IRQ_Enable & 0x02 )
  {
    if ( ( Map73_IRQ_Cnt += (S.PPU_Scanline % 2) ? 113 : 114 ) > 0xffff )
    {
      Map73_IRQ_Cnt &= 0xffff;
      IRQ_REQ;
      Map73_IRQ_Enable = 0;
    }
  }
}
