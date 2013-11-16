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

#import <Foundation/Foundation.h>
#import "NestopiaCore.h"
#import "EmulatorCore.h"

#include "Nestopia_Callback.h"

#include <sys/time.h>
#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

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

NSString *gameFilename, *saveFilename;
static NSLock *soundLock;
static short soundBuffer[0x8000];
static int soundOffset;
static uint8_t  *videoScreen;
static int cur_width, cur_height, framerate;
static void *callback;
static BOOL isPlaying;
static int controller = 0;

static Nes::Api::Video::Output* nstVideo;
static Nes::Api::Sound::Output* nstSound;
static Nes::Api::Emulator emulator;
static Nes::Api::Input::Controllers controls;
static Nes::Api::Cartridge::Database::Entry dbentry;

@implementation NestopiaCore
@synthesize gamePath;
@synthesize resolution;
@synthesize controllerLayout;

-(id)init {
    self = [ super init ];
    if (self != nil) {
        resolution.width = 0;
        resolution.height = 0;
    }
    return self;
}

-(BOOL)initializeCore {
	void* userData = (void*) 0xDEADC0DE;
	Nes::Api::Machine machine( emulator );

    gameFilename = [ gamePath copy ];
    saveFilename = [ gameFilename stringByAppendingPathExtension: @"sav" ];
    soundLock = [ [ NSLock alloc ] init ];
    isPlaying = false;

    Nes::Api::Sound::Output::lockCallback.Set( SoundLock, userData );
	Nes::Api::Sound::Output::unlockCallback.Set( SoundUnlock, userData );
	Nes::Api::Video::Output::lockCallback.Set( VideoLock, userData );
	Nes::Api::Video::Output::unlockCallback.Set( VideoUnlock, userData );
	Nes::Api::User::fileIoCallback.Set( DoFileIO, userData );
    
    [ self loadDatabase ];
    
    if ([ self loadGame ] == NO) {
        isPlaying = false;
        return NO;
    }
    
    [ self initializeVideo ];
    [ self initializeSound ];
    [ self initializeInput ];

    return YES;
}

-(void)initializeSound {
    
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

-(void)initializeInput
{
    NSLog(@"%s: %d", __PRETTY_FUNCTION__, controllerLayout);
 
    controller = 0;
	Nes::Api::Cartridge::Database database( emulator );
    if (controllerLayout == 0) {
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

-(BOOL)initializeVideo {
	
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
	
	int videoMode = [ [ NSUserDefaults standardUserDefaults] integerForKey: @"video" ];

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
	
	cur_width = Widths[filter];
	cur_height = Heights[filter];

    NSLog(@"%s initializing resolution %dx%d", __PRETTY_FUNCTION__, cur_width, cur_height);
	renderState.bits.count  = 16;
	renderState.bits.mask.r = 0xF800;
	renderState.bits.mask.g = 0x07E0;
	renderState.bits.mask.b = 0x001F;
	renderState.filter      = filters[filter];
	renderState.width       = cur_width;
	renderState.height      = cur_height;
    
	videoScreen = new unsigned char[cur_width * cur_height * 2];
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

-(void)loadDatabase
{
	Nes::Api::Cartridge::Database database( emulator );
	NSString* datFile = [ [ NSBundle mainBundle ] pathForResource:@"NstDatabase" ofType:@"dat"];
	
	std::ifstream *nstDBFile;
	
	nstDBFile = new std::ifstream([ datFile UTF8String ], std::ifstream::in|std::ifstream::binary);
	
	if (nstDBFile->is_open())
	{
		database.Load(*nstDBFile);
		database.Enable(true);
	} else {
        NSLog(@"%s could not load database", __func__);
    }

	delete nstDBFile;
}

- (BOOL)loadGame
{
	Nes::Api::Machine machine( emulator );
	Nes::Api::Cartridge::Database database( emulator );
    
	if (database.IsLoaded())
	{
        NSData *gameData = [ NSData dataWithContentsOfFile: gamePath ];
        if (gameData) {
            dbentry = database.FindEntry([ gameData bytes ], [ gameData length ]);
		}
	}
	
	std::ifstream *file = new std::ifstream([ gamePath UTF8String ] , std::ios::in|std::ios::binary );
    
	Nes::Result result = machine.Load( *file );
	if (NES_FAILED(result)) {
        return NO;
    }
    
    delete file;

	machine.Power( true );
    return YES;
}

- (void)loadState
{
	Nes::Api::Machine machine( emulator );
	Nes::Api::Cartridge::Database database( emulator );
    NSString *savPath = [ gameFilename stringByAppendingPathExtension: @"sav" ];

	std::ifstream *file = new std::ifstream([ savPath UTF8String ] , std::ios::in|std::ios::binary );
    
	machine.LoadState( *file );
    
    delete file;
}

- (void)saveState
{
	Nes::Api::Machine machine( emulator );
	Nes::Api::Cartridge::Database database( emulator );
    NSString *savPath = [ gameFilename stringByAppendingPathExtension: @"sav" ];

    std::ostringstream *buffer = new std::ostringstream(std::stringstream::out | std::stringstream::binary);
    
    NSLog(@"%s calling SaveState(%@)", __PRETTY_FUNCTION__, savPath);
    machine.SaveState( *buffer );
    
    NSData *data = [NSData dataWithBytes:buffer->str().c_str() length:buffer->tellp()];
    [data writeToFile:savPath atomically:YES];

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

- (void)startEmulation {
    isPlaying = true;

    Nestopia_Callback_OpenSound(callback, 735, 44100);

    gameTimer = [ NSTimer scheduledTimerWithTimeInterval: (1.0 / framerate) target: self selector: @selector(stepEmulator:) userInfo: nil repeats: YES ];
}

-(void)stepEmulator:(NSTimer *)timer {
    
    if (isPlaying) {
        Nestopia_Callback_InputPadState(callback,
                                       &controls.pad[controller].buttons,
                                       &controls.zapper.fire,
                                       &controls.zapper.x,
                                       &controls.zapper.y);

        emulator.Execute(nstVideo, nstSound, &controls);
    } else {
        [ timer invalidate ];
    }
}

-(void)stopEmulation
{
    isPlaying = NO;
    Nestopia_Callback_CloseSound(callback);
}

- (void)finishEmulation {
	Nes::Api::Machine machine( emulator );
	machine.Power(false);
    delete videoScreen;
    delete nstSound;
    delete nstVideo;
}

-(void)setDelegate:(id)delegate {
    callback = (__bridge void *)delegate;
}

-(id)delegate {
    return (__bridge id)callback;
}

-(void)applyCheatCodes:(NSArray *)codes
{
    Nes::Api::Cheats cheater(emulator);
    cheater.ClearCodes();
    for (NSString *code in codes) {
        if (![ code isEqualToString: @"" ])
        {
            Nes::Api::Cheats::Code ggCode;
            Nes::Api::Cheats::GameGenieDecode([ code cStringUsingEncoding: NSASCIIStringEncoding ], ggCode);
            cheater.SetCode(ggCode);
        }
    }
}

-(void)activatePad1 {
    Nes::Api::Input(emulator).ConnectController(1, Nes::Api::Input::ZAPPER);
    controller = 0;
}

-(void)activatePad2 {
    Nes::Api::Input(emulator).ConnectController(1, Nes::Api::Input::PAD2 );
    controller = 1;
}
 
@end

static bool NST_CALLBACK SoundLock(void* userData,Nes::Api::Sound::Output& sound)
{
	if ([ soundLock tryLock ]) {
        nstSound->samples[0] = soundBuffer + soundOffset;
		return true;
    }
	return false;
}

static void NST_CALLBACK SoundUnlock(void* userData,Nes::Api::Sound::Output& sound)
{
    Nestopia_Callback_OutputSample(callback, sound.length[0], soundBuffer + soundOffset);
    if (soundOffset) {
        soundOffset = 0;
    } else {
        soundOffset = 16384;
    }
    nstSound->samples[0] = soundBuffer+soundOffset;
    
	[ soundLock unlock ];
}

static bool NST_CALLBACK VideoLock(void* userData, Nes::Api::Video::Output& video)
{
	video.pixels = videoScreen;
	video.pitch = cur_width * 2;
	return true;
}

static void NST_CALLBACK VideoUnlock(void* userData, Nes::Api::Video::Output& video)
{
    video.pixels = NULL;
    Nestopia_Callback_OutputFrame(callback, videoScreen);
}

static void NST_CALLBACK DoFileIO(void* userData,Nes::Api::User::File operation,Nes::Api::User::FileData& data)
{
	switch (operation)
	{
		case Nes::Api::User::FILE_LOAD_BATTERY:
		case Nes::Api::User::FILE_LOAD_EEPROM:
		{
			NSAutoreleasePool *pool = [ [ NSAutoreleasePool alloc ] init ];
            NSString *batSaveFile = [ gameFilename stringByAppendingPathExtension: @"sram" ];
			NSData *fileData = [ NSData dataWithContentsOfFile: batSaveFile ];
			
			data.resize( [ fileData length ] );
			memcpy(&data.front(), [ fileData bytes ],[ fileData length ]);
			[ pool release ];

			break;
		}
			
		case Nes::Api::User::FILE_SAVE_BATTERY: // save battery data to a file
		case Nes::Api::User::FILE_SAVE_EEPROM: // can be treated the same as battery files
		{
            NSAutoreleasePool *pool = [ [ NSAutoreleasePool alloc ] init ];
            NSString *batSaveFile = [ gameFilename stringByAppendingPathExtension: @"sram" ];
			NSData *fileData = [ NSData dataWithBytes: (const char*)&data.front() length: data.size() ];
			
            [ fileData writeToFile: batSaveFile atomically: NO ];
			[ pool release ];
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

/* Nestopia callback hooks */
void Nestopia_Callback_OutputFrame(void *sender, void *video)
{
    EmulatorCore *sharedEmulatorCore = (__bridge EmulatorCore *)sender;
	[ sharedEmulatorCore emulatorCallbackOutputFrame: (word *)video frameCount: 1 ];
}

int Nestopia_Callback_OpenSound(void *sender, int samples_per_sync, int sample_rate) {
	EmulatorCore *sharedEmulator = (__bridge EmulatorCore *)sender;
	return [ sharedEmulator emulatorCallbackOpenSound: samples_per_sync sampleRate: sample_rate ];
}

void Nestopia_Callback_OutputSample(void *sender, int samples, void *audio) {
	EmulatorCore *sharedEmulator = (__bridge EmulatorCore *)sender;
	[ sharedEmulator emulatorCallbackOutputSampleWave: samples wave1: (short *)audio ];
}

void Nestopia_Callback_CloseSound(void *userData) {
	EmulatorCore *sharedEmulator = (__bridge EmulatorCore *) userData;
	[ sharedEmulator emulatorCallbackCloseSound ];
}

uint Nestopia_TranslateButtons(uint pad) {
    uint out = 0;
    
    if (pad & NCTL_A)
        out ^= Nes::Api::Input::Controllers::Pad::A;
    if (pad & NCTL_B)
        out ^= Nes::Api::Input::Controllers::Pad::B;
    if (pad & NCTL_UP)
        out ^= Nes::Api::Input::Controllers::Pad::UP;
    if (pad & NCTL_DOWN)
        out ^= Nes::Api::Input::Controllers::Pad::DOWN;
    if (pad & NCTL_LEFT)
        out ^= Nes::Api::Input::Controllers::Pad::LEFT;
    if (pad & NCTL_RIGHT)
        out ^= Nes::Api::Input::Controllers::Pad::RIGHT;
    if (pad & NCTL_SELECT)
        out ^= Nes::Api::Input::Controllers::Pad::SELECT;
    if (pad & NCTL_START)
        out ^= Nes::Api::Input::Controllers::Pad::START;
    
    return out;
}

void Nestopia_Callback_InputPadState(void *userData, uint *pad1, uint *zapper, uint *x, uint *y)
{
    uint p1, p2;
	EmulatorCore *sharedEmulator = (__bridge EmulatorCore *) userData;
	[ sharedEmulator emulatorCallbackInputPadState: &p1 pad2: &p2 zapper: zapper x: x y: y ];
    
    *pad1 = Nestopia_TranslateButtons(p1);
    
    if (controls.zapper.fire) {
        NSLog(@"%s zapper: %d at %ux%u", __func__, controls.zapper.fire, *x, *y);
    }
}

static bool NST_CALLBACK ZapperCallback(void* userData, Nes::Core::Input::Controllers::Zapper& zapper)
{
    return YES;
}


