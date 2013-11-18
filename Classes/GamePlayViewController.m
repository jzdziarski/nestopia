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
#import "NestopiaCore.h"
#import "AudioPlayer.h"
#import "ScreenView.h"
#import "SettingsViewController.h"
#import "PadDirectionButton.h"
#import "PadSingleButton.h"
#import "PadRoundTextButton.h"
#import "RoundTextMaskView.h"


static CGFloat const kDirectionButtonRadius = 100;
static CGFloat const kABButtonRadius = 50;
static CGFloat const kSpecialButtonRadius = 35;


@interface GamePlayViewController () <UIActionSheetDelegate, NestopiaCoreInputDelegate>

@property (nonatomic, strong) Game *game;
@property (nonatomic, assign) BOOL shouldLoadState;

@property (nonatomic, strong) ScreenView *screenView;
@property (nonatomic, strong) UIView *buttonsView;
@property (nonatomic, strong) PadDirectionButton *directionButton;
@property (nonatomic, strong) PadRoundTextButton *aButton;
@property (nonatomic, strong) PadRoundTextButton *bButton;
@property (nonatomic, strong) PadRoundTextButton *selectButton;
@property (nonatomic, strong) PadRoundTextButton *startButton;
@property (nonatomic, strong) UIButton *menuButton;

@end


@implementation GamePlayViewController {
	UIActionSheet *saveStateSheet;
    bool pad1;
    
    AudioPlayer *audioPlayer;
    NestopiaCore *nestopiaCore;
}

#pragma mark Init

- (id)initWithGame:(Game *)game loadState:(BOOL)loadState {
    if ((self = [super init])) {
        _game = game;
        _shouldLoadState = loadState;
        
        self.title = self.game.title;
    
        [self setupEmulator];
    }
    return self;
}

- (void)setupEmulator {
    nestopiaCore = [NestopiaCore sharedCore];
    
    nestopiaCore.gamePath = self.game.path;
    nestopiaCore.gameSavePath = self.game.savePath;
    BOOL success = [nestopiaCore powerOn];
	
    NSLog(@"%s loading image at path %@", __PRETTY_FUNCTION__, self.game.path);
    
	if (!success) {
        UIAlertView *myAlert = [[UIAlertView alloc]
								initWithTitle:@"Unable to Load Game ROM"
								message: @"There was an error loading the selected game image."
								delegate: self
								cancelButtonTitle: nil
								otherButtonTitles: @"OK", nil];
		[myAlert show];
		return;
	}
    
	if (self.shouldLoadState) {
		[nestopiaCore loadState];
	}
    
    audioPlayer = [[AudioPlayer alloc] init];
    nestopiaCore.audioDelegate = audioPlayer;
    
    nestopiaCore.inputDelegate = self;
    
    pad1 = YES;
}

#pragma mark Dealloc

- (void)dealloc {
    [nestopiaCore powerOff];
}

#pragma mark Life cycle

- (void)loadView {
    [super loadView];
    
    self.screenView = [[ScreenView alloc] init];
    self.screenView.antialiasing = [[self.game.settings objectForKey:@"antiAliasing"] boolValue];
    nestopiaCore.videoDelegate = self.screenView;
    [self.view addSubview:self.screenView];
    
    self.buttonsView = [[UIView alloc] init];
    self.buttonsView.alpha = 0.3;
    [self.view addSubview:self.buttonsView];
    
    self.directionButton = [[PadDirectionButton alloc] initWithFrame:CGRectMake(0, 0, 2*kDirectionButtonRadius, 2*kDirectionButtonRadius)];
    //controllerView.swapAB = [[self.game.settings objectForKey:@"swapAB"] boolValue];
    //controllerView.stickControl = [[self.game.settings objectForKey:@"controllerStickControl"] boolValue];
	[self.buttonsView addSubview:self.directionButton];
    
    self.aButton = [[PadRoundTextButton alloc] initWithFrame:CGRectMake(0, 0, 2*kABButtonRadius, 2*kABButtonRadius)];
    self.aButton.singleInput = NestopiaPadInputA;
    self.aButton.font = [UIFont boldSystemFontOfSize:40];
    self.aButton.text = @"A";
    [self.buttonsView addSubview:self.aButton];
    
    self.bButton = [[PadRoundTextButton alloc] initWithFrame:CGRectMake(0, 0, 2*kABButtonRadius, 2*kABButtonRadius)];
    self.bButton.singleInput = NestopiaPadInputB;
    self.bButton.font = [UIFont boldSystemFontOfSize:40];
    self.bButton.text = @"B";
    [self.buttonsView addSubview:self.bButton];
    
    self.selectButton = [[PadRoundTextButton alloc] initWithFrame:CGRectMake(0, 0, 2*kSpecialButtonRadius, 2*kSpecialButtonRadius)];
    self.selectButton.singleInput = NestopiaPadInputSelect;
    self.selectButton.font = [UIFont systemFontOfSize:20];
    self.selectButton.text = @"Select";
    [self.buttonsView addSubview:self.selectButton];
    
    self.startButton = [[PadRoundTextButton alloc] initWithFrame:CGRectMake(0, 0, 2*kSpecialButtonRadius, 2*kSpecialButtonRadius)];
    self.startButton.singleInput = NestopiaPadInputStart;
    self.startButton.font = [UIFont systemFontOfSize:20];
    self.startButton.text = @"Start";
    [self.buttonsView addSubview:self.startButton];
    
    self.menuButton = [UIButton buttonWithType:UIButtonTypeCustom];
    self.menuButton.bounds = CGRectMake(0, 0, 2*kSpecialButtonRadius, 2*kSpecialButtonRadius);
    [self.menuButton addTarget:self action:@selector(menuButtonClicked) forControlEvents:UIControlEventTouchUpInside];
    [self.buttonsView addSubview:self.menuButton];
    
    RoundTextMaskView *menuMaskView = [[RoundTextMaskView alloc] initWithFrame:self.menuButton.bounds];
    menuMaskView.userInteractionEnabled = NO;
    menuMaskView.text = @"Menu";
    menuMaskView.font = [UIFont systemFontOfSize:20];
    menuMaskView.color = [UIColor redColor];
    [self.menuButton addSubview:menuMaskView];
}

- (void)viewWillAppear:(BOOL)animated {
    [super viewWillAppear:animated];
    
    // TODO
    //[nestopiaCore applyCheatCodes:nil];
    [nestopiaCore startEmulation];
}

- (void)viewWillDisappear:(BOOL)animated {
    [super viewWillDisappear:animated];
    
    [nestopiaCore stopEmulation];
}

- (void)viewDidLayoutSubviews {
    [super viewDidLayoutSubviews];
    
    CGFloat top = [self respondsToSelector:@selector(topLayoutGuide)] ? [self.topLayoutGuide length] : 0;
    
    self.screenView.frame = [self frameForScreenView];
    
    self.buttonsView.frame = self.view.bounds;
    
    self.directionButton.center = CGPointMake(kDirectionButtonRadius + 5, CGRectGetMidY(self.view.bounds));
    self.aButton.center = CGPointMake(CGRectGetMaxX(self.view.bounds) - kABButtonRadius - 5, CGRectGetMidY(self.view.bounds) - 20);
    self.bButton.center = CGPointMake(CGRectGetMaxX(self.view.bounds) - 3*kABButtonRadius - 15, CGRectGetMidY(self.view.bounds) + 20);
    self.selectButton.center = CGPointMake(kSpecialButtonRadius + 5, CGRectGetMidY(self.view.bounds) - 175);
    self.startButton.center = CGPointMake(CGRectGetMaxX(self.view.bounds) - kSpecialButtonRadius - 5, CGRectGetMidY(self.view.bounds) - 175);
    self.menuButton.center = CGPointMake(CGRectGetMaxX(self.view.bounds) - kSpecialButtonRadius - 5, kSpecialButtonRadius + 5 + top);
}

- (CGRect)frameForScreenView {
    CGSize nativeSize = nestopiaCore.nativeResolution;
    CGFloat nativeRatio = nativeSize.width / nativeSize.height;
    
    CGSize viewSize = self.view.bounds.size;
    CGFloat viewRatio = viewSize.width / viewSize.height;
    
    CGFloat scale;
    if (viewRatio > nativeRatio) {
        scale = viewSize.height / nativeSize.height;
    } else {
        scale = viewSize.width / nativeSize.width;
    }
    
    CGSize screenSize = CGSizeMake(nativeSize.width * scale, nativeSize.height * scale);
    CGPoint screenOrigin = CGPointMake(floor((viewSize.width - screenSize.width) / 2),
                                       floor((viewSize.height - screenSize.height) / 2));
    
    return CGRectMake(screenOrigin.x, screenOrigin.y, screenSize.width, screenSize.height);
}

- (void)menuButtonClicked {
    [nestopiaCore stopEmulation];
    
    saveStateSheet = [[UIActionSheet alloc] init];
    saveStateSheet.title = self.game.title;
    
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

- (void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex {
    [self.delegate gamePlayViewControllerDidFinish:self];
}

- (void)actionSheet:(UIActionSheet *)actionSheet clickedButtonAtIndex:(NSInteger)buttonIndex {
	if (actionSheet == saveStateSheet) {
        if (!strstr([self.game.path cStringUsingEncoding: NSASCIIStringEncoding], "(VS)")) {
            buttonIndex++;
        }
        
		if (buttonIndex == 3) { /* Save and Exit Game */
			[nestopiaCore saveState];
            [self.delegate gamePlayViewControllerDidFinish:self];
            return;
		} else if (buttonIndex == 2) { /* Game Settings */
            SettingsViewController *settingsVC = [[SettingsViewController alloc] init];
            settingsVC.game = self.game;
            settingsVC.navigationItem.rightBarButtonItem = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemDone target:self action:@selector(settingsDoneButtonClicked)];
            
            UINavigationController *navCon = [[UINavigationController alloc] initWithRootViewController:settingsVC];
            [self presentViewController:navCon animated:YES completion:nil];
            return;
        } else if (buttonIndex == 0) { /* Insert Coin */
            
            //[nestopiaCore applyGameGenieCodes]; // TODO
            [nestopiaCore startEmulation];
            //[emulatorCore insertCoin1]; // TODO

            return;
        } else if (buttonIndex == 1) { /* Controller Toggle */
            if (pad1) {
                [nestopiaCore activatePad2];
                pad1 = NO;
            } else {
                pad1 = YES;
                [nestopiaCore activatePad1];
            }
            
            //[nestopiaCore applyGameGenieCodes]; // TODO
            [nestopiaCore startEmulation];
            return;
        } else if (buttonIndex == 5) { /* Resume Game */
            //[emulatorCore applyGameGenieCodes]; // TODO
            [nestopiaCore startEmulation];
            return;
        } else {
            [self.delegate gamePlayViewControllerDidFinish:self];
            return;
        }
	}
}

- (BOOL)shouldAutorotate {
    return YES;
}

- (void)willRotateToInterfaceOrientation:(UIInterfaceOrientation)toInterfaceOrientation duration:(NSTimeInterval)duration {
    [nestopiaCore stopEmulation];
}

- (void)didRotateFromInterfaceOrientation:(UIInterfaceOrientation)fromInterfaceOrientation {    
    //[emulatorCore applyGameGenieCodes]; // TODO
    [nestopiaCore startEmulation];
}

- (void)settingsDoneButtonClicked {
    [self dismissViewControllerAnimated:YES completion:nil];
}

- (UIStatusBarStyle)preferredStatusBarStyle {
    return UIStatusBarStyleLightContent;
}

#pragma mark NestopiaCoreInputDelegate

- (NestopiaInput)nestopiaCoreCallbackInput {
    NestopiaPadInput padInput = 0;
    padInput |= self.directionButton.input;
    padInput |= self.selectButton.input;
    padInput |= self.startButton.input;
    padInput |= self.aButton.input;
    padInput |= self.bButton.input;

    NestopiaInput input;
    input.pad1 = pad1 ? padInput : 0;
    input.pad2 = pad1 ? 0 : padInput;
    input.zapper = 0; // TODO
    input.zapperX = 0;
    input.zapperY = 0;
    return input;
}

@end

