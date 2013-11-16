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

#import <sys/time.h>
#import "EmulatorCore.h"
#import "NestopiaCore.h"
#import "Nestopia_Callback.h"
#import "ScreenView.h"

void setActiveFrameBuffer(unsigned long *buf);

@interface EmulatorCore ()

@property (nonatomic, strong) Game *currentGame;

@end


@implementation EmulatorCore
@synthesize currentROMImagePath;
@synthesize frameBufferAddress;
@synthesize frameBufferSize;
//@synthesize screenDelegate;

extern NSString *currentGamePath;

+ (EmulatorCore *)sharedEmulatorCore {
    static EmulatorCore *sharedInstance;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        sharedInstance = [[EmulatorCore alloc] init];
    });
	return sharedInstance;
}

+ (NSDictionary *)globalSettings {
    return [[NSUserDefaults standardUserDefaults] dictionaryForKey:@"GlobalSettings"];
}

+ (void)saveGlobalSettings:(NSDictionary *)settings {
    if (settings) {
        [[NSUserDefaults standardUserDefaults] setObject:settings forKey:@"GlobalSettings"];
    } else {
        [[NSUserDefaults standardUserDefaults] removeObjectForKey:@"GlobalSettings"];
    }
    
    [[NSUserDefaults standardUserDefaults] synchronize];
}

- (BOOL)loadGame:(Game *)game {
    self.currentGame = game;
	
	NSLog(@"%s loading image %@\n", __func__ , self.currentGame.path);

    nestopiaCore = [[NestopiaCore alloc] init];
    nestopiaCore.gamePath = self.currentGame.path;
    nestopiaCore.delegate = self;
    nestopiaCore.controllerLayout = [[self.currentGame.settings objectForKey: @"controllerLayout"] intValue];
    
    if (frameBufferSize.height && frameBufferSize.width) {
        nestopiaCore.resolution = frameBufferSize;
    }
    
    BOOL initialized = [nestopiaCore initializeCore];
    if (initialized == NO) {
        NSLog(@"%s [nestopiaCore initializeCore] failed", __PRETTY_FUNCTION__);
        return NO;
    }
    
	return YES;
}

- (BOOL)initializeEmulator {		
	controllerP1 = controllerP2 = 0;
	zapperState = zapperX = zapperY = 0;
		
	return YES;
}

- (BOOL)configureEmulator {
	
	NSLog(@"%s", __func__);
	
	defaultFullScreen = [[self.currentGame.settings objectForKey: @"fullScreen"] intValue];
	defaultAspectRatio = [[self.currentGame.settings objectForKey: @"aspectRatio"] intValue];
	   
	if (defaultFullScreen) {
		destinationWidth = (defaultAspectRatio) ? 341 : 479;
		destinationHeight = 320;
	} else {
		destinationWidth = 255;
		destinationHeight = 240;
	}
		
	return YES;
}

- (int)applyGameGenieCodes {
    NSMutableArray *codes = [[NSMutableArray alloc] init];
    
    NSLog(@"%s loading Game Genie codes\n", __func__);
    if ([[self.currentGame.settings objectForKey: @"gameGenie"] boolValue] == YES) {
        
        for(int i = 0; i < 4; i++) {
            NSString *code = [self.currentGame.settings objectForKey: [NSString stringWithFormat: @"gameGenieCode%d", i]];
            if (code != nil) {
                
                NSLog(@"%s applying game genie code %@", __func__, code);
                [codes addObject: code];
            }
        }
    }
    
    [nestopiaCore applyCheatCodes: codes];

	return 0;
}	

- (BOOL)saveState {
    [nestopiaCore saveState];
    return YES;
}

- (BOOL)loadState {
    
    [nestopiaCore loadState];
    return YES;
}


/* Callbacks */

- (void) emulatorCallbackOutputFrame:(word *)workFrame frameCount:(byte)frameCount {
	int x, y;
	
	/* Render */
	if (screenDelegate != nil) {
		if (defaultFullScreen == NO) {
			memcpy(frameBufferAddress, workFrame, NES_WIDTH * NES_HEIGHT * 2);
        } else {
			int px_dest_y, px_src_y, px_src_x;
            int dest_w = 320;
            int dest_h = 300;
			
            for (y=0; y<dest_h; y++)
            {
                px_dest_y = y * dest_w;
                px_src_y = (y * NESCORE_NATIVE_SCREEN_HEIGHT/dest_h);
				
                for (x=0; x < dest_w; x++) {
                    px_src_x = (x * NESCORE_NATIVE_SCREEN_WIDTH/dest_w);
                    frameBufferAddress[px_dest_y + x] 
					= workFrame[(px_src_y * NESCORE_NATIVE_SCREEN_WIDTH) + px_src_x];
                }
            }
        }

		[screenDelegate performSelectorOnMainThread: @selector(emulatorCoreDidUpdateFrameBuffer)
											  withObject:nil waitUntilDone: NO];
	} else {
		NSLog(@"%s screenDelegate = nil, skipping render", __func__);
	}
}

- (void)emulatorCallbackInputPadState:(uint *)pad1 pad2:(uint *)pad2 zapper:(uint *)zapper x:(uint *)x y:(uint *)y
{
    
    *pad1 = controllerP1;
    *pad2 = controllerP2;
    *zapper = zapperState;
    *x = zapperX;
    *y = zapperY;
}

- (void)AQBufferCallback:(void *)callbackStruct inQ:(AudioQueueRef)inQ outQB:(AudioQueueBufferRef)outQB
{
	AQCallbackStruct * inData;
    short *coreAudioBuffer;
    short sample;
    int i;
    
    inData = (AQCallbackStruct *)callbackStruct;
    coreAudioBuffer = (short*) outQB->mAudioData;
	   
    if (inData->frameCount > 0) {
        outQB->mAudioDataByteSize = 4 * inData->frameCount;
        for(i=0; i < inData->frameCount * 2; i+=2) {
            sample = waveBuffer[playNeedle];
            waveBuffer[playNeedle] = 0;
            playNeedle++;
            if (playNeedle >= BUFFERSIZE-1)
				playNeedle = 0;

            coreAudioBuffer[i] =   sample;
            coreAudioBuffer[i+1] = sample;

        }
        AudioQueueEnqueueBuffer(inQ, outQB, 0, NULL);
    }
}

- (int)emulatorCallbackOpenSound:(int)samplesPerSync sampleRate:(int)sampleRate {

	memset(&waveBuffer, 0, sizeof(waveBuffer));
    writeNeedle = 0;
    playNeedle = 0;

    audioCallback.mDataFormat.mSampleRate = sampleRate;
    audioCallback.mDataFormat.mFormatID = kAudioFormatLinearPCM;
    audioCallback.mDataFormat.mFormatFlags = kLinearPCMFormatFlagIsSignedInteger | kAudioFormatFlagsNativeEndian;
    audioCallback.mDataFormat.mBytesPerPacket = 4;
    audioCallback.mDataFormat.mFramesPerPacket = 1;
    audioCallback.mDataFormat.mBytesPerFrame = 4;
    audioCallback.mDataFormat.mChannelsPerFrame = 2;
    audioCallback.mDataFormat.mBitsPerChannel = 16;
	audioCallback.userData = (__bridge void *)(self);
	
	[self initializeSoundBuffers: samplesPerSync];
	
    return 0;
}

- (void)emulatorCallbackCloseSound {
    AudioQueueDispose(audioCallback.queue, YES);
    soundBuffersInitialized = 0;
}

- (void)emulatorCallbackOutputSampleWave:(int)samples wave1:(short *)wave1 {

    for (int i = 0; i < samples; i++)
    {
        waveBuffer[writeNeedle++] = wave1[i];
        if (writeNeedle >= BUFFERSIZE-1)
            writeNeedle = 0;
    }
}

- (void)initializeSoundBuffers:(int)samplesPerSync {
    uint32_t err;
    uint32_t bufferBytes;
    
    requiredBuffersToOpenSound = 3;
    
    if (soundBuffersInitialized >= requiredBuffersToOpenSound) {
        NSLog(@"%s skipping sound buffer initialization", __PRETTY_FUNCTION__);
        return;
    }
    
    err = AudioQueueNewOutput(&audioCallback.mDataFormat,
                              AQBufferCallback,
                              &audioCallback,
                              NULL,
                              kCFRunLoopCommonModes,
                              0,
                              &audioCallback.queue);
    if (err) {
        NSLog(@"%s AudioQueueNewOutput error %u", __func__, err);
        return;
    }
    
    audioCallback.frameCount = samplesPerSync;
    bufferBytes = audioCallback.frameCount * audioCallback.mDataFormat.mBytesPerFrame;
    
    NSLog(@"%s initializing %d sound buffers", __func__, requiredBuffersToOpenSound);
    for (int i = 0; i < requiredBuffersToOpenSound; i++) {
        err = AudioQueueAllocateBuffer(audioCallback.queue, bufferBytes, &audioCallback.mBuffers[i]);
        if (err) {
            NSLog(@"%s AudioQueueAllocateBuffer[%u] error %u", __func__, bufferBytes, err);
            continue;
        }
        AQBufferCallback(&audioCallback, audioCallback.queue, audioCallback.mBuffers[i]);
        soundBuffersInitialized++;
    }
    
    NSLog(@"%s calling AudioQueueStart", __func__);
    err = AudioQueueStart(audioCallback.queue, NULL);
    if (err) {
        NSLog(@"%s AudioQueueStart error %u", __func__, err);
    }
}

- (void)startEmulator {
    NSLog(@"%s", __PRETTY_FUNCTION__);

	soundBuffersInitialized = 0;
	[nestopiaCore startEmulation];
}

- (void)restartEmulator {
    NSLog(@"%s controllerLayout: %d", __PRETTY_FUNCTION__, [[self.currentGame.settings objectForKey: @"controllerLayout"] intValue]);
    
    nestopiaCore.controllerLayout = [[self.currentGame.settings objectForKey: @"controllerLayout"] intValue];
    [nestopiaCore initializeInput];
    screenDelegate = haltedScreenDelegate;
    soundBuffersInitialized = 0;
    [nestopiaCore startEmulation];
}

- (void)insertCoin1 {
    [self performSelectorInBackground: @selector(internalInsertCoin1) withObject: nil];
}

- (void)internalInsertCoin1 {
    NSLog(@"%s", __PRETTY_FUNCTION__);
    [nestopiaCore toggleCoin1];
    usleep(250000);
    [nestopiaCore coinOff];
}

- (void)setScreenDelegate:(id<EmulatorCoreScreenDelegate>)_screenDelegate {
    screenDelegate = _screenDelegate;
    haltedScreenDelegate = _screenDelegate;
}

- (id<EmulatorCoreScreenDelegate>)screenDelegate {
    return screenDelegate;
}

- (void)haltEmulator {
	
    haltedScreenDelegate = screenDelegate;
    screenDelegate = nil;
    
	NSLog(@"%s halting emulator\n", __func__);
	[nestopiaCore stopEmulation];	
}

- (void)finishEmulator {
    [nestopiaCore finishEmulation];
    haltedScreenDelegate = nil;
}

- (void)activatePad1 {
    [nestopiaCore activatePad1];
}

- (void)activatePad2 {
    [nestopiaCore activatePad2];
}

- (void)gameControllerZapperDidChange: (byte)status locationInWindow:(CGPoint)locationInWindow {
	
	NSLog(@"%s zapper status: %02x location: %fx%f\n", __func__, status, locationInWindow.x, locationInWindow.y);
	zapperState = status;
	zapperX = locationInWindow.x;
	zapperY = locationInWindow.y;
}

- (void)gameControllerControllerDidChange: (int)controller controllerState:(dword)controllerState {
	
	//	NSLog(@"%s status for controller %d status %d\n", __func__, controller, controllerState);
	if (controller == 0)
		controllerP1 = controllerState;
	else
		controllerP2 = controllerState;
}

void AQBufferCallback(void *callbackStruct, AudioQueueRef inQ, AudioQueueBufferRef outQB)
{
	AQCallbackStruct *inData = (AQCallbackStruct *) callbackStruct;
	EmulatorCore *sharedEmulatorCore = (__bridge EmulatorCore *) inData->userData;
	[sharedEmulatorCore AQBufferCallback: inData inQ: inQ outQB: outQB];
}
@end

