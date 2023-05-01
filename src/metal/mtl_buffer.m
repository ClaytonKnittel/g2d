
#include <Cocoa/Cocoa.h>
#include <MetalKit/MetalKit.h>

#include <g2d/metal/internal/g2d_window_objc.h>

void *_Nullable newBufferWithLength(void *_Nonnull g2d_window_ptr, NSUInteger length)
{
  G2DWindow *g2d_window = (G2DWindow *)g2d_window_ptr;
  id<MTLDevice> device = [g2d_window mtlDevice];

  return [[device newBufferWithLength:length options:MTLResourceStorageModeManaged] retain];
}

void writeToMTLBuffer(void *_Nonnull mtl_buffer_id, NSUInteger pos, void *_Nonnull bytes, NSUInteger len)
{
  id<MTLBuffer> buffer = (_Nonnull id<MTLBuffer>)mtl_buffer_id;

  memcpy((char *)[buffer contents] + pos, bytes, len);
  [buffer didModifyRange:NSMakeRange(pos, pos + len)];
}

void releaseMTLBuffer(void *_Nonnull mtl_buffer_id)
{
  id<MTLBuffer> buffer = (_Nonnull id<MTLBuffer>)mtl_buffer_id;
  [buffer release];
}
