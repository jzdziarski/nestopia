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
@synthesize orientation;
@synthesize frameBufferAddress;
@synthesize frameBufferSize;
@synthesize delegate;

- (id)initWithFrame:(CGRect)frame {	
    self = [ super initWithFrame: frame ];
	if (self != nil) {
		[ self initializeGraphics ];
    }
    return self;
}

- (void)gen_colorspace_tables
{	
	for (int i = 0; i < NES_WIDTH; i++)
	{
		unsigned long red = (unsigned long)((i & 31) * 255.0 / 31.0);
		unsigned long lowgreen = (unsigned long)(((i >> 5) & 7) * 255.0 / 63.0);
		lowtable[i] = red | (lowgreen << 8);
		
		unsigned long highgreen = (unsigned long)(((i & 7) << 3) * 255.0 / 63.0);
		unsigned long blue = (unsigned long)((i >> 3) * 255.0 / 31.0);
		hightable[i] = (blue << 16) | (highgreen << 8);
	}
}

- (void)initializeGraphics {
	
	NSLog(@"%s initializing screen layer in %s mode\n", __func__,
		  (UIInterfaceOrientationIsLandscape(orientation) == YES) ? "landscape" : "portrait");	
	
    /* Landscape Resolutions */
    if (UIInterfaceOrientationIsLandscape(orientation) == YES) {
		if ([[ [ EmulatorCore globalSettings ] objectForKey: @"fullScreen" ] boolValue ] == YES) {
            w = 320;
            h = ([[ [ EmulatorCore globalSettings ] objectForKey: @"aspectRatio" ] boolValue ]== YES) ? 341 : 480;
        } else {
            w = NES_WIDTH;
            h = NES_HEIGHT;
        }
    } else {
		if ([[ [ EmulatorCore globalSettings ] objectForKey: @"fullScreen" ] boolValue ]== YES) {
			NSLog(@"%s initializing for full screen", __func__);
            w = 320;
            h = 300;
        } else {
			NSLog(@"%s initializing for standard screen", __func__);
            w = NES_WIDTH;
            h = NES_HEIGHT;
        }
    }
    
	frameBufferSize.height = h;
	frameBufferSize.width = w;
		
    int allocSize = 2 * w * h;

	NSLog(@"%s allocating screen layer for resolution %dx%d", __func__, w, h);
    frameBufferAddress = calloc(1, allocSize);
	NSLog(@"%s raw buffer base address: %p\n", __func__, frameBufferAddress);
	
	colorSpace = CGColorSpaceCreateDeviceRGB();
	frameBuffer8888 = calloc(1, w * h * 4);
	
	provider[0] = CGDataProviderCreateWithData(NULL, frameBuffer8888, w * h * 4, NULL);
	provider[1] = CGDataProviderCreateWithData(NULL, frameBuffer8888, w * h * 4, NULL);
	currentProvider = 0;
	
	[ self gen_colorspace_tables ];
	
	self.layer.magnificationFilter = nil;
	self.layer.minificationFilter = nil;
	self.layer.compositingFilter = nil;
	self.layer.edgeAntialiasingMask = 0;
    if ([ [ [ EmulatorCore globalSettings ] objectForKey: @"shouldRasterize" ] boolValue ] == YES ){
        self.layer.shouldRasterize = YES;
        NSLog(@"%s turning on shouldRasterize", __PRETTY_FUNCTION__);
	}
    self.layer.opaque = YES;

	NSLog(@"%s graphics initialization complete\n", __func__);
}

-(void)drawLayer:(CALayer*)layer inContext:(CGContextRef)context
{
    NSLog(@"%s", __PRETTY_FUNCTION__);
    
    CGContextSetAllowsAntialiasing(context, true);
    CGContextSetShouldAntialias(context, true);
    CGContextSetFillColorWithColor(context, [UIColor greenColor].CGColor);
    CGContextFillRect(context, self.bounds);
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event {
    UITouch *touch = [ touches anyObject ];
	CGPoint point = [ touch locationInView: self ];
    CGPoint location;
    
    if (UIInterfaceOrientationIsLandscape(orientation) == YES) {
        float x, y;
        y = (([[ [ EmulatorCore globalSettings ] objectForKey: @"fullScreen" ] boolValue ]== YES) ? 320.0 : (NES_HEIGHT + self.frame.origin.x)) - point.x;
        x = point.y - self.frame.origin.y;
		
        if ([[ [ EmulatorCore globalSettings ] objectForKey: @"fullScreen" ] boolValue ]== YES) {
            x = (x * (NES_WIDTH / (([[ [ EmulatorCore globalSettings ] objectForKey: @"aspectRatio" ] boolValue ]== YES) ? 341.0 : 480.0)));
            y = (y * (NES_HEIGHT / 320.0));
            

        }
		
        NSLog(@"%s zapper touch at screen pos: %fx%f emulator pos: %fx%f layer origin: %fx%f\n",
			  __func__, point.x, point.y, x, y, self.frame.origin.x, self.frame.origin.y);
        location = CGPointMake(x, y);
	} else {
        if ([[ [ EmulatorCore globalSettings ] objectForKey: @"fullScreen" ] boolValue ]== YES) {
            
            if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPhone)
            {
                point.x = (point.x * (NES_WIDTH / 320.0));
                point.y = (point.y * (NES_HEIGHT / 300.0));
            } else
            {
                if ([[ [ EmulatorCore globalSettings ] objectForKey: @"fullScreen" ] boolValue ]== YES) {
                    point.x = (point.x * (NES_WIDTH / 768.0));
                    point.y = (point.y * (NES_HEIGHT / 576.0));
                }
            }
        }
        
		location = CGPointMake(point.x, point.y);
		NSLog(@"%s zapper touch at screen pos: %fx%f emulator pos: %fx%f layer origin: %fx%f\n",
			  __func__, point.x, point.y, location.x, location.y, self.frame.origin.x, self.frame.origin.y);
    }

    if ([ delegate respondsToSelector: @selector(gameControllerZapperDidChange:locationInWindow:) ]==YES)
    {
		[ delegate gameControllerZapperDidChange: 0x0 locationInWindow:location ];
	}
}
 
- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {
	UITouch *touch = [ touches anyObject ];
	CGPoint point = [ touch locationInView: self ];
    CGPoint location;
	  	
    if (UIInterfaceOrientationIsLandscape(orientation) == YES) {
        float x, y;
        y = (([[ [ EmulatorCore globalSettings ] objectForKey: @"fullScreen" ] boolValue ]== YES) ? 320.0 : (NES_HEIGHT + self.frame.origin.x)) - point.x;
        x = point.y - self.frame.origin.y;
		
        if ([[ [ EmulatorCore globalSettings ] objectForKey: @"fullScreen" ] boolValue ]== YES) {
            x = (x * (NES_WIDTH / (([[ [ EmulatorCore globalSettings ] objectForKey: @"aspectRatio" ] boolValue ]== YES) ? 341.0 : 480.0)));
            y = (y * (NES_HEIGHT / 320.0));
        }
		
        NSLog(@"%s zapper touch at screen pos: %fx%f emulator pos: %fx%f layer origin: %fx%f\n", 
			  __func__, point.x, point.y, x, y, self.frame.origin.x, self.frame.origin.y);
        location = CGPointMake(x, y);
	} else {
        if ([[ [ EmulatorCore globalSettings ] objectForKey: @"fullScreen" ] boolValue ]== YES) {
            point.x = (point.x * (NES_WIDTH / 320.0));
            point.y = (point.y * (NES_HEIGHT / 300.0));
        }
		location = CGPointMake(point.x, point.y);
		NSLog(@"%s zapper touch at screen pos: %fx%f emulator pos: %fx%f layer origin: %fx%f\n", 
			  __func__, point.x, point.y, location.x, location.y, self.frame.origin.x, self.frame.origin.y);
    }
	
	if ([ delegate respondsToSelector: @selector(gameControllerZapperDidChange:locationInWindow:) ]==YES) {
		[ delegate gameControllerZapperDidChange: 0x10 locationInWindow:location ];
	}
}

- (void)emulatorCoreDidUpdateFrameBuffer {
	int x, y;
	unsigned short px;

	/* Convert active framebuffer from 565L to 8888 */
	for (y=0; y < h; y++)
	{
		for (x=0; x< w; x++)
		{
			px = frameBufferAddress[w*y+x];
			frameBuffer8888[w*y+x] = hightable[px >> 8 ] + lowtable[px & 0xFF];
		}		
	}

	CGImageRef screenImage;
	screenImage = CGImageCreate(w, h, 8, 32, 4 * w, colorSpace, kCGBitmapByteOrder32Host | kCGImageAlphaNoneSkipFirst, provider[currentProvider], NULL, NO, kCGRenderingIntentDefault);
	if (currentProvider==0)
		currentProvider = 1;
	else 
		currentProvider = 0;
	self.layer.contents = (id) screenImage;
	CGImageRelease(screenImage);
}

- (void)dealloc {
	[ screenLayer release ];
    free(frameBufferAddress);
    
    [ super dealloc ];
}

@end
