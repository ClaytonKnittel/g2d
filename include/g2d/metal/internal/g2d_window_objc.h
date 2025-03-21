
#import <Cocoa/Cocoa.h>
#import <MetalKit/MetalKit.h>

typedef void (*RenderCallbackFn)(void *_Nonnull);

void *_Nullable allocAndInitG2DWindow(void *_Nonnull self_ptr,
                                      float w,
                                      float h,
                                      const char *_Nonnull title) __asm__("_allocAndInitG2DWindow");
void startMetalExecution(void *_Nonnull g2d_window) __asm__("_startMetalExecution");
void registerRenderCallbackObjc(void *_Nonnull g2d_window_ptr,
                                RenderCallbackFn _Nonnull callback) __asm__("_registerRenderCallbackObjc");

@interface G2DWindow : MTKView

- (nonnull instancetype)initWithFrame:(CGRect)frame title:(NSString *_Nonnull)title self_ptr:(void *_Nonnull)self_ptr;

- (void)start;

- (id<MTLDevice> _Nonnull)mtlDevice;

@end
