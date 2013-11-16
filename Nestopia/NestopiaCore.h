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

#ifndef __NESTOPIACORE_H__
#define __NESTOPIACORE_H__

@interface NestopiaCore : NSObject
{    
    NSString *gamePath;
    NSTimer *gameTimer;
    CGSize resolution;
    int controllerLayout;
}

- (BOOL)initializeCore;
- (void)initializeInput;
- (void)startEmulation;
- (void)stopEmulation;
- (void)finishEmulation;
- (void)loadState;
- (void)saveState;
- (void)applyCheatCodes:(NSArray *)codes;
- (void)activatePad1;
- (void)activatePad2;
- (void)toggleCoin1;
- (void)toggleCoin2;
- (void)coinOff;

@property(nonatomic,copy) NSString *gamePath;
@property(nonatomic,weak) id delegate;
@property(nonatomic,assign) CGSize resolution;
@property(nonatomic,assign) int controllerLayout;
@end

#endif