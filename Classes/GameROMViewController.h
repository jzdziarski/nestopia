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

#import <UIKit/UIKit.h>
#import <Foundation/Foundation.h>
#import <CoreFoundation/CoreFoundation.h>

#define ROM_PATH [ NSString stringWithFormat: @"%@/Documents", NSHomeDirectory() ]

@interface GameROMViewController : UITableViewController <UITableViewDelegate> {
	int nActiveSections;
    int romCount;
    bool favorites;
    
	NSMutableArray *fileList[27];
	NSMutableArray *activeSections;
	NSMutableArray *sectionTitles;
}

@property(nonatomic,assign) bool favorites;

@end

