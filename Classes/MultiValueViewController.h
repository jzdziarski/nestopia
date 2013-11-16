#import <UIKit/UIKit.h>

@protocol MultiValueViewControllerDelegate;

@interface MultiValueViewController : UITableViewController {
	id identifier;
	NSArray *options, *images;
	int selectedItemIndex;
	UITableViewCell *selectedCell;
}

@property(strong) id identifier;
@property(unsafe_unretained,assign) id<MultiValueViewControllerDelegate> delegate;
@property(assign) int selectedItemIndex;
@property(copy) NSArray *options, *images;

@end

@protocol MultiValueViewControllerDelegate <NSObject>

@required
- (void) didSelectItemFromList: (MultiValueViewController *)multiValueViewController selectedItemIndex:(int)selectedItemIndex identifier:(id)identifier;

@end
