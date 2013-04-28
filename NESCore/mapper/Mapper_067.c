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

byte Map67_IRQ_Enable;
byte Map67_IRQ_Cnt;
byte Map67_IRQ_Latch;

void Map67_Init()
{
  MapperInit = Map67_Init;
  MapperWrite = Map67_Write;
  MapperSram = Map0_Sram;
  MapperApu = Map0_Apu;
  MapperReadApu = Map0_ReadApu;
  MapperVSync = Map0_VSync;
  MapperHSync = Map67_HSync;
  MapperPPU = Map0_PPU;

  /* Callback at Rendering Screen ( 1:BG, 0:Sprite ) */
  MapperRenderScreen = Map0_RenderScreen;

  W.SRAMBANK = S.SRAM;
  W.ROMBANK0 = ROMPAGE( 0 );
  W.ROMBANK1 = ROMPAGE( 1 );
  W.ROMBANK2 = ROMLASTPAGE( 1 );
  W.ROMBANK3 = ROMLASTPAGE( 0 );

  /* Set PPU Banks */
  W.PPUBANK[ 0 ] = VROMPAGE( 0 );
  W.PPUBANK[ 1 ] = VROMPAGE( 1 );
  W.PPUBANK[ 2 ] = VROMPAGE( 2 );
  W.PPUBANK[ 3 ] = VROMPAGE( 3 );
  W.PPUBANK[ 4 ] = VROMPAGE( ( S.NesHeader.VROMSize << 3 ) - 4 );
  W.PPUBANK[ 5 ] = VROMPAGE( ( S.NesHeader.VROMSize << 3 ) - 3 );
  W.PPUBANK[ 6 ] = VROMPAGE( ( S.NesHeader.VROMSize << 3 ) - 2 );
  W.PPUBANK[ 7 ] = VROMPAGE( ( S.NesHeader.VROMSize << 3 ) - 1 );
  NESCore_Develop_Character_Data();

  /* Initialize IRQ Registers */
  Map67_IRQ_Enable = 0;
  Map67_IRQ_Cnt = 0;
  Map67_IRQ_Latch = 0;
}

void Map67_Write( word wAddr, byte byData )
{
  switch ( wAddr & 0xf800 )
  {
    /* Set PPU Banks */
    case 0x8800:
      byData <<= 1;
      byData %= ( S.NesHeader.VROMSize << 3 );

      W.PPUBANK[ 0 ] = VROMPAGE( byData + 0 );
      W.PPUBANK[ 1 ] = VROMPAGE( byData + 1 );
      NESCore_Develop_Character_Data();
      break;

    case 0x9800:
      byData <<= 1;
      byData %= ( S.NesHeader.VROMSize << 3 );

      W.PPUBANK[ 2 ] = VROMPAGE( byData + 0 );
      W.PPUBANK[ 3 ] = VROMPAGE( byData + 1 );
      NESCore_Develop_Character_Data();
      break;

    case 0xa800:
      byData <<= 1;
      byData %= ( S.NesHeader.VROMSize << 3 );

      W.PPUBANK[ 4 ] = VROMPAGE( byData + 0 );
      W.PPUBANK[ 5 ] = VROMPAGE( byData + 1 );
      NESCore_Develop_Character_Data();
      break;

    case 0xb800:
      byData <<= 1;
      byData %= ( S.NesHeader.VROMSize << 3 );

      W.PPUBANK[ 6 ] = VROMPAGE( byData + 0 );
      W.PPUBANK[ 7 ] = VROMPAGE( byData + 1 );
      NESCore_Develop_Character_Data();
      break;

    case 0xc800:
      Map67_IRQ_Cnt = Map67_IRQ_Latch;
      Map67_IRQ_Latch = byData;
      break;

    case 0xd800:
      Map67_IRQ_Enable = byData & 0x10;
      break;

    case 0xe800:
      switch ( byData & 0x03 )
      {
        case 0:
          NESCore_Mirroring( 1 );
          break;
        case 1:
          NESCore_Mirroring( 0 );
          break;
        case 2:
          NESCore_Mirroring( 3 );
          break;
        case 3:
          NESCore_Mirroring( 2 );
          break;
      }
      break;

    /* Set ROM Banks */
    case 0xf800:
      byData <<= 1;
      byData %= ( S.NesHeader.ROMSize << 1 );

      W.ROMBANK0 = ROMPAGE( byData + 0 );
      W.ROMBANK1 = ROMPAGE( byData + 1 );
      break;
  }
}

void Map67_HSync()
{
  if ( Map67_IRQ_Enable )
  {
    if ( /* 0 <= S.PPU_Scanline && */ S.PPU_Scanline <= 239 )
    {
      if ( S.PPU_R1 & R1_SHOW_BG || S.PPU_R1 & R1_SHOW_SP )
      {
        if ( --Map67_IRQ_Cnt == 0xf6 )
        {
          Map67_IRQ_Cnt = Map67_IRQ_Latch;
          IRQ_REQ;
        }
      }
    }
  }
}
