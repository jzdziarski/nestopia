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

@implementation SettingsViewController

- (void) loadSettings {
    NSDictionary *settings;
    
    if (self.game) {
        self.title = self.game.title;
        settings = self.game.settings;
    } else {
        self.title = @"Settings";
        settings = [EmulatorCore globalSettings];
    }

    /* Global Settings */
    
    swapABControl.on = [[settings objectForKey: @"swapAB"] boolValue];
    antiAliasControl.on = [[settings objectForKey: @"shouldRasterize"] boolValue];
    controllerStickControl.on = [[settings objectForKey: @"controllerStickControl"] boolValue];
    fullScreenControl.on = ([settings objectForKey: @"fullScreen"] == nil) ? YES : [[settings objectForKey: @"fullScreen"] boolValue];
    aspectRatioControl.on = ([settings objectForKey: @"aspectRatio"] == nil) ? YES : [[settings objectForKey: @"aspectRatio"] boolValue];
    
    controllerLayoutIndex =  [[settings objectForKey: @"controllerLayout"] intValue];
    controllerLayout.text = [controllerLayoutDescriptions objectAtIndex: controllerLayoutIndex];
    
    /* Game-Specific Settings */
    
    if (self.game) {
        gameGenieControl.on = [[settings objectForKey: @"gameGenie"] boolValue];
        for(int i = 0; i < 4; i++) {
            gameGenieCodeControl[i].text = [settings objectForKey: [NSString stringWithFormat: @"gameGenieCode%d", i]];
        }
    }
}

- (void)saveSettings {
    NSMutableDictionary *settings = [NSMutableDictionary dictionary];
    
    [settings setObject:@(swapABControl.on) forKey:@"swapAB"];
    [settings setObject:@(fullScreenControl.on) forKey:@"fullScreen"];
    [settings setObject:@(aspectRatioControl.on) forKey:@"aspectRatio"];
    [settings setObject:@(antiAliasControl.on) forKey:@"shouldRasterize"];
    [settings setObject:@(controllerStickControl.on) forKey:@"controllerStickControl"];
    [settings setObject:@(controllerLayoutIndex) forKey:@"controllerLayout"];
    
    if (self.game) {
        [settings setObject:@(gameGenieControl.on) forKey:@"gameGenie"];
        for (int i = 0; i < 4; i++) {
            [settings setObject:(gameGenieCodeControl[i].text ?: @"") forKey:[NSString stringWithFormat: @"gameGenieCode%d", i]];
        }
	}
    
    if (self.game) {
        self.game.settings = settings;
    } else {
        [EmulatorCore saveGlobalSettings:settings];
    }
}

- (id) init {
    self = [super initWithStyle: UITableViewStyleGrouped];
	
	if (self != nil) {
        UIImage *tabBarImage = [UIImage imageNamed: @"Settings.png"];
        self.tabBarItem = [[UITabBarItem alloc] initWithTitle: @"Default Settings" image: tabBarImage tag: 2];

        controllerLayoutDescriptions = [[NSArray alloc] initWithObjects: @"Game Pad + Zapper", @"Zapper Only", nil];
		raised = NO;
		swapABControl = [[UISwitch alloc] initWithFrame: CGRectZero];
		fullScreenControl = [[UISwitch alloc] initWithFrame: CGRectZero];
		aspectRatioControl = [[UISwitch alloc] initWithFrame: CGRectZero];
		gameGenieControl = [[UISwitch alloc] initWithFrame: CGRectZero];
        antiAliasControl = [[UISwitch alloc] initWithFrame: CGRectZero];
        controllerStickControl = [[UISwitch alloc] initWithFrame: CGRectZero];

        controllerLayout = [[UITextField alloc] initWithFrame: CGRectMake(-170, -12.0, 160.0, 30.0)];
        controllerLayout.textColor = [UIColor colorWithHue: .6027 saturation: .63 brightness: .52 alpha: 1.0];
        controllerLayout.enabled = NO;
		controllerLayout.textAlignment = NSTextAlignmentRight;
		controllerLayout.contentVerticalAlignment = UIControlContentVerticalAlignmentBottom;

        for(int i = 0; i < 4; i++) {
			gameGenieCodeControl[i] = [[UITextField alloc] initWithFrame: CGRectMake(100.0, 5.0, 200.0, 35.0)];
			gameGenieCodeControl[i].contentVerticalAlignment = UIControlContentVerticalAlignmentCenter;
			gameGenieCodeControl[i].delegate = self;
			gameGenieCodeControl[i].placeholder = @"Empty";
			gameGenieCodeControl[i].returnKeyType = UIReturnKeyDone;
		}
        
        [self loadSettings];
        [self saveSettings];
	}
	return self;
}

- (void)viewWillAppear:(BOOL)animated {
    [super viewWillAppear: animated];
    
    self.navigationController.navigationBar.hidden = NO;

    [self loadSettings];
}

- (void)viewWillDisappear:(BOOL)animated {
	
	NSLog(@"%s saving settings", __func__);
	
	[self saveSettings];
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
}


/* UITableViewDataSource methods */

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView {
    if (!self.game) {
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
	NSString *CellIdentifier = [NSString stringWithFormat: @"%d:%d", [indexPath indexAtPosition: 0],
								[indexPath indexAtPosition:1]];
	
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier: CellIdentifier];
    if (cell == nil) {
        cell = [[UITableViewCell alloc] initWithStyle: UITableViewCellStyleDefault reuseIdentifier: CellIdentifier];
		cell.selectionStyle = UITableViewCellSelectionStyleNone;
        cell.textLabel.textAlignment = NSTextAlignmentLeft;;

        DisclosureIndicator *accessory = [DisclosureIndicator accessoryWithColor: [UIColor colorWithHue: 0 saturation: 0 brightness: .5 alpha: 1.0]];
        accessory.highlightedColor = [UIColor blackColor];

		switch ([indexPath indexAtPosition: 0]) {
			case(0):
				switch([indexPath indexAtPosition: 1]) {
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
                        [cell.accessoryView addSubview: controllerLayout];
				}
				break;
			case(1):
				if ([indexPath indexAtPosition: 1] == 0) {
                    cell.accessoryView = gameGenieControl;
                    cell.textLabel.text = @"Game Genie";
                    break;
				} else {
					[cell addSubview: gameGenieCodeControl[[indexPath indexAtPosition: 1]-1]];
					if (!self.game) {
						gameGenieCodeControl[[indexPath indexAtPosition: 1]-1].text = nil;
						gameGenieCodeControl[[indexPath indexAtPosition: 1]-1].placeholder = @"None";
						gameGenieCodeControl[[indexPath indexAtPosition: 1]-1].enabled = NO;
					} else {
						gameGenieCodeControl[[indexPath indexAtPosition: 1]-1].placeholder = @"Empty";
						gameGenieCodeControl[[indexPath indexAtPosition: 1]-1].enabled = YES;
					}

					cell.textLabel.text = [NSString stringWithFormat: @"Code #%d", [indexPath indexAtPosition: 1]];
				}
                break;
            case(2):
            {
                if (! self.game.favorite) {
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
    UITableViewCell *cell = [self.tableView cellForRowAtIndexPath: indexPath];

    if ([indexPath indexAtPosition: 0] == 0) {
    
		MultiValueViewController *viewController = [[MultiValueViewController alloc] initWithStyle: UITableViewStyleGrouped];
        viewController.options = [NSArray arrayWithArray: controllerLayoutDescriptions];
        viewController.selectedItemIndex = controllerLayoutIndex;
        viewController.delegate = self;
		[self.navigationController pushViewController: viewController animated: YES];
    }
    
    if ([indexPath indexAtPosition: 0] == 2) {
        self.game.favorite = !self.game.favorite;
        
        if (! self.game.favorite) {
            cell.textLabel.text = @"Remove from Favorites";
        } else {
            cell.textLabel.text = @"Add to Favorites";
        }
    }
    
    [cell setSelected: NO animated: NO];
}

- (NSString *)tableView:(UITableView *)tv titleForFooterInSection:(NSInteger)section
{
    if (section == 0 && !self.game) {
        return @"To access Game Genie settings, enter settings from within the active game play menu.";
    }
    return nil;
}

/* UITextFieldDelegate Methods */

- (BOOL) textFieldShouldReturn:(UITextField *)textField {
	
	if (raised == YES) {
		[UIView beginAnimations: nil context: NULL]; 
		[UIView setAnimationDuration: 0.3]; 
		CGRect frame = self.view.frame; 
		frame.origin.y += 200.0; 
		self.view.frame = frame; 
		[UIView commitAnimations]; 
		raised = NO;
	}
	
	self.tableView.scrollEnabled = YES;
    [textField resignFirstResponder];
    return YES;
}

- (void)textFieldDidBeginEditing:(UITextField *)textField {
	
	if (raised == NO) {
		if (   textField == gameGenieCodeControl[0] 
			|| textField == gameGenieCodeControl[1]
			|| textField == gameGenieCodeControl[2] 
			|| textField == gameGenieCodeControl[3])
		{
			[UIView beginAnimations: nil context: NULL]; 
			[UIView setAnimationDuration: 0.3]; 
			CGRect frame = self.view.frame; 
			frame.origin.y -= 200.0; 
			self.view.frame = frame; 
			[UIView commitAnimations];
			raised = YES;
		}
	}
}

/* MultiValueViewControllerDelegate methods */

- (void) didSelectItemFromList: (MultiValueViewController *)multiValueViewController selectedItemIndex:(int)selectedItemIndex identifier:(id)identifier
{
    controllerLayoutIndex = selectedItemIndex;
    controllerLayout.text = [controllerLayoutDescriptions objectAtIndex: controllerLayoutIndex];
    [self saveSettings];
    // [self.tableView reloadData];
}
@end
