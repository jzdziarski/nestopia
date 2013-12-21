//
//  GameControllerManager.h
//  Nestopia
//
//  Created by Adam Bell on 12/20/2013.
//
//

#import "NestopiaCore.h"

@class GameControllerManager;

@protocol GameControllerManagerDelegate <NSObject>

@required
- (void)gameControllerManagerGamepadDidConnect:(GameControllerManager *)controllerManager;
- (void)gameControllerManagerGamepadDidDisconnect:(GameControllerManager *)controllerManager;

@end

@interface GameControllerManager : NSObject

+(instancetype)sharedInstance;

- (NestopiaPadInput)currentControllerInput;

@property (nonatomic, readonly) BOOL gameControllerConnected;

@property (nonatomic) BOOL swapAB;

@property (nonatomic, weak) id<GameControllerManagerDelegate> delegate;

@end
