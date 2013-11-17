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

@interface GamePlayViewController () <UIActionSheetDelegate>

@property (nonatomic, strong) Game *game;
@property (nonatomic, assign) BOOL shouldLoadState;

@end


@implementation GamePlayViewController {
    /* Initialization */
	UIActionSheet *saveStateSheet;
    bool loaded, pad1;
    
    /* Game Play */
	ScreenView *screenView;
	ControllerView *controllerView;
    EmulatorCore *emulatorCore;
}

- (id)initWithGame:(Game *)game loadState:(BOOL)loadState {
    if ((self = [super init])) {
        _game = game;
        _shouldLoadState = loadState;
        
        self.title = self.game.title;
    }
    return self;
}

- (void)loadView {
    [super loadView];
    
    [self initializeEmulatorView];
    [self initializeEmulator];
}

- (void)viewDidLayoutSubviews {
    [super viewDidLayoutSubviews];
    
    CGSize screenViewSize = [EmulatorCore sharedEmulatorCore].nativeScreenResolution;
    CGFloat scale = self.view.bounds.size.width / screenViewSize.height;
    screenViewSize.width *= scale;
    screenViewSize.height *= scale;
    screenView.frame = CGRectMake(0, 0,
                                  screenViewSize.width, screenViewSize.height);
    
    CGSize controllerViewSize = CGSizeMake(self.view.bounds.size.width, 320);
    controllerView.frame = CGRectMake(0, CGRectGetMaxY(self.view.bounds) - controllerViewSize.height,
                                      controllerViewSize.width, controllerViewSize.height);
}

- (void)initializeEmulatorView {
	screenView = [[ScreenView alloc] init];
	[self.view addSubview:screenView];
	
    controllerView = [[ControllerView alloc] init];
    controllerView.swapAB = [[self.game.settings objectForKey:@"swapAB"] boolValue];
    controllerView.stickControl = [[self.game.settings objectForKey:@"controllerStickControl"] boolValue];
	[self.view addSubview:controllerView];
}

- (void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex
{
    
    if (! [self.presentedViewController isBeingDismissed]) {
        [self dismissViewControllerAnimated: YES completion:^{}];
    }
}

- (void)initializeEmulator {
	emulatorCore = [EmulatorCore sharedEmulatorCore];
    [emulatorCore initializeEmulator];

	BOOL success = [emulatorCore loadGame:self.game];
	
    NSLog(@"%s loading image at path %@", __PRETTY_FUNCTION__, self.game.path);
    controllerView.notified = NO;

	if (success != YES) {
        UIAlertView *myAlert = [[UIAlertView alloc]
								initWithTitle:@"Unable to Load Game ROM"
								message: @"There was an error loading the selected game image."
								delegate: self
								cancelButtonTitle: nil
								otherButtonTitles: @"OK", nil];
		[myAlert show];
		return;
	}
	loaded = YES;
    
	if (self.shouldLoadState) {
		[emulatorCore loadState];
	}
    
    emulatorCore.screenDelegate = screenView;
	screenView.delegate = emulatorCore;
	controllerView.delegate = emulatorCore;
    controllerView.gamePlayDelegate = self;
	   
    [emulatorCore applyGameGenieCodes];
    [emulatorCore startEmulator];
    pad1 = YES;
}

- (void)dealloc {
    if (loaded) {
        emulatorCore.screenDelegate = nil;
        controllerView.delegate = nil;
        screenView.delegate = nil;
        
        
        [emulatorCore finishEmulator];
    }
}

- (BOOL)hasFourInchDisplay {
    return ([[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPhone && [UIScreen mainScreen].bounds.size.height == 568.0);
}

- (void)refreshControls {
    UIInterfaceOrientation orientation = [UIApplication sharedApplication].statusBarOrientation;
	CGRect frame = controllerView.frame;
    
	NSLog(@"%s re-initializing controllerView with frame %fx%f", __PRETTY_FUNCTION__, frame.size.width, frame.size.height);
	[controllerView removeFromSuperview];
	controllerView = [[ControllerView alloc] initWithFrame: frame];
	if (UIInterfaceOrientationIsLandscape(orientation) == YES) {
		controllerView.alpha = 0.5;
	}
    
	[self.view addSubview: controllerView];
}

/* UIActionSheetDelegate Methods */
- (void)actionSheet:(UIActionSheet *)actionSheet clickedButtonAtIndex:(NSInteger)buttonIndex {
	
	if (actionSheet == saveStateSheet) {
        
        if (!strstr([self.game.path cStringUsingEncoding: NSASCIIStringEncoding], "(VS)")) {
            buttonIndex++;
        }
        
		if (buttonIndex == 3) { /* Save and Exit Game */
			[emulatorCore saveState];
            [[NSNotificationCenter defaultCenter] postNotificationName: kGamePlaySavedStateNotification object: self.game.path];

		} else if (buttonIndex == 2) { /* Game Settings */
            controllerView.notified = NO;
            SettingsViewController *settingsViewController = [[SettingsViewController alloc] init];
            
            [self.navigationController pushViewController: settingsViewController animated: YES];
            return;
        } else if (buttonIndex == 0) { /* Insert Coin */
            
            controllerView.notified = NO;
            [emulatorCore applyGameGenieCodes];
            [emulatorCore restartEmulator];
            [emulatorCore insertCoin1];

            return;
            
        } else if (buttonIndex == 1) { /* Controller Toggle */

            if (pad1) {
                [emulatorCore activatePad2];
                pad1 = NO;
            } else {
                pad1 = YES;
                [emulatorCore activatePad1];
            }
            
            controllerView.notified = NO;
            [emulatorCore applyGameGenieCodes];
            [emulatorCore restartEmulator];
            return;
            
        } else if (buttonIndex == 5) { /* Resume Game */
            controllerView.notified = NO;
            [emulatorCore applyGameGenieCodes];
            [emulatorCore restartEmulator];
            return;
        }
	}
    
    if (! [self.presentedViewController isBeingDismissed]) {
        [self dismissViewControllerAnimated: YES completion:^{}];
    }
}

//- (void)viewWillAppear:(BOOL)animated {
//    if (loaded == NO)
//        return;
//    self.navigationController.navigationBar.hidden = YES;
//    if (emulatorRunning == NO) {
//        [emulatorCore applyGameGenieCodes];
//        [emulatorCore restartEmulator];
//        emulatorRunning = YES;
//        [controllerView reloadSettings];
//    }
//}

/* UINavigationControllerDelegate Methods */

- (void)userDidExitGamePlay {

    [emulatorCore haltEmulator];
        
    saveStateSheet = [[UIActionSheet alloc] init];
    saveStateSheet.title = @"Game Options";
    
    if (strstr([self.game.path cStringUsingEncoding: NSASCIIStringEncoding], "(VS)")) {
        [saveStateSheet addButtonWithTitle: @"Insert Coin"];
    }
    
    if (pad1) {
        [saveStateSheet addButtonWithTitle: @"Switch to Player 2"];
    } else {
        [saveStateSheet addButtonWithTitle: @"Switch to Player 1"];
    }

    [saveStateSheet addButtonWithTitle: @"Game Settings"];
    [saveStateSheet addButtonWithTitle: @"Save and Exit"];
    [saveStateSheet addButtonWithTitle: @"Exit Game"];

    [saveStateSheet addButtonWithTitle: @"Resume"];

    saveStateSheet.cancelButtonIndex = 4;
    saveStateSheet.destructiveButtonIndex = 3;
    saveStateSheet.delegate = self;
    
    [saveStateSheet showInView: self.view];
}

- (BOOL)shouldAutorotate {
    return YES;
}

- (void)willRotateToInterfaceOrientation:(UIInterfaceOrientation)toInterfaceOrientation duration:(NSTimeInterval)duration
{
    [emulatorCore haltEmulator];
    
    [screenView removeFromSuperview];
    [controllerView removeFromSuperview];
    
    screenView = nil;
    controllerView = nil;
}

- (void)didRotateFromInterfaceOrientation:(UIInterfaceOrientation)fromInterfaceOrientation {    
    [self initializeEmulatorView];

    emulatorCore.screenDelegate = screenView;
	screenView.delegate = emulatorCore;
	controllerView.delegate = emulatorCore;
    controllerView.gamePlayDelegate = self;
    
    [emulatorCore applyGameGenieCodes];
    [emulatorCore restartEmulator];
}

@end

