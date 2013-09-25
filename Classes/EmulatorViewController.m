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

#import "EmulatorViewController.h"


@implementation EmulatorViewController
@synthesize orientation;
@synthesize screenView;
@synthesize controllerView;
@synthesize resize;
@synthesize emulatorCore;

- (id)init {
    self = [ super init ];
	if (self != nil) {
		resize = NO;
	}
	return self;
}

- (void)loadView {
    
    [ super loadView ];
    
	CGRect screenRect, surfaceRect;
	 	
	settings = [ NSUserDefaults standardUserDefaults ];
	orientation = [ UIApplication sharedApplication ].statusBarOrientation;
	
	NSLog(@"%s initializing emulator view in %s mode\n", __func__,
		  (UIInterfaceOrientationIsLandscape(orientation) == YES) ? "landscape" : "portrait");
	
	if (UIInterfaceOrientationIsLandscape(orientation) == YES) {
		if ([ settings boolForKey: @"fullScreen" ] == YES) {
			screenRect = CGRectMake(([ settings boolForKey: @"aspectRatio" ] == YES) ? 56.0 : 0.0, 0.0,
									([ settings boolForKey: @"aspectRatio" ] == YES) ? 341.0 : 480.0, 320.0);
		} else {
			screenRect = CGRectMake(90.0, 20.0, 256.0, 240.0);
		}

	} else {
		if ([ settings boolForKey: @"fullScreen" ] == YES) {
			screenRect = CGRectMake(0.0, 0.0, 320.0, 300.0);
		} else {
			screenRect = CGRectMake(floor((self.view.frame.size.width - 256.0)/2) - 5, 
									floor((self.view.frame.size.height-48.0-100.0-240.0)/2),
									256.0, 240.0);
		}
	}
		
	surfaceRect = CGRectMake(screenRect.origin.x,
							 screenRect.origin.y,
							 screenRect.size.width + (screenRect.origin.x),
							 screenRect.size.height + (screenRect.origin.y));
		
	NSLog(@"%s initializing screen layer with frame: %fx%f size: %fx%f\n", __func__,
		  screenRect.origin.x, screenRect.origin.y,
		  screenRect.size.width, screenRect.size.height);
	screenView = [ [ ScreenView alloc ] initWithFrame: surfaceRect ];
	screenView.orientation = orientation;
	[ self.view addSubview: screenView ];
	
    NSLog(@"%s screenView: %@", __PRETTY_FUNCTION__, screenView);
	NSLog(@"%s initializing controller layer\n", __func__);
	if (UIInterfaceOrientationIsLandscape(orientation) == YES) {
			controllerView = [ [ ControllerView alloc ] initWithFrame: CGRectMake(0.0, (resize == YES) ? 20.0 : 0.0, 480.0, 300.0) ];
	} else {
		controllerView = [ [ ControllerView alloc ] initWithFrame:
						  CGRectMake(0.0, 310.0, self.view.frame.size.width, 105.0)
		];
	}
	
	if (UIInterfaceOrientationIsLandscape(orientation) == YES) {
		controllerView.alpha = 0.5;
	}
	
	[ self.view addSubview: controllerView ];
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
	
	if (interfaceOrientation == UIDeviceOrientationLandscapeLeft || interfaceOrientation == UIDeviceOrientationPortrait)
		return YES;
	
	return NO;
}

- (void)willRotateToInterfaceOrientation:(UIInterfaceOrientation)toInterfaceOrientation duration:(NSTimeInterval)duration {
	screenView.hidden = YES;
	controllerView.hidden = YES;
}

- (void)didReceiveMemoryWarning {
    [ super didReceiveMemoryWarning ];
}

- (void)refreshControls {
	CGRect frame = controllerView.frame;
	
	[ controllerView release ];
	
	NSLog(@"%s initializing controllerView with frame %fx%f", __func__, frame.size.width, frame.size.height);
	
	controllerView = [ [ ControllerView alloc ] initWithFrame: frame ];
	if (UIInterfaceOrientationIsLandscape(orientation) == YES) {
		controllerView.alpha = 0.5;
	}
	[ self.view addSubview: controllerView ];
}

- (void)drawRect:(CGRect)rect {
	CGContextRef ctx = UIGraphicsGetCurrentContext();
	
	float black[4] = { 0.0, 0.0, 0.0, 1.0 };
	CGContextSetFillColor(ctx, black);
	
	if (UIInterfaceOrientationIsLandscape(orientation) == YES) {
		CGContextFillRect(ctx, CGRectMake(self.view.frame.origin.x, self.view.frame.origin.y, 
										  self.view.frame.size.width, self.view.frame.size.height-60));
	} else {
		CGContextFillRect(ctx, CGRectMake(self.view.frame.origin.x, self.view.frame.origin.y, 
										  self.view.frame.size.width, self.view.frame.size.height-119));
	}
}

- (void)viewWillDisappear:(BOOL)animated {
    
    [ super viewWillDisappear: animated ];
	NSLog(@"%s", __func__);
	[ emulatorCore haltEmulator ];
}

- (void)dealloc {
	[ screenView release ];
	[ controllerView release ];
    [ super dealloc ];
}

@end
