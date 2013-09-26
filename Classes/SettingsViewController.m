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

- (id) init {
    self = [ super initWithStyle: UITableViewStyleGrouped ];
	
	if (self != nil) {
		self.title = @"Settings";
        UIImage *tabBarImage = [ UIImage imageNamed: @"Settings.png" ];
        self.tabBarItem = [ [ UITabBarItem alloc ] initWithTitle: @"Settings" image: tabBarImage tag: 2 ];

		raised = NO;
		
		NSLog(@"%s loading default settings", __func__);
		settings = [ NSUserDefaults standardUserDefaults ];
		
		autoSaveControl = [ [ UISwitch alloc ] initWithFrame: CGRectMake(200.0, 10.0, 0.0, 0.0) ];
		autoSaveControl.on = [ settings boolForKey: @"autoSave" ];
		
		swapABControl = [ [ UISwitch alloc ] initWithFrame: CGRectMake(200.0, 10.0, 0.0, 0.0) ];
		swapABControl.on = [ settings boolForKey: @"swapAB" ];
		
		fullScreenControl = [ [ UISwitch alloc ] initWithFrame: CGRectMake(200.0, 10.0, 0.0, 0.0) ];
		fullScreenControl.on = [ settings boolForKey: @"fullScreen" ];
		
		aspectRatioControl = [ [ UISwitch alloc ] initWithFrame: CGRectMake(200.0, 10.0, 0.0, 0.0) ];
		if ([ settings objectForKey: @"aspectRatio" ] == nil) {
			aspectRatioControl.on = YES;
		} else {
			aspectRatioControl.on = [ settings boolForKey: @"aspectRatio" ];
		}
		
		gameGenieControl = [ [ UISwitch alloc ] initWithFrame: CGRectMake(200.0, 10.0, 0.0, 0.0) ];
		gameGenieControl.on = [ settings boolForKey: @"gameGenie" ];
		
		frameSkipControl = [ [ UISegmentedControl alloc ] initWithFrame: CGRectMake(150.0, 5.0, 150.0, 35.0) ];
		[ frameSkipControl insertSegmentWithTitle: @"0" atIndex: 0 animated: NO ];
		[ frameSkipControl insertSegmentWithTitle: @"1" atIndex: 1 animated: NO ];
		[ frameSkipControl insertSegmentWithTitle: @"2" atIndex: 2 animated: NO ];
		[ frameSkipControl insertSegmentWithTitle: @"3" atIndex: 3 animated: NO ];
		[ frameSkipControl insertSegmentWithTitle: @"4" atIndex: 4 animated: NO ];
		if ([ settings objectForKey: @"frameSkip"] == nil) {
			frameSkipControl.selectedSegmentIndex = 0;
		} else {
			frameSkipControl.selectedSegmentIndex = [ settings integerForKey: @"frameSkip" ];
		}
		
		paletteControl = [ [ UISegmentedControl alloc ] initWithFrame: CGRectMake(150.0, 5.0, 150.0, 35.0) ];
		[ paletteControl insertSegmentWithTitle: @"A" atIndex: 0 animated: NO ];
		[ paletteControl insertSegmentWithTitle: @"B" atIndex: 1 animated: NO ];
		[ paletteControl insertSegmentWithTitle: @"C" atIndex: 2 animated: NO ];
		[ paletteControl insertSegmentWithTitle: @"D" atIndex: 3 animated: NO ];
		if ([ settings objectForKey: @"paletteControl"] == nil) {
			paletteControl.selectedSegmentIndex = 2;
		} else {
			paletteControl.selectedSegmentIndex = [ settings integerForKey: @"paletteControl" ];
		}
		
		cpuControl = [ [ UISegmentedControl alloc ] initWithFrame: CGRectMake(150.0, 5.0, 150.0, 35.0) ];
		[ cpuControl insertSegmentWithTitle: @"339" atIndex: 0 animated: NO ];
		[ cpuControl insertSegmentWithTitle: @"341" atIndex: 1 animated: NO ];
		cpuControl.selectedSegmentIndex = [ settings integerForKey: @"cpuCycle" ];
		
		soundBufferControl = [ [ UISegmentedControl alloc ] initWithFrame: CGRectMake(150.0, 5.0, 150.0, 35.0) ];
		for(int i = 0; i < 7; i++) {
			[ soundBufferControl insertSegmentWithTitle: [ NSString stringWithFormat: @"%d", i + 3 ] atIndex: i animated: NO ];
		}
		if ([ settings objectForKey: @"soundBuffer" ] == nil) {
			soundBufferControl.selectedSegmentIndex = 0;
		} else {
			soundBufferControl.selectedSegmentIndex = [ settings integerForKey: @"soundBuffer" ];
		}
		
		bassBoostControl = [ [ UISwitch alloc ] initWithFrame: CGRectMake(200.0, 10.0, 0.0, 0.0) ];
		bassBoostControl.on = [ settings integerForKey: @"bassBoost" ];
		
		controllerStickControl = [ [ UISwitch alloc ] initWithFrame: CGRectMake(200.0, 10.0, 0.0, 0.0) ];
		controllerStickControl.on = [ settings integerForKey: @"controllerStickControl" ]; 
		
		versionString = [ [ UITextField alloc ] initWithFrame: CGRectMake(150.0, 5.0, 150.0, 35.0) ];
		versionString.contentVerticalAlignment = UIControlContentVerticalAlignmentCenter;
		versionString.enabled = NO;
		
		NSDictionary *dict = [ NSDictionary dictionaryWithContentsOfFile: [ [ NSBundle mainBundle ] pathForResource: @"Info" ofType: @"plist" ] ];
		versionString.text = [ dict objectForKey: @"CFBundleVersion" ];
		
		for(int i = 0; i < 4; i++) {
			gameGenieCodeControl[i] = [ [ UITextField alloc ] initWithFrame: CGRectMake(100.0, 5.0, 200.0, 35.0) ];
			gameGenieCodeControl[i].contentVerticalAlignment = UIControlContentVerticalAlignmentCenter;
			gameGenieCodeControl[i].delegate = self;
			gameGenieCodeControl[i].text = nil;
			gameGenieCodeControl[i].placeholder = @"Empty";
			gameGenieCodeControl[i].returnKeyType = UIReturnKeyDone;
		}
		[ self saveSettings ];
	}
	return self;
}

- (void)viewWillAppear:(BOOL)animated {
    [ super viewWillAppear: animated ];
    
    [ self setGamePath ];
}

- (void)viewWillDisappear:(BOOL)animated {
	
	NSLog(@"%s saving settings", __func__);
	
	[ self saveSettings ];
}

- (void)loadView {
	[ super loadView ];
}

- (void)setGamePath {
    gamePath = [ currentGamePath copy ];
	currentGameName = [ [ [ [ gamePath lastPathComponent ] stringByReplacingOccurrencesOfString: @".sav" withString: @"" ] stringByReplacingOccurrencesOfString: @".nes" withString: @"" ] copy ];
	
	NSString *path = [ NSString stringWithFormat: @"%@/%@.plist", ROM_PATH, currentGameName ];

	NSLog(@"%s loading Game Genie codes from %@\n", __func__, path);
	NSMutableDictionary *gameGenieCodes = [ NSMutableDictionary dictionaryWithContentsOfFile: path ];
	for(int i = 0; i < 4; i++) {
		gameGenieCodeControl[i].text = [ gameGenieCodes objectForKey: [ NSString stringWithFormat: @"gameGenieCode%d", i ] ];
	}
	
	[ self loadView ];
}

- (void)saveSettings {
    [ settings setBool: autoSaveControl.on forKey: @"autoSave" ];
	[ settings setBool: swapABControl.on forKey: @"swapAB" ];
	[ settings setBool: fullScreenControl.on forKey: @"fullScreen" ];
	[ settings setBool: aspectRatioControl.on forKey: @"aspectRatio" ];
	[ settings setBool: gameGenieControl.on forKey: @"gameGenie" ];
	[ settings setInteger: frameSkipControl.selectedSegmentIndex forKey: @"frameSkip" ];
	[ settings setInteger: paletteControl.selectedSegmentIndex forKey: @"paletteControl" ];
	[ settings setInteger: cpuControl.selectedSegmentIndex forKey: @"cpuCycle" ];
    [ settings setInteger: soundBufferControl.selectedSegmentIndex forKey: @"soundBuffer" ];
	[ settings setBool: bassBoostControl.on forKey: @"bassBoost" ];
	[ settings setBool: controllerStickControl.on forKey: @"controllerStickControl" ];
	
	[ settings synchronize ];
	
	if (currentGameName != nil) {
		NSString *path = [ NSString stringWithFormat: @"%@/%@.plist", ROM_PATH, currentGameName ];
		NSMutableDictionary *gameGenieCodes = [ [ NSMutableDictionary alloc ] init ];
		
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
				[ gameGenieCodes setObject: gameGenieCodeControl[i].text forKey: [ NSString stringWithFormat: @"gameGenieCode%d", i ] ];
			}
			NSLog(@"%s saving Game Genie codes to %@\n", __func__, path);
			[ gameGenieCodes writeToFile: path atomically: YES ];
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
    return 3;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    switch (section) {
        case(0):
			return 5;
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
		
		switch ([ indexPath indexAtPosition: 0]) {
			case(0):
				switch([ indexPath indexAtPosition: 1]) {
					case(0):
                        cell.accessoryView = autoSaveControl;
						cell.textLabel.text = @"Auto-Save Game";
						break;
					case(1):
						cell.accessoryView = swapABControl;
						cell.textLabel.text = @"Swap A/B";
						break;
					case(2):
						cell.accessoryView = fullScreenControl;
						cell.textLabel.text = @"Full Screen";
						break;
					case(3):
						cell.accessoryView = aspectRatioControl;
						cell.textLabel.text = @"Aspect Ratio";
						break;
					case(4):
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

- (BOOL)textFieldShouldBeginEditing:(UITextField *)textField {
	self.tableView.scrollEnabled = NO;
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
