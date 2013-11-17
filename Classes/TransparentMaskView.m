//
//  TransparentMaskView.m
//  Nestopia
//
//  Created by Nick Tymchenko on 11/18/13.
//
//

#import "TransparentMaskView.h"

@implementation TransparentMaskView

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
    self.opaque = NO;
}

#pragma mark UIView

- (void)drawRect:(CGRect)rect {
    if (self.maskDrawingBlock) {
        self.maskDrawingBlock(rect);
    }
    
    CGContextRef ctx = UIGraphicsGetCurrentContext();
    CGContextConcatCTM(ctx, CGAffineTransformMake(1, 0, 0, -1, 0, CGRectGetHeight(rect)));
    
    CGImageRef image = CGBitmapContextCreateImage(ctx);
    CGImageRef mask = CGImageMaskCreate(CGImageGetWidth(image),
                                        CGImageGetHeight(image),
                                        CGImageGetBitsPerComponent(image),
                                        CGImageGetBitsPerPixel(image),
                                        CGImageGetBytesPerRow(image),
                                        CGImageGetDataProvider(image),
                                        CGImageGetDecode(image),
                                        CGImageGetShouldInterpolate(image));
    CFRelease(image);
    
    CGContextClearRect(ctx, rect);
    
    CGContextSaveGState(ctx);
    
    CGContextClipToMask(ctx, rect, mask);
    CFRelease(mask);
    
    if (self.backgroundDrawingBlock) {
        self.backgroundDrawingBlock(rect);
    }
    
    CGContextRestoreGState(ctx);
}

@end
