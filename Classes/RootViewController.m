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

#import <Foundation/NSFileManager.h>
#import "RootViewController.h"
#import "NESAppDelegate.h"
#import "EmulatorCore.h"
#include <sys/stat.h>

@implementation RootViewController
@synthesize navigationController;
@synthesize window;

- (id)init {
	self = [ super init ];
	if (self != nil) {				
		[ [ NSFileManager defaultManager ] createDirectoryAtPath: ROM_PATH withIntermediateDirectories: YES attributes: nil error: nil ];
		
		if ([ [ NSUserDefaults standardUserDefaults ] objectForKey: @"firstrun" ] == nil) {
			NSDirectoryEnumerator *dirEnum;
			NSString *file;
			
			navigationController = nil;
			currentROMImagePath = nil;
			currentROMTitle = nil;
			
			NSLog(@"%s first run looking in %@", __func__, [ NSString stringWithFormat: @"%@/Nescaline.app", NSHomeDirectory() ]);

			dirEnum = [ [ NSFileManager defaultManager ] enumeratorAtPath: [ NSString stringWithFormat: @"%@/Nescaline.app", NSHomeDirectory() ] ];
			while ((file = [ dirEnum nextObject ])) {
				NSLog(@"%s found %@", __func__, file);
				NSString *ext = [ file pathExtension ];
				if ([ [ ext lowercaseString ] isEqualToString: @"nes" ]) {
					NSLog(@"%s copying file %@/Nescaline.app/%@", __func__, NSHomeDirectory(), file);
					[ [ NSFileManager defaultManager ] copyItemAtPath: [ NSString stringWithFormat: @"%@/Nescaline.app/%@", NSHomeDirectory(), file ] toPath: [ NSString stringWithFormat: @"%@/%@", ROM_PATH, file ] error: nil ]; 
				}
			}

			[ [ NSUserDefaults standardUserDefaults ] setBool: NO forKey: @"firstrun" ];
		}
		
	}
	return self;
}

- (void)initializeWaitView {
	waitView = [ [ UIView alloc ] initWithFrame: CGRectMake(0.0, 0.0, self.view.frame.size.width, self.view.frame.size.height) ];
	waitView.backgroundColor = [ UIColor whiteColor ];
	
	activityIndicator = [ [ UIActivityIndicatorView alloc ] initWithActivityIndicatorStyle: UIActivityIndicatorViewStyleGray ];
	activityIndicator.frame = CGRectMake(105.0, 180.0, 25.0, 25.0);
	[ activityIndicator sizeToFit ];
	
	label = [ [ [ UILabel alloc ] initWithFrame: CGRectMake(135.0, 177.0, 200.0, 25.0) ] autorelease ];
	label.font = [ UIFont boldSystemFontOfSize: 14.0 ];
	label.text = @"Loading...";
	label.textColor = [ UIColor darkGrayColor ];
	label.backgroundColor = [ UIColor clearColor ];
	label.adjustsFontSizeToFitWidth = YES;
	
	[ waitView addSubview: activityIndicator ];
	[ waitView addSubview: label ];
}


- (void)loadView {
	[ super loadView ];
	
	UIImage *tabBarImage;
	
    [ [ self view ] setBounds: [ [ UIScreen mainScreen ] bounds ] ];

	gameROMViewController = [ [ GameROMViewController alloc ] init ];
	gameROMViewController.delegate = self;

	savedGameViewController = [ [ SavedGameViewController alloc ] init ];
	savedGameViewController.delegate = self;
	
	bookmarkedGameViewController = [ [ BookmarkedGameController alloc ] init ];
	bookmarkedGameViewController.propertyListFilePath = BOOKMARKS_PLIST;
	tabBarImage = [ UIImage imageWithContentsOfFile: [ [ NSBundle mainBundle ] pathForResource: @"Bookmarks" ofType: @"png" ] ]; 
	bookmarkedGameViewController.tabBarItem = [ [ UITabBarItem alloc ] initWithTitle: @"Bookmarks" image: tabBarImage tag: 0 ];
	bookmarkedGameViewController.delegate = self;

	recentGameViewController = [ [ BookmarkedGameController alloc ] init ];
	recentGameViewController.propertyListFilePath = RECENTS_PLIST;
	tabBarImage = [ UIImage imageWithContentsOfFile: [ [ NSBundle mainBundle ] pathForResource: @"MostRecent" ofType: @"png" ] ]; 
	recentGameViewController.tabBarItem = [ [ UITabBarItem alloc ] initWithTitle: @"Most Recent" image: tabBarImage tag: 0 ];
	recentGameViewController.delegate = self;
	
	settingsViewController = [ [ SettingsViewController alloc ] init ];
	
	viewControllers = [ [ NSMutableArray alloc ] init ];
	[ viewControllers addObject: gameROMViewController ];
	[ viewControllers addObject: savedGameViewController ];
	[ viewControllers addObject: bookmarkedGameViewController ];
	[ viewControllers addObject: recentGameViewController ];
	
	[ self initializeWaitView ];
	waitView.hidden = YES;
	[ self.view addSubview: waitView ];
	
	tabBarController = [ [ UITabBarController alloc ] init ];
	tabBarController.viewControllers = viewControllers;
	tabBarController.delegate = self;
	tabBarController.view.frame = self.view.bounds;
 	navigationController.delegate = self;
	[ self.view addSubview: tabBarController.view ];
	
	[ self setNavTitle ];
}

- (void)setNavTitle {
	
	if (navigationController.topViewController != settingsViewController) {
		switch (tabBarController.selectedIndex) {
			case(0):
			default:
				self.title = @"All Games";
				break;
			case(1):
				self.title = @"Saved Games";
				break;
			case(2):
				self.title = @"Bookmarks";
				break;
			case(3):
				self.title = @"Most Recent";
				break;
		}
	}
	
	if (tabBarController.selectedIndex == 0 || tabBarController.selectedIndex > 3) {
		leftButton = [ [ [ UIBarButtonItem alloc ] 
						 initWithTitle:@"Settings"
						 style: UIBarButtonItemStylePlain
						 target: self
						 action:@selector(settingsButtonWasPressed) 
					   ] autorelease ];
		
        
		rightButton = [ [ [ UIBarButtonItem alloc ]
						 initWithBarButtonSystemItem: UIBarButtonSystemItemAdd 
						 target: self 
						 action: @selector(addButtonWasPressed) ] 
					   autorelease ];
		
		self.navigationItem.rightBarButtonItem = rightButton;
		self.navigationItem.leftBarButtonItem = leftButton;
	} else {
		self.navigationItem.rightBarButtonItem = nil;
		self.navigationItem.leftBarButtonItem = nil;
	}
}

- (void)settingsButtonWasPressed {
    if (navigationController != nil) {
		[ navigationController pushViewController: settingsViewController animated: YES ];
	}
}

- (void)addButtonWasPressed {
	addROMAlertView = [ [ UIAlertView alloc ]
						 initWithTitle: @"Add ROM" 
						 message: @"Enter the URL to load:\n\n"
						 delegate: self
						 cancelButtonTitle: @"Cancel"
						 otherButtonTitles: @"Load", nil
						 ];
	
	addROMPath = [ [ UITextField alloc ] initWithFrame: CGRectMake(12.0, 70.0, 260.0, 25.0) ];
	addROMPath.backgroundColor = [ UIColor clearColor ];
	addROMPath.delegate = self;
	addROMPath.borderStyle = UITextBorderStyleRoundedRect;
    addROMPath.backgroundColor = [ UIColor whiteColor ];
	if ([ [ NSUserDefaults standardUserDefaults ] stringForKey: @"lastLoadPath" ] != nil)
		addROMPath.text = [ NSString stringWithFormat: @"%@/", [ [ NSUserDefaults standardUserDefaults ] stringForKey: @"lastLoadPath" ] ];
	else
		addROMPath.text = @"http://";
	addROMPath.clearButtonMode = UITextFieldViewModeAlways;
	addROMPath.adjustsFontSizeToFitWidth = YES;
	addROMPath.returnKeyType = UIReturnKeyDone;
		
	[ addROMAlertView addSubview: addROMPath ];
	[ addROMAlertView show ];
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
	
	if (interfaceOrientation == UIDeviceOrientationLandscapeLeft || interfaceOrientation == UIDeviceOrientationPortrait)
		return YES;
	
	return NO;
}

- (void)didReceiveMemoryWarning {
    [ super didReceiveMemoryWarning ];
}

- (void)didChangeStatusBarOrientation:(UIInterfaceOrientation)oldStatusBarOrientation {
	
	NSLog(@"%s", __func__);
	
	if (emulatorCore == nil) {
		NSLog(@"%s emulator core is nil, aborting", __func__);
		return;
	}
	
	NSLog(@"%s reconfiguring emulator display", __func__);
	
	emulatorCore.screenDelegate = nil;
	
	if (UIInterfaceOrientationIsLandscape([ UIApplication sharedApplication ].statusBarOrientation) == YES) {
		NSLog(@"%s hiding navigation bar", __func__);
		[ [ self navigationController ] setNavigationBarHidden: YES animated: NO ];
        [[UIApplication sharedApplication] setStatusBarHidden:YES withAnimation: UIStatusBarAnimationNone];
		
	} else {
		[ [ self navigationController ] setNavigationBarHidden: NO animated: NO ];
        [[UIApplication sharedApplication] setStatusBarHidden:NO withAnimation: UIStatusBarAnimationNone];
	}
	
	[ [ self view ] setBounds: [ [ UIScreen mainScreen ] bounds ] ];

	emulatorViewController.emulatorCore = nil;
	emulatorViewController = [ [ EmulatorViewController alloc ] init ];
	emulatorViewController.title = currentROMTitle;
	emulatorViewController.resize = YES;
	
	[ self.navigationController popToViewController: self animated: NO ];
	[ navigationController pushViewController: [ emulatorViewController autorelease ] animated: YES ];
	
    [ self performSelectorInBackground: @selector(hookEmulator:) withObject: nil ];
#if 0
    NSLog(@"%s assigning screenView %@", __PRETTY_FUNCTION__, emulatorViewController.screenView);
	emulatorCore.screenDelegate = emulatorViewController.screenView;
	emulatorCore.frameBufferAddress = (word *) emulatorViewController.screenView.frameBufferAddress;
	emulatorCore.frameBufferSize = emulatorViewController.view.frame.size;
	emulatorViewController.screenView.delegate = emulatorCore;
	emulatorViewController.controllerView.delegate = emulatorCore;
	emulatorViewController.emulatorCore = emulatorCore;
#endif
	navigationController.delegate = self;
}

- (void)startGame:(BOOL)loadState {
	
	if (UIInterfaceOrientationIsLandscape([ UIApplication sharedApplication ].statusBarOrientation) == YES) {
		NSLog(@"%s hiding navigation bar", __func__);
		[ [ self navigationController ] setNavigationBarHidden: YES animated: NO ];
        [[UIApplication sharedApplication] setStatusBarHidden:YES withAnimation: UIStatusBarAnimationNone];
		
	} else {
		[ [ self navigationController ] setNavigationBarHidden: NO animated: NO ];
        [[UIApplication sharedApplication] setStatusBarHidden:NO withAnimation: UIStatusBarAnimationNone];
	}

	[ [ self view ] setBounds: [ [ UIScreen mainScreen ] bounds ] ];

	emulatorViewController = [ [ EmulatorViewController alloc ] init ];
    NSLog(@"Current ROM Title: %@", currentROMTitle);
    emulatorViewController.title = currentROMTitle;
	emulatorCore = [ [ EmulatorCore alloc ] init ];
	emulatorViewController.emulatorCore = emulatorCore;

	EmulatorCoreLoadStatus success = [ emulatorCore loadROM: currentROMImagePath ];
	
	if (success != EmulatorCoreLoadStatusSuccess) {
				UIAlertView *myAlert = [ [ UIAlertView alloc ] 
								initWithTitle:@"Unable to Load ROM" 
								message: nil
								delegate: self
								cancelButtonTitle: nil 
								otherButtonTitles: @"OK", nil ];
		if (success == EmulatorCoreLoadStatusInvalidROM) {
			myAlert.message = @"This ROM may be invalid or corrupt.";
		} else {
			myAlert.message = @"This ROM is not yet supported.";
		}
		[ myAlert show ];
		return;
	}
	
	[ BookmarkedGameController addBookmarkToTop: [ currentROMImagePath lastPathComponent ] inPropertyList: RECENTS_PLIST ];
	[ recentGameViewController reloadData ];
	
	[ navigationController pushViewController: [ emulatorViewController autorelease ] animated: YES ];
	[ emulatorCore initializeEmulator ];
	
	if (loadState == YES) 
		[ emulatorCore loadState ];
	
	[ emulatorCore configureEmulator ];
	[ emulatorCore applyGameGenieCodes ];
    
    [ self performSelectorInBackground: @selector(hookEmulator:) withObject: nil ];
}

- (void)hookEmulator:(id)object {
    while(emulatorViewController.screenView == nil) {
        usleep(100);
        NSLog(@"%s waiting for emulatorViewController", __PRETTY_FUNCTION__);
    }
    NSLog(@"%s assigning screenView %@ from emulatorViewController %@", __PRETTY_FUNCTION__, emulatorViewController.screenView, emulatorViewController);

	emulatorCore.screenDelegate = emulatorViewController.screenView;
	emulatorCore.frameBufferAddress = (word *) emulatorViewController.screenView.frameBufferAddress;
	emulatorCore.frameBufferSize = emulatorViewController.view.frame.size;
	emulatorViewController.screenView.delegate = emulatorCore;
	emulatorViewController.controllerView.delegate = emulatorCore;
	
	navigationController.delegate = self;

    [ emulatorCore startEmulator ];
}

- (void)dealloc {
	[ tabBarController release ];
	[ gameROMViewController release ];
	[ savedGameViewController release ];
	[ bookmarkedGameViewController release ];
	[ recentGameViewController release ];
	[ settingsViewController release ];
    [ super dealloc ];
}

/* UITabBarControllerDelegate Methods */

- (void)tabBarController:(UITabBarController *)tabBarController didSelectViewController:(UIViewController *)viewController {
	[ self setNavTitle ];
}

/* GameROMSelectionDelegate Methods */

- (void)didSelectGameROMAtPath:(NSString *)path {
    BOOL bookmarked = [ BookmarkedGameController isBookmarked: [ path lastPathComponent ] inPropertyList: BOOKMARKS_PLIST];
	
	NSLog(@"%s game ROM selected: %@\n", __func__, path);
	currentROMImagePath = [ path copy ];
	currentROMTitle = [ [ NSString alloc ] initWithString: [ [ currentROMImagePath lastPathComponent ] stringByDeletingPathExtension ] ];
	if ([ [ currentROMTitle pathExtension ] isEqualToString: @"nes" ])
		currentROMTitle = [ currentROMTitle stringByDeletingPathExtension ];
    
	settingsViewController.currentROMImagePath = currentROMImagePath;
	
    selectedROMSheet = [ [ [ UIActionSheet alloc ] initWithFrame: CGRectMake(0.0, 480.0, 320.0, 240.0) ] autorelease ];
    selectedROMSheet.title = currentROMTitle;
	selectedROMSheet.delegate = self;
	
    if ([ [ path pathExtension ] isEqualToString: @"sav" ]) {
        [ selectedROMSheet addButtonWithTitle: @"Restore Saved Game" ];
        [ selectedROMSheet addButtonWithTitle: @"Delete Saved Game" ];
    } else {
        [ selectedROMSheet addButtonWithTitle:@"Start New Game" ];
    }
	
    if (bookmarked == NO)
        [ selectedROMSheet addButtonWithTitle:@"Bookmark" ];
	
    [ selectedROMSheet addButtonWithTitle:@"Cancel" ];

	[ selectedROMSheet showInView: (UIView *) [ [ window subviews ] objectAtIndex: 0 ] ];
}


/* UIActionSheetDelegate Methods */
- (void)actionSheet:(UIActionSheet *)actionSheet clickedButtonAtIndex:(NSInteger)buttonIndex {
	
	/* Game selection sheet */
	if (actionSheet == selectedROMSheet) {
		BOOL bookmarked = [ BookmarkedGameController isBookmarked: [ currentROMImagePath lastPathComponent ] inPropertyList: BOOKMARKS_PLIST];
		int bookmarkIndex = 1;
		if ([ [ currentROMImagePath pathExtension ] isEqualToString: @"sav" ]) {
			if (buttonIndex == 0) {
				currentROMImagePath = [ currentROMImagePath stringByReplacingOccurrencesOfString: @".sav" withString: @"" ];
				[ self startGame: YES ];
			} else if (buttonIndex == 1) {
				deleteSavedGameSheet = [ [ [ UIActionSheet alloc ] initWithFrame: CGRectMake(0.0, 480.0, 320.0, 240.0) ] autorelease ];
				deleteSavedGameSheet.title = @"Are you sure?";
				deleteSavedGameSheet.delegate = self;
				deleteSavedGameSheet.destructiveButtonIndex = 0;
				[ deleteSavedGameSheet addButtonWithTitle: @"Delete Saved Game" ];
				[ deleteSavedGameSheet addButtonWithTitle: @"Cancel" ];
				[ deleteSavedGameSheet showInView: (UIView *) [ [ window subviews ] objectAtIndex: 0 ] ];
			}
			bookmarkIndex = 2;
		} else {
			if (buttonIndex == 0) {
				[ self startGame: NO ];
			}
		}
			
		if (buttonIndex == bookmarkIndex && bookmarked == NO) {
				[ BookmarkedGameController addBookmark: [ currentROMImagePath lastPathComponent ] inPropertyList: BOOKMARKS_PLIST ];
				[ bookmarkedGameViewController reloadData ]; 
		}
	}

    /* Saved game deletion sheet */
	if (actionSheet == deleteSavedGameSheet) {
		if (buttonIndex == 0) {
			NSError *error;
			[ [ NSFileManager defaultManager ] removeItemAtPath:currentROMImagePath error: &error ];
			
			[ savedGameViewController reloadData ];
		}
	}
	
	if (actionSheet == saveStateSheet) {
		if (buttonIndex == 0) {
			[ emulatorCore saveState ];
			[ savedGameViewController reloadData ];
		}
		[ emulatorCore finishEmulator ];
		emulatorCore = nil;
	}
 }

/* UINavigationControllerDelegate Methods */

- (void)navigationController:(UINavigationController *)_navigationController willShowViewController:
	(UIViewController *)viewController animated:(BOOL)animated
{	
		
	if (viewController != self) {
		return;
	}
	
	[ [ self navigationController ] setNavigationBarHidden: NO animated: NO ];
	[[UIApplication sharedApplication] setStatusBarHidden:NO withAnimation: UIStatusBarAnimationNone];
	[ [ self view ] setBounds: [ [ UIScreen mainScreen ] bounds ] ];

	/* Stop emulator */
	if (emulatorCore != nil) {
						
		if ([ [ NSUserDefaults standardUserDefaults ] boolForKey: @"autoSave" ] == YES) {
			[ emulatorCore saveState ];
			[ savedGameViewController reloadData ];
			[ emulatorCore finishEmulator ];
			emulatorCore = nil;
		} else {
			saveStateSheet = [ [ UIActionSheet alloc ] init ];
			saveStateSheet.title = @"Do you want to save this game?";
			[ saveStateSheet addButtonWithTitle: @"Save Game" ];
			[ saveStateSheet addButtonWithTitle: @"Don't Save" ];
			saveStateSheet.destructiveButtonIndex = 1;
			saveStateSheet.delegate = self;

			[ saveStateSheet showInView: (UIView *) [ [ window subviews ] objectAtIndex: 0 ] ];
		}
	}
}

/* UIAlertViewDelegate methods */

- (void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex {
	
	/* Add ROM */
	if (alertView == addROMAlertView) {
		if (buttonIndex == 0) {
			[ alertView release ];
			[ addROMPath release ];
			return;
		}
		
		[ [ NSUserDefaults standardUserDefaults ] setObject: [ addROMPath.text stringByDeletingLastPathComponent ] forKey: @"lastLoadPath" ];
		
		/* Add wait view */
		label.text = @"Loading...";
		[ activityIndicator startAnimating ];
		waitView.hidden = NO;
		[ UIApplication sharedApplication ].networkActivityIndicatorVisible = YES;
		
		/* Initiate a ROM download */
		NSURL *url = [ NSURL URLWithString: [ addROMPath.text stringByAddingPercentEscapesUsingEncoding: NSASCIIStringEncoding ] ];
		NSURLRequest *request = [ [ [ NSURLRequest alloc ] initWithURL: url
														   cachePolicy: NSURLRequestReloadIgnoringCacheData
													   timeoutInterval: 60.0
								 ] autorelease ];
		downloadConnection = [ [ NSURLConnection alloc ] initWithRequest: request 
																delegate: self ]; 
		
		
		[ alertView release ];
	}
	
}

/* UITextFieldDelegate methods */

- (BOOL)textFieldShouldReturn:(UITextField *)textField {

    [ textField resignFirstResponder ];
    return YES;
}


/* NSURLConnection methods */

-(void)connection:(NSURLConnection *)connection didReceiveResponse:(NSURLResponse *)response {
	NSLog(@"%s", __func__);
	
	if([response respondsToSelector:@selector(statusCode)]){
        int statusCode = [(NSHTTPURLResponse *)response statusCode];
        if (statusCode >= 400){
			
			NSLog(@"%s received HTTP response code %d", __func__, statusCode);
			
			[ connection cancel ];
			[ UIApplication sharedApplication ].networkActivityIndicatorVisible = NO;
			waitView.hidden = YES;
			[ activityIndicator stopAnimating ];
			
			UIAlertView *alertView = [ [ UIAlertView alloc ] initWithTitle: @"Connection Error" message: [ NSString stringWithFormat: @"Received HTTP error code %d", statusCode ] delegate: self cancelButtonTitle: nil otherButtonTitles: @"OK", nil ];
			[ alertView show ];
			
			[ downloadConnection release ];
			[ downloadData release ];	
        }
    }
	
	downloadData = [ [ NSMutableData data ] retain ];
}

-(void)connection:(NSURLConnection *)connection didReceiveData:(NSData *)data
{
	[ downloadData appendData: data ];
	label.text = [ NSString stringWithFormat: @"Loading [%d]", [ downloadData length ] ];
	NSLog(@"%s length %d", __func__, [ downloadData length ]);
}

-(void)connectionDidFinishLoading:(NSURLConnection *)connection
{
	NSLog(@"%s", __func__);
	NSString *path = [ addROMPath.text lastPathComponent ];
	
	if ( (!([ path hasSuffix: @"nes" ])) && (!([ path hasSuffix: @"NES" ]))) {
		path = [ path stringByAppendingFormat: @".nes" ];
	}
	[ downloadData writeToFile: [ NSString stringWithFormat: @"%@/%@", ROM_PATH, path ] atomically: NO ];
	[ downloadConnection release ];
	[ downloadData release ];
	
	[ gameROMViewController reloadData ];
	[ gameROMViewController.tableView reloadData ];

	[ UIApplication sharedApplication ].networkActivityIndicatorVisible = NO;
	waitView.hidden = YES;
	[ activityIndicator stopAnimating ];
}

-(void)connection:(NSURLConnection *)connection didFailWithError:(NSError *)error
{
	NSLog(@"%s", __func__);
	
	[ UIApplication sharedApplication ].networkActivityIndicatorVisible = NO;
	waitView.hidden = YES;
	[ activityIndicator stopAnimating ];
	
	UIAlertView *alertView = [ [ UIAlertView alloc ] initWithTitle: @"Connection Error" message: [ [ error localizedDescription ] capitalizedString ] delegate: self cancelButtonTitle: nil otherButtonTitles: @"OK", nil ];
	[ alertView show ];
	
	[ downloadConnection release ];
	[ downloadData release ];	
}

@end

