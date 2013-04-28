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

/*
 * NESCore_Mapper.h: Memory Mapper Prototypes
 *                   Memory mappers allow different ROM images to map
 *                   memory in different ways. Each game has its own
 *                   mapping architecture.
 */

#ifndef NESCORE_MAPPER_H
#define NESCORE_MAPPER_H

#include "NESCore_Types.h"

/* Address of 8K units of ROM */
#define ROMPAGE(a)     &W.ROM[(a) * 0x2000]

/* Address of 8K units of ROM from last page to first */
#define ROMLASTPAGE(a) &W.ROM[S.NesHeader.ROMSize * 0x4000-((a) + 1) * 0x2000]

/* Address of 1K Units of VROM */
#define VROMPAGE(a)    &W.VROM[(a) * 0x400]

/* Address of 1K units of the CRAM */
#define CRAMPAGE(a)   &S.PPURAM[0x0000 + ((a)&0x1F) * 0x400]

/* Address of 1K units of VRAM */
#define VRAMPAGE(a)    &S.PPURAM[ 0x2000 + (a) * 0x400 ]

/* Translate the pointer to ChrBuf into the address of Pattern Table */ 
#define PATTBL(a)      (((a) - S.ChrBuf ) >> 2)

/* Address of 8K units of the Mapper 5 ROM */
#define Map5_ROMPAGE(a)     &MS5.Map5_Wram[( (a) & 0x07 ) * 0x2000]

/* Addresses of 1K VROM for various mappers */
#define Map6_VROMPAGE(a)   &Map6_Chr_Ram[(a) * 0x400]
#define Map19_VROMPAGE(a)  &Map19_Chr_Ram[(a) * 0x400]
#define Map85_VROMPAGE(a)  &Map85_Chr_Ram[(a) * 0x400]

struct MapperTable_tag
{
  int nMapperNo;
  void (*pMapperInit)();
};

extern struct MapperTable_tag MapperTable[];


/* ------------- Function prototypes for each supported mapper -------------- */


void Map0_Init();
void Map0_Write(word wAddr, byte bData);
void Map0_Sram(word wAddr, byte bData);
void Map0_Apu(word wAddr, byte bData);
byte Map0_ReadApu(word wAddr);
void Map0_VSync();
void Map0_HSync();
void Map0_PPU(word wAddr);
void Map0_RenderScreen(byte bMode);

void Map1_Init();
void Map1_Write(word wAddr, byte bData);
void Map1_set_ROM_banks();
void Map1_Restore();

void Map2_Init();
void Map2_Write(word wAddr, byte bData);

void Map3_Init();
void Map3_Write(word wAddr, byte bData);

void Map4_Init();
void Map4_Write(word wAddr, byte bData);
void Map4_HSync();
void Map4_Set_CPU_Banks();
void Map4_Set_PPU_Banks();
void Map4_Restore();

void Map5_Init();
void Map5_Write(word wAddr, byte bData);
void Map5_Apu(word wAddr, byte bData);
byte Map5_ReadApu(word wAddr);
void Map5_HSync();
void Map5_RenderScreen(byte bMode);
void Map5_Sync_Prg_Banks(void);
void Map5_Restore();

void Map6_Init();
void Map6_Write(word wAddr, byte bData);
void Map6_Apu(word wAddr, byte bData);
void Map6_HSync();

void Map7_Init();
void Map7_Write(word wAddr, byte bData);

void Map8_Init();
void Map8_Write(word wAddr, byte bData);

void Map9_Init();
void Map9_Write(word wAddr, byte bData);
void Map9_PPU(word wAddr);
void Map9_Restore();

void Map10_Init();
void Map10_Write(word wAddr, byte bData);
void Map10_PPU(word wAddr);

void Map11_Init();
void Map11_Write(word wAddr, byte bData);

void Map13_Init();
void Map13_Write(word wAddr, byte bData);

void Map15_Init();
void Map15_Write(word wAddr, byte bData);

void Map16_Init();
void Map16_Write(word wAddr, byte bData);
void Map16_HSync();

void Map17_Init();
void Map17_Apu(word wAddr, byte bData);
void Map17_HSync();

void Map18_Init();
void Map18_Write(word wAddr, byte bData);
void Map18_HSync();

void Map19_Init();
void Map19_Write(word wAddr, byte bData);
void Map19_Apu(word wAddr, byte bData);
byte Map19_ReadApu(word wAddr);
void Map19_HSync();

void Map21_Init();
void Map21_Write(word wAddr, byte bData);
void Map21_HSync();

void Map22_Init();
void Map22_Write(word wAddr, byte bData);

void Map23_Init();
void Map23_Write(word wAddr, byte bData);
void Map23_HSync();

void Map24_Init();
void Map24_Write(word wAddr, byte bData);
void Map24_HSync();

void Map25_Init();
void Map25_Write(word wAddr, byte bData);
void Map25_Sync_Vrom( int nBank );
void Map25_HSync();

void Map26_Init();
void Map26_Write(word wAddr, byte bData);
void Map26_HSync();

void Map32_Init();
void Map32_Write(word wAddr, byte bData);

void Map33_Init();
void Map33_Write(word wAddr, byte bData);
void Map33_HSync();

void Map34_Init();
void Map34_Write(word wAddr, byte bData);
void Map34_Sram(word wAddr, byte bData);

void Map40_Init();
void Map40_Write(word wAddr, byte bData);
void Map40_HSync();

void Map41_Init();
void Map41_Write(word wAddr, byte bData);
void Map41_Sram(word wAddr, byte bData);

void Map42_Init();
void Map42_Write(word wAddr, byte bData);
void Map42_HSync();

void Map43_Init();
void Map43_Write(word wAddr, byte bData);
void Map43_Apu(word wAddr, byte bData);
byte Map43_ReadApu(word wAddr);
void Map43_HSync();

void Map44_Init();
void Map44_Write(word wAddr, byte bData);
void Map44_HSync();
void Map44_Set_CPU_Banks();
void Map44_Set_PPU_Banks();

void Map45_Init();
void Map45_Sram(word wAddr, byte bData);
void Map45_Write(word wAddr, byte bData);
void Map45_HSync();
void Map45_Set_CPU_Bank4(byte bData);
void Map45_Set_CPU_Bank5(byte bData);
void Map45_Set_CPU_Bank6(byte bData);
void Map45_Set_CPU_Bank7(byte bData);
void Map45_Set_PPU_Banks();

void Map46_Init();
void Map46_Sram(word wAddr, byte bData);
void Map46_Write(word wAddr, byte bData);
void Map46_Set_ROM_Banks();

void Map47_Init();
void Map47_Sram(word wAddr, byte bData);
void Map47_Write(word wAddr, byte bData);
void Map47_HSync();
void Map47_Set_CPU_Banks();
void Map47_Set_PPU_Banks();

void Map48_Init();
void Map48_Write(word wAddr, byte bData);
void Map48_HSync();

void Map49_Init();
void Map49_Sram(word wAddr, byte bData);
void Map49_Write(word wAddr, byte bData);
void Map49_HSync();
void Map49_Set_CPU_Banks();
void Map49_Set_PPU_Banks();

void Map50_Init();
void Map50_Apu(word wAddr, byte bData);
void Map50_HSync();
void Map50_Restore();

void Map51_Init();
void Map51_Sram(word wAddr, byte bData);
void Map51_Write(word wAddr, byte bData);
void Map51_Set_CPU_Banks();

void Map57_Init();
void Map57_Write(word wAddr, byte bData);

void Map58_Init();
void Map58_Write(word wAddr, byte bData);

void Map60_Init();
void Map60_Write(word wAddr, byte bData);

void Map61_Init();
void Map61_Write(word wAddr, byte bData);

void Map62_Init();
void Map62_Write(word wAddr, byte bData);

void Map64_Init();
void Map64_Write(word wAddr, byte bData);

void Map65_Init();
void Map65_Write(word wAddr, byte bData);
void Map65_HSync();

void Map66_Init();
void Map66_Write(word wAddr, byte bData);

void Map67_Init();
void Map67_Write(word wAddr, byte bData);
void Map67_HSync();

void Map68_Init();
void Map68_Write(word wAddr, byte bData);
void Map68_SyncMirror();

void Map69_Init();
void Map69_Write(word wAddr, byte bData);
void Map69_HSync();

void Map70_Init();
void Map70_Write(word wAddr, byte bData);

void Map71_Init();
void Map71_Write(word wAddr, byte bData);

void Map72_Init();
void Map72_Write(word wAddr, byte bData);

void Map73_Init();
void Map73_Write(word wAddr, byte bData);
void Map73_HSync();

void Map74_Init();
void Map74_Write(word wAddr, byte bData);
void Map74_HSync();
void Map74_Set_CPU_Banks();
void Map74_Set_PPU_Banks();

void Map75_Init();
void Map75_Write(word wAddr, byte bData);

void Map76_Init();
void Map76_Write(word wAddr, byte bData);

void Map77_Init();
void Map77_Write(word wAddr, byte bData);

void Map78_Init();
void Map78_Write(word wAddr, byte bData);

void Map79_Init();
void Map79_Apu(word wAddr, byte bData);

void Map80_Init();
void Map80_Sram(word wAddr, byte bData);

void Map82_Init();
void Map82_Sram(word wAddr, byte bData);

void Map83_Init();
void Map83_Write(word wAddr, byte bData);
void Map83_Apu(word wAddr, byte bData);
byte Map83_ReadApu(word wAddr);
void Map83_HSync();

void Map85_Init();
void Map85_Write(word wAddr, byte bData);
void Map85_HSync();

void Map86_Init();
void Map86_Sram(word wAddr, byte bData);

void Map87_Init();
void Map87_Sram(word wAddr, byte bData);

void Map88_Init();
void Map88_Write(word wAddr, byte bData);

void Map89_Init();
void Map89_Write(word wAddr, byte bData);

void Map90_Init();
void Map90_Write(word wAddr, byte bData);
void Map90_Apu(word wAddr, byte bData);
byte Map90_ReadApu(word wAddr);
void Map90_HSync();
void Map90_Sync_Mirror(void);
void Map90_Sync_Prg_Banks(void);
void Map90_Sync_Chr_Banks(void);

void Map91_Init();
void Map91_Sram(word wAddr, byte bData);

void Map92_Init();
void Map92_Write(word wAddr, byte bData);

void Map93_Init();
void Map93_Sram(word wAddr, byte bData);

void Map94_Init();
void Map94_Write(word wAddr, byte bData);

void Map95_Init();
void Map95_Write(word wAddr, byte bData);
void Map95_Set_CPU_Banks();
void Map95_Set_PPU_Banks();

void Map96_Init();
void Map96_Write(word wAddr, byte bData);
void Map96_PPU(word wAddr);
void Map96_Set_Banks();

void Map97_Init();
void Map97_Write(word wAddr, byte bData);

void Map99_Init();
void Map99_Apu(word wAddr, byte bData);
byte Map99_ReadApu(word wAddr);

void Map100_Init();
void Map100_Write(word wAddr, byte bData);
void Map100_HSync();
void Map100_Set_CPU_Banks();
void Map100_Set_PPU_Banks();

void Map101_Init();
void Map101_Write(word wAddr, byte bData);

void Map105_Init();
void Map105_Write(word wAddr, byte bData);
void Map105_HSync();

void Map107_Init();
void Map107_Write(word wAddr, byte bData);

void Map108_Init();
void Map108_Write(word wAddr, byte bData);

void Map109_Init();
void Map109_Apu(word wAddr, byte bData);
void Map109_Set_PPU_Banks();

void Map110_Init();
void Map110_Apu(word wAddr, byte bData);

void Map112_Init();
void Map112_Write(word wAddr, byte bData);
void Map112_HSync();
void Map112_Set_CPU_Banks();
void Map112_Set_PPU_Banks();

void Map113_Init();
void Map113_Apu(word wAddr, byte bData);
void Map113_Write(word wAddr, byte bData);

void Map114_Init();
void Map114_Sram(word wAddr, byte bData);
void Map114_Write(word wAddr, byte bData);
void Map114_HSync();
void Map114_Set_CPU_Banks();
void Map114_Set_PPU_Banks();

void Map115_Init();
void Map115_Sram(word wAddr, byte bData);
void Map115_Write(word wAddr, byte bData);
void Map115_HSync();
void Map115_Set_CPU_Banks();
void Map115_Set_PPU_Banks();

void Map116_Init();
void Map116_Write(word wAddr, byte bData);
void Map116_HSync();
void Map116_Set_CPU_Banks();
void Map116_Set_PPU_Banks();

void Map117_Init();
void Map117_Write(word wAddr, byte bData);
void Map117_HSync();

void Map118_Init();
void Map118_Write(word wAddr, byte bData);
void Map118_HSync();
void Map118_Set_CPU_Banks();
void Map118_Set_PPU_Banks();
void Map118_Restore();

void Map119_Init();
void Map119_Write(word wAddr, byte bData);
void Map119_HSync();
void Map119_Set_CPU_Banks();
void Map119_Set_PPU_Banks();
void Map119_Restore();

void Map122_Init();
void Map122_Sram(word wAddr, byte bData);

void Map133_Init();
void Map133_Apu(word wAddr, byte bData);

void Map134_Init();
void Map134_Apu(word wAddr, byte bData);

void Map135_Init();
void Map135_Apu(word wAddr, byte bData);
void Map135_Set_PPU_Banks();

void Map140_Init();
void Map140_Sram(word wAddr, byte bData);
void Map140_Apu(word wAddr, byte bData);

void Map151_Init();
void Map151_Write(word wAddr, byte bData);

void Map160_Init();
void Map160_Write(word wAddr, byte bData);
void Map160_HSync();

void Map180_Init();
void Map180_Write(word wAddr, byte bData);

void Map181_Init();
void Map181_Apu(word wAddr, byte bData);

void Map182_Init();
void Map182_Write(word wAddr, byte bData);
void Map182_HSync();

void Map183_Init();
void Map183_Write(word wAddr, byte bData);
void Map183_HSync();

void Map185_Init();
void Map185_Write(word wAddr, byte bData);

void Map187_Init();
void Map187_Write(word wAddr, byte bData);
void Map187_Apu(word wAddr, byte bData);
byte Map187_ReadApu(word wAddr);
void Map187_HSync();
void Map187_Set_CPU_Banks();
void Map187_Set_PPU_Banks();

void Map188_Init();
void Map188_Write(word wAddr, byte bData);

void Map189_Init();
void Map189_Apu(word wAddr, byte bData);
void Map189_Write(word wAddr, byte bData);
void Map189_HSync();

void Map191_Init();
void Map191_Apu(word wAddr, byte bData);
void Map191_Set_CPU_Banks();
void Map191_Set_PPU_Banks();

void Map193_Init();
void Map193_Sram(word wAddr, byte bData);

void Map194_Init();
void Map194_Write(word wAddr, byte bData);

void Map200_Init();
void Map200_Write(word wAddr, byte bData);

void Map201_Init();
void Map201_Write(word wAddr, byte bData);

void Map202_Init();
void Map202_Apu(word wAddr, byte bData);
void Map202_Write(word wAddr, byte bData);
void Map202_WriteSub(word wAddr, byte bData);

void Map222_Init();
void Map222_Write(word wAddr, byte bData);

void Map225_Init();
void Map225_Write(word wAddr, byte bData);

void Map226_Init();
void Map226_Write(word wAddr, byte bData);

void Map227_Init();
void Map227_Write(word wAddr, byte bData);

void Map228_Init();
void Map228_Write(word wAddr, byte bData);

void Map229_Init();
void Map229_Write(word wAddr, byte bData);

void Map230_Init();
void Map230_Write(word wAddr, byte bData);

void Map231_Init();
void Map231_Write(word wAddr, byte bData);

void Map232_Init();
void Map232_Write(word wAddr, byte bData);

void Map233_Init();
void Map233_Write(word wAddr, byte bData);

void Map234_Init();
void Map234_Write(word wAddr, byte bData);
void Map234_Set_Banks();

void Map235_Init();
void Map235_Write(word wAddr, byte bData);

void Map236_Init();
void Map236_Write(word wAddr, byte bData);

void Map240_Init();
void Map240_Apu(word wAddr, byte bData);

void Map241_Init();
void Map241_Write(word wAddr, byte bData);

void Map242_Init();
void Map242_Write(word wAddr, byte bData);

void Map243_Init();
void Map243_Apu(word wAddr, byte bData);

void Map244_Init();
void Map244_Write(word wAddr, byte bData);

void Map245_Init();
void Map245_Write(word wAddr, byte bData);
void Map245_HSync();

void Map246_Init();
void Map246_Sram(word wAddr, byte bData);

void Map248_Init();
void Map248_Write(word wAddr, byte bData);
void Map248_Apu(word wAddr, byte bData);
void Map248_Sram(word wAddr, byte bData);
void Map248_HSync();
void Map248_Set_CPU_Banks();
void Map248_Set_PPU_Banks();

void Map249_Init();
void Map249_Write(word wAddr, byte bData);
void Map249_Apu(word wAddr, byte bData);
void Map249_HSync();

void Map251_Init();
void Map251_Write(word wAddr, byte bData);
void Map251_Sram(word wAddr, byte bData);
void Map251_Set_Banks();

void Map252_Init();
void Map252_Write(word wAddr, byte bData);
void Map252_HSync();

void Map255_Init();
void Map255_Write(word wAddr, byte bData);
void Map255_Apu(word wAddr, byte bData);
byte Map255_ReadApu(word wAddr);

#endif /* NESCORE_MAPPER_H */
