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
 * NESCore_Mapper.c: Memory mapper function mappings
 *                   Memory mappers allow different ROM images to map
 *                   memory in different ways. Each game has its own
 *                   mapping architecture.
 */

#include <string.h>

#include "NESCore.h"
#include "NESCore_Callback.h"
#include "NESCore_Mapper.h"
#include "M6502.h"

struct MapperTable_tag MapperTable[] =
{
  {   0, Map0_Init   },
  {   1, Map1_Init   },
  {   2, Map2_Init   },
  {   3, Map3_Init   },
  {   4, Map4_Init   },
  {   6, Map6_Init   },
  {   7, Map7_Init   },
  {   8, Map8_Init   },
  {   9, Map9_Init   },
  {  10, Map10_Init  },
  {  11, Map11_Init  },
  {  15, Map15_Init  },
  {  16, Map16_Init  },
  {  17, Map17_Init  },
  {  18, Map18_Init  },
  {  19, Map19_Init  },
  {  22, Map22_Init  },
  {  23, Map23_Init  },
  {  24, Map24_Init  },
  {  25, Map25_Init  },
  {  26, Map26_Init  },
  {  32, Map32_Init  },
  {  33, Map33_Init  },
  {  34, Map34_Init  },
  {  40, Map40_Init  },
  {  41, Map41_Init  },
  {  42, Map42_Init  },
  {  43, Map43_Init  },
  {  44, Map44_Init  },
  {  45, Map45_Init  },
  {  46, Map46_Init  },
  {  47, Map47_Init  },
  {  48, Map48_Init  },
  {  49, Map49_Init  },
  {  50, Map50_Init  },
  {  51, Map51_Init  },
  {  57, Map57_Init  },
  {  58, Map58_Init  },
  {  60, Map60_Init  },
  {  61, Map61_Init  },
  {  62, Map62_Init  },
  {  64, Map64_Init  },
  {  65, Map65_Init  },
  {  66, Map66_Init  },
  {  67, Map67_Init  },
  {  68, Map68_Init  },
  {  69, Map69_Init  },
  {  70, Map70_Init  },
  {  71, Map71_Init  },
  {  72, Map72_Init  },
  {  73, Map73_Init  },
  {  74, Map74_Init  },
  {  75, Map75_Init  },
  {  76, Map76_Init  },
  {  77, Map77_Init  }, 
  {  78, Map78_Init  }, 
  {  79, Map79_Init  }, 
  {  80, Map80_Init  }, 
  {  82, Map82_Init  }, 
  {  83, Map83_Init  },
  {  85, Map85_Init  },
  {  86, Map86_Init  },
  {  87, Map87_Init  },
  {  88, Map88_Init  },
  {  89, Map89_Init  },
  {  90, Map90_Init  },
  {  91, Map91_Init  },
  {  92, Map92_Init  },
  {  93, Map93_Init  },
  {  94, Map94_Init  },
  {  95, Map95_Init  },
  {  96, Map96_Init  },
  {  97, Map97_Init  },
  {  99, Map99_Init  },
  { 100, Map100_Init },
  { 101, Map101_Init },
  { 105, Map105_Init },
  { 107, Map107_Init },
  { 108, Map108_Init },
  { 109, Map109_Init },
  { 110, Map110_Init },
  { 112, Map112_Init },
  { 113, Map113_Init },
  { 114, Map114_Init },
  { 115, Map115_Init },
  { 116, Map116_Init },
  { 117, Map117_Init },
  { 118, Map118_Init },
  { 122, Map122_Init },
  { 133, Map133_Init },
  { 134, Map134_Init },
  { 135, Map135_Init },
  { 140, Map140_Init },
  { 151, Map151_Init }, 
  { 160, Map160_Init }, 
  { 180, Map180_Init }, 
  { 181, Map181_Init }, 
  { 182, Map182_Init }, 
  { 183, Map183_Init }, 
  { 185, Map185_Init }, 
  { 187, Map187_Init }, 
  { 188, Map188_Init }, 
  { 189, Map189_Init }, 
  { 191, Map191_Init }, 
  { 193, Map193_Init }, 
  { 194, Map194_Init }, 
  { 200, Map200_Init },
  { 201, Map201_Init },
  { 202, Map202_Init },
  { 222, Map222_Init },
  { 225, Map225_Init },
  { 226, Map226_Init },
  { 227, Map227_Init },
  { 228, Map228_Init },
  { 229, Map229_Init },
  { 230, Map230_Init },
  { 231, Map231_Init },
  { 232, Map232_Init },
  { 233, Map233_Init },
  { 234, Map234_Init },
  { 235, Map235_Init },
  { 236, Map236_Init },
  { 240, Map240_Init },
  { 241, Map241_Init },
  { 242, Map242_Init },
  { 243, Map243_Init },
  { 244, Map244_Init },
  { 245, Map245_Init },
  { 246, Map246_Init },
  { 248, Map248_Init },
  { 249, Map249_Init },
  { 251, Map251_Init },
  { 252, Map252_Init },
  { 255, Map255_Init },
  { -1, NULL }
};

#include "./mapper/Mapper_000.c"
#include "./mapper/Mapper_001.c"
#include "./mapper/Mapper_002.c"
#include "./mapper/Mapper_003.c"
#include "./mapper/Mapper_004.c"
#include "./mapper/Mapper_005.c"
#include "./mapper/Mapper_006.c"
#include "./mapper/Mapper_007.c"
#include "./mapper/Mapper_008.c"
#include "./mapper/Mapper_009.c"
#include "./mapper/Mapper_010.c"
#include "./mapper/Mapper_011.c"
#include "./mapper/Mapper_013.c"
#include "./mapper/Mapper_015.c"
#include "./mapper/Mapper_016.c"
#include "./mapper/Mapper_017.c"
#include "./mapper/Mapper_018.c"
#include "./mapper/Mapper_019.c"
#include "./mapper/Mapper_021.c"          
#include "./mapper/Mapper_022.c"  
#include "./mapper/Mapper_023.c"  
#include "./mapper/Mapper_024.c"  
#include "./mapper/Mapper_025.c"  
#include "./mapper/Mapper_026.c"  
#include "./mapper/Mapper_032.c"  
#include "./mapper/Mapper_033.c" 
#include "./mapper/Mapper_034.c" 
#include "./mapper/Mapper_040.c"
#include "./mapper/Mapper_041.c"
#include "./mapper/Mapper_042.c"
#include "./mapper/Mapper_043.c"
#include "./mapper/Mapper_044.c"
#include "./mapper/Mapper_045.c"
#include "./mapper/Mapper_046.c"
#include "./mapper/Mapper_047.c"
#include "./mapper/Mapper_048.c"
#include "./mapper/Mapper_049.c"
#include "./mapper/Mapper_050.c"
#include "./mapper/Mapper_051.c"
#include "./mapper/Mapper_057.c"
#include "./mapper/Mapper_058.c"
#include "./mapper/Mapper_060.c"
#include "./mapper/Mapper_061.c"
#include "./mapper/Mapper_062.c"
#include "./mapper/Mapper_064.c"
#include "./mapper/Mapper_065.c"
#include "./mapper/Mapper_066.c"
#include "./mapper/Mapper_067.c"
#include "./mapper/Mapper_068.c"
#include "./mapper/Mapper_069.c"
#include "./mapper/Mapper_070.c"
#include "./mapper/Mapper_071.c"
#include "./mapper/Mapper_072.c"
#include "./mapper/Mapper_073.c"
#include "./mapper/Mapper_074.c"
#include "./mapper/Mapper_075.c"
#include "./mapper/Mapper_076.c"
#include "./mapper/Mapper_077.c"
#include "./mapper/Mapper_078.c"
#include "./mapper/Mapper_079.c"
#include "./mapper/Mapper_080.c"
#include "./mapper/Mapper_082.c"
#include "./mapper/Mapper_083.c"
#include "./mapper/Mapper_085.c"
#include "./mapper/Mapper_086.c"
#include "./mapper/Mapper_087.c"
#include "./mapper/Mapper_088.c"
#include "./mapper/Mapper_089.c"
#include "./mapper/Mapper_090.c"
#include "./mapper/Mapper_091.c"
#include "./mapper/Mapper_092.c"
#include "./mapper/Mapper_093.c"
#include "./mapper/Mapper_094.c"
#include "./mapper/Mapper_095.c"
#include "./mapper/Mapper_096.c"
#include "./mapper/Mapper_097.c"
#include "./mapper/Mapper_099.c"
#include "./mapper/Mapper_100.c"
#include "./mapper/Mapper_101.c"
#include "./mapper/Mapper_105.c"
#include "./mapper/Mapper_107.c"
#include "./mapper/Mapper_108.c"
#include "./mapper/Mapper_109.c"
#include "./mapper/Mapper_110.c"
#include "./mapper/Mapper_112.c"
#include "./mapper/Mapper_113.c"
#include "./mapper/Mapper_114.c"
#include "./mapper/Mapper_115.c"
#include "./mapper/Mapper_116.c"
#include "./mapper/Mapper_117.c"
#include "./mapper/Mapper_118.c"
#include "./mapper/Mapper_119.c"
#include "./mapper/Mapper_122.c"
#include "./mapper/Mapper_133.c"
#include "./mapper/Mapper_134.c"
#include "./mapper/Mapper_135.c"
#include "./mapper/Mapper_140.c"
#include "./mapper/Mapper_151.c"
#include "./mapper/Mapper_160.c"
#include "./mapper/Mapper_180.c"
#include "./mapper/Mapper_181.c"
#include "./mapper/Mapper_182.c"
#include "./mapper/Mapper_183.c"
#include "./mapper/Mapper_185.c"
#include "./mapper/Mapper_187.c"
#include "./mapper/Mapper_188.c"
#include "./mapper/Mapper_189.c"
#include "./mapper/Mapper_191.c"
#include "./mapper/Mapper_193.c"
#include "./mapper/Mapper_194.c"
#include "./mapper/Mapper_200.c"
#include "./mapper/Mapper_201.c"
#include "./mapper/Mapper_202.c"
#include "./mapper/Mapper_222.c"
#include "./mapper/Mapper_225.c"
#include "./mapper/Mapper_226.c"
#include "./mapper/Mapper_227.c"
#include "./mapper/Mapper_228.c"
#include "./mapper/Mapper_229.c"
#include "./mapper/Mapper_230.c"
#include "./mapper/Mapper_231.c"
#include "./mapper/Mapper_232.c"
#include "./mapper/Mapper_233.c"
#include "./mapper/Mapper_234.c"
#include "./mapper/Mapper_235.c"
#include "./mapper/Mapper_236.c"
#include "./mapper/Mapper_240.c"
#include "./mapper/Mapper_241.c"
#include "./mapper/Mapper_242.c"
#include "./mapper/Mapper_243.c"
#include "./mapper/Mapper_244.c"
#include "./mapper/Mapper_245.c"
#include "./mapper/Mapper_246.c"
#include "./mapper/Mapper_248.c"
#include "./mapper/Mapper_249.c"
#include "./mapper/Mapper_251.c"
#include "./mapper/Mapper_252.c"
#include "./mapper/Mapper_255.c"

