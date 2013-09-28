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

#import <sys/time.h>
#import "EmulatorCore.h"
#import "NESCore.h"

static EmulatorCore *sharedEmulatorCoreInstance = nil;
extern word NESPalette[64];
void setActiveFrameBuffer(unsigned long *buf);

@implementation EmulatorCore
@synthesize currentROMImagePath;
@synthesize frameBufferAddress;
@synthesize frameBufferSize;
@synthesize screenDelegate;

extern NSString *currentGamePath;

+ (EmulatorCore *)sharedEmulatorCore {
	return sharedEmulatorCoreInstance;
}

- (id)init {
	self = [ super init ];
	if (self != nil) {
		sharedEmulatorCoreInstance = self;
		soundBuffersInitialized = 0;
		currentROMImagePath = nil;
	}
	
	return self;
}

+ (NSDictionary *)gameSettings {
    if (currentGamePath) {
        NSString *currentGameName = [ [ [ [ currentGamePath lastPathComponent ] stringByReplacingOccurrencesOfString: @".sav" withString: @"" ] stringByReplacingOccurrencesOfString: @".nes" withString: @"" ] copy ];
        NSString *path = [ NSString stringWithFormat: @"%@/%@.plist", ROM_PATH, currentGameName ];
        NSDictionary *gameSettings = [ NSDictionary dictionaryWithContentsOfFile: path ];
        if (gameSettings) {
            return gameSettings;
        }
    }
    
    return [ [ NSUserDefaults standardUserDefaults ] dictionaryRepresentation ];
}

- (EmulatorCoreLoadStatus)loadROM:(NSString *)imagePath {
	currentROMImagePath = [ imagePath copy ];

	char filename[strlen([ currentROMImagePath cStringUsingEncoding: NSASCIIStringEncoding ]) +1];
	strcpy(filename, [ currentROMImagePath cStringUsingEncoding: NSASCIIStringEncoding ]);
	int err;
	
	NSLog(@"%s loading image %@\n", __func__ , currentROMImagePath);
	
	err = NESCore_LoadROM(filename);
	if (err) {
		switch (err) {
			case(E_UNSUPPORTED_MAPPER):
				return EmulatorCoreLoadStatusROMNotSupported;
				break;
			default:
				return EmulatorCoreLoadStatusInvalidROM;
				break;
		}
	}
	return EmulatorCoreLoadStatusSuccess;
}


/* Pixel maps are used to map a stretched screen pixel to a game pixel.
 * This avoids doing a lot of math during frame render */

- (void)initializePixelMappers {
    int dest_h, dest_w;
    int px_x, px_y, x, y;
    int px_src, px_dest;
	
    /* 16x9 Full Screen Landscape */
    dest_w = 479;
    dest_h = 320;
    for (y=0; y < dest_h; y++)
    {
		px_y = y * NESCORE_NATIVE_SCREEN_HEIGHT / dest_h;
		for (x=0; x < dest_w; x++) {
			px_x = x * NESCORE_NATIVE_SCREEN_WIDTH / dest_w;
			px_dest = dest_h + ((x * dest_h) - y);
			px_src = (px_y * NESCORE_NATIVE_SCREEN_WIDTH) + px_x;
			
			pixelMap[y][x][0][0] = px_dest;
			pixelMap[y][x][1][0] = px_src;
		}
    }
	
    /* 4x3 Full Screen Landscape */
    dest_w = 341;
    dest_h = 320;
    for (y=0; y < dest_h; y++)
    {
		px_y = y * NESCORE_NATIVE_SCREEN_HEIGHT / dest_h;
		for (x=0; x < dest_w; x++) {
			px_x = x * NESCORE_NATIVE_SCREEN_WIDTH / dest_w;
			px_dest = dest_h + ((x * dest_h) - y);
			px_src = (px_y * NESCORE_NATIVE_SCREEN_WIDTH) + px_x;
			
			pixelMap[y][x][0][1] = px_dest;
			pixelMap[y][x][1][1] = px_src;
		}
    }
}

- (void)initializePalette {
	
	if ([[ [ EmulatorCore gameSettings ] objectForKey: @"paletteControl" ] intValue ]== 0) {
        word palette[64] = 
		{
			0x6b5b, 0x0123, 0x0029, 0x3823, 0x681b, 0x700f, 0x6801, 0x5881,
			0x4101, 0x3181, 0x0241, 0x0211, 0x01d7, 0x0000, 0x0000, 0x0000,
			0xadb7, 0x1af5, 0x31bf, 0x78fb, 0xb817, 0xe00b, 0xc8c7, 0xca41,
			0x7a81, 0x5301, 0x0401, 0x0399, 0x0411, 0x0000, 0x0000, 0x0000,
			0xffdf, 0x3ddf, 0x5c9f, 0x445f, 0xf3df, 0xfb96, 0xfb8c, 0xfc8d,
			0xf5c7, 0x8682, 0x4ec9, 0x5fd3, 0x075b, 0x0000, 0x0000, 0x0000,
			0xffdf, 0xaf1f, 0xc69f, 0xd65f, 0xfe1f, 0xfe1b, 0xfdd6, 0xfed5,
			0xff14, 0xef67, 0xbfef, 0xa77b, 0x9f3f, 0xbdef, 0x0000, 0x0000
		};
        memcpy(&NESPalette, &palette, sizeof(NESPalette));
    }
	
    if ([[ [ EmulatorCore gameSettings ] objectForKey: @"paletteControl" ] intValue ] == 1) {
		word palette[64] = 
		{
			0x738e, 0x20d1, 0x0015, 0x4013, 0x880e, 0xa802, 0xa000, 0x7840,
			0x4140, 0x0200, 0x0280, 0x01c2, 0x19cb, 0x0000, 0x0000, 0x0000,
			0xbdd7, 0x039d, 0x21dd, 0x801e, 0xb817, 0xe00b, 0xd940, 0xca41,
			0x8b80, 0x0480, 0x0540, 0x0487, 0x0411, 0x0000, 0x0000, 0x0000,
			0xffdf, 0x3ddf, 0x5c9f, 0x445f, 0xf3df, 0xfb96, 0xfb8c, 0xfcc7,
			0xf5c7, 0x8682, 0x4ec9, 0x5fd3, 0x075b, 0x0000, 0x0000, 0x0000,
			0xffdf, 0xaf1f, 0xc69f, 0xd65f, 0xfe1f, 0xfe1b, 0xfdd6, 0xfed5,
			0xff14, 0xe7d4, 0xaf97, 0xb7d9, 0x9fde, 0x0000, 0x0000, 0x0000
		};
		memcpy(&NESPalette, &palette, sizeof(NESPalette));
		return;
		
	}
	
    if ([[ [ EmulatorCore gameSettings ] objectForKey: @"paletteControl" ] intValue ] == 2) {
		word palette[64] = 
		{
			0x528a, 0x0010, 0x0811, 0x280f, 0x4809, 0x5000, 0x4000, 0x2040, 
			0x0900, 0x0160, 0x0180, 0x0121, 0x00e9, 0x0000, 0x0000, 0x0000, 
			0xa534, 0x01d9, 0x30bd, 0x583b, 0x8816, 0x9809, 0x90c0, 0x71a0, 
			0x4aa0, 0x0b60, 0x03a0, 0x0365, 0x02f0, 0x0000, 0x0000, 0x0000, 
			0xffff, 0x4cff, 0x7bdf, 0xa33f, 0xdadf, 0xf2b8, 0xf34a, 0xd422, 
			0xbd20, 0x7600, 0x4663, 0x2e4c, 0x3617, 0x39c7, 0x0000, 0x0000, 
			0xffff, 0xb6df, 0xce5f, 0xde1f, 0xf5ff, 0xfdfd, 0xfe18, 0xf674, 
			0xe6d2, 0xcf31, 0xbf74, 0xaf57, 0xaf5c, 0xb596, 0x0000, 0x0000
		};
        memcpy(&NESPalette, &palette, sizeof(NESPalette));
		return;
	}
	
	if ([[ [ EmulatorCore gameSettings ] objectForKey: @"paletteControl" ] intValue ] == 3) {
		word palette[64] = 
		{
			0x7390, 0x0911, 0x2874, 0x3015, 0x584e, 0x580a, 0x7065, 0x6140, 
			0x39e0, 0x2260, 0x02c0, 0x0ac3, 0x020c, 0x0000, 0x1082, 0x2105, 
			0xb5b8, 0x02fc, 0x429f, 0x5aba, 0x9977, 0xa010, 0xa989, 0x9a83, 
			0x6b42, 0x43a2, 0x2440, 0x2c49, 0x1b92, 0x2125, 0x0000, 0x0000, 
			0xe73f, 0x653f, 0x74df, 0x9cbf, 0xd37f, 0xf3bb, 0xfcd8, 0xe48b, 
			0xbd85, 0xa689, 0x764b, 0x5630, 0x5ddd, 0x422a, 0x0000, 0x0000, 
			0xe73f, 0xb67f, 0xb61f, 0xcd9f, 0xedbe, 0xfe3f, 0xfe5d, 0xfe9a, 
			0xf739, 0xdf99, 0xcfbc, 0xcf7d, 0xbf3f, 0xd69b, 0x0000, 0x0000
		};
        memcpy(&NESPalette, &palette, sizeof(NESPalette));
		return;
	}
}

- (BOOL)initializeEmulator {
	[ self initializePixelMappers ];
	[ self initializePalette ];
		
	S.userData = self;
	NESCore_Init();
	
	if ([[ [ EmulatorCore gameSettings ] objectForKey: @"cpuCycle" ] intValue ] == 0) {
		S.ClockCycles = 339;
	} else {
		S.ClockCycles = 341;
	}
	
	NESCore_Reset();

	controllerP1 = controllerP2 = 0;
	zapperState = zapperX = zapperY = S.Zapper_Sync = 0;
	frameTimer.tv_sec = frameTimer.tv_usec = 0;
	
	[ self loadSRAM ];
	
	return YES;
}

- (BOOL)configureEmulator {
	
	NSLog(@"%s", __func__);
	
	S.APU_Mute = 0;
	S.userData = self;
	S.GameGenie = 0;
	
	defaultFrameSkip = [[[ EmulatorCore gameSettings ] objectForKey: @"frameSkip" ] intValue ];
	defaultFullScreen = [[[ EmulatorCore gameSettings ] objectForKey: @"fullScreen" ] intValue ];
	defaultAspectRatio = [[[ EmulatorCore gameSettings ] objectForKey: @"aspectRatio" ] intValue ];
	
    if ([ [ EmulatorCore gameSettings ] objectForKey: @"fullScreen" ] == nil) {
        defaultFullScreen = 1;
    }
    
	NSLog(@"%s loading defaults frameSkip: %d fullScreen: %d aspectRatio: %d\n", __func__, defaultFrameSkip, defaultFullScreen, defaultAspectRatio);
	
	if (defaultFullScreen) {
		destinationWidth = (defaultAspectRatio) ? 341 : 479;
		destinationHeight = 320;
	} else {
		destinationWidth = 255;
		destinationHeight = 240;
	}
	
	if (defaultFrameSkip == 5) {
		S.FrameSkip = 0;
	} else {
		S.FrameSkip = defaultFrameSkip;
	}
	
	if ([[ [ EmulatorCore gameSettings ] objectForKey: @"bassBoost" ] boolValue ] == YES) {
		S.BassBoost = 1;
	} else {
		S.BassBoost = 0;
	}
	
	return YES;
}

- (int)applyGameGenieCodes {
	int codesAccepted = 0;
	
	S.GameGenie = ([[ [ EmulatorCore gameSettings ] objectForKey: @"gameGenie" ] boolValue ] == YES) ? 1 : 0;
	memset(&GG, 0, sizeof(GG));
	if (S.GameGenie) {
		NSLog(@"%s loading Game Genie codes\n", __func__);
		for(int i = 0; i < 4; i++) {
			NSString *code = [ [ EmulatorCore gameSettings ] objectForKey: [ NSString stringWithFormat: @"gameGenieCode%d", i ] ];
			if (code != nil) {
				strncpy(GG[i].code, [ code cStringUsingEncoding: NSASCIIStringEncoding ], [ code length ]);
				NSLog(@"%s applying game genie code %@", __func__, code);
			}	
			GG[i].enabled = 0;
			GG[i].wAddr = 0;
			
			if (GG[i].code[0]) {
				if ((!strcmp(GG[i].code, "(Empty)")))
					GG[i].code[0] = 0;
				
				/* Calculate Game Genie Address / Data / Compare Bytes */
				if (strlen(GG[i].code)!=6 && strlen(GG[i].code)!=8 && GG[i].code[0] != 0)
				{
					NSLog(@"%s invalid game genie code %d: %s\n", __func__, i, GG[i].code);
					continue;
				}
				
				if (GG[i].code[0] != 0) {
					BOOL badcode = NO;
					byte n[8];
					for(int j=0;j<strlen(GG[i].code);j++) {
						switch (toupper(GG[i].code[j])) {
							case 'A':
								n[j] = 0x0;
								break;
							case 'P':
								n[j] = 0x1;
								break;
							case 'Z':
								n[j] = 0x2;
								break;
							case 'L':
								n[j] = 0x3;
								break;
							case 'G':
								n[j] = 0x4;
								break;
							case 'I':
								n[j] = 0x5;
								break;
							case 'T':
								n[j] = 0x6;
								break;
							case 'Y':
								n[j] = 0x7;
								break;
							case 'E':
								n[j] = 0x8;
								break;
							case 'O':
								n[j] = 0x9;
								break;
							case 'X':
								n[j] = 0xA;
								break;
							case 'U':
								n[j] = 0xB;
								break;
							case 'K':
								n[j] = 0xC;
								break;
							case 'S':
								n[j] = 0xD;
								break;
							case 'V':
								n[j] = 0xE;
								break;
							case 'N':
								n[j] = 0xF;
								break;
							default:
								badcode = YES;
								continue;
								break;
						}
					}
					
					if (badcode == YES) {
						NSLog(@"%s invalid game genie code %d: %s\n", __func__, i, GG[i].code);
						continue;
					}					
					
					codesAccepted++;
					GG[i].wAddr =
                    0x8000 +
					(((n[3] & 7) << 12)
					 | ((n[5] & 7) << 8)
					 | ((n[4] & 8) << 8)
					 | ((n[2] & 7) << 4)
					 | ((n[1] & 8) << 4)
					 | (n[4] & 7)
					 | (n[3] & 8));
					
					if (strlen(GG[i].code) == 6) {
						GG[i].data = ((n[1] & 7) << 4)
						| ((n[0] & 8) << 4)
						| (n[0] & 7)
						| (n[5] & 8);
						
						GG[i].compare = 0x00;
						GG[i].length = 6;
						GG[i].enabled = 1;
					}
					else
					{
						GG[i].data =
                        ((n[1] & 7) << 4)
						| ((n[0] & 8) << 4)
						| (n[0] & 7)
						| (n[7] & 8);
						
						GG[i].compare =
                        ((n[7] & 7) << 4)
						| ((n[6] & 8) << 4)
						| (n[6] & 7)
						| (n[5] & 8);
						
						GG[i].length = 8;
						GG[i].enabled = 1;
					}
					
				}
			}
		}
	} /* if (S.GameGenie) */
	
	return codesAccepted;
}	

- (BOOL)saveState {
	NSString *savePath = [ NSString stringWithFormat: @"%@.sav", currentROMImagePath ];
	int err;
	
	NSLog(@"%s saving state to %@\n", __func__, savePath);
	err = NESCore_SaveState([ savePath cStringUsingEncoding: NSASCIIStringEncoding ]);
    if (err) {
		NSLog(@"%s save state error %d: %s\n", __func__, err, strerror(errno));
		return NO;
	}
	return YES;
}

- (BOOL)loadState {
	NSString *loadPath = [ NSString stringWithFormat: @"%@.sav", currentROMImagePath ];
	int err;
	
	NSLog(@"%s loading state from %@\n", __func__, loadPath);
	err = NESCore_LoadState([ loadPath cStringUsingEncoding: NSASCIIStringEncoding]);
	if (err) {
		NSLog(@"%s load state error %d\n", __func__, err);
		return NO;
	}
	
	S.userData = self;
	return YES;
}

- (BOOL)saveSRAM {
	NSString *sramPath;
	FILE *fh;
	
	if (!S.ROM_SRAM) {
		NSLog(@"%s image has no sram\n", __func__);
		return -1;
	}
	
	sramPath = [ NSString stringWithFormat: @"%@.sram", currentROMImagePath ];
	
	NSLog(@"%s saving sram to %@\n", __func__, sramPath);
	fh = fopen([ sramPath cStringUsingEncoding: NSASCIIStringEncoding], "wb");
	if (fh == NULL) {
		NSLog(@"%s save sram failed\n", __func__);
		return NO;
	}
	
	fwrite(S.SRAM, SRAM_SIZE, 1, fh);
	fclose(fh);
	return YES;
}

- (BOOL)loadSRAM {
	NSString *sramPath = [ NSString stringWithFormat: @"%@.sram", currentROMImagePath ];
	FILE *fh;
	
	if (!S.ROM_SRAM) return -1;
	NSLog(@"%s loading sram from %@\n", __func__, sramPath);
	
	fh = fopen([ sramPath cStringUsingEncoding: NSASCIIStringEncoding ], "rb");
	if (fh == NULL) {
		NSLog(@"%s load sram failed\n", __func__);
		return NO;
	}
	
	fread(S.SRAM, SRAM_SIZE, 1, fh);
	fclose(fh);
	return YES;
	
}

- (void)dealloc {
	[ currentROMImagePath release ];
    [ super dealloc ];
}

/* Callbacks */

- (void)emulatorCallbackWait {
	double frameRate = (S.PAL) ? 53.355 : 60.098;
    double usec_per_frame = 1000000 * (1.0 / frameRate);
	struct timeval tv;
	
    if (S.FrameSkip > 0) {
        usec_per_frame *= (S.FrameSkip + 1);
    }
    
    tv.tv_usec = usec_per_frame - 1000.0;
    tv.tv_sec = 0.0;
    select(0, NULL, NULL, NULL, &tv);
    return;
}

- (void) emulatorCallbackOutputFrame:(word *)workFrame frameCount:(byte)frameCount {
    word pixelColor;
	int x, y;
	
    /* Register hits on the zapper gun */
	{
		/* Check for blank */
        if (S.Zapper_Sync) {
			
			if (S.PAD_Zapper) {
				zapperState = 0;
				//	NSLog(@"%s zapperState reset, sync = %d", __func__, zapperState, S.Zapper_Sync);
			}
			
			//	NSLog(@"%s checkZapper frame: %d", __func__, S.Zapper_Sync);
			
			if (screenBlank == NO) {
				if (!(S.PPU_R1 & R1_SHOW_BG)) {
					screenBlank = YES;
				} else { /* Scan a few lines for games that don't turn off background */
					screenBlank = YES;
					for(int x=0; x<1024; x++) {
						if (workFrame[(120*256)+x] != 0x0000) {
							screenBlank = NO;
							break;
						}
					}
				}
			}
			
			/* Check for light if screen is blank */
			if (screenBlank == YES) {
				for(x=-5;x<5;x++) {
					for(y=-5;y<5;y++) {
						if (   zapperX+x >= 0
							&& zapperX+x < 256
							&& zapperY+y >= 0
							&& zapperY+y < 239)
						{
							pixelColor = workFrame[((zapperY+y)*256) + (zapperX+x)];
							workFrame[((zapperY+y) * 256) + (zapperX+x)] = 0xf800;
							if (pixelColor)
								zapperState |= 0x08;
						}
					}
				}
			}
			
			S.Zapper_Sync--;
			
            if (S.Zapper_Sync == 0) {
				zapperX = zapperY = zapperState = S.PAD_Zapper = 0;
				screenBlank = NO;
			}
        } else if (S.Zapper_Sync == 0 && S.PAD_Zapper) {
			//			NSLog(@"%s starting zapper sync countdown", __func__);
			S.Zapper_Sync = 10;
		}
    }

	/* Render */
	if (screenDelegate != nil) {
		if (defaultFullScreen == NO) {
			memcpy(frameBufferAddress, workFrame, 122368); 
			/* 256 * 239 * 2, but should be 240 */
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

		[ screenDelegate performSelectorOnMainThread: @selector(emulatorCoreDidUpdateFrameBuffer) 
											  withObject:nil waitUntilDone: NO ];
	} else {
		NSLog(@"%s screenDelegate = nil, skipping render", __func__);
	}
}

- (void)emulatorCallbackInputPadState:(dword *)pad1 pad2:(dword *)pad2 {
	*pad1 = controllerP1;
	*pad2 = controllerP2 | zapperState;
	if (zapperState & 0x10) {
		//		NSLog(@"%s PAD_Zapper = 1, zapperState: %d, controllerP2: %d", __func__, zapperState, controllerP2);
		S.PAD_Zapper = 1;
	}
	
	if (zapperState & 0x08) {
		zapperX = zapperY = zapperState = S.PAD_Zapper = S.Zapper_Sync = 0;
		screenBlank = NO;
	}
}	

- (void)AQBufferCallback:(void *)callbackStruct inQ:(AudioQueueRef)inQ outQB:(AudioQueueBufferRef)outQB {
	AQCallbackStruct * inData;
    short *coreAudioBuffer;
    short sample;
    int i;

    inData = (AQCallbackStruct *)callbackStruct;
    coreAudioBuffer = (short*) outQB->mAudioData;
	
    if (inData->frameCount > 0) {
        outQB->mAudioDataByteSize = 4*inData->frameCount;
        for(i=0;i<inData->frameCount*2;i+=2) {
            sample = waveBuffer[playNeedle] << 8;
            coreAudioBuffer[i] =   sample;
            coreAudioBuffer[i+1] = sample;
			playNeedle++;
            if (playNeedle == sizeof(waveBuffer))
				playNeedle = 0;
        }
        AudioQueueEnqueueBuffer(inQ, outQB, 0, NULL);
  }
}

- (int)emulatorCallbackOpenSound:(int)samplesPerSync sampleRate:(int)sampleRate {

	memset(&waveBuffer, 0, sizeof(waveBuffer));
    writeNeedle = playNeedle = soundBuffersInitialized = 0;
	
    audioCallback.mDataFormat.mSampleRate = 44100.0;
    audioCallback.mDataFormat.mFormatID = kAudioFormatLinearPCM;
    audioCallback.mDataFormat.mFormatFlags = kLinearPCMFormatFlagIsSignedInteger
	| kAudioFormatFlagIsPacked;
    audioCallback.mDataFormat.mBytesPerPacket = 4;
    audioCallback.mDataFormat.mFramesPerPacket = 1;
    audioCallback.mDataFormat.mBytesPerFrame = 4;
    audioCallback.mDataFormat.mChannelsPerFrame = 2;
    audioCallback.mDataFormat.mBitsPerChannel = 16;
	audioCallback.userData = self;
	
	[ self initializeSoundBuffers ];
	
    return 0;
}

- (void)emulatorCallbackCloseSound {
    AudioQueueDispose(audioCallback.queue, YES);
}

- (void)emulatorCallbackOutputSample:(int)samples wave1:(byte *)wave1 wave2:(byte *)wave2 wave3:(byte *)wave3 wave4:(byte *)wave4 wave5:(byte *)wave5 {

	for (int i = 0; i < samples; i++)
	{
		waveBuffer[writeNeedle++] = (wave1[i] + wave2[i] + wave3[i] + wave4[i] + wave5[i]) / 5;
		
		if (writeNeedle == sizeof(waveBuffer))
			writeNeedle = 0;
	}
	
//	if (soundBuffersInitialized == requiredBuffersToOpenSound)
//		return;
//	[ self initializeSoundBuffers ];
	
}

- (void) initializeSoundBuffers {	
	defaultSoundBuffer = [ [ [ EmulatorCore gameSettings ] objectForKey: @"soundBuffer" ] intValue ];
	requiredBuffersToOpenSound = defaultSoundBuffer + 3;
	NSLog(@"%s required buffers: %d", __func__, requiredBuffersToOpenSound);

	soundBuffersInitialized = requiredBuffersToOpenSound;
	{
		/* Pre-buffer before we turn on audio */
		UInt32 err;
		err = AudioQueueNewOutput(&audioCallback.mDataFormat,
								  AQBufferCallback,
								  &audioCallback,
								  NULL,
								  kCFRunLoopCommonModes,
								  0,
								  &audioCallback.queue);
		if (err) 
			NSLog(@"%s AudioQueueNewOutput error %ld", __func__, err);
		
		audioCallback.frameCount = WAVE_BUFFER_SIZE;
		UInt32 bufferBytes = audioCallback.frameCount * audioCallback.mDataFormat.mBytesPerFrame;
		
		NSLog(@"%s initializing %d sound buffers\n", __func__, requiredBuffersToOpenSound);
		for (int i = 0; i < soundBuffersInitialized; i++) {
			err = AudioQueueAllocateBuffer(audioCallback.queue, bufferBytes, &audioCallback.mBuffers[i]);
			if (err)
				NSLog(@"%s AudioQueueAllocateBuffer[%ld] error %ld\n", __func__, bufferBytes, err);
			/* "Prime" by calling the callback once per buffer */
			AQBufferCallback (&audioCallback, audioCallback.queue, audioCallback.mBuffers[i]);
		}
		
		NSLog(@"%s calling AudioQueueStart\n", __func__);
		err = AudioQueueStart(audioCallback.queue, NULL);
		if (err)
			NSLog(@"%s AudioQueueStart error %ld\n", __func__, err);
	}
}

- (void)startEmulator {	
	soundBuffersInitialized = 0;
	pthread_create(&emulation_tid, NULL, emulation_thread, NULL);
}

- (void)restartEmulator {
    S.FrameSkip = [[[ EmulatorCore gameSettings ] objectForKey: @"frameSkip" ] intValue ];
    
    if ([[ [ EmulatorCore gameSettings ] objectForKey: @"cpuCycle" ] intValue ] == 0) {
		S.ClockCycles = 339;
	} else {
		S.ClockCycles = 341;
	}
    
    [ self initializePalette ];
    [ self applyGameGenieCodes ];
    
    if ([[ [ EmulatorCore gameSettings ] objectForKey: @"bassBoost" ] boolValue ] == YES) {
		S.BassBoost = 1;
	} else {
		S.BassBoost = 0;
	}
    
    screenDelegate = haltedScreenDelegate;
    soundBuffersInitialized = 0;
    [ self initializeSoundBuffers ];
	pthread_create(&emulation_tid, NULL, emulation_thread, NULL);
}

- (void)haltEmulator {
	
    haltedScreenDelegate = screenDelegate;
    screenDelegate = nil;
    
	NSLog(@"%s halting emulator\n", __func__);
	NESCore_Halt();
	
	NSLog(@"%s joining emulator thread\n", __func__);
	pthread_join(emulation_tid, NULL);
	
	NSLog(@"%s saving emulator SRAM\n", __func__);
	[ self saveSRAM ];
	
	NSLog(@"%s %d S.EmulatorRun\n", __func__, S.EmulatorRun); 
}

- (void)finishEmulator {
    haltedScreenDelegate = nil;
	NESCore_Finish();
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

@end

/* C-land callback hooks */

void NESCore_Callback_Wait(void *userData) {
	EmulatorCore *sharedEmulatorCore = (EmulatorCore *)userData;
	[ sharedEmulatorCore emulatorCallbackWait ];
}

void NESCore_Callback_OutputFrame(void *userData, word *WorkFrame, byte FrameCount) {
	EmulatorCore *sharedEmulatorCore = (EmulatorCore *)userData;
	[ sharedEmulatorCore emulatorCallbackOutputFrame: WorkFrame frameCount: FrameCount ];
}

void NESCore_Callback_InputPadState(void *userData, dword *pdwPad1, dword *pdwPad2) {
	EmulatorCore *sharedEmulator = (EmulatorCore *) userData;
	[ sharedEmulator emulatorCallbackInputPadState: pdwPad1 pad2: pdwPad2 ];
}

void AQBufferCallback(void *callbackStruct, AudioQueueRef inQ, AudioQueueBufferRef outQB)
{
	AQCallbackStruct *inData = (AQCallbackStruct *) callbackStruct;
	EmulatorCore *sharedEmulatorCore = (EmulatorCore *) inData->userData;
	[ sharedEmulatorCore AQBufferCallback: inData inQ: inQ outQB: outQB ];
}

void NESCore_Callback_InitSound(void *userData) {
	NSLog(@"%s", __func__);
}

int NESCore_Callback_OpenSound(void *userData, int samples_per_sync, int sample_rate) {
	EmulatorCore *sharedEmulator = (EmulatorCore *) userData;
	return [ sharedEmulator emulatorCallbackOpenSound: samples_per_sync sampleRate: sample_rate ];
}

void NESCore_Callback_CloseSound(void *userData) {
	EmulatorCore *sharedEmulator = (EmulatorCore *) userData;
	[ sharedEmulator emulatorCallbackCloseSound ];
}

void NESCore_Callback_OutputSample(void *userData, int samples, byte *wave1, byte *wave2, byte *wave3, byte *wave4, byte *wave5) {
	EmulatorCore *sharedEmulator = (EmulatorCore *) userData;
	[ sharedEmulator emulatorCallbackOutputSample: samples wave1: wave1 wave2: wave2 wave3: wave3 wave4: wave4 wave5: wave5 ];
}

void NESCore_Debug(const char *msg, ...) {
#ifdef DEBUG
	char debug_text[1024];
	va_list args;
	
	va_start (args, msg);
	vsnprintf (debug_text, sizeof (debug_text), msg, args);
	va_end (args);
	NSLog(@"%s %s", __func__, debug_text);
#endif
}

void *emulation_thread(void *args) {
    NESCore_Run();
	return NULL;
}
