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

#import "GameROMViewController.h"
#import "GamePlayViewController.h"
#import "NESNavigationController.h"

extern BOOL emulatorRunning;

@implementation GameROMViewController

- (id)init {
	self = [ super init ];
    if (self != nil) {
        UIImage *tabBarImage = [ UIImage imageNamed: @"Games.png" ];
        self.tabBarItem = [ [ UITabBarItem alloc ] initWithTitle: @"All Games" image: tabBarImage tag: 0 ];
        
        romCount = 0;
        nActiveSections = 0;
        
        for(int i=0;i<27;i++) {
            fileList[i] = [ [ NSMutableArray alloc ] init ];
        }
        
        activeSections = [ [ NSMutableArray alloc ] init ];
        sectionTitles = [ [ NSMutableArray alloc ] init ];
    }
	return self;
}

- (void)setFavorites:(bool)_favorites {
    favorites = _favorites;

    if (favorites) {
        self.tabBarItem = [ [ UITabBarItem alloc ] initWithTabBarSystemItem: UITabBarSystemItemFavorites tag: nil ];
        
        [ [ NSNotificationCenter defaultCenter ] addObserver: self
                                                    selector: @selector(refreshData)
                                                        name: kGamePlayChangedFavoritesNotification
                                                      object: nil];
    }
}

- (bool)favorites {
    return favorites;
}

- (void)loadView {
    [ super loadView ];
    
	[ self reloadData ];
    [ self.tableView reloadData ];

    [ self.tableView setContentInset:UIEdgeInsetsMake(20, self.tableView.contentInset.left, self.tableView.contentInset.bottom, self.tableView.contentInset.right) ];
}

- (void)viewDidLoad {
    
    [ super viewDidLoad ];
    
    // [ NSTimer scheduledTimerWithTimeInterval: 60.0 target: self selector: @selector(scanForChanges:) userInfo: self repeats: YES ];
}

- (void)scanForChanges:(NSTimer *) timer {
    NSDirectoryEnumerator *dirEnum;
    NSString *file;
    int n = 0;
    
    if (emulatorRunning == YES)
        return;
    
    if (self.tableView.isDragging == YES || self.tableView.isDecelerating == YES || self.tableView.isEditing == YES || self.tableView.isTracking == YES)
    {
        return;
    }
    
    dirEnum = [ [ NSFileManager defaultManager ] enumeratorAtPath: ROM_PATH ];
    while ((file = [ dirEnum nextObject ])) {
        NSString *ext = [ file pathExtension ];
        if ([ [ ext lowercaseString ] isEqualToString: @"nes" ]) {
            n++;
        }
    }
    
    NSLog(@"%s current .nes file count: %d", __PRETTY_FUNCTION__, n);
    
    if (n != romCount) {
        romCount = n;
        [ self reloadData ];
        [ self.tableView reloadData ];
    }
}

- (void) refreshData {
    [ self reloadData ];
    [ self.tableView reloadData ];
}

- (void) reloadData {
	NSDirectoryEnumerator *dirEnum;
	NSString *file;
	int n = 0;
    
    NSLog(@"%s", __PRETTY_FUNCTION__);
    
	for(int i=0;i<27;i++) {
	    [ fileList[i] removeAllObjects ];
	}
    
	dirEnum = [ [ NSFileManager defaultManager ] enumeratorAtPath: ROM_PATH ];
	while ((file = [ dirEnum nextObject ])) {
		NSString *ext = [ file pathExtension ];
        NSString *extension = @"nes";
        if (favorites)
            extension = @"favorite";
		if ([ [ ext lowercaseString ] isEqualToString: extension ]) {
			n++;
            if (favorites)
                file = [ file stringByDeletingPathExtension ];
            // NSLog(@"%s found file %@", __PRETTY_FUNCTION__, file);
			char index = ( [ file cStringUsingEncoding: NSASCIIStringEncoding ] )[0];
			if (index >= 'a' && index <= 'z') {
				index -= 'a';
				[ fileList[(int) index] addObject: file ];
			} else if (index >= 'A' && index <= 'Z') {
				index -= 'A';
				[ fileList[(int) index] addObject: file ];
			} else {
				[ fileList[26] addObject: file ];
			}
		}
	}

    if (!n && !favorites) {
        UIAlertView *alertView = [ [ UIAlertView alloc ] initWithTitle: @"No ROM Images Found" message: @"Use iTunes file sharing to add ROM images. Click on the device in iTunes, click the Apps tab, scroll down to File Sharing, highlight Nescaline, and click the 'Add...' button." delegate: self cancelButtonTitle: nil otherButtonTitles: @"OK", nil];
        [ alertView show ];
    }
    
    romCount = n+1;
	nActiveSections = 0;
    [ activeSections removeAllObjects ];
    [ sectionTitles removeAllObjects ];

	for(int i=0;i<27;i++) {
		if ( [fileList[i] count ]>0) {
			nActiveSections++;
			[ activeSections addObject: fileList[i] ];
			if (i < 26)
				[ sectionTitles addObject: [ NSString stringWithFormat: @"%c", i + 'A' ] ];
			else
				[ sectionTitles addObject: @"0-9" ];
		}
	}
    
}

- (void)dealloc {
    
	for(int i = 0; i < 27; i ++) {
		[ fileList[i] release ];
	}
    
	[ activeSections release ];
	[ sectionTitles release ];
    
    [ super dealloc ];
}


/* UITableViewControllerDelegate Methods */

- (NSArray *)sectionIndexTitlesForTableView:(UITableView *)tableView {
	
	return [ NSMutableArray arrayWithObjects:
			@"A", @"B", @"C", @"D", @"E", @"F",
			@"G", @"H", @"I", @"J", @"K", @"L",
			@"M", @"N", @"O", @"P", @"Q", @"R",
			@"S", @"T", @"U", @"V", @"W", @"X",
			@"Y", @"Z", @"#", nil
            ];
}

- (NSString *)tableView:(UITableView *)tableView titleForHeaderInSection:(NSInteger)section {
	if (!nActiveSections) {
		return nil;
	}
	
	return [ sectionTitles objectAtIndex: section ];
}

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView {
	if (nActiveSections)
		return nActiveSections;
	else
		return 1;
}


- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
	if (!nActiveSections) {
		return 0;
	}
	
	return [ [ activeSections objectAtIndex: section] count ];
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
	if (!nActiveSections) {
		return nil;
	}
    
	NSString *CellIdentifier = [[ activeSections objectAtIndex: [ indexPath indexAtPosition: 0 ]] objectAtIndex: [ indexPath indexAtPosition: 1 ] ];
	
    UITableViewCell *cell = [ tableView dequeueReusableCellWithIdentifier: CellIdentifier ];
    if (cell == nil) {
        NSString *title = [ CellIdentifier stringByReplacingOccurrencesOfString: @".nes" withString: @"" options: NSCaseInsensitiveSearch range: NSMakeRange(0, [ CellIdentifier length ]) ];
      
#if 0
        if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPhone) {
            NSRange range = [ title rangeOfString: @"(" ];
            if (range.length > 0) {
                title = [ title substringToIndex: range.location];
            }
            
            range = [ title rangeOfString: @"[" ];
            if (range.length > 0) {
                title = [ title substringToIndex: range.location];
            }
        }
#endif
        cell = [ [ [ UITableViewCell alloc ] initWithStyle: UITableViewCellStyleDefault  reuseIdentifier: CellIdentifier ] autorelease ];
        cell.textLabel.text = title;
        cell.textLabel.font = [ UIFont fontWithName:@"HelveticaNeue" size: 16.0 ];
        cell.textLabel.adjustsFontSizeToFitWidth = YES;
        cell.textLabel.lineBreakMode = NSLineBreakByWordWrapping;
        cell.textLabel.numberOfLines = 2;
	}
	
    return cell;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
    
	UITableViewCell *cell = [ self.tableView cellForRowAtIndexPath: indexPath ];
	NSString *path = [ [ [ NSString alloc ] initWithFormat: @"%@/%@", ROM_PATH, cell.reuseIdentifier ] autorelease ];
    
    NSLog(@"%s starting game play for %@", __PRETTY_FUNCTION__, path);
    
    GamePlayViewController *gamePlayViewController = [ [ GamePlayViewController alloc ] init ];
    gamePlayViewController.gamePath = path;
    gamePlayViewController.gameTitle = [ [ path stringByDeletingPathExtension ] lastPathComponent ];
    gamePlayViewController.shouldLoadState = NO;
    
    NESNavigationController *navigationController = [ [ NESNavigationController alloc ] initWithRootViewController: [ gamePlayViewController autorelease ] ];
    
    [ self presentViewController: [ navigationController autorelease ] animated: YES completion: nil ];
}

- (NSInteger)tableView:(UITableView *)tableView sectionForSectionIndexTitle:(NSString *)title atIndex:(NSInteger)index;  {
	NSInteger idx = [ sectionTitles indexOfObject: title ];
	return idx;
}

- (void)tableView:(UITableView *)tableView
commitEditingStyle:(UITableViewCellEditingStyle) editingStyle
forRowAtIndexPath:(NSIndexPath *) indexPath
{
	if (editingStyle == UITableViewCellEditingStyleDelete) {
		UITableViewCell *cell = [ self.tableView cellForRowAtIndexPath: indexPath ];
		NSString *path = [ NSString stringWithFormat: @"%@/%@", ROM_PATH, cell.reuseIdentifier ];
        
        if (favorites) {
            path = [ path stringByAppendingPathExtension: @"favorite" ];
        }

		NSLog(@"%s deleting item at %@", __PRETTY_FUNCTION__, path);

		char index = ( [ [ path lastPathComponent ] cStringUsingEncoding: NSASCIIStringEncoding ] )[0];
		if (index >= 'a' && index <= 'z') {
			index -= 'a';
		} else if (index >= 'A' && index <= 'Z') {
			index -= 'A';
		} else {
			index = 26;
		}

		NSMutableArray *section = fileList[(int) index];
		NSError *error;
		[ [ NSFileManager defaultManager ] removeItemAtPath: path error: &error ];
		[ self reloadData ];
		
		if ([ section count ] == 1) {
			char sectionIndex;
			if (index == 26)
				sectionIndex = [ sectionTitles count ];
			else
				sectionIndex = [ sectionTitles indexOfObject: [ NSString stringWithFormat: @"%c", index + 'A' ] ];
			
			if (sectionIndex == -1)
				[ self.tableView reloadData ];
			else
				[ self.tableView deleteSections: [ NSIndexSet indexSetWithIndex: sectionIndex ] withRowAnimation: UITableViewRowAnimationTop ];
		} else {
			NSMutableArray *array;
			array = [ [ NSMutableArray alloc ] init ];
			[ array addObject: indexPath ];
			[ self.tableView deleteRowsAtIndexPaths: array withRowAnimation: UITableViewRowAnimationTop ];
			[ array removeAllObjects ];
			[ array release ];
		}
	}
}

@end
