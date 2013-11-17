//
//  PadButton.m
//  Nestopia
//
//  Created by Nick Tymchenko on 11/17/13.
//
//

#import "PadButton.h"

@implementation PadButton

#pragma mark Init

- (id)initWithFrame:(CGRect)frame {
    if ((self = [super initWithFrame:frame])) {
        [self commonInit];
    }
    return self;
}

- (id)initWithCoder:(NSCoder *)aDecoder {
    if ((self = [super initWithCoder:aDecoder])) {
        [self commonInit];
    }
    return self;
}

- (void)commonInit {
    // TODO: remove
    self.backgroundColor = [UIColor whiteColor];
}

#pragma mark UIView

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {
    for (UITouch *touch in touches) {
        CGPoint location = [touch locationInView:self];
        _input |= [self inputForLocation:location];
    }
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event {
    for (UITouch *touch in touches) {
        CGPoint location = [touch previousLocationInView:self];
        _input &= ~[self inputForLocation:location];
        
        location = [touch locationInView:self];
        _input |= [self inputForLocation:location];
    }
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event {
    for (UITouch *touch in touches) {
        CGPoint location = [touch locationInView:self];
        _input &= ~[self inputForLocation:location];
    }
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event {
    for (UITouch *touch in touches) {
        CGPoint location = [touch locationInView:self];
        _input &= ~[self inputForLocation:location];
    }
}

#pragma mark Private

- (NestopiaPadInput)inputForLocation:(CGPoint)location {
    return 0;
}

@end
