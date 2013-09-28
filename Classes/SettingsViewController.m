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

#import "SettingsViewController.h"

extern NSString *currentGamePath;

@implementation SettingsViewController
@synthesize gamePath;

- (void) loadSettings {
    if (currentGameName == nil) {
        [ [ NSUserDefaults standardUserDefaults ] synchronize ];
        settings = [ [ NSUserDefaults standardUserDefaults ] dictionaryRepresentation ];
        self.title = @"Default Settings";
    } else {
        NSString *path = [ NSString stringWithFormat: @"%@/%@.plist", ROM_PATH, currentGameName ];
        settings = [ NSDictionary dictionaryWithContentsOfFile: path ];
        if (!settings) {
            settings = [ [ NSUserDefaults standardUserDefaults ] dictionaryRepresentation ];
        }
        self.title = [ NSString stringWithFormat: @"Settings for %@", currentGameName ];
    }
    
    swapABControl.on = [ [ settings objectForKey: @"swapAB" ] boolValue ];
    fullScreenControl.on = [ [ settings objectForKey: @"fullScreen" ] boolValue ];
    gameGenieControl.on = [ [ settings objectForKey: @"gameGenie" ] boolValue ];
    controllerStickControl.on = [ [ settings objectForKey: @"controllerStickControl" ] boolValue ];
    bassBoostControl.on = [ [ settings objectForKey: @"bassBoost" ] boolValue ];
    cpuControl.selectedSegmentIndex = [ [ settings objectForKey: @"cpuCycle" ] intValue ];

    if ([ settings objectForKey: @"fullScreen" ] == nil) {
        fullScreenControl.on = YES;
    } else {
        fullScreenControl.on = [ [ settings objectForKey: @"aspectRatio" ] boolValue ];
    }
    
    if ([ settings objectForKey: @"aspectRatio" ] == nil) {
        aspectRatioControl.on = YES;
    } else {
        aspectRatioControl.on = [ [ settings objectForKey: @"aspectRatio" ] boolValue ];
    }
    
    if ([ settings objectForKey: @"frameSkip"] == nil) {
        frameSkipControl.selectedSegmentIndex = 0;
    } else {
        frameSkipControl.selectedSegmentIndex = [ [ settings objectForKey: @"frameSkip" ] intValue ];
    }

    if ([ settings objectForKey: @"paletteControl"] == nil) {
        paletteControl.selectedSegmentIndex = 2;
    } else {
        paletteControl.selectedSegmentIndex = [ [ settings objectForKey: @"paletteControl" ] intValue ];
    }

    if ([ settings objectForKey: @"soundBuffer" ] == nil) {
        soundBufferControl.selectedSegmentIndex = 0;
    } else {
        soundBufferControl.selectedSegmentIndex = [ [ settings objectForKey: @"soundBuffer" ] intValue ];
    }
    
    for(int i = 0; i < 4; i++) {
        gameGenieCodeControl[i].text = [ settings objectForKey: [ NSString stringWithFormat: @"gameGenieCode%d", i ] ];
    }
}

- (id) init {
    self = [ super initWithStyle: UITableViewStyleGrouped ];
	
	if (self != nil) {
        UIImage *tabBarImage = [ UIImage imageNamed: @"Settings.png" ];
        self.tabBarItem = [ [ UITabBarItem alloc ] initWithTitle: @"Default Settings" image: tabBarImage tag: 2 ];

		raised = NO;
				
		swapABControl = [ [ UISwitch alloc ] initWithFrame: CGRectMake(200.0, 10.0, 0.0, 0.0) ];
		fullScreenControl = [ [ UISwitch alloc ] initWithFrame: CGRectMake(200.0, 10.0, 0.0, 0.0) ];
		aspectRatioControl = [ [ UISwitch alloc ] initWithFrame: CGRectMake(200.0, 10.0, 0.0, 0.0) ];
		gameGenieControl = [ [ UISwitch alloc ] initWithFrame: CGRectMake(200.0, 10.0, 0.0, 0.0) ];
		frameSkipControl = [ [ UISegmentedControl alloc ] initWithFrame: CGRectMake(150.0, 5.0, 150.0, 35.0) ];
		[ frameSkipControl insertSegmentWithTitle: @"0" atIndex: 0 animated: NO ];
		[ frameSkipControl insertSegmentWithTitle: @"1" atIndex: 1 animated: NO ];
		[ frameSkipControl insertSegmentWithTitle: @"2" atIndex: 2 animated: NO ];
		[ frameSkipControl insertSegmentWithTitle: @"3" atIndex: 3 animated: NO ];
		[ frameSkipControl insertSegmentWithTitle: @"4" atIndex: 4 animated: NO ];
		
		paletteControl = [ [ UISegmentedControl alloc ] initWithFrame: CGRectMake(150.0, 5.0, 150.0, 35.0) ];
		[ paletteControl insertSegmentWithTitle: @"A" atIndex: 0 animated: NO ];
		[ paletteControl insertSegmentWithTitle: @"B" atIndex: 1 animated: NO ];
		[ paletteControl insertSegmentWithTitle: @"C" atIndex: 2 animated: NO ];
		[ paletteControl insertSegmentWithTitle: @"D" atIndex: 3 animated: NO ];
		
		cpuControl = [ [ UISegmentedControl alloc ] initWithFrame: CGRectMake(150.0, 5.0, 150.0, 35.0) ];
		[ cpuControl insertSegmentWithTitle: @"339" atIndex: 0 animated: NO ];
		[ cpuControl insertSegmentWithTitle: @"341" atIndex: 1 animated: NO ];
		
		soundBufferControl = [ [ UISegmentedControl alloc ] initWithFrame: CGRectMake(150.0, 5.0, 150.0, 35.0) ];
		for(int i = 0; i < 7; i++) {
			[ soundBufferControl insertSegmentWithTitle: [ NSString stringWithFormat: @"%d", i + 3 ] atIndex: i animated: NO ];
		}
		
		bassBoostControl = [ [ UISwitch alloc ] initWithFrame: CGRectMake(200.0, 10.0, 0.0, 0.0) ];
		controllerStickControl = [ [ UISwitch alloc ] initWithFrame: CGRectMake(200.0, 10.0, 0.0, 0.0) ];
		
		versionString = [ [ UITextField alloc ] initWithFrame: CGRectMake(150.0, 5.0, 150.0, 35.0) ];
		versionString.contentVerticalAlignment = UIControlContentVerticalAlignmentCenter;
		versionString.enabled = NO;
		
		NSDictionary *dict = [ NSDictionary dictionaryWithContentsOfFile: [ [ NSBundle mainBundle ] pathForResource: @"Info" ofType: @"plist" ] ];
		versionString.text = [ dict objectForKey: @"CFBundleVersion" ];
		
        for(int i = 0; i < 4; i++) {
			gameGenieCodeControl[i] = [ [ UITextField alloc ] initWithFrame: CGRectMake(100.0, 5.0, 200.0, 35.0) ];
			gameGenieCodeControl[i].contentVerticalAlignment = UIControlContentVerticalAlignmentCenter;
			gameGenieCodeControl[i].delegate = self;
			gameGenieCodeControl[i].placeholder = @"Empty";
			gameGenieCodeControl[i].returnKeyType = UIReturnKeyDone;
		}
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

- (void)saveSettings {
    
    if (currentGameName == nil) {
        [ [ NSUserDefaults standardUserDefaults ] setBool: swapABControl.on forKey: @"swapAB" ];
        [ [ NSUserDefaults standardUserDefaults ] setBool: fullScreenControl.on forKey: @"fullScreen" ];
        [ [ NSUserDefaults standardUserDefaults ] setBool: aspectRatioControl.on forKey: @"aspectRatio" ];
        [ [ NSUserDefaults standardUserDefaults ] setBool: gameGenieControl.on forKey: @"gameGenie" ];
        [ [ NSUserDefaults standardUserDefaults ] setInteger: frameSkipControl.selectedSegmentIndex forKey: @"frameSkip" ];
        [ [ NSUserDefaults standardUserDefaults ] setInteger: paletteControl.selectedSegmentIndex forKey: @"paletteControl" ];
        [ [ NSUserDefaults standardUserDefaults ] setInteger: cpuControl.selectedSegmentIndex forKey: @"cpuCycle" ];
        [ [ NSUserDefaults standardUserDefaults ] setInteger: soundBufferControl.selectedSegmentIndex forKey: @"soundBuffer" ];
        [ [ NSUserDefaults standardUserDefaults ] setBool: bassBoostControl.on forKey: @"bassBoost" ];
        [ [ NSUserDefaults standardUserDefaults ] setBool: controllerStickControl.on forKey: @"controllerStickControl" ];
        
        [ [ NSUserDefaults standardUserDefaults ] synchronize ];
    } else {
		NSString *path = [ NSString stringWithFormat: @"%@/%@.plist", ROM_PATH, currentGameName ];
		NSMutableDictionary *gameSettings = [ [ NSMutableDictionary alloc ] init ];
        [ gameSettings setObject: [ NSNumber numberWithBool: swapABControl.on ] forKey: @"swapAB" ];
        [ gameSettings setObject: [ NSNumber numberWithBool:fullScreenControl.on ] forKey: @"fullScreen" ];
        [ gameSettings setObject: [ NSNumber numberWithBool: aspectRatioControl.on ]forKey: @"aspectRatio" ];
        [ gameSettings setObject: [ NSNumber numberWithBool: gameGenieControl.on ] forKey: @"gameGenie" ];
        [ gameSettings setObject: [ NSNumber numberWithBool: bassBoostControl.on ] forKey: @"bassBoost" ];
        [ gameSettings setObject: [ NSNumber numberWithBool: controllerStickControl.on ] forKey: @"controllerStickControl" ];

        [ gameSettings setObject: [ NSNumber numberWithInt: frameSkipControl.selectedSegmentIndex ] forKey: @"frameSkip" ];
        [ gameSettings setObject: [ NSNumber numberWithInt: paletteControl.selectedSegmentIndex ] forKey: @"paletteControl" ];
        [ gameSettings setObject: [ NSNumber numberWithInt: cpuControl.selectedSegmentIndex ] forKey: @"cpuCycle" ];
        [ gameSettings setObject: [ NSNumber numberWithInt: soundBufferControl.selectedSegmentIndex  ] forKey: @"soundBuffer" ];
        
        
		/* Clean up the property list if no codes are specified */
		BOOL deleteFile = YES;
		for(int i = 0; i < 4; i++) {
			if ([ gameGenieCodeControl[i].text isEqualToString: @"" ] == NO) {
				deleteFile = NO;
			}
		}
		if (deleteFile == YES) {
			NSError *error;
			NSLog(@"%s deleting empty game genie file %@\n", __func__, path);
			[ [ NSFileManager defaultManager ] removeItemAtPath: path error: &error ];

		/* Codes exist; write a property list */ 
		} else {
			for(int i = 0; i < 4; i++) {
				if (gameGenieCodeControl[i].text == nil) {
					gameGenieCodeControl[i].text = @"";
				}
				[ gameSettings setObject: gameGenieCodeControl[i].text forKey: [ NSString stringWithFormat: @"gameGenieCode%d", i ] ];
			}
			NSLog(@"%s saving Game Genie codes to %@\n", __func__, path);
            
			[ gameSettings writeToFile: path atomically: YES ];
		}
	}
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
	if (interfaceOrientation == UIDeviceOrientationLandscapeLeft || interfaceOrientation == UIDeviceOrientationPortrait)
		return YES;
	return NO;
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
        return 2;
    } else {
        return 3;
    }
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    switch (section) {
        case(0):
			return 4;
			break;
		case(1):
			return 7;
			break;
		case(2):
			return 5;
			break;
	}
	
	return 0;
}

- (NSString *)tableView:(UITableView *)tableView titleForHeaderInSection:(NSInteger)section { 
	switch (section) {
		case(0):
			return @"General Options";
			break;
		case(1):
			return @"Advanced Options";
			break;
		case(2):
			return @"Game Genie Codes";
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
						cell.accessoryView = swapABControl;
						cell.textLabel.text = @"Swap A/B";
						break;
					case(1):
						cell.accessoryView = fullScreenControl;
						cell.textLabel.text = @"Full Screen";
						break;
					case(2):
						cell.accessoryView = aspectRatioControl;
						cell.textLabel.text = @"Aspect Ratio";
						break;
					case(3):
						cell.accessoryView = gameGenieControl;
						cell.textLabel.text = @"Game Genie";
						break;
				}
				break;
			case(1):
				switch ([ indexPath indexAtPosition: 1 ]) {
					case(0):
						cell.accessoryView = frameSkipControl;
						cell.textLabel.text = @"Frame Skip";
						break;
					case(1):
						cell.accessoryView = paletteControl;
						cell.textLabel.text = @"Palette";
						break;
					case(2):
						cell.accessoryView = cpuControl;
						cell.textLabel.text = @"CPU Cycle";
						break;
					case(3):
						cell.accessoryView = soundBufferControl;
						cell.textLabel.text = @"Sound Buffers";
						break;
					case(4):
						cell.accessoryView = bassBoostControl;
						cell.textLabel.text = @"Bass Boost";
						break;
					case(5):
						cell.accessoryView = controllerStickControl;
						cell.textLabel.text = @"Sticky Controller";
						break;
					case(6):
						cell.accessoryView = versionString;
						cell.textLabel.text = @"Version";
						break;
				}
				break;
			case(2):
				if ([ indexPath indexAtPosition: 1 ] == 0) {
					if (currentGameName == nil) {
						cell.textLabel.text = @"Choose a Game";
					} else {
						cell.textLabel.text = currentGameName;
					}
					cell.textLabel.font = [ UIFont fontWithName: @"Arial" size: 12.0 ];
					cell.textLabel.textColor = [ UIColor grayColor ];
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
 
/*
- (BOOL)textFieldShouldBeginEditing:(UITextField *)textField {
	self.tableView.scrollEnabled = NO;
	return YES;
}
*/

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
