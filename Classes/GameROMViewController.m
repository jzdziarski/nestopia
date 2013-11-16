/*
 Nestopia for iOS
 Copyright (c) 2013, Jonathan A. Zdziarski
 
 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; version 2
 of the License.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 
 */

#import "GameROMViewController.h"
#import "GamePlayViewController.h"
#import "NESNavigationController.h"
#import "NSString+TableViewAlphabeticIndexes.h"

@interface GameROMViewController ()

@property (nonatomic, strong) NSDictionary *gamesPerIndex;

@end


@implementation GameROMViewController

#pragma mark Properties

- (void)setFavorite:(BOOL)favorite {
    _favorite = favorite;
    
    [self updateTabBarItem];
    self.gamesPerIndex = nil;
}

- (void)setSaved:(BOOL)saved {
    _saved = saved;
    
    [self updateTabBarItem];
    self.gamesPerIndex = nil;
}

- (NSDictionary *)gamesPerIndex {
    if (!_gamesPerIndex) {
        [self reloadData];
    }
    return _gamesPerIndex;
}

#pragma mark Init

- (id)init {
    if ((self = [super init])) {
        [self updateTabBarItem];
    }
	return self;
}

#pragma mark UIViewController

- (void)viewWillAppear:(BOOL)animated {
    [super viewWillAppear:animated];
    
    self.gamesPerIndex = nil;
    [self.tableView reloadData];
}

#pragma mark Private

- (void)updateTabBarItem {
    if (self.favorite) {
        self.tabBarItem = [[UITabBarItem alloc] initWithTabBarSystemItem:UITabBarSystemItemFavorites tag:0];
    } else if (self.saved) {
		self.tabBarItem = [[UITabBarItem alloc] initWithTitle:@"Saved Games" image:[UIImage imageNamed:@"History.png"] tag:1];
    } else {
        self.tabBarItem = [[UITabBarItem alloc] initWithTitle:@"All Games" image:[UIImage imageNamed:@"Games.png"] tag:0];
    }
}

- (NSArray *)allGames {
    NSArray *games = [Game gamesList];
    games = [games filteredArrayUsingPredicate:[NSPredicate predicateWithBlock:^BOOL(Game *game, NSDictionary *bindings) {
        if (self.favorite && !game.favorite) {
            return NO;
        }
        if (self.saved && !game.saved) {
            return NO;
        }
        return YES;        
    }]];
    return games;
}

- (void)reloadData {
    NSMutableDictionary *gamesPerIndex = [[NSMutableDictionary alloc] init];
    
    NSArray *games = [self allGames];
    
    for (Game *game in games) {
        NSString *index = [game.title tableViewAlphabeticIndex];
        
        NSMutableArray *gamesForIndex = [gamesPerIndex objectForKey:index];
        if (!gamesForIndex) {
            gamesForIndex = [NSMutableArray array];
            [gamesPerIndex setObject:gamesForIndex forKey:index];
        }
        
        [gamesForIndex addObject:game];
    }
    
    for (NSMutableArray *gamesForIndex in [gamesPerIndex allValues]) {
        [gamesForIndex sortUsingComparator:^NSComparisonResult(Game *game1, Game *game2) {
            return [game1.title localizedCaseInsensitiveCompare:game2.title];
        }];
    }
    
    self.gamesPerIndex = [gamesPerIndex copy];
}

- (NSArray *)gamesForSection:(NSInteger)section {
    NSString *sectionIndex = [[NSString tableViewAlphabeticIndexes] objectAtIndex:section];
    return [self.gamesPerIndex objectForKey:sectionIndex];
}

- (Game *)gameAtIndexPath:(NSIndexPath *)indexPath {
    return [[self gamesForSection:indexPath.section] objectAtIndex:indexPath.row];
}

#pragma mark UITableViewDelegate & UITableViewDataSource

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView {
    return [[NSString tableViewAlphabeticIndexes] count];
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    return [[self gamesForSection:section] count];
}

- (NSArray *)sectionIndexTitlesForTableView:(UITableView *)tableView {
    return [NSString tableViewAlphabeticIndexes];
}

- (NSInteger)tableView:(UITableView *)tableView sectionForSectionIndexTitle:(NSString *)title atIndex:(NSInteger)index {
    return index;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    static NSString *cellIdentifier = @"GameCell";
    
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:cellIdentifier];
    if (!cell) {
        cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:cellIdentifier];
    }
    
    Game *game = [self gameAtIndexPath:indexPath];
    cell.textLabel.text = game.title;
    
    return cell;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
    Game *game = [self gameAtIndexPath:indexPath];
    
    NSLog(@"%s starting game play for %@", __PRETTY_FUNCTION__, game.title);
    
    GamePlayViewController *gamePlayViewController = [[GamePlayViewController alloc] init];
    gamePlayViewController.game = game;
    gamePlayViewController.shouldLoadState = self.saved;
    
    NESNavigationController *navigationController = [[NESNavigationController alloc] initWithRootViewController:gamePlayViewController];
    
    [self presentViewController:navigationController animated:YES completion:nil];
}

- (void)tableView:(UITableView *)tableView commitEditingStyle:(UITableViewCellEditingStyle) editingStyle forRowAtIndexPath:(NSIndexPath *)indexPath {
	if (editingStyle == UITableViewCellEditingStyleDelete) {        
        // TODO
	}
}

@end
