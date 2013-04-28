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

byte Map18_Regs[11];
byte Map18_IRQ_Enable;
word Map18_IRQ_Latch;
word Map18_IRQ_Cnt;

void Map18_Init()
{
  MapperInit = Map18_Init;
  MapperWrite = Map18_Write;
  MapperSram = Map0_Sram;
  MapperApu = Map0_Apu;
  MapperReadApu = Map0_ReadApu;
  MapperVSync = Map0_VSync;
  MapperHSync = Map18_HSync;
  MapperPPU = Map0_PPU;

  /* Callback at Rendering Screen ( 1:BG, 0:Sprite ) */
  MapperRenderScreen = Map0_RenderScreen;

  W.SRAMBANK = S.SRAM;
  W.ROMBANK0 = ROMPAGE( 0 );
  W.ROMBANK1 = ROMPAGE( 1 );
  W.ROMBANK2 = ROMLASTPAGE( 1 );
  W.ROMBANK3 = ROMLASTPAGE( 0 );

  int i ;
  for (i = 0; i < sizeof( Map18_Regs ); i++ )
    Map18_Regs[ i ] = 0;
  Map18_IRQ_Enable = 0;
  Map18_IRQ_Latch = 0;
  Map18_IRQ_Cnt = 0;
}

void Map18_Write( word wAddr, byte byData )
{
  switch( wAddr )
  {
    /* Set ROM Banks */
    case 0x8000:
      Map18_Regs[ 0 ] = ( Map18_Regs[ 0 ] & 0xf0 ) | ( byData & 0x0f );
      W.ROMBANK0 = ROMPAGE( Map18_Regs[ 0 ] % ( S.NesHeader.ROMSize << 1 ) );
      break;

    case 0x8001:
      Map18_Regs[ 0 ] = ( Map18_Regs[ 0 ] & 0x0f ) | ( ( byData & 0x0f ) << 4 );
      W.ROMBANK0 = ROMPAGE( Map18_Regs[ 0 ] % ( S.NesHeader.ROMSize << 1 ) );
      break;

    case 0x8002:
      Map18_Regs[ 1 ] = ( Map18_Regs[ 1 ] & 0xf0 ) | ( byData & 0x0f );
      W.ROMBANK1 = ROMPAGE( Map18_Regs[ 1 ] % ( S.NesHeader.ROMSize << 1 ) );
      break;

    case 0x8003:
      Map18_Regs[ 1 ] = ( Map18_Regs[ 1 ] & 0x0f ) | ( ( byData & 0x0f ) << 4 );
      W.ROMBANK1 = ROMPAGE( Map18_Regs[ 1 ] % ( S.NesHeader.ROMSize << 1 ) );
      break;

    case 0x9000:
      Map18_Regs[ 2 ] = ( Map18_Regs[ 2 ] & 0xf0 ) | ( byData & 0x0f );
      W.ROMBANK2 = ROMPAGE( Map18_Regs[ 2 ] % ( S.NesHeader.ROMSize << 1 ) );
      break;

    case 0x9001:
      Map18_Regs[ 2 ] = ( Map18_Regs[ 2 ] & 0x0f ) | ( ( byData & 0x0f ) << 4 );
      W.ROMBANK2 = ROMPAGE( Map18_Regs[ 2 ] % ( S.NesHeader.ROMSize << 1 ) );
      break;

    /* Set PPU Banks */
    case 0xA000:
      Map18_Regs[ 3 ]  = ( Map18_Regs[ 3 ] & 0xf0 ) | ( byData & 0x0f );
      W.PPUBANK[ 0 ] = VROMPAGE( Map18_Regs[ 3 ] % ( S.NesHeader.VROMSize << 3 ) );
      NESCore_Develop_Character_Data();      
      break;

    case 0xA001:
      Map18_Regs[ 3 ] = ( Map18_Regs[ 3 ] & 0x0f ) | ( ( byData & 0x0f ) << 4 );
      W.PPUBANK[ 0 ] = VROMPAGE( Map18_Regs[ 3 ] % ( S.NesHeader.VROMSize << 3 ) );
      NESCore_Develop_Character_Data();        
      break;

    case 0xA002:
      Map18_Regs[ 4 ]  = ( Map18_Regs[ 4 ] & 0xf0 ) | ( byData & 0x0f );
      W.PPUBANK[ 1 ] = VROMPAGE( Map18_Regs[ 4 ] % ( S.NesHeader.VROMSize << 3 ) );
      NESCore_Develop_Character_Data();         
      break;

    case 0xA003:
      Map18_Regs[ 4 ] = ( Map18_Regs[ 4 ] & 0x0f ) | ( ( byData & 0x0f ) << 4 );
      W.PPUBANK[ 1 ] = VROMPAGE( Map18_Regs[ 4 ] % ( S.NesHeader.VROMSize << 3 ) );
      NESCore_Develop_Character_Data(); 
      break;

    case 0xB000:
      Map18_Regs[ 5 ]  = ( Map18_Regs[ 5 ] & 0xf0 ) | ( byData & 0x0f );
      W.PPUBANK[ 2 ] = VROMPAGE( Map18_Regs[ 5 ] % ( S.NesHeader.VROMSize << 3 ) );
      NESCore_Develop_Character_Data();      
      break;

    case 0xB001:
      Map18_Regs[ 5 ] = ( Map18_Regs[ 5 ] &0x0f ) | ( ( byData & 0x0f ) << 4 );
      W.PPUBANK[ 2 ] = VROMPAGE( Map18_Regs[ 5 ] % ( S.NesHeader.VROMSize << 3 ) );
      NESCore_Develop_Character_Data();        
      break;

    case 0xB002:
      Map18_Regs[ 6 ]  = ( Map18_Regs[ 6 ] & 0xf0 ) | ( byData & 0x0f );
      W.PPUBANK[ 3 ] = VROMPAGE( Map18_Regs[ 6 ] % ( S.NesHeader.VROMSize << 3 ) );
      NESCore_Develop_Character_Data();         
      break;

    case 0xB003:
      Map18_Regs[ 6 ] = ( Map18_Regs[ 6 ] & 0x0f ) | ( ( byData & 0x0f ) << 4 );
      W.PPUBANK[ 3 ] = VROMPAGE( Map18_Regs[ 6 ] % ( S.NesHeader.VROMSize << 3 ) );
      NESCore_Develop_Character_Data(); 
      break;

    case 0xC000:
      Map18_Regs[ 7 ]  = ( Map18_Regs[ 7 ] & 0xf0 ) | ( byData & 0x0f );
      W.PPUBANK[ 4 ] = VROMPAGE( Map18_Regs[ 7 ] % ( S.NesHeader.VROMSize << 3 ) );
      NESCore_Develop_Character_Data();      
      break;

    case 0xC001:
      Map18_Regs[ 7 ] = ( Map18_Regs[ 7 ] & 0x0f ) | ( ( byData & 0x0f ) << 4 );
      W.PPUBANK[ 4 ] = VROMPAGE( Map18_Regs[ 7 ] % ( S.NesHeader.VROMSize << 3 ) );
      NESCore_Develop_Character_Data();        
      break;

    case 0xC002:
      Map18_Regs[ 8 ]  = ( Map18_Regs[ 8 ] & 0xf0 ) | ( byData & 0x0f );
      W.PPUBANK[ 5 ] = VROMPAGE( Map18_Regs[ 8 ] % ( S.NesHeader.VROMSize << 3 ) );
      NESCore_Develop_Character_Data();         
      break;

    case 0xC003:
      Map18_Regs[ 8 ] = ( Map18_Regs[ 8 ] & 0x0f ) | ( ( byData & 0x0f ) << 4 );
      W.PPUBANK[ 5 ] = VROMPAGE( Map18_Regs[ 8 ] % ( S.NesHeader.VROMSize << 3 ) );
      NESCore_Develop_Character_Data(); 
      break;

    case 0xD000:
      Map18_Regs[ 9 ]  = ( Map18_Regs[ 9 ] & 0xf0 ) | ( byData & 0x0f );
      W.PPUBANK[ 6 ] = VROMPAGE( Map18_Regs[ 9 ] % ( S.NesHeader.VROMSize << 3 ) );
      NESCore_Develop_Character_Data();      
      break;

    case 0xD001:
      Map18_Regs[ 9 ] = ( Map18_Regs[ 9 ] & 0x0f ) | ( ( byData & 0x0f ) << 4 );
      W.PPUBANK[ 6 ] = VROMPAGE( Map18_Regs[ 9 ] % ( S.NesHeader.VROMSize << 3 ) );
      NESCore_Develop_Character_Data();        
      break;

    case 0xD002:
      Map18_Regs[ 10 ]  = ( Map18_Regs[ 10 ] & 0xf0 ) | ( byData & 0x0f );
      W.PPUBANK[ 7 ] = VROMPAGE( Map18_Regs[ 10 ] % ( S.NesHeader.VROMSize << 3 ) );
      NESCore_Develop_Character_Data();         
      break;

    case 0xD003:
      Map18_Regs[ 10 ] = ( Map18_Regs[ 10 ] & 0x0f ) | ( ( byData & 0x0f ) << 4 );
      W.PPUBANK[ 7 ] = VROMPAGE( Map18_Regs[ 10 ] % ( S.NesHeader.VROMSize << 3 ) );
      NESCore_Develop_Character_Data(); 
      break;

    case 0xE000:
      Map18_IRQ_Latch = ( Map18_IRQ_Latch & 0xfff0 ) | ( byData & 0x0f );
      break;

    case 0xE001:
      Map18_IRQ_Latch = ( Map18_IRQ_Latch & 0xff0f ) | ( ( byData & 0x0f ) << 4 );
      break;

    case 0xE002:
      Map18_IRQ_Latch = ( Map18_IRQ_Latch & 0xf0ff ) | ( ( byData & 0x0f ) << 8 );
      break;

    case 0xE003:
      Map18_IRQ_Latch = ( Map18_IRQ_Latch & 0x0fff ) | ( ( byData & 0x0f ) << 12 );
      break;

    case 0xF000:
      if ( ( byData & 0x01 ) == 0 )
      {
        Map18_IRQ_Cnt = 0;
      } else {
        Map18_IRQ_Cnt = Map18_IRQ_Latch;
      }
      break;

    case 0xF001:
      Map18_IRQ_Enable = byData & 0x01;
      break;

    /* Name Table Mirroring */
    case 0xF002:
      switch ( byData & 0x03 )
      {
        case 0:
          NESCore_Mirroring( 0 );   /* Horizontal */
          break;
        case 1:
          NESCore_Mirroring( 1 );   /* Vertical */            
          break;
        case 2:
          NESCore_Mirroring( 3 );   /* One Screen 0x2000 */
          break;
      }    
      break;
  }
}

void Map18_HSync()
{
/*
 *  Callback at HSync
 *
 */
  if ( Map18_IRQ_Enable )
  {
    if ( Map18_IRQ_Cnt <= 113 )
    {
      IRQ_REQ;
      Map18_IRQ_Cnt = 0;
      Map18_IRQ_Enable = 0;
    } else {
      Map18_IRQ_Cnt -= 113;
    }
  }
}
