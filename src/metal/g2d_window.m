
#include <Cocoa/Cocoa.h>
#include <MetalKit/MetalKit.h>

#include <g2d/metal/internal/g2d_app_delegate_objc.h>
#include <g2d/metal/internal/g2d_library_objc.h>
#include <g2d/metal/internal/g2d_window_objc.h>

void *_Nullable allocAndInitG2DWindow(float w, float h, const char *_Nonnull title)
{
  NSString *title_str = [[NSString alloc] initWithCString:title encoding:NSUTF8StringEncoding];

  G2DWindow *window = [[G2DWindow alloc] initWithFrame:NSMakeRect(0, 0, w, h) title:title_str];
  return (void *)window;
}

void startMetalExecution(void *_Nonnull g2d_window_ptr)
{
  G2DWindow *g2d_window = (G2DWindow *)g2d_window_ptr;
  [g2d_window start];
}

@implementation G2DWindow {
  G2DAppDelegate *app_delegate_;
  G2DLibrary *library_;
  NSWindow *window_;
  id<MTLDevice> device_;
  id<MTLCommandQueue> cmd_queue_;
}

- (nonnull instancetype)initWithFrame:(CGRect)frame title:(NSString *)title
{
  device_ = MTLCreateSystemDefaultDevice();
  self = [super initWithFrame:frame device:device_];
  if (self) {
    [self setup:frame title:title];
  }
  return self;
}

- (void)start
{
  [NSApp run];
}

- (id<MTLDevice> _Nonnull)mtlDevice
{
  return device_;
}

- (BOOL)acceptsFirstResponder
{
  return true;
}

- (BOOL)becomeFirstResponder
{
  return true;
}

- (BOOL)resignFirstResponder
{
  return true;
}

- (void)setupMenuBar
{
  NSMenu *bar = [NSMenu new];
  NSMenuItem *barItem = [NSMenuItem new];
  NSMenu *menu = [NSMenu new];
  NSMenuItem *quit = [[NSMenuItem alloc] initWithTitle:@"Quit" action:@selector(terminate:) keyEquivalent:@"q"];
  [bar addItem:barItem];
  [barItem setSubmenu:menu];
  [menu addItem:quit];

  NSApp.mainMenu = bar;
}

- (void)setup:(CGRect)frame title:(NSString *)title
{
  self.colorPixelFormat = MTLPixelFormatBGRA8Unorm;
  self.depthStencilPixelFormat = MTLPixelFormatDepth32Float_Stencil8;

  @autoreleasepool {
    [NSApplication sharedApplication];
    [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
    [NSApp activateIgnoringOtherApps:YES];

    [self setupMenuBar];

    window_ = [[NSWindow alloc]
        initWithContentRect:frame
                  styleMask:NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable
                    backing:NSBackingStoreBuffered
                      defer:NO];
    [window_ cascadeTopLeftFromPoint:NSMakePoint(20, 20)];
    window_.title = title;
    [window_ makeKeyAndOrderFront:nil];
    window_.contentView = self;
    [window_ makeFirstResponder:self];

    G2DAppDelegate *delegate = [[G2DAppDelegate alloc] init];
    [NSApp setDelegate:delegate];

    self.preferredFramesPerSecond = 60;
  }

  library_ = [[G2DLibrary alloc] init];

  cmd_queue_ = [device_ newCommandQueue];
}

- (void)drawRect:(CGRect)rect
{
  id<CAMetalDrawable> drawable = self.currentDrawable;
  id<MTLTexture> texture = drawable.texture;

  MTLRenderPassDescriptor *passDescriptor = [MTLRenderPassDescriptor renderPassDescriptor];
  passDescriptor.colorAttachments[0].texture = texture;
  passDescriptor.colorAttachments[0].loadAction = MTLLoadActionClear;
  passDescriptor.colorAttachments[0].storeAction = MTLStoreActionStore;
  passDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(0.173, 0.405, 0.430, 1.0);

  MTLRenderPassDescriptor *passDescriptor2 = [MTLRenderPassDescriptor renderPassDescriptor];
  passDescriptor2.colorAttachments[0].texture = texture;
  passDescriptor2.colorAttachments[0].loadAction = MTLLoadActionLoad;
  passDescriptor2.colorAttachments[0].storeAction = MTLStoreActionStore;

  id<MTLCommandBuffer> buffer = [cmd_queue_ commandBuffer];

  id<MTLRenderCommandEncoder> commandEncoder = [buffer renderCommandEncoderWithDescriptor:passDescriptor];
  [commandEncoder endEncoding];

  id<MTLRenderCommandEncoder> commandEncoder2 = [buffer renderCommandEncoderWithDescriptor:passDescriptor2];
  [commandEncoder2 endEncoding];

  [buffer presentDrawable:self.currentDrawable];
  [buffer commit];

  [super drawRect:rect];
}

@end
