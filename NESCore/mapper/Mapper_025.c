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

byte Map25_Bank_Selector;
byte Map25_VBank[16];

byte Map25_IRQ_Count;
byte Map25_IRQ_State;
byte Map25_IRQ_Latch;

void Map25_Init()
{
  int nPage;

  MapperInit = Map25_Init;
  MapperWrite = Map25_Write;
  MapperSram = Map0_Sram;
  MapperApu = Map0_Apu;
  MapperReadApu = Map0_ReadApu;
  MapperVSync = Map0_VSync;
  MapperHSync = Map25_HSync;
  MapperPPU = Map0_PPU;
  MapperRenderScreen = Map0_RenderScreen;

  W.SRAMBANK = S.SRAM;
  W.ROMBANK0 = ROMPAGE( 0 );
  W.ROMBANK1 = ROMPAGE( 1 );
  W.ROMBANK2 = ROMLASTPAGE( 1 );
  W.ROMBANK3 = ROMLASTPAGE( 0 );

  for (nPage = 0; nPage < 16; nPage++)
      Map25_VBank[ nPage ] = 0x00;
}

void Map25_Write( word wAddr, byte byData )
{
  int nBank;

  switch ( wAddr )
  {
    case 0x8000:
      /* Set ROM Banks */
      byData %= ( S.NesHeader.ROMSize << 1 );
      if ( Map25_Bank_Selector ) 
      {
        W.ROMBANK2 = ROMPAGE( byData );
      } else {
        W.ROMBANK0 = ROMPAGE( byData );
      }
      break;

    case 0x9000:
      /* Name Table Mirroring */
      switch (byData & 0x03)
      {
        case 0:
          NESCore_Mirroring( 1 );   /* Vertical */
          break;
        case 1:
          NESCore_Mirroring( 0 );   /* Horizontal */
          break;
        case 2:
          NESCore_Mirroring( 2 );   /* One Screen 0x2000 */
          break;
        case 3:
          NESCore_Mirroring( 3 );   /* One Screen 0x2400 */
          break;
      }
      break;

    case 0x9002:
      /* TODO: SaveRAM Toggle */
      /* $8000 Switching Mode */
      Map25_Bank_Selector = byData & 0x02;
      break;

    case 0xa000:
      /* Set ROM Banks */
      byData %= ( S.NesHeader.ROMSize << 1 );
      W.ROMBANK1 = ROMPAGE( byData );
      break;
    
    default:
      /* Set PPU Banks */
      switch ( wAddr & 0xfffc)
      {
        case 0xb000:
          Map25_VBank[ 0 + ( wAddr & 0x0003 ) ] = byData & 0x0f;
          nBank = 0 + ( wAddr & 0x0001 );
          Map25_Sync_Vrom( nBank );
          break;
  
        case 0xc000:
          Map25_VBank[ 4 + ( wAddr & 0x0003 ) ] = byData & 0x0f;
          nBank = 2 + ( wAddr & 0x0001 );
          Map25_Sync_Vrom( nBank );
          break;

        case 0xd000:
          Map25_VBank[ 8 + ( wAddr & 0x0003 ) ] = byData & 0x0f;
          nBank = 4 + ( wAddr & 0x0001 );
          Map25_Sync_Vrom( nBank );
          break;

        case 0xe000:
          Map25_VBank[ 12 + ( wAddr & 0x0003 ) ] = byData & 0x0f;
          nBank = 6 + ( wAddr & 0x0001 );
          Map25_Sync_Vrom( nBank );
          break;

        case 0xf000:
          switch ( wAddr & 0x0003 )
          {
            case 0:
            case 1:
              Map25_IRQ_Latch = byData;
              break;
            case 2:
              Map25_IRQ_State = ( byData & 0x01 ) ? Map25_IRQ_State : 0x00;
              Map25_IRQ_State = ( byData & 0x02 ) ? 0x01 : Map25_IRQ_State;
              Map25_IRQ_Count = Map25_IRQ_Latch;
              break;
            case 3:
              Map25_IRQ_State = ( Map25_IRQ_State << 1 ) | ( Map25_IRQ_State & 1 );
              break;
          }
          break;
      }
      break;
  }
}

void Map25_Sync_Vrom( int nBank )
{
  byte byValue;
  
  byValue  = Map25_VBank[ ( nBank << 1 ) - ( nBank & 0x01 ) ];
  byValue |= Map25_VBank[ ( nBank << 1 ) - ( nBank & 0x01 ) + 2] << 4;
  byValue %= ( S.NesHeader.VROMSize << 3 );
  W.PPUBANK[ nBank ] = VROMPAGE( byValue );
  NESCore_Develop_Character_Data();
}

void Map25_HSync()
{
  if ( ( Map25_IRQ_State & 0x02 ) && ( ++Map25_IRQ_Count == 0 ) )
  {
    IRQ_REQ;
    Map25_IRQ_Count = Map25_IRQ_Latch;
  }
}
