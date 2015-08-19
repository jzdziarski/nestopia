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

#import "SettingsViewController.h"
#import "GamePlayViewController.h"
#import "DisclosureIndicator.h"

extern NSString *currentGamePath;

@implementation SettingsViewController
@synthesize gamePath;

- (void) loadSettings {
    [ [ NSUserDefaults standardUserDefaults ] synchronize ];
    settings = [ [ NSUserDefaults standardUserDefaults ] dictionaryRepresentation ];
    self.title = @"Settings";

    /* Global Settings */
    
    swapABControl.on = [ [ settings objectForKey: @"swapAB" ] boolValue ];
    antiAliasControl.on = [ [ settings objectForKey: @"shouldRasterize" ] boolValue ];
    controllerStickControl.on = [ [ settings objectForKey: @"controllerStickControl" ] boolValue ];
    fullScreenControl.on = ([ settings objectForKey: @"fullScreen" ] == nil) ? YES : [ [ settings objectForKey: @"fullScreen" ] boolValue ];
    aspectRatioControl.on = ([ settings objectForKey: @"aspectRatio" ] == nil) ? YES : [ [ settings objectForKey: @"aspectRatio" ] boolValue ];
    
    controllerLayoutIndex =  [ [ settings objectForKey: @"controllerLayout" ] intValue ];
    controllerLayout.text = [ controllerLayoutDescriptions objectAtIndex: controllerLayoutIndex ];
    
    /* Game-Specific Settings */
    
    if (currentGameName != nil) {
        NSString *path = [ NSString stringWithFormat: @"%@/%@.plist", ROM_PATH, currentGameName ];
        settings = [ NSDictionary dictionaryWithContentsOfFile: path ];
        if (!settings) {
            settings = [ [ NSUserDefaults standardUserDefaults ] dictionaryRepresentation ];
        }
        self.title = [ NSString stringWithFormat: @"%@", currentGameName ];
        
        gameGenieControl.on = [ [ settings objectForKey: @"gameGenie" ] boolValue ];
        for(int i = 0; i < 4; i++) {
            gameGenieCodeControl[i].text = [ settings objectForKey: [ NSString stringWithFormat: @"gameGenieCode%d", i ] ];
        }
    }
}

- (void)saveSettings {
    [ [ NSUserDefaults standardUserDefaults ] setBool: swapABControl.on
                                               forKey: @"swapAB" ];
    [ [ NSUserDefaults standardUserDefaults ] setBool: fullScreenControl.on
                                               forKey: @"fullScreen" ];
    [ [ NSUserDefaults standardUserDefaults ] setBool: aspectRatioControl.on
                                               forKey: @"aspectRatio" ];
    [ [ NSUserDefaults standardUserDefaults ] setBool: antiAliasControl.on
                                               forKey: @"shouldRasterize" ];
    [ [ NSUserDefaults standardUserDefaults ] setBool: controllerStickControl.on
                                               forKey: @"controllerStickControl" ];
    [ [ NSUserDefaults standardUserDefaults ] setInteger: controllerLayoutIndex
                                                  forKey: @"controllerLayout" ];
    [ [ NSUserDefaults standardUserDefaults ] synchronize ];
    
    if (currentGameName != nil) {
		NSString *path = [ NSString stringWithFormat: @"%@/%@.plist", ROM_PATH, currentGameName ];
		NSMutableDictionary *gameSettings = [ [ NSMutableDictionary alloc ] init ];
        [ gameSettings setObject: [ NSNumber numberWithBool: gameGenieControl.on ]
                          forKey: @"gameGenie" ];
        
        for(int i = 0; i < 4; i++) {
            if (gameGenieCodeControl[i].text == nil) {
                gameGenieCodeControl[i].text = @"";
            }
            [ gameSettings setObject: gameGenieCodeControl[i].text forKey: [ NSString stringWithFormat: @"gameGenieCode%d", i ] ];
        }
        NSLog(@"%s saving game settings to %@\n", __func__, path);
        
        [ gameSettings writeToFile: path atomically: YES ];
	}
}

- (id) init {
    self = [ super initWithStyle: UITableViewStyleGrouped ];
	
	if (self != nil) {
        UIImage *tabBarImage = [ UIImage imageNamed: @"Settings.png" ];
        self.tabBarItem = [ [ UITabBarItem alloc ] initWithTitle: @"Default Settings" image: tabBarImage tag: 2 ];

        controllerLayoutDescriptions = [ [ NSArray alloc ] initWithObjects: @"Game Pad + Zapper", @"Zapper Only", nil ];
		raised = NO;
		swapABControl = [ [ UISwitch alloc ] initWithFrame: CGRectZero ];
		fullScreenControl = [ [ UISwitch alloc ] initWithFrame: CGRectZero ];
		aspectRatioControl = [ [ UISwitch alloc ] initWithFrame: CGRectZero ];
		gameGenieControl = [ [ UISwitch alloc ] initWithFrame: CGRectZero ];
        antiAliasControl = [ [ UISwitch alloc ] initWithFrame: CGRectZero ];
        controllerStickControl = [ [ UISwitch alloc ] initWithFrame: CGRectZero ];

        controllerLayout = [ [ UITextField alloc ] initWithFrame: CGRectMake(-170, -12.0, 160.0, 30.0) ];
        controllerLayout.textColor = [ UIColor colorWithHue: .6027 saturation: .63 brightness: .52 alpha: 1.0 ];
        controllerLayout.enabled = NO;
		controllerLayout.textAlignment = NSTextAlignmentRight;
		controllerLayout.contentVerticalAlignment = UIControlContentVerticalAlignmentBottom;

        for(int i = 0; i < 4; i++) {
			gameGenieCodeControl[i] = [ [ UITextField alloc ] initWithFrame: CGRectMake(100.0, 5.0, 200.0, 35.0) ];
			gameGenieCodeControl[i].contentVerticalAlignment = UIControlContentVerticalAlignmentCenter;
			gameGenieCodeControl[i].delegate = self;
			gameGenieCodeControl[i].placeholder = @"Empty";
			gameGenieCodeControl[i].returnKeyType = UIReturnKeyDone;
		}
        
        [ self loadSettings ];
        [ self saveSettings ];
	}
	return self;
}

- (void)viewWillAppear:(BOOL)animated {
    [ super viewWillAppear: animated ];
    
    self.navigationController.navigationBar.hidden = NO;

    [ self setGamePath ];
    [ self loadSettings ];
}

- (void)viewWillDisappear:(BOOL)animated {
	
	NSLog(@"%s saving settings", __func__);
	
	[ self saveSettings ];
}

- (void)setGamePath {
    gamePath = [ currentGamePath copy ];
	currentGameName = [ [ [ [ gamePath lastPathComponent ] stringByReplacingOccurrencesOfString: @".sav" withString: @"" ] stringByReplacingOccurrencesOfString: @".nes" withString: @"" ] copy ];
}


- (void)didReceiveMemoryWarning {
    [ super didReceiveMemoryWarning ];
}

- (void)dealloc {
    [ controllerLayoutDescriptions release ];
    [ super dealloc ];
}

/* UITableViewDataSource methods */

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView {
    
    if (currentGameName == nil) {
        return 1;
    } else {
        return 3;
    }
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    switch (section) {
        case(0):
			return 6;
			break;
		case(1):
			return 5;
			break;
        case(2):
            return 1;
            break;
	}
	
	return 0;
}

- (NSString *)tableView:(UITableView *)tableView titleForHeaderInSection:(NSInteger)section { 
	switch (section) {
		case(0):
			return @"Global Settings";
			break;
		case(1):
			return @"Game Genie";
			break;
	}
	return nil;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
	NSString *CellIdentifier = [ NSString stringWithFormat: @"%d:%d", [ indexPath indexAtPosition: 0 ],
								[ indexPath indexAtPosition:1 ] ];
	
    UITableViewCell *cell = [ tableView dequeueReusableCellWithIdentifier: CellIdentifier ];
    if (cell == nil) {
        cell = [ [ [ UITableViewCell alloc ] initWithStyle: UITableViewCellStyleDefault reuseIdentifier: CellIdentifier ] autorelease ];
		cell.selectionStyle = UITableViewCellSelectionStyleNone;
        cell.textLabel.textAlignment = NSTextAlignmentLeft;;

        DisclosureIndicator *accessory = [ DisclosureIndicator accessoryWithColor: [ UIColor colorWithHue: 0 saturation: 0 brightness: .5 alpha: 1.0 ] ];
        accessory.highlightedColor = [ UIColor blackColor ];

		switch ([ indexPath indexAtPosition: 0]) {
			case(0):
				switch([ indexPath indexAtPosition: 1]) {
					case(0):
						cell.accessoryView = fullScreenControl;
						cell.textLabel.text = @"Full Screen";
						break;
					case(1):
						cell.accessoryView = aspectRatioControl;
						cell.textLabel.text = @"Aspect Ratio";
						break;
                    case(2):
                        cell.accessoryView = antiAliasControl;
                        cell.textLabel.text = @"Anti-Alias Video";
                        break;
                    case(3):
						cell.accessoryView = swapABControl;
						cell.textLabel.text = @"Swap A/B";
						break;
                    case(4):
                        cell.accessoryView = controllerStickControl;
                        cell.textLabel.text = @"Sticky Controller";
                        break;
                    case(5):
                        cell.accessoryView = accessory;
                        if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad)
                            cell.textLabel.text = @"Controller Layout";
                        else
                            cell.textLabel.text = @"Controllers";
                        cell.selectionStyle = UITableViewCellSelectionStyleBlue;
                        [ cell.accessoryView addSubview: controllerLayout ];
				}
				break;
			case(1):
				if ([ indexPath indexAtPosition: 1 ] == 0) {
                    cell.accessoryView = gameGenieControl;
                    cell.textLabel.text = @"Game Genie";
                    break;
				} else {
					[ cell addSubview: gameGenieCodeControl[[ indexPath indexAtPosition: 1 ]-1]];
					if (currentGameName == nil) {
						gameGenieCodeControl[[indexPath indexAtPosition: 1 ]-1].text = nil;
						gameGenieCodeControl[[indexPath indexAtPosition: 1 ]-1].placeholder = @"None";
						gameGenieCodeControl[[indexPath indexAtPosition: 1 ]-1].enabled = NO;
					} else {
						gameGenieCodeControl[[indexPath indexAtPosition: 1 ]-1].placeholder = @"Empty";
						gameGenieCodeControl[[indexPath indexAtPosition: 1 ]-1].enabled = YES;
					}

					cell.textLabel.text = [ NSString stringWithFormat: @"Code #%d", [ indexPath indexAtPosition: 1 ] ];
				}
                break;
            case(2):
            {
                bool isFavorite = NO;
                if ([ [ NSFileManager defaultManager ] fileExistsAtPath: [ currentGamePath stringByAppendingPathExtension: @"favorite" ] ])
                {
                    isFavorite = YES;
                }
                if (! isFavorite) {
                    cell.textLabel.text = @"Add to Favorites";
                } else {
                    cell.textLabel.text = @"Remove from Favorites";
                }
                cell.textLabel.textAlignment = NSTextAlignmentCenter;
                cell.selectionStyle  = UITableViewCellSelectionStyleBlue;
                break;
            }
		}
	}
	
    return cell;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
    UITableViewCell *cell = [ self.tableView cellForRowAtIndexPath: indexPath ];

    if ([ indexPath indexAtPosition: 0 ] == 0) {
    
		MultiValueViewController *viewController = [ [ MultiValueViewController alloc ] initWithStyle: UITableViewStyleGrouped ];
        viewController.options = [ NSArray arrayWithArray: controllerLayoutDescriptions ];
        viewController.selectedItemIndex = controllerLayoutIndex;
        viewController.delegate = self;
		[ self.navigationController pushViewController: [ viewController autorelease ] animated: YES ];
    }
    
    if ([ indexPath indexAtPosition: 0 ] == 2) {
        bool isFavorite = NO;
        if ([ [ NSFileManager defaultManager ] fileExistsAtPath: [ currentGamePath stringByAppendingPathExtension: @"favorite" ] ])
        {
            isFavorite = YES;
        }

        if (! isFavorite) {
            NSLog(@"%s adding favorite at path %@", __PRETTY_FUNCTION__, [ currentGamePath stringByAppendingPathExtension: @"favorite" ]);
            [ @"favorite" writeToFile:[ currentGamePath stringByAppendingPathExtension: @"favorite" ] atomically: YES encoding: NSASCIIStringEncoding error: nil ];
            cell.textLabel.text = @"Remove from Favorites";
        } else {
            NSLog(@"%s removing favorite at path %@", __PRETTY_FUNCTION__, [ currentGamePath stringByAppendingPathExtension: @"favorite" ]);

            [ [ NSFileManager defaultManager ] removeItemAtPath: [ currentGamePath stringByAppendingPathExtension: @"favorite" ] error: nil ];
            cell.textLabel.text = @"Add to Favorites";
        }
        
        [ [ NSNotificationCenter defaultCenter ] postNotificationName: kGamePlayChangedFavoritesNotification object: currentGamePath ];
    }
    
    [ cell setSelected: NO animated: NO ];
}

- (NSString *)tableView:(UITableView *)tv titleForFooterInSection:(NSInteger)section
{
    if (section == 0 && currentGamePath == nil) {
        return @"To access Game Genie settings, enter settings from within the active game play menu.";
    }
    return nil;
}

/* UITextFieldDelegate Methods */

- (BOOL) textFieldShouldReturn:(UITextField *)textField {
	
	if (raised == YES) {
		[ UIView beginAnimations: nil context: NULL ]; 
		[ UIView setAnimationDuration: 0.3 ]; 
		CGRect frame = self.view.frame; 
		frame.origin.y += 200.0; 
		self.view.frame = frame; 
		[ UIView commitAnimations ]; 
		raised = NO;
	}
	
	self.tableView.scrollEnabled = YES;
    [ textField resignFirstResponder ];
    return YES;
}

- (void)textFieldDidBeginEditing:(UITextField *)textField {
	
	if (raised == NO) {
		if (   textField == gameGenieCodeControl[0] 
			|| textField == gameGenieCodeControl[1]
			|| textField == gameGenieCodeControl[2] 
			|| textField == gameGenieCodeControl[3])
		{
			[ UIView beginAnimations: nil context: NULL ]; 
			[ UIView setAnimationDuration: 0.3 ]; 
			CGRect frame = self.view.frame; 
			frame.origin.y -= 200.0; 
			self.view.frame = frame; 
			[ UIView commitAnimations ];
			raised = YES;
		}
	}
}

/* MultiValueViewControllerDelegate methods */

- (void) didSelectItemFromList: (MultiValueViewController *)multiValueViewController selectedItemIndex:(int)selectedItemIndex identifier:(id)identifier
{
    controllerLayoutIndex = selectedItemIndex;
    controllerLayout.text = [ controllerLayoutDescriptions objectAtIndex: controllerLayoutIndex ];
    [ self saveSettings ];
    // [ self.tableView reloadData ];
}
@end
