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

#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>
#import "EmulatorCore.h"
#import "ControllerView.h"

#define NES_WIDTH   256
#define NES_HEIGHT  240

@interface ScreenView : UIView <EmulatorCoreScreenDelegate> {
	unsigned long hightable[NES_WIDTH], lowtable[NES_WIDTH];
	int w, h;

    NSDictionary *settings;
	CALayer *screenLayer;
	UIDeviceOrientation orientation;
	unsigned short *frameBufferAddress;
	unsigned long *frameBuffer8888;
	CGSize frameBufferSize;
	CGColorSpaceRef colorSpace;
	CGDataProviderRef provider[2];
	id delegate;	
	int currentProvider;
}

- (id)initWithFrame:(CGRect)frame settings:(NSDictionary *)settings;
- (void)initializeGraphics;

/* EmulatorCoreScreenDelegate */

- (void)emulatorCoreDidUpdateFrameBuffer;

@property(nonatomic,assign) UIDeviceOrientation orientation;
@property(nonatomic,assign,readonly) unsigned short *frameBufferAddress;
@property(nonatomic,assign) CGSize frameBufferSize;
@property(nonatomic,assign) id<GameControllerDelegate> delegate;
@end
