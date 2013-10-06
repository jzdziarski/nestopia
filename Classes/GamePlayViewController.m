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

#import <Foundation/NSFileManager.h>
#import "GamePlayViewController.h"
#import "NestopiaAppDelegate.h"
#import "EmulatorCore.h"
#include <sys/stat.h>

NSString *currentGamePath = nil;
BOOL emulatorRunning;

@implementation GamePlayViewController
@synthesize gamePath, gameTitle, shouldLoadState;

- (void)loadView {
	UIInterfaceOrientation orientation = [ UIApplication sharedApplication ].statusBarOrientation;
    float screenHeight = [ UIScreen mainScreen ].bounds.size.height;
    float screenWidth = [ UIScreen mainScreen ].bounds.size.width;
    float emuHeight, emuWidth;
    CGRect surfaceRect;

    [ super loadView ];
    
    loaded = NO;
    self.title = gameTitle;
    [ currentGamePath release ];
    currentGamePath = [ gamePath copy ];
    
	NSLog(@"%s initializing emulator view in %s mode", __PRETTY_FUNCTION__,
		  (UIInterfaceOrientationIsLandscape(orientation) == YES) ? "landscape" : "portrait");
	
    if (UIInterfaceOrientationIsLandscape(orientation) == NO) {
        self.view.backgroundColor = [ UIColor colorWithHue: 240.0/360.0 saturation: .02 brightness: .96 alpha: 1.0 ];
        
        UILabel *label = [ [ UILabel alloc ] initWithFrame: CGRectMake(10.0, 20.5, self.view.bounds.size.width - 20.0, 40.0) ];
        label.backgroundColor = [ UIColor clearColor ];
        label.textColor = [ UIColor colorWithHue: 252.0/360.0 saturation: .02 brightness: .50 alpha: 1.0 ];
        //label.textColor = [ UIColor colorWithHue: 211.0/360.0 saturation: 1.0 brightness: 1.0 alpha: 1.0 ];        label.font = [ UIFont fontWithName: @"HelveticaNeue-Regular" size: 14.0 ];
        label.text = [ gameTitle uppercaseString ];
        label.textAlignment = NSTextAlignmentLeft;
        label.adjustsFontSizeToFitWidth = YES;
        label.userInteractionEnabled = YES;
        
        UITapGestureRecognizer *tapGesture = [ [ UITapGestureRecognizer alloc ] initWithTarget: self action: @selector(userDidExitGamePlay)];
        [ label addGestureRecognizer: tapGesture ];
        [ tapGesture release ];
        
        [ self.view addSubview: [ label autorelease ] ];
    }
    
    /* landscape */
	if (UIInterfaceOrientationIsLandscape(orientation) == YES) {
        screenHeight = [ UIScreen mainScreen ].bounds.size.width;
        screenWidth = [ UIScreen mainScreen ].bounds.size.height;

		if ([[ [ EmulatorCore globalSettings ] objectForKey: @"fullScreen" ] boolValue ]== YES) {
            if ([ [ [ EmulatorCore globalSettings ] objectForKey: @"aspectRatio" ] boolValue ]== YES) {
                emuHeight = 320.0;
                emuWidth = 341.0;
            } else {
                emuHeight = screenHeight;
                emuWidth = screenWidth;
            }
		} else {
            emuHeight = NES_HEIGHT;
            emuWidth = NES_WIDTH;
        }

        surfaceRect = CGRectMake((screenWidth - emuWidth) / 2.0, (screenHeight - emuHeight) / 2.0, emuWidth, emuHeight);
        
    /* portrait */
	} else {
        float offset = 0.0;
        float controllerHeight = 125.0;
        
		if ([[ [ EmulatorCore globalSettings ] objectForKey: @"fullScreen" ] boolValue ]== YES) {
            emuHeight = 300.0;
            emuWidth = 320.0;
            
            if (![ self hasFourInchDisplay ] ) {
                offset = 28.0;
            }
        } else {
            emuHeight = NES_HEIGHT;
            emuWidth = NES_WIDTH;
            
            if (![ self hasFourInchDisplay ] ) {
                offset = 14.0;
            }
        }
        
        if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad) {
            if ([[ [ EmulatorCore globalSettings ] objectForKey: @"fullScreen" ] boolValue ]== YES) {
                emuHeight *= 2.4;
                emuWidth *= 2.4;
                offset = 20.0;
                controllerHeight = 300.0;
            }
        }
        
        surfaceRect = CGRectMake((self.view.bounds.size.width - emuWidth) / 2.0, ((self.view.bounds.size.height - (emuHeight + controllerHeight)) / 2.0) + offset, emuWidth, emuHeight);
        
        UIView *border = [ [ UIView alloc ] initWithFrame: CGRectMake(surfaceRect.origin.x - 1.0, surfaceRect.origin.y - 1.0, surfaceRect.size.width + 2.0, surfaceRect.size.height + 2.0) ];
        border.backgroundColor = [ UIColor colorWithHue: 252.0/360.0 saturation: .02 brightness: .70 alpha: 1.0 ];
        [ self.view addSubview: [ border autorelease ] ];
    
    }
    
	NSLog(@"%s initializing surface layer with frame: %fx%f size: %fx%f", __PRETTY_FUNCTION__, surfaceRect.origin.x, surfaceRect.origin.y, surfaceRect.size.width, surfaceRect.size.height);
	screenView = [ [ ScreenView alloc ] initWithFrame: surfaceRect ];
	screenView.orientation = orientation;
	[ self.view addSubview: screenView ];
	
	NSLog(@"%s initializing controller layer", __PRETTY_FUNCTION__);
	if (UIInterfaceOrientationIsLandscape(orientation) == YES) {
        controllerView = [ [ ControllerView alloc ] initWithFrame: CGRectMake((screenWidth - 480.0)/2.0, (screenHeight - 300.0) / 2.0, 480.0, 300.0) ];
	} else {
        if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPhone) {
            controllerView = [ [ ControllerView alloc ] initWithFrame: CGRectMake(0.0, self.view.bounds.size.height - 125.0, self.view.bounds.size.width, 125.0) ];
        } else {
            controllerView = [ [ ControllerView alloc ] initWithFrame: CGRectMake(0.0, 20 + (self.view.bounds.size.height - 300.0), self.view.bounds.size.width, 300.0) ];

        }
	}
    
	if (UIInterfaceOrientationIsLandscape(orientation) == YES) {
		controllerView.alpha = 0.5;
	}
	
	[ self.view addSubview: controllerView ];
    [ self initializeEmulator ];
}

- (void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex
{
    
    [ alertView release ];
    [ emulatorCore release ];
    if (! [self.presentedViewController isBeingDismissed]) {
        [ currentGamePath release ];
        currentGamePath = nil;
        [ self dismissViewControllerAnimated: YES completion:^{} ];
    }
}

- (void)initializeEmulator {
	emulatorCore = [ [ EmulatorCore alloc ] init ];
    [ emulatorCore initializeEmulator ];

    NSLog(@"%s frame buffer size: %.0fx%.0f", __PRETTY_FUNCTION__, screenView.frameBufferSize.width, screenView.frameBufferSize.height);
    emulatorCore.frameBufferSize = screenView.frameBufferSize;

	BOOL success = [ emulatorCore loadROM: gamePath ];
	
    NSLog(@"%s loading image at path %@", __PRETTY_FUNCTION__, gamePath);
    controllerView.notified = NO;

	if (success != YES) {
        UIAlertView *myAlert = [ [ UIAlertView alloc ]
								initWithTitle:@"Unable to Load Game ROM"
								message: @"There was an error loading the selected game image."
								delegate: self
								cancelButtonTitle: nil
								otherButtonTitles: @"OK", nil ];
		[ myAlert show ];
		return;
	}
	loaded = YES;
    
	if (shouldLoadState == YES) {
		[ emulatorCore loadState ];
	}
    
	[ emulatorCore configureEmulator ];
    
    emulatorCore.screenDelegate = screenView;
	emulatorCore.frameBufferAddress = (word *) screenView.frameBufferAddress;
	screenView.delegate = emulatorCore;
	controllerView.delegate = emulatorCore;
    controllerView.gamePlayDelegate = self;
	   
    [ emulatorCore applyGameGenieCodes ];
    [ emulatorCore startEmulator ];
    pad1 = YES;
    emulatorRunning = YES;
}

- (void)dealloc {
    if (loaded) {
        emulatorCore.screenDelegate = nil;
        emulatorCore.frameBufferAddress = nil;
        controllerView.delegate = nil;
        screenView.delegate = nil;
        
        [ controllerView release ];
        [ screenView release ];
        
        [ emulatorCore finishEmulator ];
        [ emulatorCore release ];
    }
    [ super dealloc ];
}

- (BOOL)hasFourInchDisplay {
    return ([[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPhone && [UIScreen mainScreen].bounds.size.height == 568.0);
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
		if (buttonIndex == 2) { /* Save and Exit Game */
			[ emulatorCore saveState ];
            [ [ NSNotificationCenter defaultCenter ] postNotificationName: kGamePlaySavedStateNotification object: gamePath ];

		} else if (buttonIndex == 1) { /* Game Settings */
            controllerView.notified = NO;
            SettingsViewController *settingsViewController = [ [ SettingsViewController alloc ] init ];
            
            [ self.navigationController pushViewController: [ settingsViewController autorelease ] animated: YES ];
            return;
        } else if (buttonIndex == 0) { /* Controller Toggle */

            if (pad1) {
                [ emulatorCore activatePad2 ];
                pad1 = NO;
            } else {
                pad1 = YES;
                [ emulatorCore activatePad1 ];
            }
            
            controllerView.notified = NO;
            [ emulatorCore applyGameGenieCodes ];
            [ emulatorCore restartEmulator ];
            emulatorRunning = YES;
            return;
            
        } else if (buttonIndex == 4) { /* Resume Game */
            controllerView.notified = NO;
            [ emulatorCore applyGameGenieCodes ];
            [ emulatorCore restartEmulator ];
            emulatorRunning = YES;
            return;
        }
	}
    
    if (! [self.presentedViewController isBeingDismissed]) {
        [ currentGamePath release ];
        currentGamePath = nil;
        [ self dismissViewControllerAnimated: YES completion:^{} ];
    }
}

- (void)viewWillAppear:(BOOL)animated {
    if (loaded == NO)
        return;
    self.navigationController.navigationBar.hidden = YES;
    if (emulatorRunning == NO) {
        [ emulatorCore applyGameGenieCodes ];
        [ emulatorCore restartEmulator ];
        emulatorRunning = YES;
        [ controllerView reloadSettings ];
    }
}

/* UINavigationControllerDelegate Methods */

- (void)userDidExitGamePlay {

    [ emulatorCore haltEmulator ];
    emulatorRunning = NO;
        
    saveStateSheet = [ [ UIActionSheet alloc ] init ];
    saveStateSheet.title = @"Game Paused";
    
    if (pad1) {
        [ saveStateSheet addButtonWithTitle: @"Switch to Player 2" ];
    } else {
        [ saveStateSheet addButtonWithTitle: @"Switch to Player 1" ];
    }

    [ saveStateSheet addButtonWithTitle: @"Game Settings" ];
    [ saveStateSheet addButtonWithTitle: @"Save and Exit" ];
    [ saveStateSheet addButtonWithTitle: @"Exit Game" ];

    [ saveStateSheet addButtonWithTitle: @"Resume" ];

    saveStateSheet.cancelButtonIndex = 4;
    saveStateSheet.destructiveButtonIndex = 3;
    saveStateSheet.delegate = self;
    
    [ saveStateSheet showInView: self.view ];
}

- (BOOL)shouldAutorotate {
    return NO;
}

@end

