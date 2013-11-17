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

#import <AudioToolbox/AudioToolbox.h>
#import <pthread.h>
#import "ControllerView.h"
#import "Game.h"
#import "NestopiaCore.h"
#include "Nestopia_Callback.h"

void AQBufferCallback(void *callbackStruct, AudioQueueRef inQ, AudioQueueBufferRef outQB);
void *emulation_thread(void *args);

#define WAVE_BUFFER_SIZE 735
#define WAVE_BUFFER_BANKS 10
#define BUFFERSIZE (WAVE_BUFFER_SIZE * WAVE_BUFFER_BANKS)

#define NESCORE_NATIVE_SCREEN_WIDTH 256
#define NESCORE_NATIVE_SCREEN_HEIGHT 240

typedef struct AQCallbackStruct {
    AudioQueueRef queue;
    UInt32 frameCount;
    AudioQueueBufferRef mBuffers[12];
    AudioStreamBasicDescription mDataFormat;
	void *userData;
} AQCallbackStruct;

@protocol EmulatorCoreScreenDelegate;


@interface EmulatorCore : NSObject <GameControllerDelegate>

+ (EmulatorCore *)sharedEmulatorCore;

+ (NSDictionary *)globalSettings;
+ (void)saveGlobalSettings:(NSDictionary *)settings;

- (BOOL)loadGame:(Game *)game;
- (BOOL)initializeEmulator;
- (int)applyGameGenieCodes;
- (BOOL)saveState;
- (BOOL)loadState;
- (void)startEmulator;
- (void)restartEmulator;
- (void)haltEmulator;
- (void)finishEmulator;
- (void)activatePad1;
- (void)activatePad2;
- (void)insertCoin1;

@property (nonatomic, readonly) CGSize nativeScreenResolution;

@property (nonatomic, readonly, copy) NSString *currentROMImagePath;
@property (nonatomic, weak) id<EmulatorCoreScreenDelegate> screenDelegate;

@end


@protocol EmulatorCoreScreenDelegate 

- (void)emulatorCoreDidRenderFrame:(CGImageRef)frameImageRef;

@end
