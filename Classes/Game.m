//
//  Game.m
//  Nestopia
//
//  Created by Nick Tymchenko on 11/15/13.
//
//

#import "Game.h"

@interface Game ()

@property (nonatomic, assign) BOOL saved;

@end


@implementation Game

#pragma mark Properties

@dynamic settings, favorite;

- (NSDictionary *)settings {
    NSDictionary *settings = [[NSUserDefaults standardUserDefaults] dictionaryForKey:[self settingsKey]];
    return settings ?: [[self class] globalSettings];
}

- (void)setSettings:(NSDictionary *)settings {
    if (settings) {
        [[NSUserDefaults standardUserDefaults] setObject:settings forKey:[self settingsKey]];
    } else {
        [[NSUserDefaults standardUserDefaults] removeObjectForKey:[self settingsKey]];
    }
    
    [[NSUserDefaults standardUserDefaults] synchronize];
}

- (BOOL)favorite {
    return [[NSUserDefaults standardUserDefaults] boolForKey:[self favoriteKey]];
}

- (void)setFavorite:(BOOL)favorite {
    [[NSUserDefaults standardUserDefaults] setBool:favorite forKey:[self favoriteKey]];
}

#pragma mark Init

- (id)initWithTitle:(NSString *)title path:(NSString *)path savePath:(NSString *)savePath {
    if ((self = [super init])) {
        _title = title;
        _path = path;
        _savePath = savePath;
    }
    return self;
}

#pragma mark Public

+ (NSArray *)gamesList {
    NSMutableArray *games = [NSMutableArray array];
    
    NSArray *searchPaths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *documentsPath = [searchPaths lastObject];
    
    NSFileManager *fileManager = [NSFileManager defaultManager];
    NSDirectoryEnumerator *enumerator = [fileManager enumeratorAtPath:documentsPath];
    NSString *filename;
    
    while ((filename = [enumerator nextObject])) {
        if ([[[filename pathExtension] lowercaseString] isEqualToString:@"nes"]) {
            NSString *title = [[filename lastPathComponent] stringByDeletingPathExtension];
            NSString *path = [documentsPath stringByAppendingPathComponent:filename];
            NSString *savePath = [path stringByAppendingPathExtension:@"sav"];

            Game *game = [[Game alloc] initWithTitle:title path:path savePath:savePath];
            game.saved = [fileManager fileExistsAtPath:savePath];
            [games addObject:game];
        }
    }
    
    return games;
}

+ (NSDictionary *)globalSettings {
    return [[NSUserDefaults standardUserDefaults] dictionaryForKey:@"GlobalSettings"];
}

+ (void)saveGlobalSettings:(NSDictionary *)settings {
    if (settings) {
        [[NSUserDefaults standardUserDefaults] setObject:settings forKey:@"GlobalSettings"];
    } else {
        [[NSUserDefaults standardUserDefaults] removeObjectForKey:@"GlobalSettings"];
    }
    
    [[NSUserDefaults standardUserDefaults] synchronize];
}

#pragma mark Private

- (NSString *)settingsKey {
    return [NSString stringWithFormat:@"Games.%@.settings", self.title];
}

- (NSString *)favoriteKey {
    return [NSString stringWithFormat:@"Games.%@.favorite", self.title];
}

@end
