//
//  PadRoundTextButton.h
//  Nestopia
//
//  Created by Nick Tymchenko on 11/18/13.
//
//

#import "PadSingleButton.h"

@interface PadRoundTextButton : PadSingleButton

@property (nonatomic, copy) NSString *text;
@property (nonatomic, copy) UIFont *font;

@end
