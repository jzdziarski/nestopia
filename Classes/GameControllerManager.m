//
//  GameControllerManager.m
//  Nestopia
//
//  Created by Adam Bell on 12/20/2013.
//
//

#import "GameControllerManager.h"
#import <GameController/GameController.h>

@implementation GameControllerManager {
    GCController *gameController;
}

- (instancetype)init {
    self = [super init];
    if (self != nil) {
        [self setupController];
        
        [[NSNotificationCenter defaultCenter] addObserver:self
                                                 selector:@selector(controllerConnected:)
                                                     name:GCControllerDidConnectNotification
                                                   object:nil];
        [[NSNotificationCenter defaultCenter] addObserver:self
                                                 selector:@selector(controllerDisconnected:)
                                                     name:GCControllerDidDisconnectNotification
                                                   object:nil];
    }
    
    return self;
}

- (void)dealloc {
    [[NSNotificationCenter defaultCenter] removeObserver:self
                                                    name:GCControllerDidConnectNotification
                                                  object:nil];
    [[NSNotificationCenter defaultCenter] removeObserver:self
                                                    name:GCControllerDidDisconnectNotification
                                                  object:nil];
}

+ (instancetype)sharedInstance {
    static dispatch_once_t p = 0;
    
    __strong static id _sharedInstance = nil;
    
    dispatch_once(&p, ^{
        _sharedInstance = [[self alloc] init];
    });
    
    return _sharedInstance;
}

#pragma mark Game Controller Handling

- (void)setupController {
    NSArray *controllers = [GCController controllers];
    // Grab first controller
    // TODO: Add support for multiple controllers
    gameController = [controllers firstObject];
}

- (void)controllerConnected:(NSNotification *)notification {
    [self setupController];
}

- (void)controllerDisconnected:(NSNotification *)notification {
    [self setupController];
}

- (NestopiaPadInput)currentControllerInput {
    NestopiaPadInput padInput = 0;
    
    if (gameController) {
        if (gameController.extendedGamepad) {
            GCExtendedGamepad *extendedGamepad = gameController.extendedGamepad;
            
            // Swap A+B because it feels awkward on Gamepad
            if (extendedGamepad.buttonA.pressed) {
                padInput |= NestopiaPadInputB;
            }
            if (extendedGamepad.buttonB.pressed) {
                padInput |= NestopiaPadInputA;
            }
            
            // This feels super awkward
            /*
             if (extendedGamepad.buttonX.pressed) {
             padInput |= NestopiaPadInputStart;
             }
             if (extendedGamepad.buttonY.pressed) {
             padInput |= NestopiaPadInputSelect;
             }
             */
            
            // Extended Gamepad gets a thumbstick as well
            if (extendedGamepad.dpad.up.pressed || extendedGamepad.leftThumbstick.up.pressed) {
                padInput |= NestopiaPadInputUp;
            }
            if (extendedGamepad.dpad.down.pressed || extendedGamepad.leftThumbstick.down.pressed) {
                padInput |= NestopiaPadInputDown;
            }
            if (extendedGamepad.dpad.left.pressed || extendedGamepad.leftThumbstick.left.pressed) {
                padInput |= NestopiaPadInputLeft;
            }
            if (extendedGamepad.dpad.right.pressed || extendedGamepad.leftThumbstick.right.pressed) {
                padInput |= NestopiaPadInputRight;
            }
        }
        
        if (gameController.gamepad) {
            GCGamepad *gamepad = gameController.gamepad;
            
            if (gamepad.buttonA.pressed) {
                padInput |= NestopiaPadInputB;
            }
            if (gamepad.buttonB.pressed) {
                padInput |= NestopiaPadInputA;
            }
            /*
             if (extendedGamepad.buttonX.pressed) {
             padInput |= NestopiaPadInputStart;
             }
             if (extendedGamepad.buttonY.pressed) {
             padInput |= NestopiaPadInputSelect;
             }
             */
            
            if (gamepad.dpad.up.pressed) {
                padInput |= NestopiaPadInputUp;
            }
            if (gamepad.dpad.down.pressed) {
                padInput |= NestopiaPadInputDown;
            }
            if (gamepad.dpad.left.pressed) {
                padInput |= NestopiaPadInputLeft;
            }
            if (gamepad.dpad.right.pressed) {
                padInput |= NestopiaPadInputRight;
            }
        }
    }
    
    return padInput;
}

@end
