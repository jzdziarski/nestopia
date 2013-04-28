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

/* MMC4 */

struct Map10_Latch 
{
  byte lo_bank;
  byte hi_bank;
  byte state;
};

struct Map10_Latch latch3; /* Latch Selector #1 */
struct Map10_Latch latch4; /* Latch Selector $2 */ 

void Map10_Init()
{
  int nPage;

  MapperInit = Map10_Init;
  MapperWrite = Map10_Write;
  MapperSram = Map0_Sram;
  MapperApu = Map0_Apu;
  MapperReadApu = Map0_ReadApu;
  MapperVSync = Map0_VSync;
  MapperHSync = Map0_HSync;
  MapperPPU = Map10_PPU;
  MapperRenderScreen = Map0_RenderScreen;

  /* Set SRAM Banks */
  W.SRAMBANK = S.SRAM;
  W.ROMBANK0 = ROMPAGE( 0 );
  W.ROMBANK1 = ROMPAGE( 1 );
  W.ROMBANK2 = ROMLASTPAGE( 1 );
  W.ROMBANK3 = ROMLASTPAGE( 0 );

  /* Set PPU Banks */
  if ( S.NesHeader.VROMSize > 0 )
  {
    for ( nPage = 0; nPage < 8; ++nPage )
        W.PPUBANK[ nPage ] = VROMPAGE( nPage );
    NESCore_Develop_Character_Data();
  }

  /* Init Latch Selector */
  latch3.state = 0xfe;
  latch3.lo_bank = 0;
  latch3.hi_bank = 0;
  latch4.state = 0xfe;
  latch4.lo_bank = 0;
  latch4.hi_bank = 0;
}

void Map10_Write( word wAddr, byte byData )
{
  word wMapAddr;

  wMapAddr = wAddr & 0xf000;
  switch ( wMapAddr )
  {
    case 0xa000:
      /* Set ROM Banks */
      byData %= S.NesHeader.ROMSize;
      byData <<= 1;
      W.ROMBANK0 = ROMPAGE( byData );
      W.ROMBANK1 = ROMPAGE( byData + 1 );
      break;

    case 0xb000:
      /* Number of 4K Banks to Number of 1K Banks */
      byData %= ( S.NesHeader.VROMSize << 1 );
      byData <<= 2;

      /* Latch Control */
      latch3.lo_bank = byData;

      if (0xfd == latch3.state)
      {
        /* Set PPU Banks */
        W.PPUBANK[ 0 ] = VROMPAGE( byData );
        W.PPUBANK[ 1 ] = VROMPAGE( byData + 1 );
        W.PPUBANK[ 2 ] = VROMPAGE( byData + 2 );
        W.PPUBANK[ 3 ] = VROMPAGE( byData + 3 );     
        NESCore_Develop_Character_Data();
      }
      break;

    case 0xc000:
      /* Number of 4K Banks to Number of 1K Banks */
      byData %= ( S.NesHeader.VROMSize << 1 );
      byData <<= 2;

      /* Latch Control */
      latch3.hi_bank = byData;

      if (0xfe == latch3.state)
      {
        /* Set PPU Banks */
        W.PPUBANK[ 0 ] = VROMPAGE( byData );
        W.PPUBANK[ 1 ] = VROMPAGE( byData + 1 );
        W.PPUBANK[ 2 ] = VROMPAGE( byData + 2 );
        W.PPUBANK[ 3 ] = VROMPAGE( byData + 3 );     
        NESCore_Develop_Character_Data();
      }
      break;

    case 0xd000:
      /* Number of 4K Banks to Number of 1K Banks */
      byData %= ( S.NesHeader.VROMSize << 1 );
      byData <<= 2;

      /* Latch Control */
      latch4.lo_bank = byData;

      if (0xfd == latch4.state)
      {
        /* Set PPU Banks */
        W.PPUBANK[ 4 ] = VROMPAGE( byData );
        W.PPUBANK[ 5 ] = VROMPAGE( byData + 1 );
        W.PPUBANK[ 6 ] = VROMPAGE( byData + 2 );
        W.PPUBANK[ 7 ] = VROMPAGE( byData + 3 );    
        NESCore_Develop_Character_Data();
      }
      break;

    case 0xe000:
      /* Number of 4K Banks to Number of 1K Banks */
      byData %= ( S.NesHeader.VROMSize << 1 );
      byData <<= 2;

      /* Latch Control */
      latch4.hi_bank = byData;

      if (0xfe == latch4.state)
      {
        /* Set PPU Banks */
        W.PPUBANK[ 4 ] = VROMPAGE( byData );
        W.PPUBANK[ 5 ] = VROMPAGE( byData + 1 );
        W.PPUBANK[ 6 ] = VROMPAGE( byData + 2 );
        W.PPUBANK[ 7 ] = VROMPAGE( byData + 3 ); 
        NESCore_Develop_Character_Data();
      }
      break;

    case 0xf000:
      /* Name Table Mirroring */
      NESCore_Mirroring( byData & 0x01 ? 0 : 1);
      break;
  }  
}

void Map10_PPU( word wAddr )
{
  /* Control Latch Selector */ 
  switch ( wAddr & 0x3ff0 )
  {
    case 0x0fd0:
      /* Latch Control */
      latch3.state = 0xfd;
      /* Set PPU Banks */
      W.PPUBANK[ 0 ] = VROMPAGE( latch3.lo_bank );
      W.PPUBANK[ 1 ] = VROMPAGE( latch3.lo_bank + 1 );
      W.PPUBANK[ 2 ] = VROMPAGE( latch3.lo_bank + 2 );
      W.PPUBANK[ 3 ] = VROMPAGE( latch3.lo_bank + 3 );     
      NESCore_Develop_Character_Data();
      break;

    case 0x0fe0:
      /* Latch Control */
      latch3.state = 0xfe;
      /* Set PPU Banks */
      W.PPUBANK[ 0 ] = VROMPAGE( latch3.hi_bank );
      W.PPUBANK[ 1 ] = VROMPAGE( latch3.hi_bank + 1 );
      W.PPUBANK[ 2 ] = VROMPAGE( latch3.hi_bank + 2 );
      W.PPUBANK[ 3 ] = VROMPAGE( latch3.hi_bank + 3 );     
      NESCore_Develop_Character_Data();      
      break;

    case 0x1fd0:
      /* Latch Control */
      latch4.state = 0xfd;
      /* Set PPU Banks */
      W.PPUBANK[ 4 ] = VROMPAGE( latch4.lo_bank );
      W.PPUBANK[ 5 ] = VROMPAGE( latch4.lo_bank + 1 );
      W.PPUBANK[ 6 ] = VROMPAGE( latch4.lo_bank + 2 );
      W.PPUBANK[ 7 ] = VROMPAGE( latch4.lo_bank + 3 );     
      NESCore_Develop_Character_Data();
      break;      

    case 0x1fe0:
      /* Latch Control */
      latch4.state = 0xfe;
      /* Set PPU Banks */
      W.PPUBANK[ 4 ] = VROMPAGE( latch4.hi_bank );
      W.PPUBANK[ 5 ] = VROMPAGE( latch4.hi_bank + 1 );
      W.PPUBANK[ 6 ] = VROMPAGE( latch4.hi_bank + 2 );
      W.PPUBANK[ 7 ] = VROMPAGE( latch4.hi_bank + 3 );     
      NESCore_Develop_Character_Data();            
      break;
  }
}
