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

/* MMC2 */

struct Map9_Latch 
{
  byte Lo;
  byte Hi;
  byte State;
};

struct Map9_State {
    struct Map9_Latch Latch1;
    struct Map9_Latch Latch2;
};

struct Map9_State MS9;

void Map9_Init()
{
  int nPage;

  MapperInit = Map9_Init;
  MapperWrite = Map9_Write;
  MapperSram = Map0_Sram;
  MapperApu = Map0_Apu;
  MapperReadApu = Map0_ReadApu;
  MapperVSync = Map0_VSync;
  MapperHSync = Map0_HSync;
  MapperPPU = Map9_PPU;
  MapperRenderScreen = Map0_RenderScreen;
  MapperRestore = Map9_Restore;

  W.SRAMBANK = S.SRAM;
  W.ROMBANK0 = ROMPAGE( 0 );
  W.ROMBANK1 = ROMLASTPAGE( 2 );
  W.ROMBANK2 = ROMLASTPAGE( 1 );
  W.ROMBANK3 = ROMLASTPAGE( 0 );

  if ( S.NesHeader.VROMSize > 0 )
  {
    for ( nPage = 0; nPage < 8; ++nPage )
        W.PPUBANK[ nPage ] = VROMPAGE( nPage );
    NESCore_Develop_Character_Data();
  }

  MS9.Latch1.State = 0xFE;
  MS9.Latch1.Lo = 0;
  MS9.Latch1.Hi = 0;

  MS9.Latch2.State = 0xFE;
  MS9.Latch2.Lo = 0;
  MS9.Latch2.Hi = 0;

  M.state = &MS9;
  M.size = sizeof(MS9);
}

void Map9_Write( word wAddr, byte bData )
{
    word wMapAddr;

    wMapAddr = wAddr & 0xf000;
    switch (wMapAddr)
    {
      case 0xA000:
        bData %= ( S.NesHeader.ROMSize << 1 );
        W.ROMBANK0 = ROMPAGE( bData );
        break;

      case 0xB000:
        /* Number of 4K Banks to Number of 1K Banks */
        bData %= ( S.NesHeader.VROMSize << 1 );
        bData <<= 2;

        /* Latch Control */
        if (MS9.Latch1.State == 0xFD)
        {
            W.PPUBANK[ 0 ] = VROMPAGE( bData );
            W.PPUBANK[ 1 ] = VROMPAGE( bData + 1 );
            W.PPUBANK[ 2 ] = VROMPAGE( bData + 2 );
            W.PPUBANK[ 3 ] = VROMPAGE( bData + 3 );     
            NESCore_Develop_Character_Data();
        }
        MS9.Latch1.Lo = bData;
        break;

      case 0xc000:
        /* Number of 4K Banks to Number of 1K Banks */
        bData %= ( S.NesHeader.VROMSize << 1 );
        bData <<= 2;
        if (MS9.Latch1.State == 0xFE)
        {
            W.PPUBANK[ 0 ] = VROMPAGE( bData );
            W.PPUBANK[ 1 ] = VROMPAGE( bData + 1 );
            W.PPUBANK[ 2 ] = VROMPAGE( bData + 2 );
            W.PPUBANK[ 3 ] = VROMPAGE( bData + 3 );     
            NESCore_Develop_Character_Data();
        }
        MS9.Latch1.Hi = bData;
        break;

      case 0xd000:
        /* Number of 4K Banks to Number of 1K Banks */
        bData %= ( S.NesHeader.VROMSize << 1 );
        bData <<= 2;
  
        if (MS9.Latch2.State == 0xFD)
        {
            W.PPUBANK[ 4 ] = VROMPAGE( bData );
            W.PPUBANK[ 5 ] = VROMPAGE( bData + 1 );
            W.PPUBANK[ 6 ] = VROMPAGE( bData + 2 );
            W.PPUBANK[ 7 ] = VROMPAGE( bData + 3 );    
            NESCore_Develop_Character_Data();
        }
        MS9.Latch2.Lo = bData;
        break;

      case 0xe000:
        /* Number of 4K Banks to Number of 1K Banks */
        bData %= ( S.NesHeader.VROMSize << 1 );
        bData <<= 2;
  
        if (MS9.Latch2.State == 0xFE)
        {
            W.PPUBANK[ 4 ] = VROMPAGE( bData );
            W.PPUBANK[ 5 ] = VROMPAGE( bData + 1 );
            W.PPUBANK[ 6 ] = VROMPAGE( bData + 2 );
            W.PPUBANK[ 7 ] = VROMPAGE( bData + 3 ); 
            NESCore_Develop_Character_Data();
        }
        MS9.Latch2.Hi = bData;
        break;

      case 0xf000:
        NESCore_Mirroring(bData & 0x01 ? 0 : 1);
        break;
    }  
}

void Map9_PPU( word wAddr )
{
  switch (wAddr & 0x3FF0)
  {
    case 0x0FD0:
      MS9.Latch1.State = 0xFD;
      W.PPUBANK[ 0 ] = VROMPAGE( MS9.Latch1.Lo );
      W.PPUBANK[ 1 ] = VROMPAGE( MS9.Latch1.Lo + 1 );
      W.PPUBANK[ 2 ] = VROMPAGE( MS9.Latch1.Lo + 2 );
      W.PPUBANK[ 3 ] = VROMPAGE( MS9.Latch1.Lo + 3 );     
      NESCore_Develop_Character_Data();
      break;

    case 0x0FE0:
      MS9.Latch1.State = 0xFE;
      W.PPUBANK[ 0 ] = VROMPAGE( MS9.Latch1.Hi );
      W.PPUBANK[ 1 ] = VROMPAGE( MS9.Latch1.Hi + 1 );
      W.PPUBANK[ 2 ] = VROMPAGE( MS9.Latch1.Hi + 2 );
      W.PPUBANK[ 3 ] = VROMPAGE( MS9.Latch1.Hi + 3 );     
      NESCore_Develop_Character_Data();      
      break;

    case 0x1FD0:
      MS9.Latch2.State = 0xFD;
      W.PPUBANK[ 4 ] = VROMPAGE( MS9.Latch2.Lo );
      W.PPUBANK[ 5 ] = VROMPAGE( MS9.Latch2.Lo + 1 );
      W.PPUBANK[ 6 ] = VROMPAGE( MS9.Latch2.Lo + 2 );
      W.PPUBANK[ 7 ] = VROMPAGE( MS9.Latch2.Lo + 3 );     
      NESCore_Develop_Character_Data();
      break;      

    case 0x1FE0:
      MS9.Latch2.State = 0xFE;
      W.PPUBANK[ 4 ] = VROMPAGE( MS9.Latch2.Hi );
      W.PPUBANK[ 5 ] = VROMPAGE( MS9.Latch2.Hi + 1 );
      W.PPUBANK[ 6 ] = VROMPAGE( MS9.Latch2.Hi + 2 );
      W.PPUBANK[ 7 ] = VROMPAGE( MS9.Latch2.Hi + 3 );     
      NESCore_Develop_Character_Data();            
      break;
  }
}

void Map9_Restore() { }
