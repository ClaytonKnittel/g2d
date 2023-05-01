
#import <Cocoa/Cocoa.h>
#import <MetalKit/MetalKit.h>

void *_Nullable allocAndInitG2DWindow(float w, float h, const char *_Nonnull title) __asm__("_allocAndInitG2DWindow");
void startMetalExecution(void *_Nonnull g2d_window) __asm__("_startMetalExecution");

@interface G2DWindow : MTKView

- (nonnull instancetype)initWithFrame:(CGRect)frame title:(NSString *_Nonnull)title;

- (void)start;

- (id<MTLDevice> _Nonnull)mtlDevice;

@end
