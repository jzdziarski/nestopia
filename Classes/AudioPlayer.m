//
//  AudioPlayer.m
//  Nestopia
//
//  Created by Nick Tymchenko on 11/17/13.
//
//

#import "AudioPlayer.h"
#import <AudioToolbox/AudioToolbox.h>

#define WAVE_BUFFER_SIZE 735
#define WAVE_BUFFER_BANKS 10
#define BUFFERSIZE (WAVE_BUFFER_SIZE * WAVE_BUFFER_BANKS)


typedef struct AQCallbackStruct {
    AudioQueueRef queue;
    UInt32 frameCount;
    AudioQueueBufferRef mBuffers[12];
    AudioStreamBasicDescription mDataFormat;
	void *userData;
} AQCallbackStruct;


@implementation AudioPlayer {
	AQCallbackStruct audioCallback;
	long writeNeedle, playNeedle;
	int soundBuffersInitialized;
	int requiredBuffersToOpenSound;
    short waveBuffer[WAVE_BUFFER_SIZE * WAVE_BUFFER_BANKS];
}

#pragma mark NestopiaCoreAudioDelegate

- (void)nestopiaCoreCallbackOpenSound:(int)samplesPerSync sampleRate:(int)sampleRate {
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
}

- (void)nestopiaCoreCallbackOutputSamples:(int)samples waves:(short *)waves {
    for (int i = 0; i < samples; i++) {
        waveBuffer[writeNeedle++] = waves[i];
        if (writeNeedle >= BUFFERSIZE-1)
            writeNeedle = 0;
    }
}

- (void)nestopiaCoreCallbackCloseSound {
    AudioQueueDispose(audioCallback.queue, YES);
    soundBuffersInitialized = 0;
}

#pragma mark Private

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

- (void)audioQueueBufferCallback:(void *)callbackStruct inQ:(AudioQueueRef)inQ outQB:(AudioQueueBufferRef)outQB {
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

void AQBufferCallback(void *callbackStruct, AudioQueueRef inQ, AudioQueueBufferRef outQB)
{
	AQCallbackStruct *inData = (AQCallbackStruct *)callbackStruct;
	AudioPlayer *audioPlayer = (__bridge AudioPlayer *) inData->userData;
	[audioPlayer audioQueueBufferCallback:inData inQ:inQ outQB:outQB];
}

@end
