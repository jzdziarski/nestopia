//
//  PadRoundTextButton.m
//  Nestopia
//
//  Created by Nick Tymchenko on 11/18/13.
//
//

#import "PadRoundTextButton.h"
#import "RoundTextMaskView.h"

@interface PadRoundTextButton ()

@property RoundTextMaskView *maskView;

@end


@implementation PadRoundTextButton

#pragma mark Properties

- (NSString *)text {
    return self.maskView.text;
}

- (void)setText:(NSString *)text {
    self.maskView.text = text;
}

- (UIFont *)font {
    return self.maskView.font;
}

- (void)setFont:(UIFont *)font {
    self.maskView.font = font;
}

#pragma mark Init

- (void)commonInit {
    [super commonInit];
    
    _maskView = [[RoundTextMaskView alloc] init];
    _maskView.color = [UIColor whiteColor];
    [self addSubview:_maskView];
}

#pragma mark UIView

- (void)layoutSubviews {
    [super layoutSubviews];
    self.maskView.frame = self.bounds;
}

@end
