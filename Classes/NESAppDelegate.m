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

#import "NESAppDelegate.h"
#import "RootViewController.h"

@implementation NESAppDelegate

- (void)applicationDidFinishLaunching:(UIApplication *)application {
	window = [ [ UIWindow alloc ] initWithFrame: [ [ UIScreen mainScreen ] bounds ] ];
	rootViewController = [ [ RootViewController alloc ] init ];
	navigationController = [ [ [ NavigationController alloc ] initWithRootViewController: rootViewController ] retain ];
	rootViewController.navigationController = navigationController;
	rootViewController.window = window;
	
    window.rootViewController = navigationController;
	[ window makeKeyAndVisible ];
}

- (void)applicationWillTerminate:(UIApplication *)application {

}

- (void)application:(UIApplication *)application didChangeStatusBarOrientation:(UIInterfaceOrientation)oldStatusBarOrientation {

	[ rootViewController didChangeStatusBarOrientation: oldStatusBarOrientation ];
}

- (void)dealloc {
	[ navigationController release ];
	[ rootViewController release ];
	[ window release ];
	[ super dealloc ];
}

@end
