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
 * NESCore.c: The NES Emulator Core Project
 *            Primary NES Emulation
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <unistd.h>

#include "NESCore.h"
#include "NESCore_Callback.h"
#include "NESCore_Mapper.h"
#include "NESCore_pAPU.h"
#include "M6502.h"

struct NES_State S;
struct NES_Wiring W;
struct NES_Mapper_State M;
struct NES_GameGenie GG[GAMEGENIE_CODES];
byte *PPU_ScanTable;
word NESPalette[64];

#ifdef UNIX_WAIT
#include <sys/time.h>
struct timeval ptv;
#endif

void (*MapperInit)    ( );
void (*MapperWrite)   ( word wAddr, byte bData );
void (*MapperSram)    ( word wAddr, byte bData );
void (*MapperApu)     ( word wAddr, byte bData );
byte (*MapperReadApu) ( word wAddr );
void (*MapperVSync)   ( );
void (*MapperHSync)   ( );
void (*MapperPPU)     ( word wAddr );
void (*MapperRenderScreen)( byte byMode ); /* 1: BG, 0: Sprite */
void (*MapperRestore) ( );

/* NESCore_LoadROM(): Load a ROM Image into memory
 *         Syntax: result = NESCore_LoadROM("/path/to/filename.nes")
 *      Arguments: const char *		Path to NES image
 *        Returns: 0			Success
 *                 E_MEMORY_ERR		Memory error (malloc)
 *                 E_INVALID_FILE	Invalid filename
 *                 E_INVALID_IMAGE	Invalid ROM image
 *                 E_UNSUPPORTED_MAPPER	Mapper not supported
 *
 *    Description: NESCore_LoadROM() is a nice POSIX compliant function to
 *                 load a ROM image into memory. 
 */

int NESCore_LoadROM(const char *fileName) {
    FILE *fp;
    int nIdx, mapperSupported = 0;
	
    fp = fopen(fileName, "rb" );
    if (!fp)
        return E_INVALID_FILE;
	
    /* Read ROM Header */
    fread(&S.NesHeader, sizeof(S.NesHeader), 1, fp);
    if (memcmp(S.NesHeader.ID, "NES\x1a", 4 ) != 0 ) {
        /* Not a valid .NES ROM */
        fclose(fp);
        return E_INVALID_IMAGE;
    }
	
    if (S.NesHeader.Info1 & 4)
        fseek(fp, 528, SEEK_SET);
	
    W.ROM = (unsigned char*) malloc(S.NesHeader.ROMSize * 0x4000);
    if (!W.ROM) 
        return E_MEMORY_ERR;
	
    fread(W.ROM, 0x4000, S.NesHeader.ROMSize, fp);
    if (S.NesHeader.VROMSize > 0) {
        W.VROM = (unsigned char *) malloc(S.NesHeader.VROMSize * 0x2000);
        if (!W.VROM) {
            free(W.ROM);
            return E_MEMORY_ERR;
        }
        fread(W.VROM, 0x2000, S.NesHeader.VROMSize, fp);
    } else {
		W.VROM = NULL;
	}
	
    fclose(fp);
	
    S.PAL = (S.NesHeader.Reserve[1] & 0x01) || strstr(fileName, "(E)");
	
    /* Validate Mapper */
    S.MapperNo = S.NesHeader.Info1 >> 4;
    for (nIdx = 4; nIdx < 8 && S.NesHeader.Reserve[nIdx] == 0; ++nIdx)
		;
    if ( nIdx == 8 )
		S.MapperNo |= (S.NesHeader.Info2 & 0xf0);
    for (nIdx = 0; MapperTable[ nIdx ].nMapperNo != -1; ++nIdx)
    {
		if (MapperTable[ nIdx ].nMapperNo == S.MapperNo) {
			mapperSupported = 1;
			break;
		}
    }
	
    if (!mapperSupported)
		return E_UNSUPPORTED_MAPPER;
	
    S.MapperIdx = nIdx;
    return 0;
}

/* NESCore_Callback_Wait(): POSIX Compliant wait function
 *         Syntax: Compile with -DPOSIX_WAIT
 *      Arguments: None
 *        Returns: None
 *
 *    Description: NESCore_Callback_Wait() is a nice POSIX compliant frame
 *                 wait function to automatically adjust execution speed.
 */

#ifdef UNIX_WAIT
void NESCore_Callback_Wait(void *userData) {
    struct timeval tv;
    float frameRate = (S.PAL) ? 53.355 : 60.098;
    signed long usec_per_frame = (1000000 / frameRate) * (S.FrameSkip + 1);
    signed long usec_this_frame;
    signed long delta;
	
    if (gettimeofday(&tv, NULL))
        return;
	
    usec_this_frame = (1000000 * (tv.tv_sec - ptv.tv_sec))
	+ tv.tv_usec
	- ptv.tv_usec;
    if (ptv.tv_sec == 0) {
        gettimeofday(&ptv, NULL);
        return;
    }
    delta = usec_per_frame - usec_this_frame;
    if (delta > 0) {
        tv.tv_sec = delta / 1000000;
        tv.tv_usec = delta % 1000000;
        select(-1, NULL, NULL, NULL, &tv);
        fprintf(stderr, "%s waiting %d usec", __func__, tv.tv_usec);
        if (S.FrameSkip != 0)
            S.FrameSkip--;
    } else {
        if (S.FrameSkip != 4) 
            S.FrameSkip++;
    }
    gettimeofday(&ptv, NULL);
    return;
}
#endif

/* NESCore_SaveState(): Save a game state to file
 *         Syntax: result = NESCore_SaveState("/path/to/filename.sav")
 *      Arguments: const char *         Path to save state file
 *        Returns: 0                    Success
 *                 E_FILE		File write error
 *    Description: NESCore_SaveState() is a nice POSIX compliant function to
 *                 save a game state to disk. It should be called after calling
 *                 NESCore_Halt(), but prior to NESCore_Finish()
 */
int NESCore_SaveState(const char *fn)
{
	FILE *fp;
	
	fp = fopen(fn, "wb");
	if (!fp)
		return E_FILE;
	
	fwrite(&S, sizeof(S), 1, fp);
	fwrite(&W, sizeof(W), 1, fp);
	
	if (M.state && M.size)
		fwrite(M.state, M.size, 1, fp);
	
	fclose(fp);
	return 0;
}

/* NESCore_LoadState(): Load a game state into memory
 *         Syntax: result = NESCore_LoadState("/path/to/filename.sav")
 *      Arguments: const char *         Path to save state file
 *        Returns: 0                    Success
 *                 E_UNINITIALIZED	No game ROM loaded
 *                 E_MISMATCH		Wiring mismatch; wrong game?
 *
 *    Description: NESCore_LoadState() is a nice POSIX compliant function to
 *                 load a game state back into memory. It should be called
 *                 after calling NESCore_Reset(), but prior to NESCore_Run()
 */

int NESCore_LoadState(const char *fn)
{
	struct NES_Wiring I;
	FILE *fp;
	
	fp = fopen(fn, "rb");
	if (!fp)
		return -1;
	
	fread(&S, sizeof(S), 1, fp);
	fread(&I, sizeof(W), 1, fp);
	
	if (M.state && M.size)
		fread(M.state, M.size, 1, fp);
	
	fclose(fp);
	
	NESCore_Import_Wiring(&I);
	NESCore_Mirroring(S.PPU_LastMirror);
	if (M.state && M.size)
		MapperRestore();
	
	return 0;
}

/* NESCore_Init(): Initialize Emulator Core and 6502
 *         Syntax: NESCore_Init()
 *      Arguments: None
 *        Returns: None
 *
 *    Description: NESCore_Init() Should be called only once prior to any
 *                 other emulator calls. 
 */

void NESCore_Init()
{
	memset(&S.m6502_state, 0, sizeof(S.m6502_state));
	
	PPU_ScanTable = NULL;
	S.ClockCycles = 339; /* Should be 341 when we get more accurate */
	
	/* Name Table Mirroring */
	S.PPU_MirrorTable[0][0] = NAME_TABLE0;
	S.PPU_MirrorTable[0][1] = NAME_TABLE0;
	S.PPU_MirrorTable[0][2] = NAME_TABLE1;
	S.PPU_MirrorTable[0][3] = NAME_TABLE1;
	
	S.PPU_MirrorTable[1][0] = NAME_TABLE0;
	S.PPU_MirrorTable[1][1] = NAME_TABLE1;
	S.PPU_MirrorTable[1][2] = NAME_TABLE0;
	S.PPU_MirrorTable[1][3] = NAME_TABLE1;
	
	S.PPU_MirrorTable[2][0] = NAME_TABLE1;
	S.PPU_MirrorTable[2][1] = NAME_TABLE1;
	S.PPU_MirrorTable[2][2] = NAME_TABLE1;
	S.PPU_MirrorTable[2][3] = NAME_TABLE1;
	
	S.PPU_MirrorTable[3][0] = NAME_TABLE0;
	S.PPU_MirrorTable[3][1] = NAME_TABLE0;
	S.PPU_MirrorTable[3][2] = NAME_TABLE0;
	S.PPU_MirrorTable[3][3] = NAME_TABLE0;
	
	S.PPU_MirrorTable[4][0] = NAME_TABLE0;
	S.PPU_MirrorTable[4][1] = NAME_TABLE1;
	S.PPU_MirrorTable[4][2] = NAME_TABLE2;
	S.PPU_MirrorTable[4][3] = NAME_TABLE3;
	
	S.PPU_MirrorTable[5][0] = NAME_TABLE0;
	S.PPU_MirrorTable[5][1] = NAME_TABLE0;
	S.PPU_MirrorTable[5][2] = NAME_TABLE0;
	S.PPU_MirrorTable[5][3] = NAME_TABLE1;
	
	S.FrameSkip = 0;
	S.GameGenie = 0;
	S.CpuLag = 0;
	S.BassBoost = 0;
	S.APU_Mute = 0;
	memset(&GG, 0, sizeof(GG));
#ifdef UNIX_WAIT
	ptv.tv_sec = 0;
#endif
	
	W.PPURAM = S.PPURAM;
	W.SPRRAM = S.SPRRAM;
	W.CHRRAM = S.ChrBuf;
	W.DRAM = S.DRAM;
	W.SRAM = S.SRAM;
	
	M.state = NULL;
	M.size = 0;
}

/* NESCore_Reset(): Reset the Emulator
 *          Syntax: err = NESCore_Reset()
 *       Arguments: NoneN
 *         Returns: 0                     Success
 *                  E_UNSUPPORTED_MAPPER  Unsupported mapper
 *
 *     Description: NESCore_Reset() Resets the emulator and initializes RAM.
 *                  It should be called after a ROM image is loaded, before
 *                  a call to NESCore_Run() is called.
 *
 *                  Subsequent calls are made to these callback functions:
 *
 *                      NESCore_Callback_InitSound()
 *                      NESCore_Callback_OpenSound(nSamplesPerSync, nSampleRate)
 */

int NESCore_Reset()
{
    int nIdx, mapperSupported = 0;
	
    /* Find and Validate Mapper */
    S.MapperNo = S.NesHeader.Info1 >> 4;
    for (nIdx = 4; nIdx < 8 && S.NesHeader.Reserve[nIdx] == 0; ++nIdx) 
		;
    if ( nIdx == 8 ) 
		S.MapperNo |= (S.NesHeader.Info2 & 0xf0);
    for (nIdx = 0; MapperTable[ nIdx ].nMapperNo != -1; ++nIdx)
    {
        if (MapperTable[ nIdx ].nMapperNo == S.MapperNo) {
            mapperSupported = 1;
            break;
        }
    }
	
    if (!mapperSupported)
		return E_UNSUPPORTED_MAPPER;
	
    /* ROM Header Information */
    S.ROM_Mirroring = S.NesHeader.Info1 & 1;
    S.ROM_SRAM      = S.NesHeader.Info1 & 2;
    S.ROM_Trainer   = S.NesHeader.Info1 & 4;
    S.ROM_FourScr   = S.NesHeader.Info1 & 8;
	
    if (S.PAL) {
        S.VBlankEndScanline = PAL_SCAN_VBLANK_END;
        S.ScanlinesPerFrame = PAL_SCANLINES_PER_FRAME;
        S.MasterCycleMultiplier = PAL_CYCLE_MULT;
    } else {
        S.VBlankEndScanline = NTSC_SCAN_VBLANK_END;
        S.ScanlinesPerFrame = NTSC_SCANLINES_PER_FRAME;
        S.MasterCycleMultiplier = NTSC_CYCLE_MULT;
    }
	
    NESCore_Debug("ROM Type: %s", (S.PAL) ? "PAL" : "NTSC");
    NESCore_Debug("Mirroring: %d", S.ROM_Mirroring);
    NESCore_Debug("SRAM: %s", (S.ROM_SRAM) ? "YES" : "NO");
    NESCore_Debug("Four Screen Mirroring: %s", (S.ROM_FourScr) ? "YES" : "NO");
	
    if (S.ROM_FourScr)
        S.ROM_Mirroring = 4;
	
    /* Set up the master clock and scanline table based on ROM type */
	
    S.Clock = S.ClockCycles * S.MasterCycleMultiplier;
    S.ClockFrame = S.Clock * S.ScanlinesPerFrame;
    free(PPU_ScanTable);
    PPU_ScanTable = malloc(S.ScanlinesPerFrame);
    if (!PPU_ScanTable)
        return E_MEMORY_ERR;
	
    /* Initialize Scanline Table */
    for ( nIdx = 0; nIdx < S.ScanlinesPerFrame; ++nIdx ) {
        if ( nIdx < SCAN_ON_SCREEN_START )
            PPU_ScanTable[ nIdx ] = SCAN_ON_SCREEN;
        else if ( nIdx < SCAN_BOTTOM_OFF_SCREEN_START )
            PPU_ScanTable[ nIdx ] = SCAN_ON_SCREEN;
        else if ( nIdx < SCAN_UNKNOWN_START )
            PPU_ScanTable[ nIdx ] = SCAN_ON_SCREEN;
        else if ( nIdx < SCAN_VBLANK_START )
            PPU_ScanTable[ nIdx ] = SCAN_UNKNOWN;
        else
            PPU_ScanTable[ nIdx ] = SCAN_VBLANK;
    }
	
    /* Initialize Resources */
    memset(S.RAM, 0, sizeof S.RAM);
    S.APU_Mute  = 0;
    S.FrameCnt  = 0;
    S.ChrBufUpdate = 0xff;
#ifdef DOUBLE_BUFFERING
    S.WorkFrame = S.DoubleFrame[ 0 ];
    S.WorkFrameIdx = 0;
#endif
	
    memset(S.PalTable, 0, sizeof S.PalTable); /* Palette Table */
    memset(S.APU_Reg, 0, sizeof S.APU_Reg);   /* Sound Registers */
	
    S.PAD1_Latch = S.PAD2_Latch = S.PAD1_Bit = S.PAD2_Bit;
    S.PAD_Zapper = S.Zapper_Sync = 0;
	
    NESCore_Debug("Initializing PPU");
    NESCore_Init_PPU();                /* Picture Processing Unit */
	
    NESCore_Debug("Initializing pAPU");
    if (!S.APU_Mute)
        NESCore_Init_pAPU();           /* Pseudo-Audio Processing Unit */
	
    NESCore_Debug("Initializing mapper %d [Idx %d]", S.MapperNo, S.MapperIdx);
    MapperTable[ S.MapperIdx ].pMapperInit(); /* Memory Mapper */
    NESCore_Debug("Mapper initialized");
	
    NESCore_Debug("Initializing 6502");
    Reset6502(&S.m6502_state);         /* 6502 CPU */
    NESCore_Debug("6502 Initialized");
	
    return 0;
}

/* NESCore_Init_PPU(): Initialize PPU (Picture Processing Unit)
 *             Syntax: NESCore_Init_PPU()
 *          Arguments: None
 *            Returns: None
 *
 *        Description: NESCore_Init_PPU() Initializes the PPU memory and
 *                     registers. It is called automatically when a call to
 *                     NESCore_Reset() is made.
 */

void NESCore_Init_PPU()
{
	int nPage;
	
	memset(S.PPURAM, 0, sizeof S.PPURAM);
	memset(S.SPRRAM, 0, sizeof S.SPRRAM);
	
	S.PPU_R0 
    = S.PPU_R1 
    = S.PPU_R2 
    = S.PPU_R3 
    = S.PPU_R7 = 0;
	S.PPU_Latch_Flag = 0;
	S.PPU_UpDown_Clip = 0;
	S.FrameStep = 0;
	S.FrameIRQ_Enable = 0;
	
	S.vAddr = S.vAddr_Latch = 0;
	S.PPU_LastMirror = 0;
	S.PPU_Scanline = 0;
	S.SpriteJustHit = 0;
	
	/* Reset information on PPU_R0 */
	W.PPU_BG = S.ChrBuf;
	W.PPU_SP = S.ChrBuf + 0x4000;
	S.PPU_SP_Height = 8;
	S.VRAMWriteEnable = (S.NesHeader.VROMSize == 0) ? 1 : 0;
	
	/* Reset PPU Bank Wiring */
	for (nPage = 0; nPage < 16; ++nPage)
		W.PPUBANK[nPage] = &S.PPURAM[nPage * 0x400];
	
	NESCore_Mirroring(S.ROM_Mirroring);
}

/* NESCore_Run(): Begin NESCore Emulation Loop
 *        Syntax: NESCore_Run()
 *     Arguments: None
 *       Returns: None
 *   Description: NESCore_Run() is the main emulation loop, and cycles until
 *                a call to NESCore_Halt(). The run routine will cycle the
 *                emulator and call the necessary callback functions from
 *                your port:
 *                
 *                    NESCore_Callback_OutputFrame()
 *                    NESCore_Callback_OutputSample()
 *                    NESCore_Callback_InputPadState()
 *                    NESCore_Callback_Wait()
 */
void NESCore_Run()
{
	S.EmulatorRun = 0xFF;
	
	while (S.EmulatorRun)
	{
		NESCore_Cycle();
	}
	
	NESCore_pAPU_Finish();
}

/* NESCore_Halt(): Halt Emulation Loop
 *         Syntax: NESCore_Halt()
 *       Arguments: None
 *        Returns: None
 *    Description: NESCore_Halt() instructs the emulator to halt after it has
 *                 finished its current emulation cycle. Calls to NESCore_Halt()
 *                 should be promptly followed by a pthread_join() to close out
 *                 the emulation thread. Upon halt, the following callback
 *                 functions are called:
 *
 *                    NESCore_Callback_CloseSound()
 */

void NESCore_Halt()
{
    S.EmulatorRun = 0;
    usleep(250);
}

/* NESCore_Init_GameGenie(): Initializes Game Genie Codes
 *         Syntax: valid_codes = NESCore_Init_GameGenie()
 *      Arguments: None
 *        Returns: Number of game genie codes accepted
 *
 *    Description: NESCore_Init_GameGenie loads codes written to GG[0-4] and
 *                 converts them to active memory offsets/values. It returns
 *                 the number of valid game genie codes, and iniializes Game
 *                 Genie support if at least one was found.
 */

int NESCore_Init_GameGenie()
{
    int i, valid = 0;
    for( i=0; i<4; i++ ) 
    {
		if (GG[i].code[0] != 0) {
			int j, badcode = 0;
			byte n[8];
			for(j=0;j<strlen(GG[i].code);j++) {
				switch (toupper(GG[i].code[j])) {
					case 'A':
						n[j] = 0x0;
						break;
					case 'P':
						n[j] = 0x1;
						break;
					case 'Z':
						n[j] = 0x2;
						break;
					case 'L':
						n[j] = 0x3;
						break;
					case 'G':
						n[j] = 0x4;
						break;
					case 'I':
						n[j] = 0x5;
						break;
					case 'T':
						n[j] = 0x6;
						break;
					case 'Y':
						n[j] = 0x7;
						break;
					case 'E':
						n[j] = 0x8;
						break;
					case 'O':
						n[j] = 0x9;
						break;
					case 'X':
						n[j] = 0xA;
						break;
					case 'U':
						n[j] = 0xB;
						break;
					case 'K':
						n[j] = 0xC;
						break;
					case 'S':
						n[j] = 0xD;
						break;
					case 'V':
						n[j] = 0xE;
						break;
					case 'N':
						n[j] = 0xF;
						break;
					default:
						badcode = 1;
						continue;
						break;
				}
			}
			
			if (badcode)
				GG[i].enabled = 0;
			else
			{
				valid++;
				GG[i].wAddr =
                0x8000 +
				( ((n[3] & 7) << 12)
				 | ((n[5] & 7) << 8)
				 | ((n[4] & 8) << 8)
				 | ((n[2] & 7) << 4)
				 | ((n[1] & 8) << 4)
				 | (n[4] & 7)
				 | (n[3] & 8) );
				
				if (strlen(GG[i].code) == 6) {
					GG[i].data =
					((n[1] & 7) << 4)
					| ((n[0] & 8) << 4)
					| (n[0] & 7)
					| (n[5] & 8);
					
					GG[i].compare = 0x00;
					GG[i].length = 6;
					GG[i].enabled = 1;
				}
				else
				{
					GG[i].data =
                    ((n[1] & 7) << 4)
					| ((n[0] & 8) << 4)
					| (n[0] & 7)
					| (n[7] & 8);
					
					GG[i].compare =
                    ((n[7] & 7) << 4)
					| ((n[6] & 8) << 4)
					| (n[6] & 7)
					| (n[5] & 8);
					
					GG[i].length = 8;
					GG[i].enabled = 1;
				}
			}
		}
    }
    if (valid) 
        S.GameGenie = 1;
    return valid;
}

/* NESCore_Finish(): Release ROM and any other resources used
 *         Syntax: NESCore_Finish()
 *      Arguments: None
 *        Returns: None
 *
 *    Description: NESCore_Finish should be called after a call to 
 *                 NESCore_Halt(), or after NESCore_SaveState() if saving a
 *                 game. This function releases all remaining resources used.
 */

void NESCore_Finish() {
    free(PPU_ScanTable);
    free(W.ROM);
    free(W.VROM);
}

/* NESCore_Import_Wiring(): Calculates new offsets and rewires a saved wiring
 *         Syntax: ret = NESCore_Import__Wiring(Saved_W)
 *      Arguments: NES_Wiring *: Pointer to saved wiring structure
 *        Returns: 0: Success
 *                 E_MISMATCH: Wiring mismatch; e.g. VROM vs. no VROM
 *                 E_UNINIIALIZED: ROM/VROM has not yet been iniialized
 *                     (call NESCore_LoadROM)
 *                 
 *
 *    Description: NESCore_Import_Wiring() converts an exported game wiring to
 *                 the proper offsets in a new wiring. This allows W to be
 *                 exported with the game state and re-imported again. 
 *                 During play, the game wiring, a series of memory pointers, 
 *                 gets updated frequently. This function converts those 
 *                 pointers to offsets.
 */

int NESCore_Import_Wiring(struct NES_Wiring *I) {
    int i;
	
    if (!W.ROM)
        return E_UNINITIALIZED;
    if ((!W.VROM && I->VROM) || (W.VROM && !I->VROM))
        return E_MISMATCH;
	
    W.SRAMBANK = NESCore_WireBank(I, I->SRAMBANK, "SRAMBANK");
    W.ROMBANK0 = NESCore_WireBank(I, I->ROMBANK0, "ROMBANK0");
    W.ROMBANK1 = NESCore_WireBank(I, I->ROMBANK1, "ROMBANK1");
    W.ROMBANK2 = NESCore_WireBank(I, I->ROMBANK2, "ROMBANK2");
    W.ROMBANK3 = NESCore_WireBank(I, I->ROMBANK3, "ROMBANK3");
	
    for(i=0;i<8;i++) 
		W.pbyPrevBank[i] = NESCore_WireBank(I, I->pbyPrevBank[i], "pbyPrevBank");
	
    for(i=0;i<16;i++)
        W.PPUBANK[i] = NESCore_WireBank(I, I->PPUBANK[i], "PPUBANK"); 
    W.PPU_BG = NESCore_WireBank(I, I->PPU_BG, "PPU_BG");
    W.PPU_SP = NESCore_WireBank(I, I->PPU_SP, "PPU_SP");
    return 0;
};

/* ----------------------- END Public Functions ----------------------- */

/* NESCore_Mirroring(): Configure name table mirroring
 *             Syntax: NESCore_Mirroring(mirrorType)
 *          Arguments: mirrorType: Type of mirroring
 *                     0 : Horizontal
 *                     1 : Vertical
 *                     2 : One Screen 0x2400
 *                     3 : One Screen 0x2000
 *                     4 : Four Screen
 *            Returns: None
 *        Description: NESCore_Mirroring() Rewires PPU banks 8-11 to mirror the
 *                     name tables in the configuration expected by the game.
 */
void NESCore_Mirroring(int mType)
{
	S.PPU_LastMirror = mType;
	W.PPUBANK[NAME_TABLE0] = &S.PPURAM[ S.PPU_MirrorTable[ mType ][ 0 ] * 0x400 ];
	W.PPUBANK[NAME_TABLE1] = &S.PPURAM[ S.PPU_MirrorTable[ mType ][ 1 ] * 0x400 ];
	W.PPUBANK[NAME_TABLE2] = &S.PPURAM[ S.PPU_MirrorTable[ mType ][ 2 ] * 0x400 ];
	W.PPUBANK[NAME_TABLE3] = &S.PPURAM[ S.PPU_MirrorTable[ mType ][ 3 ] * 0x400 ];
}


/* NESCore_Step(): Execute CPU cycles and sync video 
 *         Syntax: NESCore_Step(nCycles)
 *      Arguments: int nStep: Number of CPU cycles to execute
 *        Returns: None 
 *    Description: NESCore_Step() balances the CPU cycles with video HSync and
 *                 back-throttles all calls to balance the CPU. Although we can
 *                 request cycles, sometimes more than that are executed.
 *                 (for example, a DMA write burns 512). 
 */

void NESCore_Step(int nStep) {
    int nExec = nStep - S.CpuLag;
    S.CpuLag = abs(Exec6502(&S.m6502_state, nStep));
    S.FrameStep += (nExec + S.CpuLag);
	
    while(S.CpuLag >= S.Clock) {
        NESCore_HSync();
        S.CpuLag -= S.Clock;
        S.FrameStep += S.Clock;
    }
}

/* NESCore_Cycle(): Emulation loop
 *          Syntax: NESCore_Cycle()
 *       Arguments: None
 *         Returns: None
 *    Description: NESCore_Cycle() instructs the emulator to perform its 
 *                 cycles until it has been told to halt, at which point
 *                 control is returned back to NESCore_Run(). NMI and IRQ
 *                 requests are also serviced here.
 */

void NESCore_Cycle()
{
	for (;;)
	{    
		if ( S.PPU_Scanline == S.SpriteJustHit && 
			PPU_ScanTable[S.PPU_Scanline] == SCAN_ON_SCREEN )
		{
			/* Set sprite hit register flag */
			if ((S.PPU_R1 & R1_SHOW_SP))
				S.PPU_R2 |= R2_HIT_SP;
		}
		
		NESCore_Step(S.Clock);
		
		/* Call H-Sync Mapper Callback */
		MapperHSync();
		
		/* HSync will return -1 if the emulator is told to quit */
		if (NESCore_HSync() == -1)
			return;
		
		/* Audio IRQ */
		if (S.FrameStep > S.ClockFrame && S.FrameIRQ_Enable)
		{
			S.FrameStep %= S.ClockFrame;
			IRQ_REQ;
			S.APU_Reg[0x15] |= 0x40;
		}
	}
}

/* NESCore_HSync(): Perform an HSync
 *          Syntax: NESCore_HSync()
 *       Arguments: None
 *         Returns: 0: Success, -1: Quit Emulation
 *     Description: NESCore_HSync() is responsible for calling the rendering
 *                  for a particular scanline, handling VBLANKs, and detecting
 *                  halt requests.
 */

int NESCore_HSync()
{
	/* Render the CURRENT scanline */
	if ( (S.FrameCnt == 0 || S.Zapper_Sync) && PPU_ScanTable[ S.PPU_Scanline ] == SCAN_ON_SCREEN)
	{
		NESCore_DrawScanline();
	}
	
	/* Operate on NEXT Scanline */
	
	S.PPU_Scanline = (S.PPU_Scanline == S.VBlankEndScanline) ? 0 : S.PPU_Scanline + 1;
	
	if (S.PPU_Scanline == S.VBlankEndScanline) {
		S.PPU_R2 &= ~R2_IN_VBLANK; /* Reset $2002 VBLANK */
	}
	
	switch (S.PPU_Scanline)
	{    
		case SCAN_TOP_OFF_SCREEN:
			S.PPU_R2 &= ~R2_HIT_SP;    /* Reset $2002 Sprite Hit */
			if (S.PPU_R1 & R1_SHOW_BG)
				S.vAddr = S.vAddr_Latch & 0x7FFF;
			
			/* Get scanline of first sprite */
			NESCore_GetSprHitY();
			
			/* Set up character data */
			if ((S.FrameCnt == 0 || S.Zapper_Sync) && S.NesHeader.VROMSize == 0)
				NESCore_Develop_Character_Data();
			break;
			
		case SCAN_UNKNOWN_START:
			/* We output the frame if the background is off for zapper games */
			if (S.FrameCnt == 0 || S.Zapper_Sync)
			{
				/* Calback function for rendering */
				NESCore_Callback_OutputFrame(S.userData, S.WorkFrame, S.FrameCnt);
				
				/* Wait callback (for timing) */
				NESCore_Callback_Wait(S.userData);
				
#ifdef DOUBLE_BUFFERING 
				S.WorkFrameIdx = 1 - S.WorkFrameIdx;
				S.WorkFrame = S.DoubleFrame[ S.WorkFrameIdx ];
#endif
			}
			break;
			
		case SCAN_VBLANK_START:
			S.FrameCnt = (S.FrameCnt >= S.FrameSkip) ? 0 : S.FrameCnt + 1;
			/* Set VBLANK in $2002 register */
			S.PPU_R2 |= R2_IN_VBLANK;
			
			NESCore_pAPU_VSync();
			MapperVSync();
			NESCore_Callback_InputPadState(S.userData, &S.PAD1_Latch, &S.PAD2_Latch);
			
			/* There is a delay between vblank flag being set and NMI */
			S.CpuLag += abs(Exec6502(&S.m6502_state, 12 * S.MasterCycleMultiplier));
			
			/* NMI on VBLANK */
			if (S.PPU_R0 & R0_NMI_VB) 
				NMI_REQ;
			
			if (S.EmulatorRun == 0)
				return -1;
			break;
	}
	
	return 0;
}

/* NESCore_DrawScanline(): Renders a scanline to internal frame buffer
 *          Syntax: NESCore_DrawScanline()
 *       Arguments: None
 *         Returns: None
 *     Description: NESCore_DrawScanline() renders the video output for a single
 *                  scanline to the work buffer. 
 */

void NESCore_DrawScanline()
{
	int nX, nXCache;
	int nY, nYCache;
	int nY4, nYBit;
	
	word *pPalTbl;
	word *pPoint;
	byte *pAttrBase;
	byte *pbyNameTable;
	byte *pbyChrData;
	byte *pSPRRAM;
	byte *bgIndex = S.NES_BG_Index_Temp;
	
	byte bySprCol;
	byte pSprBuf[ NES_DISP_WIDTH + 7 ];
	
	byte nNameTable;
	byte Scroll_Y_Course;
	byte Scroll_Y_Fine;
	byte Scroll_X_Course;
	byte Scroll_X_Fine = 0;
	
	int nAttr, nSprCnt, nIdx, nSprData;
	
	/* Render Background */
	/* ================= */
	
	/* MMC5 VROM Switch */
	MapperRenderScreen(1);
	
	pPoint = &S.WorkFrame[ S.PPU_Scanline * NES_DISP_WIDTH ];
	
	if ( (!(S.PPU_R1 & R1_SHOW_BG)))
	{
		/* Clear scanline if display is off */
		memset(&S.WorkFrame[ S.PPU_Scanline * NES_DISP_WIDTH ], 0, NES_DISP_WIDTH << 1); /* Assumes 16-Bit buffer! */
	} else {
		/* Vertical Increment */
		S.vAddr = (S.vAddr + 0x1000) & 0x7FFF;
		if ((S.vAddr & 0x7000) == 0)
		{
			S.vAddr = (S.vAddr & 0x7C1F) | ((S.vAddr + 0x20) & 0x3E0);
			if ((S.vAddr & 0x3E0) == 0x3C0)
				S.vAddr = ((S.vAddr & 0x7C1F) ^ 0x800);
		}
		
		/* Horizontal Reset: PPU 257 */
		S.vAddr = (S.vAddr & 0xFBE0) | (S.vAddr_Latch & 0x41F);
		
		/* Develop Scroll Values */
		
		if (S.FrameCnt == 0 || S.Zapper_Sync) {
			
			Scroll_Y_Course = (S.vAddr >> 5) & 0x1F;
			Scroll_Y_Fine   = (S.vAddr >> 12) & 0x07;
			Scroll_X_Course = (S.vAddr & 0x1F);
			Scroll_X_Fine   = S.FineX;
			
			nY = nYCache = Scroll_Y_Course;
			nX = nXCache = Scroll_X_Course;
			nY4 = ( ( nY & 0x02 ) << 1 );
			
			nYBit = Scroll_Y_Fine;
			nYBit <<= 3;
			
			nNameTable = NAME_TABLE0 + ((S.vAddr >> 10) & 0x03);
			
			/* Render Block to Left */
			/* -------------------- */
			
			pbyNameTable = W.PPUBANK[ nNameTable ] + (nY << 5) + nX;
			pbyChrData = W.PPU_BG + ( *pbyNameTable << 6 ) + nYBit;
			pAttrBase = W.PPUBANK[ nNameTable ] + 0x3c0 + ((nY >> 2) << 3);
			pPalTbl = &S.PalTable[(((pAttrBase[nX >> 2] >> ((nX & 2) + nY4)) &3) << 2)];
			
			//memcpy(bgIndex, &pbyChrData[Scroll_X_Fine], 8 - Scroll_X_Fine);
			for ( nIdx = Scroll_X_Fine; nIdx < 8; ++nIdx ) {
				*( pPoint++ ) = pPalTbl[ pbyChrData[ nIdx ] ];
				*( bgIndex++ ) = pbyChrData[ nIdx ];
			}
			//bgIndex += (8 - Scroll_X_Fine);
			
			MapperPPU(PATTBL(pbyChrData));
			
			++nX;
			++pbyNameTable;
			
			/* Render Left Table */
			/* ----------------- */
			
			for (; nX < 32; ++nX )
			{
				pbyChrData = W.PPU_BG + (*pbyNameTable << 6 ) + nYBit;
				pPalTbl = &S.PalTable[(((pAttrBase[nX>>2]>> ((nX & 2) + nY4)) & 3) << 2)];
				//memcpy(bgIndex, pbyChrData, 8);
				pPoint[ 0 ] = pPalTbl[ pbyChrData[ 0 ] ]; 
				pPoint[ 1 ] = pPalTbl[ pbyChrData[ 1 ] ];
				pPoint[ 2 ] = pPalTbl[ pbyChrData[ 2 ] ];
				pPoint[ 3 ] = pPalTbl[ pbyChrData[ 3 ] ];
				pPoint[ 4 ] = pPalTbl[ pbyChrData[ 4 ] ];
				pPoint[ 5 ] = pPalTbl[ pbyChrData[ 5 ] ];
				pPoint[ 6 ] = pPalTbl[ pbyChrData[ 6 ] ];
				pPoint[ 7 ] = pPalTbl[ pbyChrData[ 7 ] ];
				bgIndex[ 0 ] = pbyChrData[ 0 ]; 
				bgIndex[ 1 ] = pbyChrData[ 1 ];
				bgIndex[ 2 ] = pbyChrData[ 2 ];
				bgIndex[ 3 ] = pbyChrData[ 3 ];
				bgIndex[ 4 ] = pbyChrData[ 4 ];
				bgIndex[ 5 ] = pbyChrData[ 5 ];
				bgIndex[ 6 ] = pbyChrData[ 6 ];
				bgIndex[ 7 ] = pbyChrData[ 7 ];
				pPoint += 8;
				bgIndex += 8;
				MapperPPU(PATTBL(pbyChrData));
				++pbyNameTable;
			}
			
		} /* FrameCnt == 0 */
		
		S.vAddr ^= 0x400; 
		
		if (S.FrameCnt == 0 || S.Zapper_Sync) {
			
			/* Horizontal Mirror */
			nNameTable = NAME_TABLE0 + ((S.vAddr >> 10) & 0x03);
			pbyNameTable = W.PPUBANK[ nNameTable ] + ( nYCache << 5);
			pAttrBase = W.PPUBANK[ nNameTable ] + 0x3c0 + (( nYCache >> 2 ) << 3);
			
			/* Render Right Table */
			/* ------------------ */
			
			for ( nX = 0; nX < Scroll_X_Course; ++nX )
			{
				pbyChrData = W.PPU_BG + (*pbyNameTable << 6) + nYBit;
				pPalTbl = &S.PalTable[(((pAttrBase[nX >> 2] >> ((nX & 2)+ nY4)) & 3)<<2)];
				//memcpy(bgIndex, pbyChrData, 8);
				pPoint[ 0 ] = pPalTbl[ pbyChrData[ 0 ] ]; 
				pPoint[ 1 ] = pPalTbl[ pbyChrData[ 1 ] ];
				pPoint[ 2 ] = pPalTbl[ pbyChrData[ 2 ] ];
				pPoint[ 3 ] = pPalTbl[ pbyChrData[ 3 ] ];
				pPoint[ 4 ] = pPalTbl[ pbyChrData[ 4 ] ];
				pPoint[ 5 ] = pPalTbl[ pbyChrData[ 5 ] ];
				pPoint[ 6 ] = pPalTbl[ pbyChrData[ 6 ] ];
				pPoint[ 7 ] = pPalTbl[ pbyChrData[ 7 ] ];
				bgIndex[ 0 ] = pbyChrData[ 0 ]; 
				bgIndex[ 1 ] = pbyChrData[ 1 ];
				bgIndex[ 2 ] = pbyChrData[ 2 ];
				bgIndex[ 3 ] = pbyChrData[ 3 ];
				bgIndex[ 4 ] = pbyChrData[ 4 ];
				bgIndex[ 5 ] = pbyChrData[ 5 ];
				bgIndex[ 6 ] = pbyChrData[ 6 ];
				bgIndex[ 7 ] = pbyChrData[ 7 ];
				pPoint += 8;
				bgIndex += 8;
				
				MapperPPU( PATTBL( pbyChrData ) );
				++pbyNameTable;
			}
			
			/* Render Block to Right */
			/* --------------------- */
			
			pbyChrData = W.PPU_BG + ( *pbyNameTable << 6 ) + nYBit;
			pPalTbl = &S.PalTable[(((pAttrBase[nX >> 2] >> ((nX & 2) + nY4)) & 3)<< 2)];
			//memcpy(bgIndex, pbyChrData, Scroll_X_Fine);
			for ( nIdx = 0; nIdx < Scroll_X_Fine; ++nIdx ) {
				pPoint[ nIdx ] = pPalTbl[ pbyChrData[ nIdx ] ];
				bgIndex[ nIdx ] = pbyChrData[ nIdx ];  
			}
			
			MapperPPU(PATTBL(pbyChrData));
			
			/* Background Clipping */
			if ( !( S.PPU_R1 & R1_CLIP_BG ) )
			{
				word *pPointTop;
				pPointTop = &S.WorkFrame[ S.PPU_Scanline * NES_DISP_WIDTH ];
				memset(pPointTop, 0, 16);
			}
			
			/* Clear a scanline if Up/Down clipping flag is set */
			if (S.PPU_UpDown_Clip && 
				(SCAN_ON_SCREEN_START > S.PPU_Scanline || S.PPU_Scanline > SCAN_BOTTOM_OFF_SCREEN_START))
			{
				word *pPointTop;
				
				pPointTop = &S.WorkFrame[ S.PPU_Scanline * NES_DISP_WIDTH ];
				memset( pPointTop, 0, NES_DISP_WIDTH << 1 );
			}  
			
		} /* FrameCnt == 0 */
		
	}
	
	/* Render Sprites */
	/* ============== */
	
	/* MMC5 VROM switch */
	MapperRenderScreen( 0 );
	if ((S.PPU_R1 & R1_SHOW_SP))
	{
		/* Reset scanline sprite count */
		S.PPU_R2 &= ~R2_MAX_SP;
		memset( pSprBuf, 0, sizeof pSprBuf );
		
		nSprCnt = 0;
		for ( pSPRRAM = S.SPRRAM + (252); pSPRRAM >= S.SPRRAM; pSPRRAM -= 4 ) {
			nY = pSPRRAM[ SPR_Y ] + 1;
			if ( nY > S.PPU_Scanline || nY + S.PPU_SP_Height <= S.PPU_Scanline )
				continue;  /* Next Sprite */
			
			/* Holizontal Sprite Count +1 */
			++nSprCnt;
			
			nAttr = pSPRRAM[SPR_ATTR];
			nYBit = S.PPU_Scanline - nY;
			nYBit = (nAttr & SPR_ATTR_V_FLIP) ? (S.PPU_SP_Height - nYBit - 1) << 3 : nYBit << 3;
			
			if ( S.PPU_R0 & R0_SP_SIZE )
			{
				/* Sprite size 8x16 */
				if (pSPRRAM[ SPR_CHR ] & 1)
				{
					pbyChrData = S.ChrBuf + 0x4000
					+ ((pSPRRAM[ SPR_CHR ] & 0xfe ) << 6) + nYBit;
				}
				else
				{
					pbyChrData = S.ChrBuf 
					+ ( ( pSPRRAM[ SPR_CHR ] & 0xfe ) << 6 ) + nYBit;
				}
			}
			else
			{
				/* Sprite size 8x8 */
				pbyChrData = W.PPU_SP + ( pSPRRAM[ SPR_CHR ] << 6 ) + nYBit;
			}
			
			bySprCol = (nAttr & ( SPR_ATTR_COLOR | SPR_ATTR_PRI )) << 2;
			
			nX = pSPRRAM[ SPR_X ];
			
			if ( nAttr & SPR_ATTR_H_FLIP )
			{
				/* Horizontal Flip */
				if ( pbyChrData[ 7 ] )
					pSprBuf[nX]     = bySprCol | pbyChrData[7];
				if ( pbyChrData[ 6 ] )
					pSprBuf[nX + 1] = bySprCol | pbyChrData[6];
				if ( pbyChrData[ 5 ] )
					pSprBuf[nX + 2] = bySprCol | pbyChrData[5];
				if ( pbyChrData[ 4 ] )
					pSprBuf[nX + 3] = bySprCol | pbyChrData[4];
				if ( pbyChrData[ 3 ] )
					pSprBuf[nX + 4] = bySprCol | pbyChrData[3];
				if ( pbyChrData[ 2 ] )
					pSprBuf[nX + 5] = bySprCol | pbyChrData[2];
				if ( pbyChrData[ 1 ] )
					pSprBuf[nX + 6] = bySprCol | pbyChrData[1];
				if ( pbyChrData[ 0 ] )
					pSprBuf[nX + 7] = bySprCol | pbyChrData[0];
			}
			else
			{
				/* No Flip */
				if ( pbyChrData[ 0 ] )
					pSprBuf[nX]     = bySprCol | pbyChrData[0];
				if ( pbyChrData[ 1 ] )
					pSprBuf[nX + 1] = bySprCol | pbyChrData[1];
				if ( pbyChrData[ 2 ] )
					pSprBuf[nX + 2] = bySprCol | pbyChrData[2];
				if ( pbyChrData[ 3 ] )
					pSprBuf[nX + 3] = bySprCol | pbyChrData[3];
				if ( pbyChrData[ 4 ] )
					pSprBuf[nX + 4] = bySprCol | pbyChrData[4];
				if ( pbyChrData[ 5 ] )
					pSprBuf[nX + 5] = bySprCol | pbyChrData[5];
				if ( pbyChrData[ 6 ] )
					pSprBuf[nX + 6] = bySprCol | pbyChrData[6];
				if ( pbyChrData[ 7 ] )
					pSprBuf[nX + 7] = bySprCol | pbyChrData[7];
			}
		}
		
		/* Render the sprite */
		pPoint -= ( NES_DISP_WIDTH - Scroll_X_Fine );
		for ( nX = 0; nX < NES_DISP_WIDTH; ++nX )
		{
			nSprData = pSprBuf[ nX ];
			
			if ( nSprData  && ( !(nSprData & 0x80) || ((S.NES_BG_Index_Temp[nX] & 3) == 0)))
			{
					pPoint[ nX ] = S.PalTable[ ( nSprData & 0xf ) + 0x10 ];
			}
		}
		
		/* Sprite Clipping */
		if (!( S.PPU_R1 & R1_CLIP_SP ))
		{
			word *pPointTop;
			pPointTop = &S.WorkFrame[ S.PPU_Scanline * NES_DISP_WIDTH ];
			memset( pPointTop, 0, 16 );
		}
		
		if ( nSprCnt >= 8 )
			S.PPU_R2 |= R2_MAX_SP;  /* Set flag for max sprites on scanline */
	}
}
/* NESCore_GetSprHitY(): Get position of Sprite #0
 *          Syntax: NESCore_GetSprHitY()
 *       Arguments: None
 *         Returns: None
 *     Description: NESCore_GetSprHitY() calculates the position of
 *                  sprite #0, which is used by NESCore_Step() to calculate
 *                  the number of cpu steps to reach it.
 */

void NESCore_GetSprHitY()
{
	int nYBit;
	dword *pdwChrData;
	int nOff;
	
	if (S.SPRRAM[ SPR_ATTR ] & SPR_ATTR_V_FLIP)
	{
		/* Vertical Flip */
		nYBit = ( S.PPU_SP_Height - 1 ) << 3;
		nOff = -2;
	}
	else
	{
		nYBit = 0;
		nOff = 2;
	}
	
	if (S.PPU_R0 & R0_SP_SIZE)
	{
		/* Sprite size 8x16 */
		if (S.SPRRAM[ SPR_CHR ] & 0x01)
		{
			pdwChrData = (dword *)(S.ChrBuf + 0x4000
								   + ((S.SPRRAM[ SPR_CHR ] & 0xfe) << 6) + nYBit);
		}
		else
		{
			pdwChrData = (dword *)(S.ChrBuf 
								   + ((S.SPRRAM[ SPR_CHR ] & 0xfe) << 6) + nYBit);
		}
	} else {
		/* Sprite size 8x8 */
		pdwChrData = (dword *)(W.PPU_SP + (S.SPRRAM[ SPR_CHR ] << 6) + nYBit);
	}
	
	if ((S.SPRRAM[ SPR_Y ] + 1 <= SCAN_UNKNOWN_START) && (S.SPRRAM[SPR_Y] > 0))
	{
		int nLine;
		for ( nLine = 0; nLine < S.PPU_SP_Height; nLine++ )
		{
			if ((word)pdwChrData[ 0 ] | (word)pdwChrData[ 1 ])
			{
				/* Scanline Hits Sprite #0 */
				S.SpriteJustHit = S.SPRRAM[SPR_Y] + nLine + 1;
				nLine = S.VBlankEndScanline;
			}
			pdwChrData += nOff;
		}
	} else {
		/* Scanline didn't hit sprite #0 */
		S.SpriteJustHit = SCAN_UNKNOWN_START + 1;
	}
	
}


/* NESCore_Develop_Character_Data(): Develop character data
 *          Syntax: NESCore_Develop_Character_Data()
 *       Arguments: None
 *         Returns: None
 *     Description: NESCore_Develop_Character_Data() reads character data from 
 *                  VROM and copies it to character RAM for future use in 
 *                  rendering.
 */

void NESCore_Develop_Character_Data()
{
	byte *pbyBGData;
	byte bData1;
	byte bData2;
	int nIdx;
	int nY;
	int nOff;
	int nBank;
	
	for (nBank = 0; nBank < 8; ++nBank)
	{
		if ( W.pbyPrevBank[ nBank ] == W.PPUBANK[ nBank ] 
			&& ! ((S.ChrBufUpdate >> nBank) & 1) )
		{
			continue;
		}
		
		/* An address has changed, or an update flag has been set */
		
		for ( nIdx = 0; nIdx < 64; ++nIdx )
		{
			nOff = ( nBank << 12 ) + ( nIdx << 6 );
			for ( nY = 0; nY < 8; ++nY )
			{
				pbyBGData = W.PPUBANK[ nBank ] + ( nIdx << 4 ) + nY;
				
				bData1 = (( pbyBGData[ 0 ] >> 1 ) & 0x55) | (pbyBGData[ 8 ] & 0xAA);
				bData2 = (pbyBGData[ 0 ] & 0x55 ) | (( pbyBGData[ 8 ] << 1 ) & 0xAA);
				
				S.ChrBuf[ nOff ]     = ( bData1 >> 6 ) & 3;
				S.ChrBuf[ nOff + 1 ] = ( bData2 >> 6 ) & 3;
				S.ChrBuf[ nOff + 2 ] = ( bData1 >> 4 ) & 3;
				S.ChrBuf[ nOff + 3 ] = ( bData2 >> 4 ) & 3;
				S.ChrBuf[ nOff + 4 ] = ( bData1 >> 2 ) & 3;
				S.ChrBuf[ nOff + 5 ] = ( bData2 >> 2 ) & 3;
				S.ChrBuf[ nOff + 6 ] = bData1 & 3;
				S.ChrBuf[ nOff + 7 ] = bData2 & 3;
				
				nOff += 8;
			}
		}
		W.pbyPrevBank[ nBank ] = W.PPUBANK[ nBank ];
	}
	
	S.ChrBufUpdate = 0;
}

/* NESCore_WireBank(): Wire a memory address to a specific bank and offset
 *          Syntax: offset = NESCore_WireBank(&I, ptr, "Description")
 *       Arguments: struct NES_Wiring *	Pointer to wiring being imported
 *                  byte *c		Import address to be wired
 *                  const char *desc	Description (for debugging only)
 *         Returns: Pointer to the address in newly wired memory
 *     Description: NESCore_WireBank() is used to wire an old NES_Wiring
 *                  structure to a new one. It is called exclusively by 
 *                  NESCore_Import_Wiring() and corrects the pointer offsets of
 *                  the old wiring structure to match the new one. Since memory
 *                  changes every time the process is resarted, this wiring
 *                  allows the wiring structure to be re-used.
 */

byte *NESCore_WireBank(struct NES_Wiring *I, byte *c, const char *desc) {
    signed long dROM, dVROM, dSPRRAM, dPPURAM, dCHRRAM, dDRAM, dSRAM;
    
    dROM = W.ROM - I->ROM;
    dVROM = W.VROM - I->VROM;
    dPPURAM = W.PPURAM - I->PPURAM;
    dSPRRAM = W.SPRRAM - I->SPRRAM;
    dCHRRAM = W.CHRRAM - I->CHRRAM;
    dDRAM = W.DRAM - I->DRAM;
    dSRAM = W.SRAM - I->SRAM;
    
    if (c >= I->ROM && c < I->ROM + (S.NesHeader.ROMSize * 0x4000))
    {
        NESCore_Debug("%s -> ROM 0x%04x", desc, c - I->ROM); 
        return c + dROM;
    }   
    if (c >= I->VROM && c < I->VROM + (S.NesHeader.VROMSize * 0x2000))
    {
        NESCore_Debug("%s -> VROM 0x%04x", desc, c - I->VROM);
        return c + dVROM;
    }   
    if (c >= I->PPURAM && c < I->PPURAM + PPURAM_SIZE)
    {       
        NESCore_Debug("%s -> PPURAM 0x%04x", desc, c - I->PPURAM);
        return c + dPPURAM;
    }
    if (c >= I->SPRRAM && c < I->SPRRAM + SPRRAM_SIZE)
    {
        NESCore_Debug("%s -> SPRRAM 0x%04x", desc, c - I->SPRRAM);
        return c + dSPRRAM;
    }
    if (c >= I->CHRRAM && c < I->CHRRAM + sizeof(S.ChrBuf))
    {      
        NESCore_Debug("%s -> CHRRAM 0x%04x", desc, c - I->CHRRAM);
        return c + dCHRRAM;
    }
    if (c >= I->DRAM && c < I->DRAM + DRAM_SIZE)
    {
        NESCore_Debug("%s -> DRAM 0x%04x", desc, c - I->DRAM);
        return c + dDRAM;
    }
    if (c >= I->SRAM && c < I->SRAM + SRAM_SIZE)
    {
        NESCore_Debug("%s -> SRAM 0x%04x", desc, c - I->SRAM);
        return c + dSRAM;
    }
	
    NESCore_Debug("%s -> UNKNOWN!", desc);
	
    return NULL;
}

/* Stub for M6502: We don't handle any invalid opcodes */

byte Patch6502(register byte Op,register M6502 *R) { return 0; }
