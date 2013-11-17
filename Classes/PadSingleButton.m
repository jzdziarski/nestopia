//
//  PadSingleButton.m
//  Nestopia
//
//  Created by Nick Tymchenko on 11/17/13.
//
//

#import "PadSingleButton.h"

@implementation PadSingleButton

- (NestopiaPadInput)inputForLocation:(CGPoint)location {
    if (CGRectContainsPoint(self.bounds, location)) {
        return self.singleInput;
    } else {
        return 0;
    }
}

@end
