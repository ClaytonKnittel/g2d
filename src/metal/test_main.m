
#import <Cocoa/Cocoa.h>
#import <MetalKit/MetalKit.h>

#include <g2d/metal/metal_shader_lib_source.h>

#import <simd/simd.h>

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

@interface HelloMetalView : MTKView
@end

enum VertexAttributes {
    VertexAttributePosition = 0,
    VertexAttributeColor = 1,
};

enum BufferIndex  {
    MeshVertexBuffer = 0,
    FrameUniformBuffer = 1,
};

struct FrameUniforms {
    simd_float4x4 projectionViewModel;
};


void
app_main(int argc, char *argv[])
{
	(void) argc;
	(void) argv;

	@autoreleasepool {

		[NSApplication sharedApplication];
        [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
        [NSApp activateIgnoringOtherApps:YES];
		//Delegate* delegate = [[Delegate alloc] init];

		// Menu.
		NSMenu* bar = [NSMenu new];
		NSMenuItem * barItem = [NSMenuItem new];
		NSMenu* menu = [NSMenu new];
		NSMenuItem* quit = [[NSMenuItem alloc]
			initWithTitle:@"Quit"
				   action:@selector(terminate:)
			keyEquivalent:@"q"];
		[bar addItem:barItem];
		[barItem setSubmenu:menu];
		[menu addItem:quit];
		NSApp.mainMenu = bar;

		// Window.
		NSRect frame = NSMakeRect(0, 0, 256, 256);
		NSWindow* window = [[NSWindow alloc]
			initWithContentRect:frame styleMask:NSTitledWindowMask
						backing:NSBackingStoreBuffered defer:NO];
		[window cascadeTopLeftFromPoint:NSMakePoint(20,20)];
		window.title = [[NSProcessInfo processInfo] processName];
		[window makeKeyAndOrderFront:nil];

		// Custom MTKView.
		HelloMetalView* view = [[HelloMetalView alloc] initWithFrame:frame];
		window.contentView = view;


		//[NSApp setDelegate : delegate];
		[NSApp run];
	}
}


// Vertex structure on CPU memory.
struct Vertex {
    float position[3];
    unsigned char color[4];
};

// For pipeline executing.
const int uniformBufferCount = 3;

// The main view.
@implementation HelloMetalView {
    id <MTLLibrary> _library;
    id <MTLCommandQueue> _commandQueue;
    id <MTLRenderPipelineState> _pipelineState;
    id <MTLDepthStencilState> _depthState;
    dispatch_semaphore_t _semaphore;
    id <MTLBuffer> _uniformBuffers[uniformBufferCount];
    id <MTLBuffer> _vertexBuffer;
    int uniformBufferIndex;
    long frame;
}

- (id)initWithFrame:(CGRect)inFrame {
    id<MTLDevice> device = MTLCreateSystemDefaultDevice();
    self = [super initWithFrame:inFrame device:device];
    if (self) {
        [self setup];
    }
    return self;
}

- (void)setup {
    // Set view settings.
    self.colorPixelFormat = MTLPixelFormatBGRA8Unorm;
    self.depthStencilPixelFormat = MTLPixelFormatDepth32Float_Stencil8;

	dispatch_data_t dat = dispatch_data_create(g_metal_shader_lib_source,
			g_metal_shader_lib_source_size, NULL, NULL);

    // Load shaders.
    NSError *error = nil;
    //_library = [self.device newLibraryWithFile: @"shaders.metallib" error:&error];
    _library = [self.device newLibraryWithData:dat error:&error];
    if (!_library) {
        NSLog(@"Failed to load library. error %@", error);
        exit(0);
    }
    id <MTLFunction> vertFunc = [_library newFunctionWithName:@"vert"];
    id <MTLFunction> fragFunc = [_library newFunctionWithName:@"frag"];

    // Create depth state.
    MTLDepthStencilDescriptor *depthDesc = [MTLDepthStencilDescriptor new];
    depthDesc.depthCompareFunction = MTLCompareFunctionLess;
    depthDesc.depthWriteEnabled = YES;
    _depthState = [self.device newDepthStencilStateWithDescriptor:depthDesc];

    // Create vertex descriptor.
    MTLVertexDescriptor *vertDesc = [MTLVertexDescriptor new];
    vertDesc.attributes[VertexAttributePosition].format = MTLVertexFormatFloat3;
    vertDesc.attributes[VertexAttributePosition].offset = 0;
    vertDesc.attributes[VertexAttributePosition].bufferIndex = MeshVertexBuffer;
    vertDesc.attributes[VertexAttributeColor].format = MTLVertexFormatUChar4;
	vertDesc.attributes[VertexAttributeColor].offset = 12;
    vertDesc.attributes[VertexAttributeColor].bufferIndex = MeshVertexBuffer;
    vertDesc.layouts[MeshVertexBuffer].stride = 16;
    vertDesc.layouts[MeshVertexBuffer].stepRate = 1;
    vertDesc.layouts[MeshVertexBuffer].stepFunction = MTLVertexStepFunctionPerVertex;

    // Create pipeline state.
    MTLRenderPipelineDescriptor *pipelineDesc = [MTLRenderPipelineDescriptor new];
    pipelineDesc.sampleCount = self.sampleCount;
    pipelineDesc.vertexFunction = vertFunc;
    pipelineDesc.fragmentFunction = fragFunc;
    pipelineDesc.vertexDescriptor = vertDesc;
    pipelineDesc.colorAttachments[0].pixelFormat = self.colorPixelFormat;
    pipelineDesc.depthAttachmentPixelFormat = self.depthStencilPixelFormat;
    pipelineDesc.stencilAttachmentPixelFormat = self.depthStencilPixelFormat;
    _pipelineState = [self.device newRenderPipelineStateWithDescriptor:pipelineDesc error:&error];
    if (!_pipelineState) {
        NSLog(@"Failed to create pipeline state, error %@", error);
        exit(0);
    }

    // Create vertices.
    struct Vertex verts[] = {
        {{-0.5, -0.5, 0}, {255, 0, 0, 255}},
        {{0, 0.5, 0}, {0, 255, 0, 255}},
        {{0.5, -0.5, 0}, {0, 0, 255, 255}}
    };
    _vertexBuffer = [self.device newBufferWithBytes:verts
                                             length:sizeof(verts)
                                            options:MTLResourceStorageModePrivate];

    // Create uniform buffers.
    for (int i = 0; i < uniformBufferCount; i++) {
        _uniformBuffers[i] = [self.device newBufferWithLength:64
                                          options:MTLResourceCPUCacheModeWriteCombined];
    }
    frame = 0;

    // Create semaphore for each uniform buffer.
    _semaphore = dispatch_semaphore_create(uniformBufferCount);
    uniformBufferIndex = 0;

    // Create command queue
    _commandQueue = [self.device newCommandQueue];

}

- (void)drawRect:(CGRect)rect {
	dispatch_semaphore_wait(_semaphore, DISPATCH_TIME_FOREVER);

    // Animation.
    frame++;
    float rad = frame * 0.01f;
    float sinv = sin(rad), cosv = cos(rad);

	simd_float4 a = {cosv, -sinv, 0, 0};
	simd_float4 b = {sinv, cosv, 0, 0};
	simd_float4 c = {0, -0, 1, 0};
	simd_float4 d = {0, 0, 0, 1};
    simd_float4x4 rot = {a, b, c, d};

    // Update the current uniform buffer.
    uniformBufferIndex = (uniformBufferIndex + 1) % uniformBufferCount;
    struct FrameUniforms *uniforms = (struct FrameUniforms *)[_uniformBuffers[uniformBufferIndex] contents];
    uniforms->projectionViewModel = rot;

    // Create a command buffer.
    id <MTLCommandBuffer> commandBuffer = [_commandQueue commandBuffer];

    // Encode render command.
    id <MTLRenderCommandEncoder> encoder =
        [commandBuffer renderCommandEncoderWithDescriptor:self.currentRenderPassDescriptor];

	MTLViewport vp = {0, 0, self.drawableSize.width, self.drawableSize.height, 0, 1};
    [encoder setViewport:vp];
    [encoder setDepthStencilState:_depthState];
    [encoder setRenderPipelineState:_pipelineState];
    [encoder setVertexBuffer:_uniformBuffers[uniformBufferIndex]
                      offset:0 atIndex:FrameUniformBuffer];
    [encoder setVertexBuffer:_vertexBuffer offset:0 atIndex:MeshVertexBuffer];
    [encoder drawPrimitives:MTLPrimitiveTypeTriangle vertexStart:0 vertexCount:3];
    [encoder endEncoding];

    // Set callback for semaphore.
    __block dispatch_semaphore_t semaphore = _semaphore;
    [commandBuffer addCompletedHandler:^(id<MTLCommandBuffer> buffer) {
        dispatch_semaphore_signal(semaphore);
    }];
    [commandBuffer presentDrawable:self.currentDrawable];
    [commandBuffer commit];

    // Draw children.
    [super drawRect:rect];
}

@end

