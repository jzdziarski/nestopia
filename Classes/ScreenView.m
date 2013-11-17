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

#import "ScreenView.h"
#import <QuartzCore/QuartzCore.h>

@implementation ScreenView {
    unsigned long hightable[256], lowtable[256];
    
    int width, height;
    unsigned long *frameBuffer8888;
    CGColorSpaceRef colorSpace;
    CGDataProviderRef provider[2];
	int currentProvider;
}

#pragma mark Properties

- (void)setAntialiasing:(BOOL)antialiasing {
    _antialiasing = antialiasing;
    self.layer.minificationFilter = self.layer.magnificationFilter = antialiasing ? kCAFilterLinear : kCAFilterNearest;
}

#pragma mark Init

- (id)initWithFrame:(CGRect)frame {
    if ((self = [super initWithFrame:frame])) {
        [self commonInit];
    }
    return self;
}

- (id)initWithCoder:(NSCoder *)aDecoder {
    if ((self = [super initWithCoder:aDecoder])) {
        [self commonInit];
    }
    return self;
}

- (void)commonInit {
    [self generateColorTables];
    
    self.antialiasing = NO;
    
	self.layer.compositingFilter = nil;
	self.layer.edgeAntialiasingMask = 0;    
    self.layer.opaque = YES;
}

#pragma mark Private

- (void)generateColorTables {
    for (int i = 0; i < 256; i++) {
		unsigned long red = (unsigned long)((i & 31) * 255.0 / 31.0);
		unsigned long lowgreen = (unsigned long)(((i >> 5) & 7) * 255.0 / 63.0);
		lowtable[i] = red | (lowgreen << 8);
		
		unsigned long highgreen = (unsigned long)(((i & 7) << 3) * 255.0 / 63.0);
		unsigned long blue = (unsigned long)((i >> 3) * 255.0 / 31.0);
		hightable[i] = (blue << 16) | (highgreen << 8);
	}
}

- (void)setupBuffers {
    size_t size = width * height * 4;
    frameBuffer8888 = malloc(size);
    
    colorSpace = CGColorSpaceCreateDeviceRGB();
    
    provider[0] = CGDataProviderCreateWithData(NULL, frameBuffer8888, size, NULL);
    provider[1] = CGDataProviderCreateWithData(NULL, frameBuffer8888, size, NULL);
    
    currentProvider = 0;
}

- (void)destroyBuffers {
    if (frameBuffer8888) {
        free(frameBuffer8888);
        frameBuffer8888 = NULL;
    }
    if (colorSpace) {
        CGColorSpaceRelease(colorSpace);
        colorSpace = NULL;
    }
    if (provider[0]) {
        CGDataProviderRelease(provider[0]);
        provider[0] = NULL;
    }
    if (provider[1]) {
        CGDataProviderRelease(provider[1]);
        provider[1] = NULL;
    }
}


//- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event {
//    UITouch *touch = [touches anyObject];
//	CGPoint point = [touch locationInView: self];
//    CGPoint location;
//    
//    if (UIInterfaceOrientationIsLandscape(orientation) == YES) {
//        float x, y;
//        y = (([[settings objectForKey: @"fullScreen"] boolValue]== YES) ? 320.0 : (NES_HEIGHT + self.frame.origin.x)) - point.x;
//        x = point.y - self.frame.origin.y;
//		
//        if ([[settings objectForKey: @"fullScreen"] boolValue]== YES) {
//            x = (x * (NES_WIDTH / (([[settings objectForKey: @"aspectRatio"] boolValue]== YES) ? 341.0 : 480.0)));
//            y = (y * (NES_HEIGHT / 320.0));
//            
//
//        }
//		
//        NSLog(@"%s zapper touch at screen pos: %fx%f emulator pos: %fx%f layer origin: %fx%f\n",
//			  __func__, point.x, point.y, x, y, self.frame.origin.x, self.frame.origin.y);
//        location = CGPointMake(x, y);
//	} else {
//        if ([[settings objectForKey: @"fullScreen"] boolValue]== YES) {
//            
//            if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPhone)
//            {
//                point.x = (point.x * (NES_WIDTH / 320.0));
//                point.y = (point.y * (NES_HEIGHT / 300.0));
//            } else
//            {
//                if ([[settings objectForKey: @"fullScreen"] boolValue]== YES) {
//                    point.x = (point.x * (NES_WIDTH / 768.0));
//                    point.y = (point.y * (NES_HEIGHT / 576.0));
//                }
//            }
//        }
//        
//		location = CGPointMake(point.x, point.y);
//		NSLog(@"%s zapper touch at screen pos: %fx%f emulator pos: %fx%f layer origin: %fx%f\n",
//			  __func__, point.x, point.y, location.x, location.y, self.frame.origin.x, self.frame.origin.y);
//    }
//
//    if ([delegate respondsToSelector: @selector(gameControllerZapperDidChange:locationInWindow:)]==YES)
//    {
//		[delegate gameControllerZapperDidChange: 0x0 locationInWindow:location];
//	}
//}
// 
//- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {
//	UITouch *touch = [touches anyObject];
//	CGPoint point = [touch locationInView: self];
//    CGPoint location;
//	  	
//    if (UIInterfaceOrientationIsLandscape(orientation) == YES) {
//        float x, y;
//        y = (([[settings objectForKey: @"fullScreen"] boolValue]== YES) ? 320.0 : (NES_HEIGHT + self.frame.origin.x)) - point.x;
//        x = point.y - self.frame.origin.y;
//		
//        if ([[settings objectForKey: @"fullScreen"] boolValue]== YES) {
//            x = (x * (NES_WIDTH / (([[settings objectForKey: @"aspectRatio"] boolValue]== YES) ? 341.0 : 480.0)));
//            y = (y * (NES_HEIGHT / 320.0));
//        }
//		
//        NSLog(@"%s zapper touch at screen pos: %fx%f emulator pos: %fx%f layer origin: %fx%f\n", 
//			  __func__, point.x, point.y, x, y, self.frame.origin.x, self.frame.origin.y);
//        location = CGPointMake(x, y);
//	} else {
//        if ([[settings objectForKey: @"fullScreen"] boolValue]== YES) {
//            point.x = (point.x * (NES_WIDTH / 320.0));
//            point.y = (point.y * (NES_HEIGHT / 300.0));
//        }
//		location = CGPointMake(point.x, point.y);
//		NSLog(@"%s zapper touch at screen pos: %fx%f emulator pos: %fx%f layer origin: %fx%f\n", 
//			  __func__, point.x, point.y, location.x, location.y, self.frame.origin.x, self.frame.origin.y);
//    }
//	
//	if ([delegate respondsToSelector: @selector(gameControllerZapperDidChange:locationInWindow:)]==YES) {
//		[delegate gameControllerZapperDidChange: 0x10 locationInWindow:location];
//	}
//}

#pragma mark NestopiaCoreVideoDelegate

- (void)nestopiaCoreCallbackInitializeVideoWithWidth:(int)aWidth height:(int)aHeight {
    width = aWidth;
    height = aHeight;

    [self setupBuffers];
}

- (void)nestopiaCoreCallbackOutputFrame:(unsigned short *)frameBuffer {
    int x, y;
    unsigned short px;
    
    /* Convert active framebuffer from 565L to 8888 */
    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
            px = frameBuffer[width*y + x];
            frameBuffer8888[width*y+x] = hightable[px >> 8] + lowtable[px & 0xFF];
        }
    }
    
    CGImageRef screenImage;
    screenImage = CGImageCreate(width, height, 8, 32, 4 * width, colorSpace, kCGBitmapByteOrder32Host | kCGImageAlphaNoneSkipFirst, provider[currentProvider], NULL, NO, kCGRenderingIntentDefault);
    
    currentProvider = 1 - currentProvider;
    
    self.layer.contents = (__bridge id)screenImage;
    CGImageRelease(screenImage);
}

- (void)nestopiaCoreCallbackDestroyVideo {
    [self destroyBuffers];
}

@end
