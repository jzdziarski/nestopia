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
 * NESCore_pAPU.c: Pseudo Audio Processing Unit
 *                 The pAPU is responsible for all sound generation. 
 */

#include <stdio.h>
#include <string.h>

#include "M6502.h"
#include "M6502_rw.h"
#include "NESCore_Callback.h"
#include "NESCore.h"

#define APU_WRITEFUNC(name, evtype) \
void ApuWrite##name(word addr, byte value) \
{ \
  S.pAPU.ApuEventQueue[S.cur_event].type = APUET_W_##evtype; \
  S.pAPU.ApuEventQueue[S.cur_event].data = value; \
  S.cur_event++; \
  if (S.cur_event == APU_EVENT_MAX) \
      S.cur_event = 0; \
}

APU_WRITEFUNC(C1a, C1A);
APU_WRITEFUNC(C1b, C1B);
APU_WRITEFUNC(C1c, C1C);
APU_WRITEFUNC(C1d, C1D);

APU_WRITEFUNC(C2a, C2A);
APU_WRITEFUNC(C2b, C2B);
APU_WRITEFUNC(C2c, C2C);
APU_WRITEFUNC(C2d, C2D);

APU_WRITEFUNC(C3a, C3A);
APU_WRITEFUNC(C3b, C3B);
APU_WRITEFUNC(C3c, C3C);
APU_WRITEFUNC(C3d, C3D);

APU_WRITEFUNC(C4a, C4A);
APU_WRITEFUNC(C4b, C4B);
APU_WRITEFUNC(C4c, C4C);
APU_WRITEFUNC(C4d, C4D);

APU_WRITEFUNC(C5a, C5A);
APU_WRITEFUNC(C5b, C5B);
APU_WRITEFUNC(C5c, C5C);
APU_WRITEFUNC(C5d, C5D);

APU_WRITEFUNC(Control, CTRL);

ApuWritefunc pAPUSoundRegs[20] = 
{
  ApuWriteC1a,
  ApuWriteC1b,
  ApuWriteC1c,
  ApuWriteC1d,
  ApuWriteC2a,
  ApuWriteC2b,
  ApuWriteC2c,
  ApuWriteC2d,
  ApuWriteC3a,
  ApuWriteC3b,
  ApuWriteC3c,
  ApuWriteC3d,
  ApuWriteC4a,
  ApuWriteC4b,
  ApuWriteC4c,
  ApuWriteC4d,
  ApuWriteC5a,
  ApuWriteC5b,
  ApuWriteC5c,
  ApuWriteC5d,
};

#include "NESCore_pAPU.h"

/* Wave out */

byte wave_buffers[5][735];      /* 44100 / 60 = 735 samples per sync */
byte pcm_buffer[735];

/* APU Quality Resources */

int ApuQuality;
dword ApuPulseMagic;
dword ApuTriangleMagic;
dword ApuNoiseMagic;
unsigned int ApuSamplesPerSync;
unsigned int ApuCyclesPerSample;
unsigned int ApuSampleRate;
dword ApuCycleRate;

struct ApuQualityData_t 
{
  dword pulse_magic;
  dword triangle_magic;
  dword noise_magic;
  unsigned int samples_per_sync;
  unsigned int cycles_per_sample;
  unsigned int sample_rate;
  dword cycle_rate;
} ApuQual[] = {
  { 0xa2567000, 0xa2567000, 0xa2567000, 183, 164, 11025, 1062658 },
  { 0x512b3800, 0x512b3800, 0x512b3800, 367,  82, 22050, 531329 },
  { 0x289d9c00, 0x289d9c00, 0x289d9c00, 735,  41, 44100, 265664 },
};

/* Wave Data */

byte pulse_25[0x20] = {
  0x11, 0x11, 0x11, 0x11,
  0x11, 0x11, 0x11, 0x11,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
};

byte pulse_50[0x20] = {
  0x11, 0x11, 0x11, 0x11,
  0x11, 0x11, 0x11, 0x11,
  0x11, 0x11, 0x11, 0x11,
  0x11, 0x11, 0x11, 0x11,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
};

byte pulse_75[0x20] = {
  0x11, 0x11, 0x11, 0x11,
  0x11, 0x11, 0x11, 0x11,
  0x11, 0x11, 0x11, 0x11,
  0x11, 0x11, 0x11, 0x11,
  0x11, 0x11, 0x11, 0x11,
  0x11, 0x11, 0x11, 0x11,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
};

byte pulse_87[0x20] = {
  0x11, 0x11, 0x11, 0x11,
  0x11, 0x11, 0x11, 0x11,
  0x11, 0x11, 0x11, 0x11,
  0x11, 0x11, 0x11, 0x11,
  0x11, 0x11, 0x11, 0x11,
  0x11, 0x11, 0x11, 0x11,
  0x11, 0x11, 0x11, 0x11,
  0x00, 0x00, 0x00, 0x00,
};

byte triangle_50[0x20] = {
  0x00, 0x10, 0x20, 0x30,
  0x40, 0x50, 0x60, 0x70,
  0x80, 0x90, 0xa0, 0xb0,
  0xc0, 0xd0, 0xe0, 0xf0,
  0xff, 0xef, 0xdf, 0xcf,
  0xbf, 0xaf, 0x9f, 0x8f,
  0x7f, 0x6f, 0x5f, 0x4f,
  0x3f, 0x2f, 0x1f, 0x0f,
};

byte *pulse_waves[4] = {
  pulse_87, pulse_75, pulse_50, pulse_25,
};

/*  Active Time Left Data  */
byte ApuAtl[0x20] = 
{
  5, 127, 10, 1, 19,  2, 40,  3, 80,  4, 30,  5, 7,  6, 13,  7,
  6,   8, 12, 9, 24, 10, 48, 11, 96, 12, 36, 13, 8, 14, 16, 15,
};

/* Frequency Limit of Rectangle Channels */
word ApuFreqLimit[8] = 
{
   0x3FF, 0x555, 0x666, 0x71C, 0x787, 0x7C1, 0x7E0, 0x7F0
};

/* Noise Frequency Lookup Table */
dword ApuNoiseFreq[ 16 ] =
{
    0x002, 0x004, 0x008, 0x010, 0x020, 0x030, 0x040, 0x050, 
    0x065, 0x07F, 0x0BE, 0x0FE, 0x17D, 0x1FC, 0x3F9, 0x7F2
};

/* DMC Transfer Clocks Table */
dword ApuDpcmCycles[ 16 ] = 
{
  428, 380, 340, 320, 286, 254, 226, 214,
  190, 160, 142, 128, 106,  85,  72,  54
};


/* --------------------- Channel 1: Rectangular Wave ----------------------- */

int ApuWriteWave1(int event)
{
    while ( event < S.cur_event ) 
    {
      if ((S.pAPU.ApuEventQueue[event].type & APUET_MASK ) == APUET_C1) 
      {
	switch ( S.pAPU.ApuEventQueue[event].type & 0x03 ) 
        {
	case 0:
	  S.pAPU.ApuC1a    = S.pAPU.ApuEventQueue[event].data;
	  S.pAPU.ApuC1Wave = pulse_waves[MC1DutyCycle >> 6];
	  break;

	case 1:
	  S.pAPU.ApuC1b    = S.pAPU.ApuEventQueue[event].data; 
	  break;
	  
	case 2:
	  S.pAPU.ApuC1c = S.pAPU.ApuEventQueue[event].data;
	  S.pAPU.ApuC1Freq = ((((word) S.pAPU.ApuC1d & 0x07) << 8) + S.pAPU.ApuC1c);
	  S.pAPU.ApuC1Atl = ApuAtl[ ( S.pAPU.ApuC1d & 0xf8 ) >> 3 ];
	  if ( S.pAPU.ApuC1Freq ) 
          {
              S.pAPU.ApuC1Skip = (ApuPulseMagic << 1) / S.pAPU.ApuC1Freq;
	  } else {
              S.pAPU.ApuC1Skip = 0;
	  }
	  break;

	case 3:
	  S.pAPU.ApuC1d = S.pAPU.ApuEventQueue[event].data;
	  S.pAPU.ApuC1Freq = ((((word) S.pAPU.ApuC1d & 0x07 ) << 8) + S.pAPU.ApuC1c);
	  S.pAPU.ApuC1Atl = ApuAtl[( S.pAPU.ApuC1d & 0xf8 ) >> 3];
	  
	  if (S.pAPU.ApuC1Freq) 
          {
	    S.pAPU.ApuC1Skip = ( ApuPulseMagic << 1 ) / S.pAPU.ApuC1Freq;
	  } else {
	    S.pAPU.ApuC1Skip = 0;
	  }
	  break;
	}
      } 
      else if (S.pAPU.ApuEventQueue[event].type == APUET_W_CTRL) 
      {
	S.ApuCtrlNew = S.pAPU.ApuEventQueue[event].data;

	if( !(S.pAPU.ApuEventQueue[event].data&(1<<0)) ) {
	  S.pAPU.ApuC1Atl = 0;
	}
      }
      event++;
    }
    return event;
}

void ApuRenderingWave1( void )
{
  int event = 0;

  S.ApuCtrlNew = S.ApuCtrl;
  unsigned int i;

  if (S.pAPU.ApuC1EnvEnabled)
      S.pAPU.ApuC1Volume = S.pAPU.ApuC1EnvVol;

  for (i = 0; i < ApuSamplesPerSync; i++)
  { 
    /* Write registers */
    event = ApuWriteWave1(event);

  /* Envelope Decay */
  if (!i || i == 367 || i == 734) {
      /* Envelope decay at a rate of ( Envelope Delay + 1 ) / 240 secs */
      if (S.pAPU.ApuC1EnvReset) {
          S.pAPU.ApuC1EnvReset = 0;
          S.pAPU.ApuC1EnvVol = 0xF;
          S.pAPU.ApuC1EnvDelay = S.pAPU.ApuC1EnvPeriod;
      }
      else if ( --S.pAPU.ApuC1EnvDelay == 0)
      {
          S.pAPU.ApuC1EnvDelay = S.pAPU.ApuC1EnvPeriod;
          if (S.pAPU.ApuC1EnvLoop || S.pAPU.ApuC1EnvVol > 0)
              S.pAPU.ApuC1EnvVol = (S.pAPU.ApuC1EnvVol - 1) & 0xF;
      }

    }

    if (S.pAPU.ApuC1EnvEnabled)
        S.pAPU.ApuC1Volume = S.pAPU.ApuC1EnvVol;

    if ((S.pAPU.ApuC1Freq < 8 || (!MC1SweepIncDec && S.pAPU.ApuC1Freq > MC1FreqLimit)))
    {
      wave_buffers[0][i] = 0;
      memset((void *)wave_buffers[0], 0, ApuSamplesPerSync);
      break;
    }

    /* Frequency sweeping (Sweep Delay + 1) / 120 secs */
    if ( MC1SweepOn && MC1SweepShifts && MC1SweepDelay )
    {
      S.pAPU.ApuC1SweepPhase -= 2;         /* 120/60 */
      while ( S.pAPU.ApuC1SweepPhase < 0 && MC1SweepDelay)
      {
        S.pAPU.ApuC1SweepPhase += MC1SweepDelay;

        if ( MC1SweepIncDec )
        {
          /* Ramp Down */
          S.pAPU.ApuC1Freq += ~( S.pAPU.ApuC1Freq >> MC1SweepShifts );
        } else {
          /* Ramp Up */
          S.pAPU.ApuC1Freq += ( S.pAPU.ApuC1Freq >> MC1SweepShifts );
        }
      }
      if ( S.pAPU.ApuC1Freq ) {
          S.pAPU.ApuC1Skip = ( ApuPulseMagic << 1 ) / S.pAPU.ApuC1Freq;
      }
    }

    if ( ( S.ApuCtrlNew & 0x01 ) && ( S.pAPU.ApuC1Atl || MC1Hold ) ) 
    {
      S.pAPU.ApuC1Index += S.pAPU.ApuC1Skip;
      S.pAPU.ApuC1Index &= 0x1fffffff;
      
      wave_buffers[0][i] = (S.pAPU.ApuC1Wave[S.pAPU.ApuC1Index >> 24] 
             * S.pAPU.ApuC1Volume) >> 1;
    } else {
      wave_buffers[0][i] = 0;
    }
  }
  if ( S.pAPU.ApuC1Atl ) { S.pAPU.ApuC1Atl--;  }
}

/* --------------------- Channel 2: Rectangular Wave ----------------------- */

int ApuWriteWave2(int event )
{
    while ( ( event < S.cur_event )) 
    {
      if ( ( S.pAPU.ApuEventQueue[event].type & APUET_MASK ) == APUET_C2 ) 
      {
	switch ( S.pAPU.ApuEventQueue[event].type & 0x03 ) 
        {
	case 0:
	  S.pAPU.ApuC2a    = S.pAPU.ApuEventQueue[event].data;
	  S.pAPU.ApuC2Wave = pulse_waves[ MC2DutyCycle >> 6 ];
	  break;

	case 1:
	  S.pAPU.ApuC2b    = S.pAPU.ApuEventQueue[event].data; 
	  break;
	  
	case 2:
	  S.pAPU.ApuC2c = S.pAPU.ApuEventQueue[event].data;
	  S.pAPU.ApuC2Freq = ( ( ( (word)S.pAPU.ApuC2d & 0x07 ) << 8 ) + S.pAPU.ApuC2c );
	  S.pAPU.ApuC2Atl = ApuAtl[ ( S.pAPU.ApuC2d & 0xf8 ) >> 3 ];
	  
	  if ( S.pAPU.ApuC2Freq ) 
          {
	    S.pAPU.ApuC2Skip = ( ApuPulseMagic << 1 ) / S.pAPU.ApuC2Freq;
	  } else {
	    S.pAPU.ApuC2Skip = 0;
	  }
	  break;

	case 3:
	  S.pAPU.ApuC2d = S.pAPU.ApuEventQueue[event].data;
	  S.pAPU.ApuC2Freq = ( ( ( (word)S.pAPU.ApuC2d & 0x07 ) << 8 ) + S.pAPU.ApuC2c );
	  S.pAPU.ApuC2Atl = ApuAtl[ ( S.pAPU.ApuC2d & 0xf8 ) >> 3 ];
	  if ( S.pAPU.ApuC2Freq ) 
          {
	    S.pAPU.ApuC2Skip = ( ApuPulseMagic << 1 ) / S.pAPU.ApuC2Freq;
	  } else {
	    S.pAPU.ApuC2Skip = 0;
	  }
	  break;
	}
      } 
      else if ( S.pAPU.ApuEventQueue[event].type == APUET_W_CTRL ) 
      {
	S.ApuCtrlNew = S.pAPU.ApuEventQueue[event].data;

	if( !(S.pAPU.ApuEventQueue[event].data&(1<<1)) ) {
	  S.pAPU.ApuC2Atl = 0;
	}
      }
      event++;
    }
    return event;
}

void ApuRenderingWave2( void )
{
  int event = 0;

  S.ApuCtrlNew = S.ApuCtrl;
  unsigned int i;
  for ( i = 0; i < ApuSamplesPerSync; i++ )
  {
    event = ApuWriteWave2( event );

    if (!i || i == 367 || i == 734) {
      /* Envelope decay at a rate of ( Envelope Delay + 1 ) / 240 secs */
      if (S.pAPU.ApuC2EnvReset) {
          S.pAPU.ApuC2EnvReset = 0;
          S.pAPU.ApuC2EnvVol = 0xF;
          S.pAPU.ApuC2EnvDelay = S.pAPU.ApuC2EnvPeriod;
      }
      else if ( --S.pAPU.ApuC2EnvDelay == 0)
      {
          S.pAPU.ApuC2EnvDelay = S.pAPU.ApuC2EnvPeriod;
          if (S.pAPU.ApuC2EnvLoop || S.pAPU.ApuC2EnvVol > 0)
              S.pAPU.ApuC2EnvVol = (S.pAPU.ApuC2EnvVol - 1) & 0xF;
      }
    }

    if (S.pAPU.ApuC2EnvEnabled)
      S.pAPU.ApuC2Volume = S.pAPU.ApuC2EnvVol;

    if ( S.pAPU.ApuC2Freq < 8 || (!MC2SweepIncDec && S.pAPU.ApuC2Freq > MC2FreqLimit ))
    {
      wave_buffers[1][i] = 0;
      memset((void *)wave_buffers[1], 0, ApuSamplesPerSync);
      break;
    }

    /* Frequency sweeping (Sweep Delay + 1) / 120 secs */
    if ( MC2SweepOn && MC2SweepShifts && MC2SweepDelay)
    {
      S.pAPU.ApuC2SweepPhase -= 2;           /* 120/60 */
      while ( S.pAPU.ApuC2SweepPhase <= 0 && MC2SweepDelay)
      {
        S.pAPU.ApuC2SweepPhase += MC2SweepDelay;

        if ( MC2SweepIncDec ) 
        {
          /* Ramp Down */
          S.pAPU.ApuC2Freq -= ( S.pAPU.ApuC2Freq >> MC2SweepShifts );
        } else {
          /* Ramp Up */
          S.pAPU.ApuC2Freq += ( S.pAPU.ApuC2Freq >> MC2SweepShifts );
        }
      }
      if (S.pAPU.ApuC2Freq) {
	S.pAPU.ApuC2Skip = ( ApuPulseMagic << 1 ) / S.pAPU.ApuC2Freq;
      }
    }

    if ( ( S.ApuCtrlNew & 0x02 ) && ( S.pAPU.ApuC2Atl || MC2Hold ) ) 
    {
      S.pAPU.ApuC2Index += S.pAPU.ApuC2Skip;
      S.pAPU.ApuC2Index &= 0x1fffffff;
      
      /* Fixed volume */
      wave_buffers[1][i] = (S.pAPU.ApuC2Wave[S.pAPU.ApuC2Index >> 24] 
          * S.pAPU.ApuC2Volume) >> 1;
    } else {
      wave_buffers[1][i] = 0;
    }
  }
  if ( S.pAPU.ApuC2Atl ) { S.pAPU.ApuC2Atl--;  }
}

/* ------------------------ Channel 3: Triangle Wave ----------------------- */

int ApuWriteWave3( int event )
{
  while ( event < S.cur_event ) 
  {
    if ( ( S.pAPU.ApuEventQueue[event].type & APUET_MASK ) == APUET_C3 ) 
    {
      switch ( S.pAPU.ApuEventQueue[event].type & 3 ) 
      {
      case 0:
	S.pAPU.ApuC3a = S.pAPU.ApuEventQueue[event].data;
	S.pAPU.ApuC3Llc = MC3LinearLength;
	break;

      case 1:
	S.pAPU.ApuC3b = S.pAPU.ApuEventQueue[event].data;
	break;

      case 2:
	S.pAPU.ApuC3c = S.pAPU.ApuEventQueue[event].data;
	if ( MC3Freq ) 
        {
	  S.pAPU.ApuC3Skip = ApuTriangleMagic / MC3Freq;
	} else {
	  S.pAPU.ApuC3Skip = 0;  
	}
	break;

      case 3:
	S.pAPU.ApuC3d = S.pAPU.ApuEventQueue[event].data;
	S.pAPU.ApuC3Atl = MC3LengthCounter;
	if ( MC3Freq ) 
	{
	  S.pAPU.ApuC3Skip = ApuTriangleMagic / MC3Freq;
	} else {
	  S.pAPU.ApuC3Skip = 0;
	}
      }
    } else if ( S.pAPU.ApuEventQueue[event].type == APUET_W_CTRL ) {
      S.ApuCtrlNew = S.pAPU.ApuEventQueue[event].data;

      if( !(S.pAPU.ApuEventQueue[event].data&(1<<2)) ) {
	S.pAPU.ApuC3Atl = 0;
	S.pAPU.ApuC3Llc = 0;
      }
    }
    event++;
  }
  return event;
}

void ApuRenderingWave3( void )
{
  int event = 0;
      
  S.ApuCtrlNew = S.ApuCtrl;
  unsigned int i;
  for ( i = 0; i < ApuSamplesPerSync; i++) 
  {
    event = ApuWriteWave3 (event);

    /* Cutting Min Frequency */
    if (MC3Freq < 8)
    {
      memset(wave_buffers[2], 0, ApuSamplesPerSync);
      break;
    }

    /* Counter Control */
    if (! i || i == 367 || i == 734) 
    {
        if (S.pAPU.ApuC3CounterStarted)
        {
            if (S.pAPU.ApuC3Atl > 0 && !MC3Holdnote) 
                S.pAPU.ApuC3Atl--;
            if (S.pAPU.ApuC3Llc > 0)
                S.pAPU.ApuC3Llc--;
        } else if (!MC3Holdnote && S.pAPU.ApuC3WriteLatency > 0) {
            if ( --S.pAPU.ApuC3WriteLatency == 0 )
                S.pAPU.ApuC3CounterStarted = 0x01;
        }
    }

    if ( (S.ApuCtrlNew & 0x04) && 
         (S.pAPU.ApuC3Atl>0 || MC3Holdnote) && S.pAPU.ApuC3Llc > 0)
    {
      S.pAPU.ApuC3Index += S.pAPU.ApuC3Skip;
      S.pAPU.ApuC3Index &= 0x1fffffff;
      wave_buffers[2][i] = triangle_50[ S.pAPU.ApuC3Index >> 24 ] >> S.pAPU.ApuVolumeDivider;
      if (!S.BassBoost) 
          wave_buffers[2][i] >>= 1;
    } else {
      wave_buffers[2][i] = 0;
    }
  }
}


/* ----------------------- Channel 4: White Noise -------------------------- */

int ApuWriteWave4( int event )
{
  while (event < S.cur_event) 
  {
    if ( ( S.pAPU.ApuEventQueue[event].type & APUET_MASK ) == APUET_C4 ) 
    {
      switch (S.pAPU.ApuEventQueue[event].type & 3) {
      case 0:
	S.pAPU.ApuC4a = S.pAPU.ApuEventQueue[event].data;
	break;

      case 1:
	S.pAPU.ApuC4b = S.pAPU.ApuEventQueue[event].data;
	break;

      case 2:
	S.pAPU.ApuC4c = S.pAPU.ApuEventQueue[event].data;

	if (MC4Small) {
	  S.pAPU.ApuC4Sr = 0x1F;
	} else {
	  S.pAPU.ApuC4Sr = 10922;
	}

	/* Frequency */ 
	if ( MC4Freq ) {
	  S.pAPU.ApuC4Skip = ApuNoiseMagic / MC4Freq;
	} else {
	  S.pAPU.ApuC4Skip = 0;
	}
	S.pAPU.ApuC4Atl = MC4LengthCounter;

	break;

      case 3:
	S.pAPU.ApuC4d = S.pAPU.ApuEventQueue[event].data;

	/* Frequency */ 
	if (MC4Freq) {
	  S.pAPU.ApuC4Skip = ApuNoiseMagic / MC4Freq;
	} else {
	  S.pAPU.ApuC4Skip = 0;
	}
	S.pAPU.ApuC4Atl = MC4LengthCounter;
      }
    } else if (S.pAPU.ApuEventQueue[event].type == APUET_W_CTRL) {
      S.ApuCtrlNew = S.pAPU.ApuEventQueue[event].data;

      if (! (S.pAPU.ApuEventQueue[event].data & 0x08 )) {
	S.pAPU.ApuC4Atl = 0;
      }
    } 
    event++;
  }
  return event;
}

void ApuRenderingWave4(void)
{
    int event = 0;

    S.ApuCtrlNew = S.ApuCtrl;
    unsigned int i;

    for (i = 0; i<ApuSamplesPerSync; i++ ) 
    {
      event = ApuWriteWave4( event );

      if (!i || i == 367 || i == 734) {
        /* Envelope decay at a rate of ( Envelope Delay + 1 ) / 240 secs */
        if (S.pAPU.ApuC4EnvReset) {
            S.pAPU.ApuC4EnvReset = 0;
            S.pAPU.ApuC4EnvVol = 0xF;
            S.pAPU.ApuC4EnvDelay = S.pAPU.ApuC4EnvPeriod;
        }
        else if ( --S.pAPU.ApuC4EnvDelay == 0)
        {
            S.pAPU.ApuC4EnvDelay = S.pAPU.ApuC4EnvPeriod;
            if (S.pAPU.ApuC4EnvLoop || S.pAPU.ApuC4EnvVol > 0)
                S.pAPU.ApuC4EnvVol = (S.pAPU.ApuC4EnvVol - 1) & 0xF;
        }
    }

    if (S.pAPU.ApuC4EnvEnabled)
        S.pAPU.ApuC4Volume = S.pAPU.ApuC4EnvVol;

    if (S.ApuCtrlNew & 0x08)
    {
      S.pAPU.ApuC4Index += S.pAPU.ApuC4Skip;
      if (S.pAPU.ApuC4Index > 0x1fffffff)
      {
	if (MC4Small)
          S.pAPU.ApuC4Sr |= ((!(S.pAPU.ApuC4Sr & 1)) ^ (!(S.pAPU.ApuC4Sr & 4))) << 5;
        else 
	  S.pAPU.ApuC4Sr |= ((!(S.pAPU.ApuC4Sr & 1)) ^ (!(S.pAPU.ApuC4Sr & 16))) << 9;

          S.pAPU.ApuC4Sr >>= 1;
      }
      S.pAPU.ApuC4Index &= 0x1fffffff;

      if (S.pAPU.ApuC4Atl && (S.pAPU.ApuC4Sr & 1 ))
	  wave_buffers[3][i] = S.pAPU.ApuC4Volume >> S.pAPU.ApuVolumeDivider;
       else 
          wave_buffers[3][i] = 0;
    } else {
       wave_buffers[3][i] = 0;
    }
  }

  if (S.pAPU.ApuC4Atl && !MC4Hold) 
      S.pAPU.ApuC4Atl--;
}

/* -------------------------- Channel 5: PCM ------------------------------- */

int ApuWriteWave5(int event)
{
  while (event < S.cur_event) 
  {
    if ((S.pAPU.ApuEventQueue[event].type & APUET_MASK) == APUET_C5) 
    {
      S.pAPU.ApuC5Reg[S.pAPU.ApuEventQueue[event].type & 3] = S.pAPU.ApuEventQueue[event].data;

      switch (S.pAPU.ApuEventQueue[event].type & 3) {
        case 0:
          S.pAPU.ApuC5Freq = ApuDpcmCycles[(S.pAPU.ApuEventQueue[event].data & 0x0F)] << 13;
          S.pAPU.ApuC5Looping = S.pAPU.ApuEventQueue[event].data & 0x40;
          memset(pcm_buffer, 0, ApuSamplesPerSync); 
          break;
        case 1:
          S.pAPU.ApuC5DpcmValue = (S.pAPU.ApuEventQueue[event].data & 0x7F);
          if (S.pAPU.ApuEventQueue[event].data == 0x7F) 
              S.pAPU.ApuVolumeDivider = 1;
          else if (S.pAPU.ApuEventQueue[event].data == 0) 
              S.pAPU.ApuVolumeDivider = 0;
          if (S.pAPU.ApuC5Freq) 
              pcm_buffer[S.pAPU.ApuC5Bytes++] = S.pAPU.ApuC5DpcmValue;
          break;
        case 2:
          S.pAPU.ApuC5CacheAddr = 0xC000 + (word)(S.pAPU.ApuEventQueue[event].data << 6);
          break;
        case 3:
          S.pAPU.ApuC5CacheDmaLength = ( ( S.pAPU.ApuEventQueue[event].data << 4 ) + 1 ) << 3;
          break;
      }
    } else if (S.pAPU.ApuEventQueue[event].type == APUET_W_CTRL) {
      S.ApuCtrlNew = S.pAPU.ApuEventQueue[event].data;
      if( ! (S.pAPU.ApuEventQueue[event].data & (0x10)) ) {
	S.pAPU.ApuC5Enable    = 0;
	S.pAPU.ApuC5DmaLength = 0;
        S.pAPU.ApuC5DpcmValue = 0;
      } else {
	S.pAPU.ApuC5Enable = 0xFF;
	if( !S.pAPU.ApuC5DmaLength ) {
	  S.pAPU.ApuC5Address   = S.pAPU.ApuC5CacheAddr;
	  S.pAPU.ApuC5DmaLength = S.pAPU.ApuC5CacheDmaLength;
          if (S.pAPU.ApuC5DmaLength == 0) {
              S.pAPU.ApuC5Enable = 0;
          }
	}
      }
    }
    event++;
  }
  return event;
}

void ApuRenderingWave5(void)
{
  int event = 0;
  byte wrotePCM = 0;

  S.ApuCtrlNew = S.ApuCtrl;
  S.pAPU.ApuC5Bytes = 0;
  unsigned int i;

  if (!S.pAPU.ApuC5Zero) {
      memset(wave_buffers[4], 0, ApuSamplesPerSync);
      S.pAPU.ApuC5Zero = 1;
  }

  for (i=0; i<ApuSamplesPerSync; i++) 
  {
    event = ApuWriteWave5(event);

    if (S.pAPU.ApuC5DmaLength && S.pAPU.ApuC5Enable) {
        S.pAPU.ApuC5Phaseacc -= ApuCycleRate;

        while( S.pAPU.ApuC5Phaseacc < 0 ) {
           S.pAPU.ApuC5Phaseacc += S.pAPU.ApuC5Freq;
           if (!(S.pAPU.ApuC5DmaLength & 7)) {
               S.pAPU.ApuC5CurByte = Rd6502( S.pAPU.ApuC5Address );
               if (0xFFFF == S.pAPU.ApuC5Address)
                   S.pAPU.ApuC5Address = 0x8000;
               else
                   S.pAPU.ApuC5Address++;
           }

            /* Sample has finished playing; loop? */
           if (!(--S.pAPU.ApuC5DmaLength)) {
               if( S.pAPU.ApuC5Looping ) {
                   S.pAPU.ApuC5Address = S.pAPU.ApuC5CacheAddr;
                   S.pAPU.ApuC5DmaLength = S.pAPU.ApuC5CacheDmaLength;
               } else {
                   S.pAPU.ApuC5Enable = 0;
                   if (!S.pAPU.ApuC5Zero) {
                       memset((void *)wave_buffers[4], 0, ApuSamplesPerSync);
                       S.pAPU.ApuC5Zero = 1;
                       S.ApuCtrlNew &= ~0x10;
                   }
                   S.pAPU.ApuC5Address = 0;
                   S.pAPU.ApuC5Enable = 0;
                   return;
               }
           }

           /* Delta Modulation */
           if (S.pAPU.ApuC5CurByte & (1 << ((S.pAPU.ApuC5DmaLength & 7)^7))) {
               if (S.pAPU.ApuC5DpcmValue < 0x3F)
                   S.pAPU.ApuC5DpcmValue += 1;
           } else {
               if( S.pAPU.ApuC5DpcmValue > 1 )
                   S.pAPU.ApuC5DpcmValue -= 1;
           }
        }
    } else if (!S.pAPU.ApuC5DpcmValue) {
        if (S.pAPU.ApuC5Address != 0 || !S.pAPU.ApuC5Enable) {
            if (!S.pAPU.ApuC5Zero) {
                memset((void *)wave_buffers[4], 0, ApuSamplesPerSync);
                S.pAPU.ApuC5Zero = 1;
            }
            S.pAPU.ApuC5Address = 0;
            return;
        } 
    }
        
    /* Wave Rendering */
    if ( S.ApuCtrlNew & 0x10 ) {
        S.pAPU.ApuC5Zero = 0;
        wrotePCM = 1;
        wave_buffers[4][i] = ((S.pAPU.ApuC5Reg[1] & 0x01) + (S.pAPU.ApuC5DpcmValue << 1 ));
    } else {
      if (S.pAPU.ApuC5Looping) {
          S.pAPU.ApuC5Looping = 0; 
          if (!S.pAPU.ApuC5Zero) {
              memset((void *)wave_buffers[4], 0, ApuSamplesPerSync);
              S.pAPU.ApuC5Zero = 1;
          }
      /* Just incase */
      } else if (S.pAPU.ApuC5Address) {
          if (!S.pAPU.ApuC5Zero) {
              memset((void *)wave_buffers[4], 0, ApuSamplesPerSync);
              S.pAPU.ApuC5Zero = 1;
          } 
          S.pAPU.ApuC5Address = 0;
      }
    }
  }

  /* Ugly hack to spread out our DMA PCM data across the sample */
  if (!wrotePCM && S.pAPU.ApuC5Bytes) {
      int spread = (ApuSamplesPerSync / S.pAPU.ApuC5Bytes) + 1;
      int pcmctr = 0;
      int j;
      byte sample;
      memset(wave_buffers[4], 0, ApuSamplesPerSync);
      S.pAPU.ApuC5Zero = 0;
      for(i=0;i<ApuSamplesPerSync;i++) {
          sample = pcm_buffer[pcmctr++];
          if (sample < 8) sample = 0;
          wave_buffers[4][i] = sample;
          for(j=1;j<spread;j++) {
              if (i<ApuSamplesPerSync-1)
                  wave_buffers[4][++i] = sample;
          } 
          spread = ((ApuSamplesPerSync - i) / (S.pAPU.ApuC5Bytes - pcmctr)) + 1;
      }
  }
}

void NESCore_pAPU_VSync(void)
{
  if (S.APU_Mute)
      return;

  ApuRenderingWave1();
  ApuRenderingWave2();
  ApuRenderingWave3();
  ApuRenderingWave4();
  ApuRenderingWave5();

  S.ApuCtrl = S.ApuCtrlNew;
  NESCore_Callback_OutputSample(
	   S.userData,
       ApuSamplesPerSync, 
       wave_buffers[0],
       wave_buffers[1],
       wave_buffers[2], 
       wave_buffers[3],
       wave_buffers[4] 
  );

  S.cur_event = 0;
}

void NESCore_Init_pAPU(void)
{
  memset(&S.pAPU, 0, sizeof(S.pAPU));
  NESCore_Callback_InitSound(S.userData);

  ApuQuality = pAPU_QUALITY - 1;
  ApuPulseMagic      = ApuQual[ ApuQuality ].pulse_magic;
  ApuTriangleMagic   = ApuQual[ ApuQuality ].triangle_magic;
  ApuNoiseMagic      = ApuQual[ ApuQuality ].noise_magic;
  ApuSamplesPerSync  = ApuQual[ ApuQuality ].samples_per_sync;
  ApuCyclesPerSample = ApuQual[ ApuQuality ].cycles_per_sample;
  ApuSampleRate      = ApuQual[ ApuQuality ].sample_rate;
  ApuCycleRate       = ApuQual[ ApuQuality ].cycle_rate;

  NESCore_Callback_OpenSound(S.userData, ApuSamplesPerSync, ApuSampleRate );

  /* Initialize Rectangular, Noise Wave Registers */

  S.pAPU.ApuC1Wave = pulse_50;
  S.pAPU.ApuC2Wave = pulse_50;

  /* Initialize Triangle Wave Registers */

  S.pAPU.ApuC3WriteLatency = 3; /* Magic Number */

  /* Initialize DPCM Registers */

  S.pAPU.ApuC5Freq = S.pAPU.ApuC5Phaseacc;

  memset((void *) wave_buffers[0], 0, 735);  
  memset((void *) wave_buffers[1], 0, 735);  
  memset((void *) wave_buffers[2], 0, 735);  
  memset((void *) wave_buffers[3], 0, 735);  
  memset((void *) wave_buffers[4], 0, 735);  

  S.pAPU.ApuVolumeDivider = 0;

  S.cur_event = 0;
}

void NESCore_pAPU_Finish(void)
{
    NESCore_Callback_CloseSound(S.userData);
}

