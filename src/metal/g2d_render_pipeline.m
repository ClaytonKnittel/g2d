
#include <g2d/metal/internal/g2d_library_objc.h>
#include <g2d/metal/internal/g2d_render_pipeline_objc.h>

@implementation G2DRenderPipeline {
  id<MTLCommandQueue> command_queue_;
  id<MTLRenderPipelineState> pipeline_state_;
}

- (nonnull instancetype)init:(G2DLibrary *)g2d_library
                vertexFnName:(NSString *)vertex_fn_name
              fragmentFnName:(NSString *)fragment_fn_name
            vertexDescriptor:(MTLVertexDescriptor *)vertex_descriptor
{
  self = [super init];
  if (self) {
    NSError *error;

    id<MTLLibrary> mtl_library = [g2d_library library];
    id<MTLFunction> vert_fn = [mtl_library newFunctionWithName:vertex_fn_name];
    id<MTLFunction> frag_fn = [mtl_library newFunctionWithName:fragment_fn_name];

    MTLRenderPipelineDescriptor *pipeline_desc = [MTLRenderPipelineDescriptor new];
    pipeline_desc.sampleCount = 4;
    pipeline_desc.vertexFunction = vert_fn;
    pipeline_desc.fragmentFunction = frag_fn;
    pipeline_desc.vertexDescriptor = vertex_descriptor;
    pipeline_desc.colorAttachments[0].pixelFormat = MTLPixelFormatBGRA8Unorm;

    pipeline_state_ = [[g2d_library device] newRenderPipelineStateWithDescriptor:pipeline_desc error:&error];
    if (pipeline_state_ == nil) {
      NSLog(@"Failed to initialize render pipeline: %@", error.localizedDescription);
    }
  }
  return self;
}

@end
