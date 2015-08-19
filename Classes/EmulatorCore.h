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
#import "GameROMViewController.h"
#import "ControllerView.h"
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

#pragma pack(2)
@interface EmulatorCore : NSObject <GameControllerDelegate> {
	NSString *currentROMImagePath;
	pthread_t emulation_tid;

	/* Resources for video rendering */
	id screenDelegate, haltedScreenDelegate;
	word *frameBufferAddress;
	CGSize frameBufferSize;

	int destinationHeight, destinationWidth;
	BOOL defaultFullScreen;
	BOOL defaultAspectRatio;
	
	/* Resources for audio playback */
	AQCallbackStruct audioCallback;
	long writeNeedle, playNeedle;
	int soundBuffersInitialized;
	int requiredBuffersToOpenSound;
	
	/* Resources for controller feedback */
	dword controllerP1;
	dword controllerP2;
	byte zapperState, zapperX, zapperY;
    
    NestopiaCore *nestopiaCore;
    short waveBuffer[WAVE_BUFFER_SIZE * WAVE_BUFFER_BANKS];
}

+ (EmulatorCore *)sharedEmulatorCore;
+ (NSDictionary *)gameSettings;
+ (NSDictionary *)globalSettings;
- (BOOL)loadROM:(NSString *)imagePath;
- (BOOL)initializeEmulator;
- (BOOL)configureEmulator;
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

/* GameControllerDelegate Methods */
- (void)gameControllerZapperDidChange: (byte)status locationInWindow:(CGPoint)locationInWindow;
- (void)gameControllerControllerDidChange:(int)controller controllerState:(dword)controllerState;

/* Calbacks */
- (void)emulatorCallbackOutputFrame:(word *)WorkFrame frameCount:(byte)frameCount;
- (void)emulatorCallbackInputPadState:(uint *)pad1 pad2:(uint *)pad2 zapper:(uint *)zapper x:(uint *)x y:(uint *)y;
- (int)emulatorCallbackOpenSound:(int)samplesPerSync sampleRate:(int)sampleRate;
- (void)emulatorCallbackCloseSound;
- (void)emulatorCallbackOutputSampleWave:(int)samples wave1:(short *)wave1;
- (void)AQBufferCallback:(void *)callbackStruct inQ:(AudioQueueRef)inQ outQB:(AudioQueueBufferRef)outQB;

@property(nonatomic,readonly,copy) NSString *currentROMImagePath;
@property(nonatomic,assign) word *frameBufferAddress;
@property(nonatomic,assign) CGSize frameBufferSize;
@property(nonatomic,assign) id<EmulatorCoreScreenDelegate> screenDelegate;

@end

@protocol EmulatorCoreScreenDelegate 

@required
- (void)emulatorCoreDidUpdateFrameBuffer;
@end
