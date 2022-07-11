
#import <Cocoa/Cocoa.h>
#import <MetalKit/MetalKit.h>

@interface Renderer : NSObject<MTKViewDelegate>

- (nonnull instancetype) initWithMetalKitView : (nonnull MTKView*) mtkView;

- (void) mtkView : (MTKView*) view drawableSizeWillChange : (CGSize) size;

- (void) drawInMTKView : (MTKView*) view;

@end

// Main class performing the rendering
@implementation Renderer

- (nonnull instancetype) initWithMetalKitView : (nonnull MTKView*) mtkView
{
	NSLog(@"Initializing renderer!\n");
    self = [super init];
    if(self)
    {
	}

	return self;
}

- (void) mtkView : (MTKView*) view drawableSizeWillChange : (CGSize) size
{
	(void) size;
}

- (void) drawInMTKView : (MTKView*) view
{
	(void) view;
}

@end


@interface ViewController : NSViewController

@end

@implementation ViewController
{
    MTKView* _view;
}

- (instancetype) init
{
	self = [super initWithNibName:@"ViewController" bundle:nil];
	return self;
}

- (void) viewDidLoad
{
    [super viewDidLoad];

    // Set the view to use the default device
    _view = (MTKView*) self.view;
    _view.device = MTLCreateSystemDefaultDevice();
    NSAssert(_view.device, @"Metal is not supported on this device");

    Renderer* renderer = [[Renderer alloc] initWithMetalKitView:_view];
    NSAssert(renderer, @"Renderer failed initialization");

    // Initialize our renderer with the view size
    [renderer mtkView:_view drawableSizeWillChange:_view.drawableSize];

    _view.delegate = renderer;
}

@end


@interface Delegate : NSObject<NSApplicationDelegate>
@end

@implementation Delegate
{
	NSWindow* _window;
}

- (void) applicationDidFinishLaunching : (NSNotification*) notification
{
	NSError* error;

    NSRect frame = NSMakeRect(0, 0, 640, 480);

	_window = [[NSWindow alloc] initWithContentRect:frame
		styleMask : NSWindowStyleMaskTitled | NSWindowStyleMaskClosable
		backing : NSBackingStoreBuffered
		defer:NO];

	//NSBundle* bundle = [[NSBundle alloc] initWithPath :
	//	@"/Users/ClaytonKnittel/VSCode/g2d/src/metal/res"];

	//ViewController* view_controller = [[ViewController alloc] initWithNibName : @"ViewController" bundle : bundle];
	ViewController* view_controller = [[ViewController alloc] init];
	[_window setContentViewController : view_controller];
}

- (BOOL) applicationShouldTerminateAfterLastWindowClosed : (NSApplication*) sender
{
    return YES;
}

@end


void
app_main(int argc, char *argv[])
{
	(void) argc;
	(void) argv;

    [NSApplication sharedApplication];
	Delegate* delegate = [[Delegate alloc] init];

	[NSApp setDelegate : delegate];
    [NSApp run];
}

