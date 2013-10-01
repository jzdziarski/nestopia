/*
 Nestopia for iOS
 Copyright (c) 2013, Jonathan A. Zdziarski
 
 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; version 2
 of the License.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 
 */

#ifndef __NESTOPIA_CALLBACK_H_

#include <stdio.h>

void Nestopia_Callback_OutputFrame(void *sender, void *video);
int Nestopia_Callback_OpenSound(void *sender, int samples_per_sync, int sample_rate);
void Nestopia_Callback_OutputSample(void *sender, int samples, void *audio);
void Nestopia_Callback_CloseSound(void *userData);
void Nestopia_Callback_InputPadState(void *userData, uint *pad1, uint *zapper, uint *x, uint*y);

#define NCTL_A      0x01
#define NCTL_B      0x02
#define NCTL_SELECT 0x04
#define NCTL_START  0x08
#define NCTL_UP     0x10
#define NCTL_DOWN   0x20
#define NCTL_LEFT   0x40
#define NCTL_RIGHT  0x80

#ifndef DWORD_TYPE_DEFINED
typedef unsigned long dword;
#define DWORD_TYPE_DEFINED
#endif

#ifndef SDWORD_TYPE_DEFINED
typedef signed long sdword;
#define SDWORD_TYPE_DEFINED
#endif

#ifndef WORD_TYPE_DEFINED
typedef unsigned short word;
#define WORD_TYPE_DEFINED
#endif

#ifndef BYTE_TYPE_DEFINED
typedef unsigned char byte;
#define BYTE_TYPE_DEFINED
#endif

#endif