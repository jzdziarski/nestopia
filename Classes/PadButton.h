//
//  PadButton.h
//  Nestopia
//
//  Created by Nick Tymchenko on 11/17/13.
//
//

#import <UIKit/UIKit.h>
#import "NestopiaCore.h"

@interface PadButton : UIView

@property (nonatomic, readonly) NestopiaPadInput input;

- (void)commonInit;

// Method to implement:
- (NestopiaPadInput)inputForLocation:(CGPoint)location;

@end
