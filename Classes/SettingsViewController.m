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

		raised = NO;
		swapABControl = [ [ UISwitch alloc ] initWithFrame: CGRectZero ];
		fullScreenControl = [ [ UISwitch alloc ] initWithFrame: CGRectZero ];
		aspectRatioControl = [ [ UISwitch alloc ] initWithFrame: CGRectZero ];
		gameGenieControl = [ [ UISwitch alloc ] initWithFrame: CGRectZero ];
        antiAliasControl = [ [ UISwitch alloc ] initWithFrame: CGRectZero ];
        controllerStickControl = [ [ UISwitch alloc ] initWithFrame: CGRectZero ];
        
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
    [ super dealloc ];
}

/* UITableViewDataSource methods */

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView {
    
    if (currentGameName == nil) {
        return 1;
    } else {
        return 2;
    }
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    switch (section) {
        case(0):
			return 5;
			break;
		case(1):
			return 5;
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
		}
	}
	
    return cell;
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


@end
