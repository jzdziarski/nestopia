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

@implementation ScreenView

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
	self.layer.magnificationFilter = nil;
	self.layer.minificationFilter = nil;
	self.layer.compositingFilter = nil;
	self.layer.edgeAntialiasingMask = 0;    
    self.layer.opaque = YES;
}

- (void)drawLayer:(CALayer*)layer inContext:(CGContextRef)context {
    CGContextSetAllowsAntialiasing(context, true);
    CGContextSetShouldAntialias(context, true);
    CGContextSetFillColorWithColor(context, [UIColor greenColor].CGColor);
    CGContextFillRect(context, self.bounds);
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

#pragma mark EmulatorCoreScreenDelegate

- (void)emulatorCoreDidRenderFrame:(CGImageRef)frameImageRef {
    self.layer.contents = (__bridge id)frameImageRef;
}

@end
