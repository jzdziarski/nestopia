@interface DisclosureIndicator : UIControl
{
	UIColor *_accessoryColor;
	UIColor *_highlightedColor;
}

@property (nonatomic, strong) UIColor *accessoryColor;
@property (nonatomic, strong) UIColor *highlightedColor;

+ (DisclosureIndicator *)accessoryWithColor:(UIColor *)color;

@end