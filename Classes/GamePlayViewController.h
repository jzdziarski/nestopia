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

#import "ControllerView.h"
#import "EmulatorCore.h"
#import "ScreenView.h"
#import "SettingsViewController.h"

#define kGamePlaySavedStateNotification         @"GamePlaySavedStateNotification"
#define kGamePlayChangedFavoritesNotification   @"GamePlayChangedFavoritesNotification"

@interface GamePlayViewController : UIViewController <UIActionSheetDelegate>
{
    /* Initialization */
	UIActionSheet *saveStateSheet;
	NSString *gamePath, *gameTitle;
    bool shouldLoadState, loaded, pad1;
    
    /* Game Play */
    EmulatorCore *emulatorCore;
	ScreenView *screenView;
	ControllerView *controllerView;
}

@property(nonatomic,copy) NSString *gamePath;
@property(nonatomic,copy) NSString *gameTitle;
@property(nonatomic,assign) bool shouldLoadState;
@end
