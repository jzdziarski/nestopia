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

#import "BookmarkedGameController.h"

@implementation BookmarkedGameController
@synthesize delegate;
@synthesize propertyListFilePath;

+ (BOOL)isBookmarked:(NSString *)path inPropertyList:(NSString *)propertyList {	
	NSArray *bookmarks = [ [ NSArray alloc ] initWithContentsOfFile: [ NSString stringWithFormat: @"%@", propertyList ] ];
	BOOL isBookmarked = [ bookmarks containsObject: path ];
	[ bookmarks release ];
	return isBookmarked;
}

+ (void)deleteBookmark:(NSString *)path inPropertyList:(NSString *)propertyList {
	
	NSLog(@"%s deleting bookmark: %@ from %@\n", __func__, path, propertyList);

    if ([ self isBookmarked: path inPropertyList: propertyList ] == NO) {
		return;
	}

	NSMutableArray *bookmarks = [ [ NSMutableArray alloc ] initWithContentsOfFile: [ NSString stringWithFormat: @"%@", propertyList ] ];
	[ bookmarks removeObject: path ];
	[ bookmarks writeToFile: [ NSString stringWithFormat: @"%@", propertyList ] atomically: YES ];
	[ bookmarks release ];
}

+ (void)addBookmark:(NSString *)path inPropertyList:(NSString *)propertyList {
	NSString *propertyListPath = [ NSString stringWithFormat: @"%@", propertyList ] ;
	NSLog(@"%s adding bookmark: %@ to %@\n", __func__, path, propertyListPath);

    if ([ self isBookmarked: path inPropertyList: propertyList ] == YES) {
		return;
	}
	
	NSMutableArray *bookmarks = [ [ NSMutableArray alloc ] initWithContentsOfFile: propertyListPath ];
	if (bookmarks == nil) 
		bookmarks = [ [ NSMutableArray alloc ] init ];
	[ bookmarks addObject: path ];
	[ bookmarks writeToFile: propertyListPath atomically: YES ];
	[ bookmarks release ];
}

+ (void)addBookmarkToTop:(NSString *)path inPropertyList:(NSString *)propertyList {
	NSString *propertyListPath = [ NSString stringWithFormat: @"%@", propertyList ] ;

	NSLog(@"%s adding bookmark: %@ to top of %@\n", __func__, path, propertyListPath);
	
	NSMutableArray *bookmarks = [ [ NSMutableArray alloc ] initWithContentsOfFile: propertyListPath ];
	if (bookmarks == nil) 
		bookmarks = [ [ NSMutableArray alloc ] init ];
	[ bookmarks removeObject: path ];
	[ bookmarks insertObject: path atIndex: 0 ];
	[ bookmarks writeToFile: propertyListPath atomically: NO ];
	[ bookmarks release ];
}

- (void)loadView {
    [ super loadView ];
	
	[ self reloadData ];
    self.tableView.delegate = self;
}

- (void)viewDidLoad {
    [ super viewDidLoad ];
}

- (void)didReceiveMemoryWarning {
    [ super didReceiveMemoryWarning ]; 
}

- (void) reloadData {
	NSString *path = [ NSString stringWithFormat: @"%@", propertyListFilePath ];	
	NSLog(@"%s loading %@", __func__, path);
	
	bookmarks = [ [ NSMutableArray alloc ] initWithContentsOfFile: path ];
	

	if ([ propertyListFilePath isEqualToString: BOOKMARKS_PLIST ]) {
		NSArray *sorted = [ bookmarks sortedArrayUsingSelector: @selector(caseInsensitiveCompare:)];
		[ bookmarks release ];
		bookmarks = [ [ NSMutableArray alloc ] initWithArray: sorted ];
	}
		
	[ self.tableView reloadData ];
}

- (void)dealloc {
    [ super dealloc ];
}

/* UITableViewControllerDelegate Methods */

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView {
	return 1;
}


- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
	
	return [ bookmarks count ];
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
	NSString *CellIdentifier = [ bookmarks objectAtIndex: [ indexPath indexAtPosition: 1 ] ];
	
    UITableViewCell *cell = [ tableView dequeueReusableCellWithIdentifier: CellIdentifier ];
    if (cell == nil) {
        cell = [ [ [ UITableViewCell alloc ] initWithStyle: UITableViewCellStyleDefault reuseIdentifier: CellIdentifier ] autorelease ];
		
		NSString *ext = [ CellIdentifier pathExtension ];
		if ([ [ ext lowercaseString ] isEqualToString: @"sav" ]) {
			cell.imageView.image = [ [ UIImage alloc ] initWithContentsOfFile: [ [ NSBundle mainBundle ] pathForResource: @"History" ofType: @"png" ] ];
			cell.textLabel.text = [ CellIdentifier stringByReplacingOccurrencesOfString: @".sav" withString: @"" options: NSCaseInsensitiveSearch range: NSMakeRange(0, [ CellIdentifier length ])  ];
		} else {
			cell.textLabel.text = [ CellIdentifier stringByReplacingOccurrencesOfString: @".nes" withString: @"" options: NSCaseInsensitiveSearch range: NSMakeRange(0, [ CellIdentifier length ])  ];
			cell.imageView.image = nil;
		}
	}
	
    return cell;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
	
	UITableViewCell *cell = [ self.tableView cellForRowAtIndexPath: indexPath ];
	NSString *path = [ [ [ NSString alloc ] initWithFormat: @"%@/%@", ROM_PATH, cell.reuseIdentifier ] autorelease ];
	if ([ delegate respondsToSelector:@selector(didSelectGameROMAtPath:) ]) {
        [ self.delegate didSelectGameROMAtPath: path ];
	}
}


- (void)tableView:(UITableView *)tableView 
commitEditingStyle:(UITableViewCellEditingStyle) editingStyle 
forRowAtIndexPath:(NSIndexPath *) indexPath 
{ 
	if (editingStyle == UITableViewCellEditingStyleDelete) {
		
	    /* Delete cell from data source */
		
		int num = [ indexPath indexAtPosition: 1 ];
		[ bookmarks removeObjectAtIndex: num ];
		[ bookmarks writeToFile: [ NSString stringWithFormat: @"%@", propertyListFilePath ] atomically: YES ];
		
		/* Delete cell from table */
	    NSMutableArray *array = [ [ NSMutableArray alloc ] init ];
	    [ array addObject: indexPath ];
        [ self.tableView deleteRowsAtIndexPaths: array withRowAnimation: UITableViewRowAnimationTop ];
	} 
}

@end
