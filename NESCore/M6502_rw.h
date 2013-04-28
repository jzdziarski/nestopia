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
 * M6502_rw.h: 6502 Read/Write Functions for NESCore
 *             This file is included by M6502.c and NESCore_pAPU.c
 */

#ifndef M6502_RW_H
#define M6502_RW_H

#include <string.h>

#include "NESCore.h"
#include "NESCore_pAPU.h"
#include "NESCore_Callback.h"

extern word NESPalette[64];

/* NES Memory Layout
 *
 * 0x0000 - 0x1fff  RAM ( 0x800 - 0x1fff is mirror of 0x0 - 0x7ff )
 * 0x2000 - 0x3fff  PPU
 * 0x4000 - 0x5fff  Sound
 * 0x6000 - 0x7fff  SRAM ( Battery Backed )
 * 0x8000 - 0xffff  ROM
 */

#define INIT_GAMEGENIE {                           \
    if (S.GameGenie) {                             \
      for(bI = 0; bI < 4; bI++) {                  \
          if (GG[bI].wAddr == wAddr) {             \
              if (GG[bI].length == 6)              \
                  { return GG[bI].wAddr; }         \
              else {                               \
                  bComp = GG[bI].compare;          \
                  bRet = GG[bI].data;              \
                  bI = 0xFF;                       \
                  break;                           \
              }                                    \
          }                                        \
      }                                            \
    }                                              \
}

static byte Rd6502(word wAddr)
{
  byte bScratch;
  word wScratch;
  byte bComp, bRet, bI = 0;

  switch (wAddr & 0xe000)
  {
    case 0x0000:  /* RAM */
      return S.RAM[ wAddr & 0x7ff ];

    case 0x2000:  /* PPU */
        switch( wAddr ) 
        {
            case (0x2007): /* VRAM Read */
                if (S.vAddr <0x3F00) {
                    wScratch = S.vAddr;
                    wScratch &= 0x3FFF;
                    bScratch = S.PPU_R7;
                    S.PPU_R7 = W.PPUBANK[ wScratch >> 10 ][ wScratch & 0x3FF ]; 
                } else {
                    bScratch = W.PPUBANK[ (wScratch >> 10) & 0x10 ] [ wScratch & 0x3FF ];
                }

                S.vAddr += (S.PPU_R0 & R0_INC_ADDR) ? 0x20 : 0x01;
                S.vAddr &= 0x3FFF;

                return bScratch;
                break;

            case (0x2004): /* SPR-RAM Read */
                return S.SPRRAM[ S.PPU_R3 ];
                break;

            case (0x2002):
                S.PPU_Latch_Flag = 0;
                bScratch = S.PPU_R2;
                S.PPU_R2 &= 0x7F;

                return bScratch;
                break;

            default: /* $2000, $2001, $2003, $2005, $2006 */
                return S.PPU_R7;
                break;
        }
        break;

    case 0x4000:  /* pAPU */
        switch ( wAddr )
        {
            case (0x4014):
                return wAddr & 0xff;
                break;
            case (0x4015): /* APU Control */
                bScratch = S.APU_Reg[ 0x15 ];
                if (S.pAPU.ApuC1Atl > 0) bScratch |= (1 << 0);
                if (S.pAPU.ApuC2Atl > 0) bScratch |= (1 << 1);
		if (!MC3Holdnote) {
                    if (S.pAPU.ApuC3Atl > 0) bScratch |= (1 << 2);
                } else {
                    if (S.pAPU.ApuC3Llc > 0) bScratch |= (1 << 2);
                }

                if (S.pAPU.ApuC4Atl > 0)
                    bScratch |= (1 << 3);
                S.APU_Reg[ 0x15 ] &= ~0x40;
                return bScratch;
                break;

            case (0x4016): /* Joypad 1 */
                bScratch = ((S.PAD1_Latch >> S.PAD1_Bit) & 1 ) | 0x40;
                S.PAD1_Bit = (S.PAD1_Bit == 23) ? 0 : (S.PAD1_Bit + 1);
                return bScratch;
                break;

            case (0x4017): /* Joypad 2 */
                if (!S.PAD_Zapper) {
                    bScratch = (byte) ((S.PAD2_Latch >> S.PAD2_Bit) & 1) | 0x40;
                    S.PAD2_Bit = (S.PAD2_Bit == 23) ? 0 : (S.PAD2_Bit + 1);
                } else {
					NESCore_Callback_InputPadState(S.userData, &S.PAD1_Latch, &S.PAD2_Latch);

                    bScratch = S.PAD2_Latch;
                    /* Sprite hit: Invert value */
                    if (bScratch & 0x08) 
                        bScratch &= ~0x08;
                    else
                        bScratch |= 0x08;
                }
                return bScratch;
                break;

            default:
                return MapperReadApu(wAddr);
      }
      break;

    case 0x6000: 
      if (S.ROM_SRAM)
        return S.SRAM[ wAddr & 0x1fff ];
      else 
        return W.SRAMBANK[ wAddr & 0x1fff ];

    case 0x8000:  /* ROM BANK 0 */
      INIT_GAMEGENIE
      if (bI == 0xFF && W.ROMBANK0[ wAddr & 0x1fff] == bComp)
            return bRet;
      return W.ROMBANK0[ wAddr & 0x1fff ];
      break;

    case 0xa000:  /* ROM BANK 1 */
      INIT_GAMEGENIE
      if (bI == 0xFF && W.ROMBANK1[ wAddr & 0x1fff] == bComp)
            return bRet;
      return W.ROMBANK1[ wAddr & 0x1fff ];
      break;

    case 0xc000:  /* ROM BANK 2 */
      INIT_GAMEGENIE
      if (bI == 0xFF && W.ROMBANK2[ wAddr & 0x1fff] == bComp)
            return bRet;
      return W.ROMBANK2[ wAddr & 0x1fff ];
      break;

    case 0xe000:  /* ROM BANK 3 */
      INIT_GAMEGENIE
      if (bI == 0xFF && W.ROMBANK3[ wAddr & 0x1fff] == bComp)
            return bRet;
      return W.ROMBANK3[ wAddr & 0x1fff ];
      break;
  }

  return wAddr >> 8; /* When a register is not readable,
                        the upper half address is returned. */
}

static void Wr6502(word wAddr, byte bData)
{
  word wScratch;
  word lowerBits;
  word upperBits;

  switch ( wAddr & 0xe000 )
  {
    case 0x0000:  /* RAM */
      S.RAM[ wAddr & 0x7ff ] = bData;
      break;

    case 0x2000:  /* PPU */
      switch ( wAddr ) 
      {
        case 0x2000: /* PPU Control Register 1 */
          S.PPU_R0 = bData;
          S.PPU_SP_Height = (S.PPU_R0 & R0_SP_SIZE) ? 0x10 : 0x08;
          W.PPU_BG = (S.PPU_R0 & R0_BG_ADDR) ? S.ChrBuf + 0x4000 : S.ChrBuf;
          W.PPU_SP = (S.PPU_R0 & R0_SP_ADDR) ? S.ChrBuf + 0x4000 : S.ChrBuf;
          S.vAddr_Latch = (S.vAddr_Latch & 0xF3FF) | ((word) (bData & 3) << 10);
          break;

        case 0x2001: /* PPU Control Register 2 */
          S.PPU_R1 = bData;
          break;

        case 0x2002: /* PPU Status - NOT WRITABLE */
          break;

        case 0x2003: /* Sprite RAM ADDR */
          S.PPU_R3 = bData;
          break;

        case 0x2004: /* Sprite RAM DATA */
          S.SPRRAM[S.PPU_R3] = bData;
          break;

        case 0x2005: /* Scroll Register */
          lowerBits = (bData & 7);
          upperBits = (bData >> 3);

          if (S.PPU_Latch_Flag) {
             S.vAddr_Latch = (S.vAddr_Latch & 0x8C1F) | (upperBits << 5) |
                (lowerBits << 12);
          }
          else {
             S.vAddr_Latch = (S.vAddr_Latch & 0xFFE0) | upperBits;
             S.FineX = lowerBits;
          }

          S.PPU_Latch_Flag ^= 1;
          break;

        case 0x2006: /* VRAM Address Register */
          if (S.PPU_Latch_Flag) {
             S.vAddr_Latch = (S.vAddr_Latch & 0xFF00) | bData;
             S.vAddr = S.vAddr_Latch;
          }
          else {
             S.vAddr_Latch = ((bData & 0x3F) << 8) | (S.vAddr_Latch & 0xFF);
          }
          S.PPU_Latch_Flag ^= 1;
          break;

        case 0x2007: /* VRAM Data */
        {
            wScratch = S.vAddr;
            wScratch &= 0x3FFF;

            S.vAddr += (S.PPU_R0 & R0_INC_ADDR) ? 0x20 : 0x01;
            if (S.vAddr > 0x3FFF)
                S.vAddr &= 0x3FFF;

            if (wScratch < 0x2000 && S.VRAMWriteEnable)
            {
              /* Pattern Data */
              S.ChrBufUpdate |= ( 1 << ( wScratch >> 10 ) );
              W.PPUBANK[ wScratch >> 10 ][ wScratch & 0x3FF ] = bData;
            }
            else if (wScratch < 0x3F00 )  /* 0x2000 - 0x3EFF */
            {
              /* Name Table and Mirror */
              W.PPUBANK[ (wScratch) >> 10 ][ wScratch & 0x3ff ] = bData;
              W.PPUBANK[ (wScratch ^ 0x1000) >> 10][ wScratch & 0x3FF ] = bData;
            }
            else if (!(wScratch & 0xF))  /* 0x3F00 or 0x3F10 */
            {
                /* Palette Mirror */
                S.PPURAM[ 0x3f10 ] = S.PPURAM[ 0x3f14 ] = S.PPURAM[ 0x3f18 ] 
              = S.PPURAM[ 0x3f1c ] = S.PPURAM[ 0x3f00 ] = S.PPURAM[ 0x3f04 ] 
              = S.PPURAM[ 0x3f08 ] = S.PPURAM[ 0x3f0c ] = bData;

                S.PalTable[ 0x00 ] = S.PalTable[ 0x04 ] = S.PalTable[ 0x08 ] 
              = S.PalTable[ 0x0c ] = S.PalTable[ 0x10 ] = S.PalTable[ 0x14 ] 
              = S.PalTable[ 0x18 ] = S.PalTable[ 0x1c ] 
              = NESPalette[ bData ];
            }
            else if (wScratch & 0x03)
            {
              /* Palette */
              S.PPURAM[ wScratch ] = bData;
              S.PalTable[ wScratch & 0x1f ] = NESPalette[ bData ];
            }
        }
        break;
    }
    break;

    case 0x4000:  /* pAPU and Sprite */
      switch ( wAddr & 0x1f )
      {
        case 0x01:
        case 0x02:
        case 0x05:
        case 0x06:
        case 0x08:
        case 0x09:
        case 0x0a:
        case 0x0b:
        case 0x0d:
        case 0x0e:
        case 0x10:
        case 0x11:
        case 0x12:
        case 0x13:
          pAPUSoundRegs[wAddr & 0x1f](wAddr,  bData);
          break;

        case 0x00:
          pAPUSoundRegs[wAddr & 0x1f](wAddr,  bData);
          S.pAPU.ApuC1EnvLoop = (bData & 0x20) != 0;
          S.pAPU.ApuC1EnvPeriod = (bData & 0x0F) + 1;
          S.pAPU.ApuC1EnvEnabled = !(bData & 0x10);
          S.pAPU.ApuC1Volume = (S.pAPU.ApuC1EnvEnabled) 
                             ? S.pAPU.ApuC1EnvVol : bData & 0xF;
          break;

        case 0x03:
          pAPUSoundRegs[wAddr & 0x1f](wAddr,  bData);
          S.pAPU.ApuC1EnvReset = 1;
          break;

        case 0x04:
          pAPUSoundRegs[wAddr & 0x1f](wAddr,  bData);
          S.pAPU.ApuC2EnvLoop = (bData & 0x20) != 0;
          S.pAPU.ApuC2EnvPeriod = (bData & 0x0F) + 1;
          S.pAPU.ApuC2EnvEnabled = !(bData & 0x10);
          S.pAPU.ApuC2Volume = (S.pAPU.ApuC2EnvEnabled) 
                             ? S.pAPU.ApuC2EnvVol : bData & 0xF;
          break;

           
        case 0x07:
          pAPUSoundRegs[wAddr & 0x1f](wAddr,  bData);
          S.pAPU.ApuC2EnvReset = 1;
          break;

                  
        case 0x0c:
          pAPUSoundRegs[wAddr & 0x1f](wAddr,  bData);
          S.pAPU.ApuC4EnvLoop = (bData & 0x20) != 0;
          S.pAPU.ApuC4EnvPeriod = (bData & 0x0F) + 1;
          S.pAPU.ApuC4EnvEnabled = !(bData & 0x10);
          S.pAPU.ApuC4Volume = (S.pAPU.ApuC4EnvEnabled) 
                             ? S.pAPU.ApuC4EnvVol : bData & 0xF;
          break;

        case 0x0f:
          S.pAPU.ApuC4EnvReset = 1;
          break;

        case 0x14:  /* 0x4014: Sprite DMA */
          S.PPU_R3 += SPRRAM_SIZE;
          S.RunCycles -= 24624; /* 513 x 48 */
          switch ( bData >> 5)
          {
            case 0x00:  /* RAM */
                memcpy(S.SPRRAM, &S.RAM[((word)bData << 8) & 0x7ff],
                       SPRRAM_SIZE);
                break;

            case 0x03:  /* SRAM */
                memcpy(S.SPRRAM, &S.SRAM[((word) bData << 8) & 0x1fff],
                       SPRRAM_SIZE);
                break;

            case 0x04:  /* ROM BANK 0 */
                memcpy(S.SPRRAM, &W.ROMBANK0[((word) bData << 8) & 0x1fff],
                       SPRRAM_SIZE);
                break;

            case 0x05:  /* ROM BANK 1 */
                memcpy(S.SPRRAM, &W.ROMBANK1[((word) bData << 8) & 0x1fff],
                       SPRRAM_SIZE);
                break;

            case 0x06:  /* ROM BANK 2 */
                memcpy(S.SPRRAM, &W.ROMBANK2[((word) bData << 8) & 0x1fff],
                       SPRRAM_SIZE);
                break;

            case 0x07:  /* ROM BANK 3 */
                memcpy(S.SPRRAM, &W.ROMBANK3[((word) bData << 8) & 0x1fff ],
                       SPRRAM_SIZE);
                break;
          }
          break;

        case 0x15:  /* 0x4015 */
          NESCore_pAPUWriteControl(wAddr, bData);
          break;

        case 0x16:  /* 0x4016 */
          if ( !( S.APU_Reg[ 0x16 ] & 1 ) && ( bData & 1 ) )
          {
            S.PAD1_Bit = 0;
            S.PAD2_Bit = 0;
          }
          break;

        case 0x17:  /* 0x4017 */
          S.FrameStep = 0;
          if ( ! (bData & 0xc0) )
            S.FrameIRQ_Enable = 1;
          else 
            S.FrameIRQ_Enable = 0;
          break;
      }

      if ( wAddr <= 0x4017 ) 
        S.APU_Reg[ wAddr & 0x1f ] = bData;
      else  
        MapperApu(wAddr, bData);
      break;

    case 0x6000:  /* SRAM */
      S.SRAM[ wAddr & 0x1fff ] = bData;

      /* Write to SRAM, when no SRAM */
      if ( !S.ROM_SRAM )
      {
        MapperSram( wAddr, bData );
      }
      break;

    case 0x8000:  /* ROM BANK 0 */
    case 0xa000:  /* ROM BANK 1 */
    case 0xc000:  /* ROM BANK 2 */
    case 0xe000:  /* ROM BANK 3 */
      MapperWrite( wAddr, bData );
      break;
  }
}

#endif
