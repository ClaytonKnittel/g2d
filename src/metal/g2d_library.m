
#include <g2d/metal/internal/g2d_library_objc.h>
#include <g2d/metal/metal_shader_lib_source.h>

@implementation G2DLibrary

- (nonnull instancetype)init
{
  self = [super init];
  if (self) {
    _device = MTLCreateSystemDefaultDevice();
    [self loadLibrary];
  }
  return self;
}

- (BOOL)loadLibrary
{
  NSError *error = nil;

  dispatch_data_t library_src =
      dispatch_data_create(g_metal_shader_lib_source, g_metal_shader_lib_source_size, NULL, NULL);

  _library = [self.device newLibraryWithData:library_src error:&error];
  if (error) {
    NSLog(@"Failed to initialize shader library: %@", error.localizedDescription);
    return false;
  }

  return true;
}

@end
