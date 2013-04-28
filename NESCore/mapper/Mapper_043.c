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

dword Map43_IRQ_Cnt;
byte Map43_IRQ_Enable;

void Map43_Init()
{
  MapperInit = Map43_Init;
  MapperWrite = Map43_Write;
  MapperSram = Map0_Sram;
  MapperApu = Map43_Apu;
  MapperReadApu = Map43_ReadApu;
  MapperVSync = Map0_VSync;
  MapperHSync = Map43_HSync;
  MapperPPU = Map0_PPU;

  /* Callback at Rendering Screen ( 1:BG, 0:Sprite ) */
  MapperRenderScreen = Map0_RenderScreen;

  W.SRAMBANK = ROMPAGE( 2 );
  W.ROMBANK0 = ROMPAGE( 1 );
  W.ROMBANK1 = ROMPAGE( 0 );
  W.ROMBANK2 = ROMPAGE( 4 );
  W.ROMBANK3 = ROMPAGE( 9 );

  /* Initialize State Registers */
  Map43_IRQ_Enable = 1;
  Map43_IRQ_Cnt = 0;

  if ( S.NesHeader.VROMSize > 0 )
  {
    int nPage ;
    for (nPage = 0; nPage < 8; ++nPage )
        W.PPUBANK[ nPage ] = VROMPAGE( nPage );
    NESCore_Develop_Character_Data();
  }

}

byte Map43_ReadApu( word wAddr )
{
  if (0x5000 <= wAddr && wAddr < 0x6000) 
      return W.ROM[ 0x2000*8 + 0x1000 + (wAddr - 0x5000) ];
  return (byte)(wAddr >> 8);
}

void Map43_Apu( word wAddr, byte byData )
{
	if( (wAddr&0xF0FF) == 0x4022 ) 
  {
		switch( byData&0x07 ) 
    {
			case	0x00:
			case	0x02:
			case	0x03:
			case	0x04:
        W.ROMBANK2 = ROMPAGE( 4 );
				break;
			case	0x01:
        W.ROMBANK2 = ROMPAGE( 3 );
				break;
			case	0x05:
        W.ROMBANK2 = ROMPAGE( 7 );
				break;
			case	0x06:
        W.ROMBANK2 = ROMPAGE( 5 );
				break;
			case	0x07:
        W.ROMBANK2 = ROMPAGE( 6 );
				break;
		}
	}
}

void Map43_Write( word wAddr, byte byData )
{
	if( wAddr == 0x8122 ) {
		if( byData&0x03 ) {
			Map43_IRQ_Enable = 1;
		} else {
			Map43_IRQ_Cnt = 0;
			Map43_IRQ_Enable = 0;
		}
	}
}

void Map43_HSync()
{
	if( Map43_IRQ_Enable ) 
  {
		Map43_IRQ_Cnt += 114;
		if( Map43_IRQ_Cnt >= 4096 ) {
			Map43_IRQ_Cnt -= 4096;
			IRQ_REQ;
		}
	}
}
