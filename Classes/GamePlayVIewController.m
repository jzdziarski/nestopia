/*
 Nescaline
 Copyright (c) 2007-2013, Jonathan A. Zdziarski
 
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

#import <Foundation/NSFileManager.h>
#import "GamePlayViewController.h"
#import "NESAppDelegate.h"
#import "EmulatorCore.h"
#include <sys/stat.h>

NSString *currentGamePath = nil;

@implementation GamePlayViewController
@synthesize gamePath, gameTitle, shouldLoadState;

- (void)loadView {
	UIInterfaceOrientation orientation = [ UIApplication sharedApplication ].statusBarOrientation;
    float screenHeight = [ UIScreen mainScreen ].bounds.size.height;
    float screenWidth = [ UIScreen mainScreen ].bounds.size.width;
    float emuHeight, emuWidth;
    CGRect surfaceRect;

    [ super loadView ];
    
    self.title = gameTitle;
    [ currentGamePath release ];
    currentGamePath = [ gamePath copy ];
    
	NSLog(@"%s initializing emulator view in %s mode", __PRETTY_FUNCTION__,
		  (UIInterfaceOrientationIsLandscape(orientation) == YES) ? "landscape" : "portrait");
	
    /* landscape */
	if (UIInterfaceOrientationIsLandscape(orientation) == YES) {
        screenHeight = [ UIScreen mainScreen ].bounds.size.width;
        screenWidth = [ UIScreen mainScreen ].bounds.size.height;

		if ([ [ NSUserDefaults standardUserDefaults ] boolForKey: @"fullScreen" ] == YES) {
            if ([ [ NSUserDefaults standardUserDefaults ] boolForKey: @"aspectRatio" ] == YES) {
                emuHeight = 320.0;
                emuWidth = 341.0;
            } else {
                emuHeight = screenHeight;
                emuWidth = screenWidth;
            }
		} else {
            emuHeight = 240.0;
            emuWidth = 256.0;
        }
        
        surfaceRect = CGRectMake((screenWidth - emuWidth) / 2.0, (screenHeight - emuHeight) / 2.0, emuWidth, emuHeight);
        
    /* portrait */
	} else {
		if ([ [ NSUserDefaults standardUserDefaults ] boolForKey: @"fullScreen" ] == YES) {
            emuHeight = 300.0;
            emuWidth = 320.0;
        } else {
            emuHeight = 240.0;
            emuWidth = 256.0;
        }
        
        surfaceRect = CGRectMake((self.view.bounds.size.width - emuWidth) / 2.0, (self.view.bounds.size.height - emuHeight) / 2.0, emuWidth, emuHeight);
	}
    
	NSLog(@"%s initializing surface layer with frame: %fx%f size: %fx%f", __PRETTY_FUNCTION__, surfaceRect.origin.x, surfaceRect.origin.y, surfaceRect.size.width, surfaceRect.size.height);
	screenView = [ [ ScreenView alloc ] initWithFrame: surfaceRect ];
	screenView.orientation = orientation;
	[ self.view addSubview: screenView ];
	
	NSLog(@"%s initializing controller layer", __PRETTY_FUNCTION__);
	if (UIInterfaceOrientationIsLandscape(orientation) == YES) {
        controllerView = [ [ ControllerView alloc ] initWithFrame: CGRectMake((screenWidth - 480.0)/2.0, (screenHeight - 300.0) / 2.0, 480.0, 300.0) ];
	} else {
		controllerView = [ [ ControllerView alloc ] initWithFrame: CGRectMake(0.0, self.view.bounds.size.height - 125.0, self.view.bounds.size.width, 125.0) ];
	}
    
	if (UIInterfaceOrientationIsLandscape(orientation) == YES) {
		controllerView.alpha = 0.5;
	}
	
	[ self.view addSubview: controllerView ];

}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
		
	return NO;
}

- (BOOL)shouldAutorotate {
    
    return NO;
}

- (void)viewDidAppear:(BOOL)animated {
	emulatorCore = [ [ EmulatorCore alloc ] init ];
	EmulatorCoreLoadStatus success = [ emulatorCore loadROM: gamePath ];
	
    NSLog(@"%s loading image at path %@", __PRETTY_FUNCTION__, gamePath);

	if (success != EmulatorCoreLoadStatusSuccess) {
        UIAlertView *myAlert = [ [ UIAlertView alloc ]
								initWithTitle:@"Unable to Load Game ROM"
								message: nil
								delegate: self
								cancelButtonTitle: nil
								otherButtonTitles: @"OK", nil ];
		if (success == EmulatorCoreLoadStatusInvalidROM) {
			myAlert.message = @"There was an error loading the selected game image. This game image may be invalid or corrupt.";
		} else {
			myAlert.message = @"There was an error loading the selected game image. This game image is not yet supported.";
		}
		[ myAlert show ];
		return;
	}
	
	[ emulatorCore initializeEmulator ];
	
	if (shouldLoadState == YES) {
		[ emulatorCore loadState ];
	}
    
	[ emulatorCore configureEmulator ];
	[ emulatorCore applyGameGenieCodes ];
    
    emulatorCore.screenDelegate = screenView;
	emulatorCore.frameBufferAddress = (word *) screenView.frameBufferAddress;
	emulatorCore.frameBufferSize = self.view.frame.size;
	screenView.delegate = emulatorCore;
	controllerView.delegate = emulatorCore;
    controllerView.gamePlayDelegate = self;
	   
    [ emulatorCore startEmulator ];
}

- (void)dealloc {
    [ emulatorCore haltEmulator ];
    
    emulatorCore.screenDelegate = nil;
    emulatorCore.frameBufferAddress = nil;
    controllerView.delegate = nil;
    screenView.delegate = nil;

    [ controllerView release ];
    [ screenView release ];
    
    [ emulatorCore finishEmulator ];
    [ emulatorCore release ];
    [ super dealloc ];
}

- (void)refreshControls {
    UIInterfaceOrientation orientation = [ UIApplication sharedApplication ].statusBarOrientation;
	CGRect frame = controllerView.frame;
    
	NSLog(@"%s re-initializing controllerView with frame %fx%f", __PRETTY_FUNCTION__, frame.size.width, frame.size.height);
	[ controllerView removeFromSuperview ];
    [ controllerView release ];
	controllerView = [ [ ControllerView alloc ] initWithFrame: frame ];
	if (UIInterfaceOrientationIsLandscape(orientation) == YES) {
		controllerView.alpha = 0.5;
	}
    
	[ self.view addSubview: controllerView ];
}

/* UIActionSheetDelegate Methods */
- (void)actionSheet:(UIActionSheet *)actionSheet clickedButtonAtIndex:(NSInteger)buttonIndex {
	
	if (actionSheet == saveStateSheet) {
		if (buttonIndex == 0) {
			[ emulatorCore saveState ];
            [ [ NSNotificationCenter defaultCenter ] postNotificationName: kEmulatorCoreSavedStateNotification object: gamePath ];

		}
	}
    
    [ self dismissViewControllerAnimated: YES completion: nil ];
 }

/* UINavigationControllerDelegate Methods */

- (void)userDidExitGamePlay {
		
    [ emulatorCore haltEmulator ];
    
    if ([ [ NSUserDefaults standardUserDefaults ] boolForKey: @"autoSave" ] == YES) {
        [ emulatorCore saveState ];
    } else {
        saveStateSheet = [ [ UIActionSheet alloc ] init ];
        saveStateSheet.title = @"Do you want to save this game?";
        [ saveStateSheet addButtonWithTitle: @"Save Game" ];
        [ saveStateSheet addButtonWithTitle: @"Don't Save" ];
        saveStateSheet.destructiveButtonIndex = 1;
        saveStateSheet.delegate = self;
        
        [ saveStateSheet showInView: self.view ];
    }

}

@end

