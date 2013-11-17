//
//  PadDirectionButton.m
//  Nestopia
//
//  Created by Nick Tymchenko on 11/17/13.
//
//

#import "PadDirectionButton.h"

@implementation PadDirectionButton

- (NestopiaPadInput)inputForLocation:(CGPoint)location {
    if (!CGRectContainsPoint(self.bounds, location)) {
        return 0;
    }
    
    NestopiaPadInput input = 0;
    CGSize size = self.bounds.size;
    CGFloat k = 0.4;
    
    if (location.y <= size.height * k) {
        input |= NestopiaPadInputUp;
    }
    if (location.y >= size.height * (1-k)) {
        input |= NestopiaPadInputDown;
    }
    if (location.x <= size.width * k) {
        input |= NestopiaPadInputLeft;
    }
    if (location.x >= size.width * (1-k)) {
        input |= NestopiaPadInputRight;
    }
    
    return input;
}

@end
