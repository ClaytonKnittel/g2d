
#import <g2d/metal/internal/g2d_utils_objc.h>

@implementation Utils

+ (NSURL *)getMainBundleURL
{
  NSBundle *main_bundle = [NSBundle mainBundle];
  if (main_bundle == nil) {
    return nil;
  }

  return [main_bundle bundleURL];
}

+ (NSURL *)getResourceURL
{
  NSBundle *main_bundle = [NSBundle mainBundle];
  if (main_bundle == nil) {
    return nil;
  }

  return [main_bundle resourceURL];
}

+ (NSURL *)getPrivateFrameworksURL
{
  NSBundle *main_bundle = [NSBundle mainBundle];
  if (main_bundle == nil) {
    return nil;
  }

  return [main_bundle privateFrameworksURL];
}

+ (NSURL *)getSharedFrameworksURL
{
  NSBundle *main_bundle = [NSBundle mainBundle];
  if (main_bundle == nil) {
    return nil;
  }

  return [main_bundle sharedFrameworksURL];
}

@end
