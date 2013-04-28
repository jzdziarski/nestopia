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
 * NESCore_pAPU.h: Pseudo Audio Processing Unit Prototypes
 *                 The pAPU is responsible for all sound generation.
 */

#ifndef NESCORE_PAPU_H
#define NESCORE_PAPU_H

#define APU_EVENT_MAX   15000

struct ApuEvent_t {
  long time;
  byte type;
  byte data;
};

struct pAPU_State {

    struct ApuEvent_t ApuEventQueue[ APU_EVENT_MAX ];

    /* Rectangle Wave 1 Resources */

    byte ApuC1a, ApuC1b, ApuC1c, ApuC1d;
    byte* ApuC1Wave;
    dword ApuC1Skip;
    dword ApuC1Index;

    byte ApuC1EnvEnabled;
    byte ApuC1EnvLoop;
    byte ApuC1EnvReset;
    int ApuC1EnvPeriod;
    int ApuC1EnvDelay;
    int ApuC1EnvVol;
    int ApuC1Volume;
    
    byte ApuC1Atl;
    sdword ApuC1SweepPhase;
    dword ApuC1Freq;

    /* Rectangle Wave 2 Resources */

    byte ApuC2a, ApuC2b, ApuC2c, ApuC2d;
    byte* ApuC2Wave;
    dword ApuC2Skip;
    dword ApuC2Index;
    byte ApuC2Atl;
    sdword ApuC2SweepPhase;
    dword ApuC2Freq;

    byte ApuC2EnvEnabled;
    byte ApuC2EnvLoop;
    byte ApuC2EnvReset;
    int ApuC2EnvPeriod;
    int ApuC2EnvDelay;
    int ApuC2EnvVol;
    int ApuC2Volume;

    /* Triangle Wave Resources */

    byte ApuC3a, ApuC3b, ApuC3c, ApuC3d;
    dword ApuC3Skip;
    dword ApuC3Index;
    byte  ApuC3Atl;
    dword ApuC3Llc;                             /* Linear Length Counter */
    byte  ApuC3WriteLatency;
    byte  ApuC3CounterStarted;

    /* Noise Resources */

    byte ApuC4a, ApuC4b, ApuC4c, ApuC4d;
    dword ApuC4Sr;                                     /* Shift register */
    dword ApuC4Fdc;                          /* Frequency divide counter */
    dword ApuC4Skip;
    dword ApuC4Index;
    byte  ApuC4Atl;

    byte ApuC4EnvEnabled;
    byte ApuC4EnvLoop;
    byte ApuC4EnvReset;
    int ApuC4EnvPeriod;
    int ApuC4EnvDelay;
    int ApuC4EnvVol;
    int ApuC4Volume;
    int ApuVolumeDivider;

    /* DPCM Resources */

    byte  ApuC5Reg[4];
    byte  ApuC5Enable;
    byte  ApuC5Looping;
    byte  ApuC5CurByte;
    byte  ApuC5DpcmValue;
    int   ApuC5Freq;
    int   ApuC5Phaseacc;
    word  ApuC5Address, ApuC5CacheAddr;
    int   ApuC5DmaLength, ApuC5CacheDmaLength, ApuC5Bytes;
    int ApuC5Zero;
};

/* Rectangle Wave #0 Macros */

#define MC1Vol            ( S.pAPU.ApuC1a & 0x0F )
#define MC1Env            ( S.pAPU.ApuC1a & 0x10 )
#define MC1Hold           ( S.pAPU.ApuC1a & 0x20 )
#define MC1DutyCycle      ( S.pAPU.ApuC1a & 0xc0 )
#define MC1EnvDecay       ( ( S.pAPU.ApuC1a & 0x0F ) )
#define MC1SweepOn        ( S.pAPU.ApuC1b & 0x80 )
#define MC1SweepIncDec    ( S.pAPU.ApuC1b & 0x08 )
#define MC1SweepShifts    ( S.pAPU.ApuC1b & 0x07 ) 
#define MC1SweepDelay     ( ( ( (word) S.pAPU.ApuC1b & 0x70 ) >> 4 ) << 10)
#define MC1FreqLimit      ( ApuFreqLimit[ (S.pAPU.ApuC1b & 0x07) ] )

/* Rectangle Wave #1 Macros */
#define MC2Vol            ( S.pAPU.ApuC2a & 0x0F )
#define MC2Env            ( S.pAPU.ApuC2a & 0x10 )
#define MC2Hold           ( S.pAPU.ApuC2a & 0x20 )
#define MC2DutyCycle      ( S.pAPU.ApuC2a & 0xc0 )
#define MC2EnvDecay       ( ( S.pAPU.ApuC2a & 0x0F ) )
#define MC2SweepOn        ( S.pAPU.ApuC2b & 0x80 )
#define MC2SweepIncDec    ( S.pAPU.ApuC2b & 0x08 )
#define MC2SweepShifts    ( S.pAPU.ApuC2b & 0x07 ) 
#define MC2SweepDelay     ( ( ( (word) S.pAPU.ApuC2b & 0x70 ) >> 4 ) << 10)
#define MC2FreqLimit      ( ApuFreqLimit[ ( S.pAPU.ApuC2b & 0x07 ) ] )

/* Triangle Wave Macros */
#define MC3Holdnote       ( S.pAPU.ApuC3a & 0x80 )
#define MC3LinearLength   ( ( (word)S.pAPU.ApuC3a & 0x7f ) << 6 )
#define MC3LengthCounter  ( ApuAtl[ ( ( S.pAPU.ApuC3d & 0xf8) >> 3 ) ] )
#define MC3Freq           ( ( ( (word)S.pAPU.ApuC3d & 0x07) << 8) + S.pAPU.ApuC3c )

/* White Noise Macros */
#define MC4Vol            ( S.pAPU.ApuC4a & 0x0F )
#define MC4Env            ( S.pAPU.ApuC4a & 0x10 )
#define MC4Hold           ( S.pAPU.ApuC4a & 0x20 )
#define MC4Freq           ( ApuNoiseFreq [ (S.pAPU.ApuC4c & 0x0f) ])
#define MC4Small          ( S.pAPU.ApuC4c & 0x80 )
#define MC4LengthCounter  ( ApuAtl[ ( S.pAPU.ApuC4d >> 3 ) ] << 1 )

#define APUET_MASK      0xfc
#define APUET_C1        0x00
#define APUET_W_C1A     0x00
#define APUET_W_C1B     0x01
#define APUET_W_C1C     0x02
#define APUET_W_C1D     0x03
#define APUET_C2        0x04
#define APUET_W_C2A     0x04
#define APUET_W_C2B     0x05
#define APUET_W_C2C     0x06
#define APUET_W_C2D     0x07
#define APUET_C3        0x08
#define APUET_W_C3A     0x08
#define APUET_W_C3B     0x09
#define APUET_W_C3C     0x0a
#define APUET_W_C3D     0x0b
#define APUET_C4        0x0c
#define APUET_W_C4A     0x0c
#define APUET_W_C4B     0x0d
#define APUET_W_C4C     0x0e
#define APUET_W_C4D     0x0f
#define APUET_C5        0x10
#define APUET_W_C5A     0x10
#define APUET_W_C5B     0x11
#define APUET_W_C5C     0x12
#define APUET_W_C5D     0x13
#define APUET_W_CTRL    0x20
#define APUET_SYNC      0x40

typedef void (*ApuWritefunc)(word addr, byte value);
extern ApuWritefunc pAPUSoundRegs[20];
void ApuWriteControl(word addr, byte value);

#define NESCore_pAPUWriteControl(addr,value) \
{ \
  ApuWriteControl(addr,value); \
}

void NESCore_Init_pAPU(void);
void NESCore_pAPU_Finish(void);
void NESCore_pAPU_VSync(void);

/* pAPU Quality: Configures sample rate for playback */
/* 1 = 11015 Hz. */
/* 2 = 22050 Hz. */
/* 3 = 44100 Hz. */

#define pAPU_QUALITY 3
extern int ApuQuality;

#endif /* NESCORE_PAPU_H */
