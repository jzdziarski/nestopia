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
#include "NESCore.h"
#include "NESCore_Callback.h"

@protocol GameControllerDelegate;

@interface ControllerView : UIView {
	
	UIDeviceOrientation orientation;
	NSDictionary *settings;
	id delegate;
    id gamePlayDelegate;
	bool swapAB;
	
    UIImage *controllerImage;
    UIImageView *indicatorUp;
	UIImageView *indicatorDown;
    UIImageView *indicatorLeft;
    UIImageView *indicatorRight;
    UIImageView *indicatorA;
    UIImageView *indicatorB;
	
    CGRect Up;
    CGRect Left;
    CGRect Down;
    CGRect Right;
    CGRect Select;
    CGRect Start;
    CGRect UL;
    CGRect UR;
    CGRect DL;
    CGRect DR;
    CGRect B;
    CGRect A;
    CGRect AB;
    CGRect notifyUpdateRect;
    CGRect Exit;
	
	dword controllerState[2];
	dword padDir;
	dword padButton;
	dword padSpecial;
    
	BOOL notified;
    int currentController;	
}
- (void)updateNotifyIcons;
- (UIImage *)getControllerImage;

@property(nonatomic,assign) UIDeviceOrientation orientation;
@property(nonatomic,assign) id<GameControllerDelegate> delegate;
@property(nonatomic,assign) int currentController;
@property(nonatomic,assign) id gamePlayDelegate;
@property(nonatomic,assign) BOOL notified;
@end

@protocol GameControllerDelegate

@optional
- (void)gameControllerZapperDidChange:(byte)status locationInWindow:(CGPoint)locationInWindow;

@required
- (void)gameControllerControllerDidChange:(int)controller controllerState:(dword)controllerState;

@end
