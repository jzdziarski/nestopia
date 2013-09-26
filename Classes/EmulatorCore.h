/*
 Nescaline
 Copyright (c) 2007, Jonathan A. Zdziarski
 
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
#include "NESCore_Types.h"
#include "NESCore_Callback.h"

void AQBufferCallback(void *callbackStruct, AudioQueueRef inQ, AudioQueueBufferRef outQB);
void *emulation_thread(void *args);

typedef enum {
	EmulatorCoreLoadStatusSuccess,
	EmulatorCoreLoadStatusInvalidROM,
	EmulatorCoreLoadStatusROMNotSupported
} EmulatorCoreLoadStatus;

#define WAVE_BUFFER_SIZE 735
#define WAVE_BUFFER_BANKS 25

#define NESCORE_NATIVE_SCREEN_WIDTH 256
#define NESCORE_NATIVE_SCREEN_HEIGHT 239

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
	NSUserDefaults *settings;
	pthread_t emulation_tid;
	
	/* Resources for video rendering */
	id screenDelegate;
	word *frameBufferAddress;
	CGSize frameBufferSize;
	struct timeval frameTimer;
	int pixelMap[320][480][2][2];
	int destinationHeight, destinationWidth;
	int defaultFrameSkip;
	BOOL defaultFullScreen;
	BOOL defaultAspectRatio;
	
	/* Resources for audio playback */
	AQCallbackStruct audioCallback;
	long writeNeedle, playNeedle;
	int soundBuffersInitialized;
	int requiredBuffersToOpenSound;
	int defaultSoundBuffer;
	byte waveBuffer[WAVE_BUFFER_SIZE * WAVE_BUFFER_BANKS];
	
	/* Resources for controller feedback */
	dword controllerP1;
	dword controllerP2;
	byte zapperState, zapperX, zapperY;
	bool screenBlank;
} ;

+ (EmulatorCore *)sharedEmulatorCore;
- (EmulatorCoreLoadStatus)loadROM:(NSString *)imagePath;
- (void)initializePixelMappers;
- (void)initializePalette;
- (BOOL)initializeEmulator;
- (void)initializeSoundBuffers;
- (BOOL)configureEmulator;
- (int)applyGameGenieCodes;
- (BOOL)saveState;
- (BOOL)loadState;
- (BOOL)saveSRAM;
- (BOOL)loadSRAM;
- (void)startEmulator;
- (void)haltEmulator;
- (void)finishEmulator;

/* GameControllerDelegate Methods */
- (void)gameControllerZapperDidChange: (byte)status locationInWindow:(CGPoint)locationInWindow;
- (void)gameControllerControllerDidChange:(int)controller controllerState:(dword)controllerState;

/* Calbacks */
- (void)emulatorCallbackWait;
- (void)emulatorCallbackOutputFrame:(word *)WorkFrame frameCount:(byte)frameCount;
- (void)emulatorCallbackInputPadState:(dword *)pad1 pad2:(dword *)pad2;
- (int)emulatorCallbackOpenSound:(int)samplesPerSync sampleRate:(int)sampleRate;
- (void)emulatorCallbackCloseSound;
- (void)emulatorCallbackOutputSample:(int)samples wave1:(byte *)wave1 wave2:(byte *)wave2 wave3:(byte *)wave3 wave4:(byte *)wave4 wave5:(byte *)wave5;
- (void)AQBufferCallback:(void *)callbackStruct inQ:(AudioQueueRef)inQ outQB:(AudioQueueBufferRef)outQB;

@property(nonatomic,readonly,copy) NSString *currentROMImagePath;
@property(nonatomic,assign) word *frameBufferAddress;
@property(nonatomic,assign) CGSize frameBufferSize;
@property(nonatomic,assign) id<EmulatorCoreScreenDelegate> screenDelegate;

@end

@protocol EmulatorCoreScreenDelegate 

@required
- (void)emulatorCoreDidUpdateFrameBuffer; //:(EmulatorCore *)emulatorCore frameBufferAddress:(word *)frameBufferAddress;

@end
