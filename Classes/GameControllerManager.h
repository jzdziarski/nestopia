//
//  GameControllerManager.h
//  Nestopia
//
//  Created by Adam Bell on 12/20/2013.
//
//

#import "NestopiaCore.h"

@interface GameControllerManager : NSObject

+(instancetype)sharedInstance;

- (NestopiaPadInput)currentControllerInput;

@end
