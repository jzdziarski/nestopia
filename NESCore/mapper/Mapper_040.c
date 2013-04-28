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

byte  Map40_IRQ_Enable;
dword Map40_Line_To_IRQ;

void Map40_Init()
{
  MapperInit = Map40_Init;
  MapperWrite = Map40_Write;
  MapperSram = Map0_Sram;
  MapperApu = Map0_Apu;
  MapperReadApu = Map0_ReadApu;
  MapperVSync = Map0_VSync;
  MapperHSync = Map40_HSync;
  MapperPPU = Map0_PPU;

  /* Callback at Rendering Screen ( 1:BG, 0:Sprite ) */
  MapperRenderScreen = Map0_RenderScreen;

  W.SRAMBANK = ROMPAGE( 6 );

  /* Initialize IRQ Registers */
  Map40_IRQ_Enable = 0;
  Map40_Line_To_IRQ = 0;

  W.ROMBANK0 = ROMPAGE( 4 );
  W.ROMBANK1 = ROMPAGE( 5 );
  W.ROMBANK2 = ROMPAGE( 0 );
  W.ROMBANK3 = ROMPAGE( 7 );

  if ( S.NesHeader.VROMSize > 0 )
  {
    int nPage ;
    for (nPage = 0; nPage < 8; ++nPage )
        W.PPUBANK[ nPage ] = VROMPAGE( nPage );
    NESCore_Develop_Character_Data();
  }
}

void Map40_Write( word wAddr, byte byData )
{
  switch ( wAddr & 0xe000 )
  {
    case 0x8000:
      Map40_IRQ_Enable = 0;
      break;

    case 0xa000:
      Map40_IRQ_Enable = 1;
      Map40_Line_To_IRQ = 37;
      break;

    case 0xc000:
      break;

    case 0xe000:
      /* Set ROM Banks */
      W.ROMBANK2 = ROMPAGE ( ( byData & 0x07 ) % ( S.NesHeader.ROMSize << 1 ) );
      break;
  }
}

void Map40_HSync()
{
  if ( Map40_IRQ_Enable )
  {
    if ( ( --Map40_Line_To_IRQ ) <= 0 )
    {
      IRQ_REQ;
    }
  }
}

