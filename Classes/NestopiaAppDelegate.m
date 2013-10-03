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

#import "NestopiaAppDelegate.h"

@implementation NestopiaAppDelegate

- (void)applicationDidFinishLaunching:(UIApplication *)application {
	window = [ [ UIWindow alloc ] initWithFrame: [ [ UIScreen mainScreen ] bounds ] ];

    tabBarController = [ self initializeTabBar ];
    window.rootViewController = tabBarController;

	[ window makeKeyAndVisible ];
}

- (void)applicationWillTerminate:(UIApplication *)application {

}

- (void)dealloc {
    [ gameROMViewController release ];
    [ bookmarksViewController release ];
    [ savedGameViewController release ];
    [ settingsViewController release ];
    [ tabBarController release ];

	[ window release ];
	[ super dealloc ];
}

- (NESTabBarController *)initializeTabBar {
    tabBarController = [ [ NESTabBarController alloc ] init ];
	gameROMViewController = [ [ GameROMViewController alloc ] init ];
    bookmarksViewController = [ [ GameROMViewController alloc ] init ];
	savedGameViewController = [ [ SavedGameViewController alloc ] init ];
    settingsViewController = [ [ SettingsViewController alloc ] init ];

    bookmarksViewController.favorites = YES;
    tabBarController.viewControllers = [ NSArray arrayWithObjects: gameROMViewController, savedGameViewController, bookmarksViewController, settingsViewController, nil ];
	tabBarController.tabBar.translucent = NO;

    return tabBarController;
}

@end
