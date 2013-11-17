//
//  PadButton.m
//  Nestopia
//
//  Created by Nick Tymchenko on 11/17/13.
//
//

#import "PadButton.h"

@interface PadButton ()

@property (nonatomic, strong) NSMutableDictionary *touchInputDictionary;

@end


@implementation PadButton

#pragma mark Properties

@dynamic input;

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
    _touchInputDictionary = [NSMutableDictionary dictionary];
    
    self.opaque = NO;
}

#pragma mark UIView

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {
    for (UITouch *touch in touches) {
        CGPoint location = [touch locationInView:self];
        [self setInput:[self inputForLocation:location] forTouch:touch];
    }
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event {
    for (UITouch *touch in touches) {
        CGPoint location = [touch locationInView:self];
        [self setInput:[self inputForLocation:location] forTouch:touch];
    }
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event {
    for (UITouch *touch in touches) {
        [self setInput:0 forTouch:touch];
    }
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event {
    for (UITouch *touch in touches) {
        [self setInput:0 forTouch:touch];
    }
}

#pragma mark Abstract

- (NestopiaPadInput)inputForLocation:(CGPoint)location {
    return 0;
}

#pragma mark Private

- (NestopiaPadInput)inputForTouch:(UITouch *)touch {
    return [[self.touchInputDictionary objectForKey:[NSValue valueWithNonretainedObject:touch]] intValue];
}

- (void)setInput:(NestopiaPadInput)input forTouch:(UITouch *)touch {
    if (input) {
        [self.touchInputDictionary setObject:@(input) forKey:[NSValue valueWithNonretainedObject:touch]];
    } else {
        [self.touchInputDictionary removeObjectForKey:[NSValue valueWithNonretainedObject:touch]];
    }
}

- (NestopiaPadInput)input {
    NestopiaPadInput input = 0;
    for (NSNumber *wrapper in [self.touchInputDictionary allValues]) {
        input |= [wrapper intValue];
    }
    return input;
}

@end
