#import "MultiValueViewController.h"

@implementation MultiValueViewController
@synthesize identifier;
@synthesize delegate;
@synthesize options, images;
@synthesize selectedItemIndex;

- (id)initWithStyle:(UITableViewStyle)style {
    if (self = [super initWithStyle:style]) {
		selectedItemIndex = 0;
    }
    return self;
}

- (void)loadView {
	[ super loadView ];
}

- (void)didReceiveMemoryWarning {
    [ super didReceiveMemoryWarning ];
}

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView {
    return 1;
}


- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    return [ options count ];
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    
	NSString *CellIdentifier = [ options objectAtIndex: [ indexPath indexAtPosition: 1 ] ]; //@"Cell";
    UITableViewCell *cell = [ tableView dequeueReusableCellWithIdentifier:CellIdentifier ];
    if (cell == nil) {
        cell = [[[UITableViewCell alloc] initWithStyle: UITableViewCellStyleDefault reuseIdentifier:CellIdentifier] autorelease];
		cell.textLabel.text = [ options objectAtIndex: [ indexPath indexAtPosition: 1 ] ];
		
		if ([ images count ] > [ indexPath indexAtPosition: 1 ]) {
			cell.imageView.image = [ images objectAtIndex: [ indexPath indexAtPosition: 1 ] ];
		}
		
		if (selectedItemIndex == [ indexPath indexAtPosition: 1 ]) {
			cell.accessoryType = UITableViewCellAccessoryCheckmark;
			selectedCell = cell;
		} else {
			cell.accessoryType = UITableViewCellAccessoryNone;
		}
    }
	
    return cell;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {

	UITableViewCell *cell = [ self.tableView cellForRowAtIndexPath: indexPath ];

	[ cell setSelected: NO animated: NO ];
	[ selectedCell setSelected: NO animated: NO ];
	[ selectedCell setAccessoryType: UITableViewCellAccessoryNone ];
	[ cell setAccessoryType: UITableViewCellAccessoryCheckmark ];
	[ cell setSelected: YES animated: YES ];
	[ cell setSelected: NO animated: YES ];
	
	selectedCell = cell;
    if ([ delegate respondsToSelector: @selector(didSelectItemFromList:selectedItemIndex:identifier:) ]) {
        NSLog(@"%s selected item: %d", __PRETTY_FUNCTION__, [ indexPath indexAtPosition: 1 ]);
		[ delegate didSelectItemFromList: self selectedItemIndex: [ indexPath indexAtPosition: 1 ] identifier: identifier ];
	}
}

- (int)selectedItemIndex {
    
    return selectedItemIndex;
}

- (void)setSelectedItemIndex:(int)index {
	UITableViewCell *cell = [ self.tableView cellForRowAtIndexPath: [ [ NSIndexPath indexPathWithIndex: 0 ] indexPathByAddingIndex: index ] ];
	
	selectedItemIndex = index;
	[ selectedCell setSelected: NO animated: NO ];
	[ selectedCell setAccessoryType: UITableViewCellAccessoryNone ];
	
	if (cell != nil) {
		[ cell setAccessoryType: UITableViewCellAccessoryCheckmark ];
		[ cell setSelected: YES animated: YES ];
		[ cell setSelected: NO animated: YES ];

		selectedCell = cell;
	}
}

- (void)dealloc {
	
	NSLog(@"%s", __func__);
	
	[ options dealloc ];
    [ super dealloc ];
}


@end

