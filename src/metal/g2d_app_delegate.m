
#include <g2d/metal/internal/g2d_app_delegate_objc.h>

@implementation G2DAppDelegate

- (nonnull instancetype)init
{
  self = [super init];
  return self;
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)sender
{
  return true;
}

@end
