@interface DisclosureIndicator : UIControl
{
	UIColor *_accessoryColor;
	UIColor *_highlightedColor;
}

@property (nonatomic, retain) UIColor *accessoryColor;
@property (nonatomic, retain) UIColor *highlightedColor;

+ (DisclosureIndicator *)accessoryWithColor:(UIColor *)color;

@end