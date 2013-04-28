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
 * NESCore.h: The NES Emulator Core Project
 *            Primary NES Emulation Prototypes
 */

#ifndef __NESCORE_H_
#define __NESCORE_H_

#include "NESCore_Types.h"
#include "NESCore_Mapper.h"
#include "NESCore_pAPU.h"
#include "M6502.h"

#define GAMEGENIE_CODES	4

#define RAM_SIZE     0x2000
#define SRAM_SIZE    0x2000
#define PPURAM_SIZE  0x4000
#define SPRRAM_SIZE  256
#define DRAM_SIZE    0xA000

#define NAME_TABLE0  8
#define NAME_TABLE1  9
#define NAME_TABLE2  10
#define NAME_TABLE3  11

#define NES_DISP_WIDTH      256
#define NES_DISP_HEIGHT     240

/* Game Genie Codes */

struct NES_GameGenie
{
    char code[9];
    word wAddr;
    byte data;
    byte compare;
    byte enabled;
    byte length;
};

/* File header for .NES files */

struct NES_Header
{
  byte ID[4];
  byte ROMSize;
  byte VROMSize;
  byte Info1;
  byte Info2;
  byte Reserve[8];
};

/* NES Emulator State */

struct NES_State {

    /* Options */
    byte FrameSkip;
    byte FrameCnt;
    byte EmulatorRun;
    byte GameGenie;
    byte BassBoost;
    int ClockCycles;

    /* NES Resources */
    byte RAM[ RAM_SIZE ];
    byte SRAM[ SRAM_SIZE ];
    byte DRAM[ DRAM_SIZE ];
    int Clock;
    int ClockFrame;

    /* PPU Resources */
    int SpriteJustHit;          /* Sprite #0 Scanline */
    byte PPURAM[ PPURAM_SIZE ];
    byte SPRRAM[ SPRRAM_SIZE ];
    word PPU_SP_Height;
    word PPU_Scanline;
    word vAddr; /* PPU VRAM Address Pointer */
    word vAddr_Latch;

    /* APU Resources */
    byte APU_Mute;
    byte APU_Reg[ 0x18 ];
    int cur_event;
    byte ApuCtrl;
    byte ApuCtrlNew;
    struct pAPU_State pAPU;

    /* PPU Registers */
    byte PPU_R0;
    byte PPU_R1;
    byte PPU_R2;
    byte PPU_R3;
    byte PPU_R7;
    byte FineX;

    /* Flags */
    byte VRAMWriteEnable;    /* VRAM Write Enabled */
    byte PPU_Latch_Flag;    /* PPU Address and Scroll Latch */
    byte PPU_UpDown_Clip;   /* Up and Down Clipping */
    byte FrameIRQ_Enable;   /* IRQ */
    byte ChrBufUpdate;	    /* Character buffer update */

    /* Display Resources */
    byte ChrBuf[256 * 2 * 8 * 8];   /* Character buffer */
    word PalTable[64];              /* Palette table */
    int FrameStep;
    int CpuLag;
    int RunCycles;
    byte NES_BG_Index_Temp[256];

    /* Display Buffer */
#ifdef DOUBLE_BUFFERING
    word DoubleFrame[ 2 ][ NES_DISP_WIDTH * NES_DISP_HEIGHT ];
    word *WorkFrame;
    word WorkFrameIdx;
#else
    word WorkFrame[ NES_DISP_WIDTH * NES_DISP_HEIGHT ];
#endif

    /* Controller Pad Resources */
    dword PAD1_Latch;
    dword PAD2_Latch;
    dword PAD1_Bit;
    dword PAD2_Bit;

    /* Mirroring Table */
    byte PPU_MirrorTable[6][4];
    byte PPU_LastMirror;

    /* ROM Information */
    struct NES_Header NesHeader;
    byte MapperNo;
    byte MapperIdx;
    byte ROM_Mirroring; /* 0: Horizontal, 1: Vertical */
    byte ROM_SRAM;      /* Has SRAM */
    byte ROM_Trainer;   /* Has Trainer */
    byte ROM_FourScr;   /* Has Four-Screen VRAM */
    byte PAL;		/* 1=PAL, 0=NTSC */
    byte MasterCycleMultiplier;
    word VBlankEndScanline;
    word ScanlinesPerFrame;

    M6502 m6502_state;
    byte PAD_Zapper;
    byte Zapper_Sync;
	
	void *userData;
};

/* NES Emulator Wiring */

struct NES_Wiring {
    byte *ROM;
    byte *SRAMBANK;
    byte *ROMBANK0; /* 8K ROM Banks */
    byte *ROMBANK1;
    byte *ROMBANK2;
    byte *ROMBANK3;
    byte *pbyPrevBank[ 8 ];

    byte *VROM;
    byte *PPUBANK[ 16 ]; /* 1K PPU Banks */
    byte *PPU_BG;   /* Background Memory */
    byte *PPU_SP;   /* Sprite Memory */

    /* Used only for save state / wiring import offsets */
    byte *PPURAM;
    byte *SPRRAM;
    byte *CHRRAM;
    byte *DRAM;
    byte *SRAM;
};

/* Mapper State */

struct NES_Mapper_State {
    void *state;
    unsigned long size;
};

extern struct NES_State S;
extern struct NES_Wiring W;
extern struct NES_Mapper_State M;
extern struct NES_GameGenie GG[GAMEGENIE_CODES];

extern void (*MapperInit)    ();
extern void (*MapperWrite)   (word wAddr, byte bData);
extern void (*MapperSram)    (word wAddr, byte bData);
extern void (*MapperApu)     (word wAddr, byte bData);
extern byte (*MapperReadApu) (word wAddr);
extern void (*MapperVSync)   ( );
extern void (*MapperHSync)   ( );
extern void (*MapperPPU)     (word wAddr);
extern void (*MapperRenderScreen)(byte byMode); /* 1: BG, 0: Sprite */
extern void (*MapperRestore) ( );

#define NAME_TABLE_V_MASK 2
#define NAME_TABLE_H_MASK 1

#define SPR_Y    0
#define SPR_CHR  1
#define SPR_ATTR 2
#define SPR_X    3
#define SPR_ATTR_COLOR  0x3
#define SPR_ATTR_V_FLIP 0x80
#define SPR_ATTR_H_FLIP 0x40
#define SPR_ATTR_PRI    0x20

#define R0_NMI_VB      0x80
#define R0_NMI_SP      0x40
#define R0_SP_SIZE     0x20
#define R0_BG_ADDR     0x10
#define R0_SP_ADDR     0x08
#define R0_INC_ADDR    0x04
#define R0_NAME_ADDR   0x03

#define R1_BACKCOLOR   0xe0
#define R1_SHOW_SP     0x10
#define R1_SHOW_BG     0x08
#define R1_CLIP_SP     0x04
#define R1_CLIP_BG     0x02
#define R1_MONOCHROME  0x01

#define R2_IN_VBLANK   0x80
#define R2_HIT_SP      0x40
#define R2_MAX_SP      0x20
#define R2_WRITE_FLAG  0x10

#define SCAN_TOP_OFF_SCREEN     0
#define SCAN_ON_SCREEN          1
#define SCAN_BOTTOM_OFF_SCREEN  2
#define SCAN_UNKNOWN            3
#define SCAN_VBLANK             4

#define SCAN_TOP_OFF_SCREEN_START       0 
#define SCAN_ON_SCREEN_START            8
#define SCAN_BOTTOM_OFF_SCREEN_START  232
#define SCAN_UNKNOWN_START            240
#define SCAN_VBLANK_START             241


#define NTSC_SCAN_VBLANK_END          261
#define PAL_SCAN_VBLANK_END           311

#define NTSC_CYCLE_MULT 16
#define PAL_CYCLE_MULT  15

#define PAL_CYCLES_PER_FRAME  1595880
#define NTSC_CYCLES_PER_FRAME 1429472

#define PAL_SCANLINES_PER_FRAME  312
#define NTSC_SCANLINES_PER_FRAME 262

#define PAD_SYS_QUIT   0x01
#define PAD_SYS_OK     0x02
#define PAD_SYS_CANCEL 0x04
#define PAD_SYS_UP     0x08
#define PAD_SYS_DOWN   0x10
#define PAD_SYS_LEFT   0x20
#define PAD_SYS_RIGHT  0x40

#define PAD_PUSH(a,b)  (((a) & (b)) != 0)

void NESCore_Init();
void NESCore_Finish();
int  NESCore_Reset();
void NESCore_Init_PPU();
void NESCore_Mirroring(int mType);
void NESCore_Run();
void NESCore_Halt();
void NESCore_Cycle();
int  NESCore_HSync();
void NESCore_DrawScanline();
void NESCore_WriteIndex();
void NESCore_GetSprHitY();
void NESCore_Develop_Character_Data();
int NESCore_Init_GameGenie();
int NESCore_Export_Wiring(struct NES_Wiring *I);
byte *NESCore_WireBank(struct NES_Wiring *I, byte *c, const char *desc);
int NESCore_SaveState(const char *);
int NESCore_LoadState(const char *);
int NESCore_LoadROM(const char *fileName);
int NESCore_Import_Wiring(struct NES_Wiring *I);

/* Return Codes */

#define E_MEMORY_ERR  		-0x02
#define E_INVALID_FILE		-0x01
#define E_INVALID_IMAGE		-0x03
#define E_UNSUPPORTED_MAPPER    -0x04
#define E_MISMATCH		-0x01
#define E_UNINITIALIZED		-0x02
#define E_FILE			-0x01

#define NMI_REQ Int6502(&S.m6502_state, INT_NMI)
#define IRQ_REQ Int6502(&S.m6502_state, INT_IRQ)

#endif /* !__NESCORE_H_ */
