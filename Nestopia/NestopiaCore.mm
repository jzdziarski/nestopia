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

#import "NestopiaCore.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include "../core/api/NstApiEmulator.hpp"
#include "../core/api/NstApiVideo.hpp"
#include "../core/api/NstApiSound.hpp"
#include "../core/api/NstApiInput.hpp"
#include "../core/api/NstApiMachine.hpp"
#include "../core/api/NstApiCartridge.hpp"
#include "../core/api/NstApiUser.hpp"
#include "../core/api/NstApiCheats.hpp"
#include "../core/api/NstApiDipSwitches.hpp"
#include "../core/api/NstApiBarcodeReader.hpp"


static bool NST_CALLBACK ZapperCallback(void* userData, Nes::Core::Input::Controllers::Zapper& zapper);
static bool NST_CALLBACK SoundLock(void* userData,Nes::Api::Sound::Output& sound);
static void NST_CALLBACK SoundUnlock(void* userData,Nes::Api::Sound::Output& sound);
static bool NST_CALLBACK VideoLock(void* userData,Nes::Api::Video::Output& video);
static void NST_CALLBACK VideoUnlock(void* userData,Nes::Api::Video::Output& video);
static void NST_CALLBACK DoFileIO(void* userData,Nes::Api::User::File operation,Nes::Api::User::FileData& data);


@interface NestopiaCore () {
    short soundBuffer[0x8000];
    int soundOffset;
    
    uint8_t *videoScreen;
    int screenWidth;
    int screenHeight;
    int framerate;
    
    BOOL isPlaying;
    int controller;
    
    Nes::Api::Video::Output *nstVideo;
    Nes::Api::Sound::Output *nstSound;
    Nes::Api::Emulator emulator;
    Nes::Api::Input::Controllers controls;
    Nes::Api::Cartridge::Database::Entry dbentry;
}

@property (nonatomic, strong) NSTimer *gameTimer;
@property (nonatomic, strong) NSLock *soundLock;

@end


@implementation NestopiaCore

#pragma mark Shared

+ (NestopiaCore *)sharedCore {
    static NestopiaCore *sharedInstance;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        sharedInstance = [[NestopiaCore alloc] init];
    });
    return sharedInstance;
}

#pragma mark Properties

@dynamic nativeResolution;

- (CGSize)nativeResolution {
    return CGSizeMake(screenWidth, screenHeight);
}

#pragma mark Init

- (id)init {
    if ((self = [super init])) {
        Nes::Api::Machine machine(emulator);
        
        _soundLock = [[NSLock alloc] init];
        isPlaying = false;
        
        void *userData = (__bridge void *)self;
        Nes::Api::Sound::Output::lockCallback.Set(SoundLock, userData);
        Nes::Api::Sound::Output::unlockCallback.Set(SoundUnlock, userData);
        Nes::Api::Video::Output::lockCallback.Set(VideoLock, userData);
        Nes::Api::Video::Output::unlockCallback.Set(VideoUnlock, userData);
        Nes::Api::User::fileIoCallback.Set(DoFileIO, userData);
        
        [self loadDatabase];
        
        if (![self initializeVideo]) {
            return nil;
        }
        [self initializeSound];
        [self initializeInput];
    }
    return self;
}

- (void)initializeSound {
    nstSound = new Nes::Api::Sound::Output;
	nstSound->samples[0] = soundBuffer;
	nstSound->length[0]  = 735;
	nstSound->samples[1] = NULL;
	nstSound->length[1]  = 0;

    soundOffset = 0;
    memset(soundBuffer, 0, sizeof(soundBuffer));
    
    Nes::Api::Sound sound( emulator );
	sound.SetSampleBits( 16 );
	sound.SetSampleRate( 44100 );
	sound.SetVolume(Nes::Api::Sound::ALL_CHANNELS, 30);
	sound.SetSpeaker( Nes::Api::Sound::SPEAKER_MONO );
}

- (void)initializeInput {
    NSLog(@"%s: %d", __PRETTY_FUNCTION__, self.controllerLayout);
 
    controller = 0;
	Nes::Api::Cartridge::Database database( emulator );
    if (self.controllerLayout == 0) {
        Nes::Api::Input(emulator).ConnectController( 0, Nes::Api::Input::PAD1 );
        Nes::Api::Input(emulator).ConnectController( 1, Nes::Api::Input::ZAPPER );
    } else {
        Nes::Api::Input(emulator).ConnectController( 0, Nes::Api::Input::ZAPPER );
        Nes::Api::Input(emulator).ConnectController( 1, Nes::Api::Input::PAD1 );
    }
    controls.vsSystem.insertCoin = 0;
    
    void* userData = (void*) 0xDEADC0DE;
    controls.zapper.callback.Set(ZapperCallback, userData);
}

- (BOOL)initializeVideo {
	Nes::Api::Video::RenderState renderState;
	nstVideo = new Nes::Api::Video::Output;
	
    int filter = 0;
	Nes::Api::Video::RenderState::Filter filters[2] =
	{
		Nes::Api::Video::RenderState::FILTER_NONE,
		Nes::Api::Video::RenderState::FILTER_NTSC,
	};
	int Widths[2] =
	{
		Nes::Api::Video::Output::WIDTH,
		Nes::Api::Video::Output::NTSC_WIDTH,
	};
	int Heights[2] =
	{
		Nes::Api::Video::Output::HEIGHT,
		Nes::Api::Video::Output::NTSC_HEIGHT,
	};
	
	Nes::Api::Machine machine( emulator );
	Nes::Api::Cartridge::Database database( emulator );
	
	int videoMode = [[NSUserDefaults standardUserDefaults] integerForKey: @"video"];

    if (!videoMode && !database.IsLoaded())
        videoMode = 1;
    
	if (videoMode == 2)             /* PAL */
	{
		machine.SetMode(Nes::Api::Machine::PAL);
		framerate = 50;
	} else if (videoMode == 1)      /* NTSC */
	{
        framerate = 60;
		machine.SetMode(Nes::Api::Machine::NTSC);
	} else                          /* Detect */
	{
        if (database.GetRegion(dbentry) == Nes::Api::Cartridge::REGION_PAL)
        {
            machine.SetMode(Nes::Api::Machine::PAL);
            framerate = 50;
        }
        else
        {
            machine.SetMode(Nes::Api::Machine::NTSC);
            framerate = 60;
        }
		
	}
	
	screenWidth = Widths[filter];
	screenHeight = Heights[filter];

    NSLog(@"%s initializing resolution %dx%d", __PRETTY_FUNCTION__, screenWidth, screenHeight);
	renderState.bits.count  = 16;
	renderState.bits.mask.r = 0xF800;
	renderState.bits.mask.g = 0x07E0;
	renderState.bits.mask.b = 0x001F;
	renderState.filter      = filters[filter];
	renderState.width       = screenWidth;
	renderState.height      = screenHeight;
    
	videoScreen = new unsigned char[screenWidth * screenHeight * 2];
	Nes::Api::Video video( emulator );
	video.EnableUnlimSprites(true);
	
	// NTSC
	switch (0)
	{
		case 0:	// Composite
			video.SetSharpness(Nes::Api::Video::DEFAULT_SHARPNESS_COMP);
			video.SetColorResolution(Nes::Api::Video::DEFAULT_COLOR_RESOLUTION_COMP);
			video.SetColorBleed(Nes::Api::Video::DEFAULT_COLOR_BLEED_COMP);
			video.SetColorArtifacts(Nes::Api::Video::DEFAULT_COLOR_ARTIFACTS_COMP);
			video.SetColorFringing(Nes::Api::Video::DEFAULT_COLOR_FRINGING_COMP);
			break;
			
		case 1:	// S-Video
			video.SetSharpness(Nes::Api::Video::DEFAULT_SHARPNESS_SVIDEO);
			video.SetColorResolution(Nes::Api::Video::DEFAULT_COLOR_RESOLUTION_SVIDEO);
			video.SetColorBleed(Nes::Api::Video::DEFAULT_COLOR_BLEED_SVIDEO);
			video.SetColorArtifacts(Nes::Api::Video::DEFAULT_COLOR_ARTIFACTS_SVIDEO);
			video.SetColorFringing(Nes::Api::Video::DEFAULT_COLOR_FRINGING_SVIDEO);
			break;
			
		case 2:	// RGB
			video.SetSharpness(Nes::Api::Video::DEFAULT_SHARPNESS_RGB);
			video.SetColorResolution(Nes::Api::Video::DEFAULT_COLOR_RESOLUTION_RGB);
			video.SetColorBleed(Nes::Api::Video::DEFAULT_COLOR_BLEED_RGB);
			video.SetColorArtifacts(Nes::Api::Video::DEFAULT_COLOR_ARTIFACTS_RGB);
			video.SetColorFringing(Nes::Api::Video::DEFAULT_COLOR_FRINGING_RGB);
			break;
	}
	
	if (NES_FAILED(video.SetRenderState( renderState )))
	{
		printf("%s unable to assign render state", __func__);
        return NO;
    }
    
    return YES;
}

#pragma mark Dealloc

- (void)dealloc {
    delete videoScreen;
    delete nstSound;
    delete nstVideo;
}

#pragma mark Stuff

- (void)loadDatabase {
	Nes::Api::Cartridge::Database database( emulator );
	NSString* datFile = [[NSBundle mainBundle] pathForResource:@"NstDatabase" ofType:@"dat"];
	
	std::ifstream *nstDBFile;
	
	nstDBFile = new std::ifstream([datFile UTF8String], std::ifstream::in|std::ifstream::binary);
	
	if (nstDBFile->is_open())
	{
		database.Load(*nstDBFile);
		database.Enable(true);
	} else {
        NSLog(@"%s could not load database", __func__);
    }

	delete nstDBFile;
}

- (BOOL)powerOn {
	Nes::Api::Machine machine( emulator );
	Nes::Api::Cartridge::Database database( emulator );
    
	if (database.IsLoaded())
	{
        NSData *gameData = [NSData dataWithContentsOfFile:self.gamePath];
        if (gameData) {
            dbentry = database.FindEntry([gameData bytes], [gameData length]);
		}
	}
	
	std::ifstream *file = new std::ifstream([self.gamePath UTF8String] , std::ios::in|std::ios::binary );
    
	Nes::Result result = machine.Load( *file );
	if (NES_FAILED(result)) {
        return NO;
    }
    
    delete file;

	machine.Power( true );
    return YES;
}

- (void)startEmulation {
    isPlaying = true;
    
	[self.audioDelegate nestopiaCoreCallbackOpenSound:735 sampleRate:44100];
    [self.videoDelegate nestopiaCoreCallbackInitializeVideoWithWidth:screenWidth height:screenHeight];
    
    self.gameTimer = [NSTimer scheduledTimerWithTimeInterval: (1.0 / framerate) target: self selector: @selector(stepEmulator:) userInfo: nil repeats: YES];
}

- (void)stepEmulator:(NSTimer *)timer {
    if (isPlaying) {
        NestopiaInput input = [self.inputDelegate nestopiaCoreCallbackInput];
        
        controls.pad[controller].buttons = input.pad1;
        controls.zapper.fire = input.zapper;
        controls.zapper.x = input.zapperX;
        controls.zapper.y = input.zapperY;
        
        if (controls.zapper.fire) {
            NSLog(@"%s zapper: %d at %ux%u", __func__, controls.zapper.fire, controls.zapper.x, controls.zapper.y);
        }
        
        emulator.Execute(nstVideo, nstSound, &controls);
    } else {
        [timer invalidate];
    }
}

- (void)stopEmulation {
    isPlaying = NO;
    
    [self.audioDelegate nestopiaCoreCallbackCloseSound];
    [self.videoDelegate nestopiaCoreCallbackDestroyVideo];
}

- (void)powerOff {
    if (isPlaying) {
        [self stopEmulation];
    }
    
	Nes::Api::Machine machine(emulator);
	machine.Power(false);
}

- (void)loadState {
	Nes::Api::Machine machine(emulator);
	Nes::Api::Cartridge::Database database(emulator);

	std::ifstream *file = new std::ifstream([self.gameSavePath UTF8String] , std::ios::in|std::ios::binary);
    
	machine.LoadState( *file );
    
    delete file;
}

- (void)saveState {
	Nes::Api::Machine machine( emulator );
	Nes::Api::Cartridge::Database database( emulator );

    std::ostringstream *buffer = new std::ostringstream(std::stringstream::out | std::stringstream::binary);
    
    NSLog(@"%s calling SaveState(%@)", __PRETTY_FUNCTION__, self.gameSavePath);
    machine.SaveState( *buffer );
    
    NSData *data = [NSData dataWithBytes:buffer->str().c_str() length:buffer->tellp()];
    [data writeToFile:self.gameSavePath atomically:YES];

    delete buffer;
    
    NSLog(@"%s returning", __PRETTY_FUNCTION__);
}

- (void)toggleCoin1 {
    controls.vsSystem.insertCoin |= Nes::Core::Input::Controllers::VsSystem::COIN_1;
}

- (void)toggleCoin2 {
    controls.vsSystem.insertCoin |= Nes::Core::Input::Controllers::VsSystem::COIN_2;
}

- (void)coinOff {
    controls.vsSystem.insertCoin = 0;
}

- (void)applyCheatCodes:(NSArray *)codes {
    Nes::Api::Cheats cheater(emulator);
    cheater.ClearCodes();
    for (NSString *code in codes) {
        if (![code isEqualToString: @""])
        {
            Nes::Api::Cheats::Code ggCode;
            Nes::Api::Cheats::GameGenieDecode([code cStringUsingEncoding: NSASCIIStringEncoding], ggCode);
            cheater.SetCode(ggCode);
        }
    }
}

- (void)activatePad1 {
    Nes::Api::Input(emulator).ConnectController(1, Nes::Api::Input::ZAPPER);
    controller = 0;
}

- (void)activatePad2 {
    Nes::Api::Input(emulator).ConnectController(1, Nes::Api::Input::PAD2);
    controller = 1;
}

#pragma mark Callbacks

- (bool)nestopiaSoundLock {
    if ([self.soundLock tryLock]) {
        nstSound->samples[0] = soundBuffer + soundOffset;
		return true;
    }
	return false;
}

- (void)nestopiaSoundUnlock {
    [self.audioDelegate nestopiaCoreCallbackOutputSamples:nstSound->length[0] waves:soundBuffer + soundOffset];
    
    if (soundOffset) {
        soundOffset = 0;
    } else {
        soundOffset = 16384;
    }
    
    nstSound->samples[0] = soundBuffer + soundOffset;
    
	[self.soundLock unlock];
}

- (bool)nestopiaVideoLock {
    nstVideo->pixels = videoScreen;
	nstVideo->pitch = screenWidth * 2;
	return true;
}

- (void)nestopiaVideoUnlock {
    nstVideo->pixels = NULL;
    [self.videoDelegate nestopiaCoreCallbackOutputFrame:(unsigned short *)videoScreen];
}

- (void)nestopiaDoFileIOWithOperation:(Nes::Api::User::File)operation andData:(Nes::Api::User::FileData&)data {
    switch (operation)
	{
		case Nes::Api::User::FILE_LOAD_BATTERY:
		case Nes::Api::User::FILE_LOAD_EEPROM:
		{
			@autoreleasepool {
                NSString *batSaveFile = [self.gamePath stringByAppendingPathExtension: @"sram"];
                NSData *fileData = [NSData dataWithContentsOfFile: batSaveFile];
                
                data.resize( [fileData length] );
                memcpy(&data.front(), [fileData bytes],[fileData length]);
			}
            
			break;
		}
			
		case Nes::Api::User::FILE_SAVE_BATTERY: // save battery data to a file
		case Nes::Api::User::FILE_SAVE_EEPROM: // can be treated the same as battery files
		{
            @autoreleasepool {
                NSString *batSaveFile = [self.gamePath stringByAppendingPathExtension: @"sram"];
                NSData *fileData = [NSData dataWithBytes: (const char*)&data.front() length: data.size()];
                
                [fileData writeToFile: batSaveFile atomically: NO];
			}
			break;
		}
			
		case Nes::Api::User::FILE_SAVE_FDS:
		case Nes::Api::User::FILE_LOAD_TAPE:
		case Nes::Api::User::FILE_SAVE_TAPE:
		case Nes::Api::User::FILE_LOAD_TURBOFILE:
		case Nes::Api::User::FILE_SAVE_TURBOFILE:
			break;
	}
}
 
@end


static bool NST_CALLBACK SoundLock(void* userData, Nes::Api::Sound::Output& sound)
{
    return [(__bridge NestopiaCore *)userData nestopiaSoundLock];
}

static void NST_CALLBACK SoundUnlock(void* userData, Nes::Api::Sound::Output& sound)
{
    [(__bridge NestopiaCore *)userData nestopiaSoundUnlock];
}

static bool NST_CALLBACK VideoLock(void* userData, Nes::Api::Video::Output& video)
{
	return [(__bridge NestopiaCore *)userData nestopiaVideoLock];
}

static void NST_CALLBACK VideoUnlock(void* userData, Nes::Api::Video::Output& video)
{
    [(__bridge NestopiaCore *)userData nestopiaVideoUnlock];
}

static void NST_CALLBACK DoFileIO(void* userData,Nes::Api::User::File operation,Nes::Api::User::FileData& data)
{
    [(__bridge NestopiaCore *)userData nestopiaDoFileIOWithOperation:operation andData:data];
}

static bool NST_CALLBACK ZapperCallback(void* userData, Nes::Core::Input::Controllers::Zapper& zapper)
{
    return YES;
}
