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

#import <UIKit/UIKit.h>

#import "GameROMViewController.h"
#import "SavedGameViewController.h"
#import "BookmarkedGameController.h"
#import "EmulatorViewController.h"
#import "SettingsViewController.h"
#import "EmulatorCore.h"
#import "NavigationController.h"

@interface RootViewController : UIViewController <UITabBarControllerDelegate,UITextFieldDelegate,UIAlertViewDelegate,GameROMSelectionDelegate,UIActionSheetDelegate,UINavigationControllerDelegate> {
	GameROMViewController *gameROMViewController;
	SavedGameViewController *savedGameViewController;
	BookmarkedGameController *bookmarkedGameViewController;
	BookmarkedGameController *recentGameViewController;
	EmulatorViewController *emulatorViewController;
	SettingsViewController *settingsViewController;

	NSMutableArray *viewControllers;
	UITabBarController *tabBarController;
	UIBarButtonItem *rightButton, *leftButton;
	NavigationController *navigationController;
	UIActionSheet *selectedROMSheet, *deleteSavedGameSheet, *saveStateSheet;
	NSString *currentROMImagePath, *currentROMTitle;
	EmulatorCore *emulatorCore;
	UIWindow *window;
	
	/* Objects for ROM download */
	UIAlertView *addROMAlertView;
	UITextField *addROMPath;
	NSMutableData *downloadData;
	NSURLConnection *downloadConnection;
	UIView *waitView;
	UIActivityIndicatorView *activityIndicator;
	UILabel *label;
}
- (void)setNavTitle;
- (void)settingsButtonWasPressed;
- (void)didSelectGameROMAtPath:(NSString *)path;
- (void)startGame:(BOOL)loadState;
- (void)didChangeStatusBarOrientation:(UIInterfaceOrientation)oldStatusBarOrientation;
- (void)initializeWaitView;

@property(nonatomic,assign) NavigationController *navigationController;
@property(nonatomic,assign) UIWindow *window;
@end
