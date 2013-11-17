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

#ifndef __NESTOPIACORE_H__
#define __NESTOPIACORE_H__

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


@protocol NestopiaCoreAudioDelegate;
@protocol NestopiaCoreVideoDelegate;
@protocol NestopiaCoreInputDelegate;


typedef NS_OPTIONS(int, NestopiaPadInput) {
    NestopiaPadInputA       = 1 << 0,
    NestopiaPadInputB       = 1 << 1,
    NestopiaPadInputSelect  = 1 << 2,
    NestopiaPadInputStart   = 1 << 3,
    NestopiaPadInputUp      = 1 << 4,
    NestopiaPadInputDown    = 1 << 5,
    NestopiaPadInputLeft    = 1 << 6,
    NestopiaPadInputRight   = 1 << 7
};


typedef struct {
    NestopiaPadInput pad1;
    NestopiaPadInput pad2;
    int zapper;
    int zapperX;
    int zapperY;
} NestopiaInput;


@interface NestopiaCore : NSObject

+ (NestopiaCore *)sharedCore;

- (BOOL)powerOn;
- (void)startEmulation;
- (void)stopEmulation;
- (void)powerOff;

- (void)loadState;
- (void)saveState;

- (void)applyCheatCodes:(NSArray *)codes;
- (void)activatePad1;
- (void)activatePad2;
- (void)toggleCoin1;
- (void)toggleCoin2;
- (void)coinOff;

@property (nonatomic, readonly) CGSize nativeResolution;

@property (nonatomic, weak) id<NestopiaCoreAudioDelegate> audioDelegate;
@property (nonatomic, weak) id<NestopiaCoreVideoDelegate> videoDelegate;
@property (nonatomic, weak) id<NestopiaCoreInputDelegate> inputDelegate;

@property (nonatomic, copy) NSString *gamePath;
@property (nonatomic, copy) NSString *gameSavePath;
@property (nonatomic, assign) int controllerLayout; // TODO: enum

@end


@protocol NestopiaCoreAudioDelegate <NSObject>

- (void)nestopiaCoreCallbackOpenSound:(int)samplesPerSync sampleRate:(int)sampleRate;
- (void)nestopiaCoreCallbackOutputSamples:(int)samples waves:(short *)waves;
- (void)nestopiaCoreCallbackCloseSound;

@end


@protocol NestopiaCoreVideoDelegate <NSObject>

- (void)nestopiaCoreCallbackInitializeVideoWithWidth:(int)width height:(int)height;
- (void)nestopiaCoreCallbackOutputFrame:(unsigned short *)frameBuffer;
- (void)nestopiaCoreCallbackDestroyVideo;

@end


@protocol NestopiaCoreInputDelegate <NSObject>

- (NestopiaInput)nestopiaCoreCallbackInput;

@end

#endif