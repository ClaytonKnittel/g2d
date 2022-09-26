
#include <MetalKit/MetalKit.h>

#import <g2d/metal/internal/g2d_library_objc.h>

@interface G2DRenderPipeline : NSObject

- (nonnull instancetype)init:(G2DLibrary *_Nonnull)g2d_library
                vertexFnName:(NSString *_Nonnull)vertex_fn_name
              fragmentFnName:(NSString *_Nonnull)fragment_fn_name
            vertexDescriptor:(MTLVertexDescriptor *_Nonnull)vertex_descriptor;

@end
