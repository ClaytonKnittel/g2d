
#include <g2d/metal/key_process.h>

#include <Foundation/Foundation.h>

#include <Cocoa/Cocoa.h>
#include <Metal/Metal.h>
#include <MetalKit/MetalKit.h>
#include <QuartzCore/QuartzCore.h>

@interface Renderer : NSObject
{
	id<MTLDevice> _device;
	id<MTLLibrary> _library;
	id<MTLRenderPipelineState> _pipelineState;
}
@end

@implementation Renderer

- (instancetype) init : (id<MTLDevice>) device
{
	self = [super init];
	if (self) {
		_device = [device retain];
	}
	return self;
}

- (void) dealloc
{
	[_device release];
	[super dealloc];
}

@end

@interface ViewDelegate : NSObject<MTKViewDelegate>
{
	Renderer* renderer;
}
@end

@implementation ViewDelegate

- (instancetype) init : (id<MTLDevice>) device
{
	self = [super init];
	if (self) {
		renderer = [[Renderer alloc] init : device];
	}
	return self;
}

- (void) dealloc
{
	[renderer dealloc];
	[super dealloc];
}

- (void) mtkView:(MTKView*) view drawableSizeWillChange:(CGSize) size
{
	// Window is not resizable
	(void)view;
	(void)size;
}

- (void) drawInMTKView:(MTKView*) view
{
	(void) view;
}

@end

@interface AppDelegate : NSObject<NSApplicationDelegate>
{
	id<MTLDevice> _device;
	NSWindow* _window;
	MTKView* _view;
	ViewDelegate* _view_delegate;
}
@end

@implementation AppDelegate

- (instancetype) init : (id<MTLDevice>) device
{
	self = [super init];
	if (self) {
		_device = [device retain];

	}
	return self;
}

- (void) dealloc
{
	[_device release];
	[super dealloc];
}

@end

int
test_objc(int a)
{
	@autoreleasepool {
		id<MTLDevice> device = MTLCreateSystemDefaultDevice();

		printf("%@\n", device);
	}
	return a;
}

