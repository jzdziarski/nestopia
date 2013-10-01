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

#import "SavedGameViewController.h"

@implementation SavedGameViewController

- (id)init {
	self = [ super init ];

	if (self != nil) {
		UIImage *tabBarImage = [ UIImage imageNamed: @"History.png" ] ;
		self.tabBarItem = [ [ UITabBarItem alloc ] initWithTitle: @"Saved Games" image: tabBarImage tag: 1 ];
        [ self reloadData ];
	}
	
	return self;
}


- (void)loadView {
    [ super loadView ];
    [ self.tableView setContentInset:UIEdgeInsetsMake(20, self.tableView.contentInset.left, self.tableView.contentInset.bottom, self.tableView.contentInset.right) ];
    
    [ [ NSNotificationCenter defaultCenter ] addObserver: self
                                                selector: @selector(reloadData)
                                                    name: kEmulatorCoreSavedStateNotification
                                                  object: nil];
}

- (void)reloadData {
	NSDirectoryEnumerator *dirEnum;
	NSString *file;
	
    fileList = [ [ NSMutableArray alloc ] init ];
	dirEnum = [ [ NSFileManager defaultManager ] enumeratorAtPath: ROM_PATH ];
	while ((file = [ dirEnum nextObject ])) {
		NSString *ext = [ file pathExtension ];
		if ([ [ ext lowercaseString ] isEqualToString: @"sav" ]) {
				[ fileList addObject: file ];
		}
	}
	
	[ self.tableView reloadData ];
}

- (void)dealloc {
	[ fileList release ];
    [ super dealloc ];
}

/* UITableViewControllerDelegate Methods */

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView {
	return 1;
}


- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
	
	return [ fileList count ];
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
	NSString *CellIdentifier = [ fileList objectAtIndex: [ indexPath indexAtPosition: 1 ] ];
	
    UITableViewCell *cell = [ tableView dequeueReusableCellWithIdentifier: CellIdentifier ];
    if (cell == nil) {
        cell = [ [ [ UITableViewCell alloc ] initWithStyle: UITableViewCellStyleDefault  reuseIdentifier: CellIdentifier ] autorelease ];
        
        cell.textLabel.text = [[[[[[[[[ CellIdentifier stringByReplacingOccurrencesOfString: @".nes.sav" withString: @"" options: NSCaseInsensitiveSearch range: NSMakeRange(0, [ CellIdentifier length ])  ] stringByReplacingOccurrencesOfString: @"[!]" withString: @"" ] stringByReplacingOccurrencesOfString: @"(U)" withString: @"" ] stringByReplacingOccurrencesOfString: @"(Unl)" withString: @"" ] stringByReplacingOccurrencesOfString: @"[!p]" withString: @"" ] stringByReplacingOccurrencesOfString: @"[U]" withString: @"" ] stringByReplacingOccurrencesOfString: @"[p1]" withString: @"" ] stringByReplacingOccurrencesOfString: @"(PRG1)" withString: @"" ] stringByReplacingOccurrencesOfString: @"  " withString: @"" ];
        cell.textLabel.font = [ UIFont fontWithName:@"HelveticaNeue" size: 16.0 ];

        
		cell.imageView.image = [ [ UIImage alloc ] initWithContentsOfFile: [ [ NSBundle mainBundle ] pathForResource: @"History" ofType: @"png" ] ];
	}
	
    return cell;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
	UITableViewCell *cell = [ self.tableView cellForRowAtIndexPath: indexPath ];
	NSString *path = [ [ [ NSString alloc ] initWithFormat: @"%@/%@", ROM_PATH, cell.reuseIdentifier ] autorelease ];

    NSLog(@"%s starting game play for %@", __PRETTY_FUNCTION__, path);
    
    GamePlayViewController *gamePlayViewController = [ [ GamePlayViewController alloc ] init ];
    gamePlayViewController.gamePath = [ path stringByDeletingPathExtension ];
    gamePlayViewController.gameTitle = [ [ [ path stringByDeletingPathExtension ] stringByDeletingPathExtension ] lastPathComponent ];
    gamePlayViewController.shouldLoadState = YES;

    UINavigationController *navigationController = [ [ UINavigationController alloc ] initWithRootViewController: [ gamePlayViewController autorelease ] ];
    
    [ self presentViewController: [ navigationController autorelease ] animated: YES completion: nil ];
}


- (void)tableView:(UITableView *)tableView 
commitEditingStyle:(UITableViewCellEditingStyle) editingStyle 
forRowAtIndexPath:(NSIndexPath *) indexPath 
{ 
	if (editingStyle == UITableViewCellEditingStyleDelete) {
		
	    /* Delete cell from data source */
		
		UITableViewCell *cell = [ self.tableView cellForRowAtIndexPath: indexPath ];
		for(int i = 0; i < [ fileList count ]; i++) {
			if ([ cell.reuseIdentifier isEqualToString: [ fileList objectAtIndex: i ] ]) {
				[ fileList removeObjectAtIndex: i ];
				break;
			}
		}
		
		/* Delete cell from table */
	    NSMutableArray *array = [ [ NSMutableArray alloc ] init ];
	    [ array addObject: indexPath ];
        [ self.tableView deleteRowsAtIndexPaths: array withRowAnimation: UITableViewRowAnimationTop ];
		
		/* Delete saved game */
		NSError *error;
		[ [ NSFileManager defaultManager ] removeItemAtPath: [ NSString stringWithFormat: @"%@/%@", ROM_PATH, cell.reuseIdentifier ] error: &error
		];
	} 
}


@end
