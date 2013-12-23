//
//  RoundTextMaskView.h
//  Nestopia
//
//  Created by Nick Tymchenko on 11/18/13.
//
//

#import "TransparentMaskView.h"

@interface RoundTextMaskView : TransparentMaskView

@property (nonatomic, copy) NSString *text;
@property (nonatomic, copy) UIFont *font;
@property (nonatomic, copy) UIColor *color;

@end
