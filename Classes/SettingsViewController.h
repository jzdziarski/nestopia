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

#import <UIKit/UIKit.h>
#import "GameROMViewController.h"

@interface SettingsViewController : UITableViewController <UITextFieldDelegate> {

	NSUserDefaults *settings;
	UIBarButtonItem *leftButton;
	NSString *currentROMImagePath, *currentGameName;

	UISwitch *autoSaveControl;
	UISwitch *swapABControl;
	UISwitch *fullScreenControl;
	UISwitch *aspectRatioControl;
	UISwitch *gameGenieControl;
	
	UISegmentedControl *frameSkipControl;
	UISegmentedControl *paletteControl;
	UISegmentedControl *cpuControl;
	UISegmentedControl *soundBufferControl;
	UISwitch *bassBoostControl;
	UISwitch *controllerStickControl;
	UITextField *versionString;
	
	UITextField *gameGenieCodeControl[4];
	bool raised;
}

- (void)saveSettings;

@property(nonatomic,copy) NSString *currentROMImagePath;
@end
