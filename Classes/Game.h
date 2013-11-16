//
//  Game.h
//  Nestopia
//
//  Created by Nick Tymchenko on 11/15/13.
//
//

#import <Foundation/Foundation.h>

@interface Game : NSObject

@property (nonatomic, strong, readonly) NSString *title;
@property (nonatomic, strong, readonly) NSString *path;
@property (nonatomic, strong, readonly) NSString *savePath;

@property (nonatomic, assign) BOOL favorite;
@property (nonatomic, assign) BOOL saved;

@property (nonatomic, copy) NSDictionary *settings;

+ (NSArray *)gamesList;

@end
