
#import <Foundation/Foundation.h>

@interface Utils : NSObject

+ (NSURL *)getMainBundleURL;

+ (NSURL *)getResourceURL;

+ (NSURL *)getPrivateFrameworksURL;

+ (NSURL *)getSharedFrameworksURL;

@end
