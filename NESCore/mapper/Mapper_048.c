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

byte Map48_Regs[ 1 ];
byte Map48_IRQ_Enable;
byte Map48_IRQ_Cnt;

void Map48_Init()
{
  MapperInit = Map48_Init;
  MapperWrite = Map48_Write;
  MapperSram = Map0_Sram;
  MapperApu = Map0_Apu;
  MapperReadApu = Map0_ReadApu;
  MapperVSync = Map0_VSync;
  MapperHSync = Map48_HSync;
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

  /* Initialize IRQ Registers */
  Map48_Regs[ 0 ] = 0;
  Map48_IRQ_Enable = 0;
  Map48_IRQ_Cnt = 0;
}

void Map48_Write( word wAddr, byte byData )
{
  switch ( wAddr )
  {
    case 0x8000:
      /* Name Table Mirroring */ 
      if ( !Map48_Regs[ 0 ] )
      {
        if ( byData & 0x40 )
        {
          NESCore_Mirroring( 0 );
        } else {
          NESCore_Mirroring( 1 );
        }
      }
      /* Set ROM Banks */
      W.ROMBANK0 = ROMPAGE( byData % ( S.NesHeader.ROMSize << 1 ) );
      break;

    case 0x8001:
      /* Set ROM Banks */
      W.ROMBANK1 = ROMPAGE( byData % ( S.NesHeader.ROMSize << 1 ) );
      break;  
 
    /* Set PPU Banks */
    case 0x8002:
      W.PPUBANK[ 0 ] = VROMPAGE( ( ( byData << 1 ) + 0 ) % ( S.NesHeader.VROMSize << 3 ) );
      W.PPUBANK[ 1 ] = VROMPAGE( ( ( byData << 1 ) + 1 ) % ( S.NesHeader.VROMSize << 3 ) );
      NESCore_Develop_Character_Data();
      break;

    case 0x8003:
      W.PPUBANK[ 2 ] = VROMPAGE( ( ( byData << 1 ) + 0 ) % ( S.NesHeader.VROMSize << 3 ) );
      W.PPUBANK[ 3 ] = VROMPAGE( ( ( byData << 1 ) + 1 ) % ( S.NesHeader.VROMSize << 3 ) );
      NESCore_Develop_Character_Data();
      break;

    case 0xa000:
      W.PPUBANK[ 4 ] = VROMPAGE( ( ( byData << 1 ) + 0 ) % ( S.NesHeader.VROMSize << 3 ) );
      NESCore_Develop_Character_Data();
      break;

    case 0xa001:
      W.PPUBANK[ 5 ] = VROMPAGE( ( ( byData << 1 ) + 0 ) % ( S.NesHeader.VROMSize << 3 ) );
      NESCore_Develop_Character_Data();
      break;

    case 0xa002:
      W.PPUBANK[ 6 ] = VROMPAGE( ( ( byData << 1 ) + 0 ) % ( S.NesHeader.VROMSize << 3 ) );
      NESCore_Develop_Character_Data();
      break;

    case 0xa003:
      W.PPUBANK[ 7 ] = VROMPAGE( ( ( byData << 1 ) + 0 ) % ( S.NesHeader.VROMSize << 3 ) );
      NESCore_Develop_Character_Data();
      break;

    case 0xc000:
      Map48_IRQ_Cnt = byData;
      break;

    case 0xc001:
      Map48_IRQ_Enable = byData & 0x01;
      break;

    case 0xe000:
      /* Name Table Mirroring */ 
      if ( byData & 0x40 )
      {
        NESCore_Mirroring( 0 );
      } else {
        NESCore_Mirroring( 1 );
      }
      Map48_Regs[ 0 ] = 1;
      break;
  }
}

void Map48_HSync()
{
  if ( Map48_IRQ_Enable )
  {
    if ( /* 0 <= S.PPU_Scanline && */ S.PPU_Scanline <= 239 )
    {
      if ( S.PPU_R1 & R1_SHOW_BG || S.PPU_R1 & R1_SHOW_SP )
      {
        if ( Map48_IRQ_Cnt == 0xff )
        {
          IRQ_REQ;
          Map48_IRQ_Enable = 0;
        } else {
          Map48_IRQ_Cnt++;
        }
      }
    }
  }
}
