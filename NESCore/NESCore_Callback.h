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
 * NESCore_Callback.h: NESCore Callback Prototypes
 *                     These are the callback functions that hook the emulator
 *                     core to an external application, and must be written
 *                     by the project's author / porter.
 */

#ifndef NESCORE_CALLBACK_H
#define NESCORE_CALLBACK_H

#include "NESCore_Types.h"

/* NESCore Callbacks */
void NESCore_Callback_Wait(void *userData);
void NESCore_Callback_OutputFrame(void *userData, word *WorkFrame, byte FrameCount);
void NESCore_Callback_InputPadState(void *userData, dword *pdwPad1, dword *pdwPad2);
void NESCore_Callback_InitSound(void *userData);
int  NESCore_Callback_OpenSound(void *userData, int samples_per_sync, int sample_rate);
void NESCore_Callback_CloseSound(void *userData);
void NESCore_Callback_OutputSample(void *userData, int samples, byte *wave1, byte *wave2, byte* wave3, byte *wave4, byte *wave5);
void NESCore_Debug(const char *msg, ...);

#define NCTL_A      0x01
#define NCTL_B      0x02
#define NCTL_SELECT 0x04
#define NCTL_START  0x08
#define NCTL_UP     0x10
#define NCTL_DOWN   0x20
#define NCTL_LEFT   0x40
#define NCTL_RIGHT  0x80

#endif /* NESCORE_CALLBACK_H */
