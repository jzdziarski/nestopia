//
//  NSString+TableViewAlphabeticIndexes.m
//  Nestopia
//
//  Created by Nick Tymchenko on 11/16/13.
//
//

#import "NSString+TableViewAlphabeticIndexes.h"

@implementation NSString (TableViewAlphabeticIndexes)

#pragma mark Public

+ (NSArray *)tableViewAlphabeticIndexes {
    static NSArray *indexes;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        indexes = [@[ @"#" ] arrayByAddingObjectsFromArray:[self tableViewLettersArray]];
    });
    return indexes;
}

- (NSString *)tableViewAlphabeticIndex {
    NSMutableString *firstLetter = [[[self substringToIndex:1] uppercaseString] mutableCopy];
    CFStringTransform((__bridge CFMutableStringRef)firstLetter, NULL, kCFStringTransformStripCombiningMarks, NO);
    if ([[[self class] tableViewLettersSet] containsObject:firstLetter]) {
        return firstLetter;
    } else {
        return @"#";
    }
}

#pragma mark Private

+ (NSArray *)tableViewLettersArray {
    static NSArray *letters;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        NSMutableArray *array = [NSMutableArray array];
        for (unichar c = 'A'; c <= 'Z'; ++c) {
            [array addObject:[NSString stringWithCharacters:&c length:1]];
        }
        letters = [array copy];
    });
    return letters;
}

+ (NSSet *)tableViewLettersSet {
    static NSSet *letters;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        letters = [NSSet setWithArray:[self tableViewLettersArray]];
    });
    return letters;
}

@end
