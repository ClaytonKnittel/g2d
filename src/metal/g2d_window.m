
#include <Cocoa/Cocoa.h>
#include <MetalKit/MetalKit.h>

#include <g2d/metal/internal/g2d_app_delegate_objc.h>
#include <g2d/metal/internal/g2d_window_objc.h>
#include <g2d/metal/metal_shader_lib_source.h>

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
  NSWindow *window_;
}

- (nonnull instancetype)initWithFrame:(CGRect)frame title:(NSString *)title
{
  id<MTLDevice> device = MTLCreateSystemDefaultDevice();
  self = [super initWithFrame:frame device:device];
  if (self) {
    [self setup:frame title:title];
  }
  return self;
}

#include <syslog.h>
- (void)start
{
  [NSApp run];
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
  }
}

- (void)drawRect:(CGRect)rect
{
  [super drawRect:rect];
}

@end
