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

byte  Map69_IRQ_Enable;
dword Map69_IRQ_Cnt;
byte  Map69_Regs[ 1 ];

void Map69_Init()
{
  MapperInit = Map69_Init;
  MapperWrite = Map69_Write;
  MapperSram = Map0_Sram;
  MapperApu = Map0_Apu;
  MapperReadApu = Map0_ReadApu;
  MapperVSync = Map0_VSync;
  MapperHSync = Map69_HSync;
  MapperPPU = Map0_PPU;

  /* Callback at Rendering Screen ( 1:BG, 0:Sprite ) */
  MapperRenderScreen = Map0_RenderScreen;

  W.SRAMBANK = S.SRAM;
  W.ROMBANK0 = ROMPAGE( 0 );
  W.ROMBANK1 = ROMPAGE( 1 );
  W.ROMBANK2 = ROMLASTPAGE( 1 );
  W.ROMBANK3 = ROMLASTPAGE( 0 );

  /* Set PPU Banks */
  if ( S.NesHeader.VROMSize > 0 )
  {
    int nPage ;
    for (nPage = 0; nPage < 8; ++nPage )
        W.PPUBANK[ nPage ] = VROMPAGE( nPage );
    NESCore_Develop_Character_Data();
  }

  /* Initialize IRQ Reg */
  Map69_IRQ_Enable = 0;
  Map69_IRQ_Cnt    = 0;

}

void Map69_Write( word wAddr, byte byData )
{
  switch ( wAddr )
  {
    case 0x8000:
      Map69_Regs[ 0 ] = byData & 0x0f;
      break;

    case 0xA000:
      switch ( Map69_Regs[ 0 ] )
      {
        /* Set PPU Banks */
        case 0x00:
        case 0x01:
        case 0x02:
        case 0x03:
        case 0x04:
        case 0x05:
        case 0x06:
        case 0x07:
          byData %= ( S.NesHeader.VROMSize << 3 );
          W.PPUBANK[ Map69_Regs[ 0 ] ] = VROMPAGE( byData );
          NESCore_Develop_Character_Data();
          break;

        /* Set ROM Banks */
#if 0
        case 0x08:
          if ( !( byData & 0x40 ) )
          {
            byData %= ( S.NesHeader.ROMSize << 1 );
            W.SRAMBANK = ROMPAGE( byData );
          }
          break;
#endif

        case 0x09:
          byData %= ( S.NesHeader.ROMSize << 1 );
          W.ROMBANK0 = ROMPAGE( byData );
          break;

        case 0x0a:
          byData %= ( S.NesHeader.ROMSize << 1 );
          W.ROMBANK1 = ROMPAGE( byData );
          break;

        case 0x0b:
          byData %= ( S.NesHeader.ROMSize << 1 );
          W.ROMBANK2 = ROMPAGE( byData );
          break;

        /* Name Table Mirroring */
        case 0x0c:  
          switch ( byData & 0x03 )
          {
            case 0:
              NESCore_Mirroring( 1 );   /* Vertical */
              break;
            case 1:
              NESCore_Mirroring( 0 );   /* Horizontal */
              break;
            case 2:
              NESCore_Mirroring( 3 );   /* One Screen 0x2400 */
              break;
            case 3:
              NESCore_Mirroring( 2 );   /* One Screen 0x2000 */
              break;
          }
          break;

        case 0x0d:
          Map69_IRQ_Enable = byData;
          break;

        case 0x0e:
          Map69_IRQ_Cnt = ( Map69_IRQ_Cnt & 0xff00) | (dword)byData;
          break;

        case 0x0f:
          Map69_IRQ_Cnt = ( Map69_IRQ_Cnt & 0x00ff) | ( (dword)byData << 8 );
          break;
      }
      break;
  }
}

void Map69_HSync()
{
  if ( Map69_IRQ_Enable )
  {
    if ( Map69_IRQ_Cnt <= 113 )
    {
      IRQ_REQ;
      Map69_IRQ_Cnt = 0;
    } else {
      Map69_IRQ_Cnt -= 113;
    }
  }
}
