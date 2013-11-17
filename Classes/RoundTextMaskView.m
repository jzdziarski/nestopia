//
//  RoundTextMaskView.m
//  Nestopia
//
//  Created by Nick Tymchenko on 11/18/13.
//
//

#import "RoundTextMaskView.h"

@implementation RoundTextMaskView

#pragma mark Properties

- (void)setText:(NSString *)text {
    _text = [text copy];
    [self setNeedsDisplay];
}

- (void)setFont:(UIFont *)font {
    _font = [font copy];
    [self setNeedsDisplay];
}

- (void)setColor:(UIColor *)color {
    _color = [color copy];
    [self setNeedsDisplay];
}

#pragma mark Init

- (void)commonInit {
    [super commonInit];
    
    __typeof__(self) __weak weakSelf = self;
    
    self.maskDrawingBlock = ^(CGRect rect) {
        CGContextRef ctx = UIGraphicsGetCurrentContext();
        CGContextSetFillColorWithColor(ctx, [UIColor whiteColor].CGColor);
        
        [weakSelf.text drawInRect:CGRectMake(rect.origin.x,
                                             (rect.size.height - weakSelf.font.lineHeight) / 2,
                                             rect.size.width,
                                             weakSelf.font.lineHeight)
                         withFont:weakSelf.font
                    lineBreakMode:NSLineBreakByClipping
                        alignment:NSTextAlignmentCenter];
    };
    
    self.backgroundDrawingBlock = ^(CGRect rect) {
        CGContextRef ctx = UIGraphicsGetCurrentContext();
        CGContextSetFillColorWithColor(ctx, weakSelf.color.CGColor);
        CGContextFillEllipseInRect(ctx, rect);
    };
}

@end
