//
//  TransparentMaskView.h
//  Nestopia
//
//  Created by Nick Tymchenko on 11/18/13.
//
//

#import <UIKit/UIKit.h>

@interface TransparentMaskView : UIView

- (void)commonInit;

@property (nonatomic, copy) void (^maskDrawingBlock)(CGRect rect);
@property (nonatomic, copy) void (^backgroundDrawingBlock)(CGRect rect);

@end
