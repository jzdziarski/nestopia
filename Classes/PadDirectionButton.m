//
//  PadDirectionButton.m
//  Nestopia
//
//  Created by Nick Tymchenko on 11/17/13.
//
//

#import "PadDirectionButton.h"
#import "TransparentMaskView.h"

@interface PadDirectionButton ()

@property TransparentMaskView *maskView;

@end


@implementation PadDirectionButton

#pragma mark Init

- (void)commonInit {
    [super commonInit];
    
    _maskView = [[TransparentMaskView alloc] init];
    _maskView.maskDrawingBlock = ^(CGRect rect) {
        CGContextRef ctx = UIGraphicsGetCurrentContext();
        CGContextSetFillColorWithColor(ctx, [UIColor whiteColor].CGColor);
        
        CGSize edge = CGSizeMake(floor(rect.size.width / 15), floor(rect.size.height / 15));
        CGSize tr = CGSizeMake(floor(rect.size.width / 6), floor(rect.size.height / 6));
        
        // up arrow
        CGContextMoveToPoint(ctx, CGRectGetMidX(rect), CGRectGetMinY(rect) + edge.height);
        CGContextAddLineToPoint(ctx, CGRectGetMidX(rect) - tr.width, CGRectGetMinY(rect) + tr.height + edge.height);
        CGContextAddLineToPoint(ctx, CGRectGetMidX(rect) + tr.width, CGRectGetMinY(rect) + tr.height + edge.height);
        CGContextClosePath(ctx);
        CGContextFillPath(ctx);
        
        // down arrow
        CGContextMoveToPoint(ctx, CGRectGetMidX(rect), CGRectGetMaxY(rect) - edge.height);
        CGContextAddLineToPoint(ctx, CGRectGetMidX(rect) - tr.width, CGRectGetMaxY(rect) - tr.height - edge.height);
        CGContextAddLineToPoint(ctx, CGRectGetMidX(rect) + tr.width, CGRectGetMaxY(rect) - tr.height - edge.height);
        CGContextClosePath(ctx);
        CGContextFillPath(ctx);
        
        // left arrow
        CGContextMoveToPoint(ctx, CGRectGetMinX(rect) + edge.width, CGRectGetMidY(rect));
        CGContextAddLineToPoint(ctx, CGRectGetMinX(rect) + edge.width + tr.width, CGRectGetMidY(rect) - tr.height);
        CGContextAddLineToPoint(ctx, CGRectGetMinX(rect) + edge.width + tr.width, CGRectGetMidY(rect) + tr.height);
        CGContextClosePath(ctx);
        CGContextFillPath(ctx);
        
        // right arrow
        CGContextMoveToPoint(ctx, CGRectGetMaxX(rect) - edge.width, CGRectGetMidY(rect));
        CGContextAddLineToPoint(ctx, CGRectGetMaxX(rect) - edge.width - tr.width, CGRectGetMidY(rect) - tr.height);
        CGContextAddLineToPoint(ctx, CGRectGetMaxX(rect) - edge.width - tr.width, CGRectGetMidY(rect) + tr.height);
        CGContextClosePath(ctx);
        CGContextFillPath(ctx);
    };
    _maskView.backgroundDrawingBlock = ^(CGRect rect) {
        CGContextRef ctx = UIGraphicsGetCurrentContext();
        CGContextSetFillColorWithColor(ctx, [UIColor whiteColor].CGColor);
        CGContextFillEllipseInRect(ctx, rect);
    };
    [self addSubview:_maskView];
}

#pragma mark UIView

- (void)layoutSubviews {
    [super layoutSubviews];
    self.maskView.frame = self.bounds;
}

#pragma mark PadButton

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
