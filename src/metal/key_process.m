
#include <g2d/metal/key_process.h>

#include <Foundation/Foundation.h>

#include <Cocoa/Cocoa.h>
#include <Metal/Metal.h>
#include <MetalKit/MetalKit.h>
#include <AppKit/AppKit.h>
#include <QuartzCore/QuartzCore.h>

/*
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

// *
- (NSMenu*) createMenuBar
{
	NSMenu* main_menu = [[NSMenu alloc] init];
	NSMenuItem* menu_item = [[NSMenuItem alloc] init];
	NSMenu* app_menu = [[NSMenu alloc] initWithTitle : @"Appname"];

	NSString* app_name = [[NSRunningApplication currentApplication] localizedName];
	NSString* quit_item_name = [@"Quit " stringByAppendingString : app_name];
	quit_cb = [NSMenuItem registerActionCallback : "appQuit";

	return main_menu;

	NS::String* appName = NS::RunningApplication::currentApplication()->localizedName();
	NS::String* quitItemName = NS::String::string( "Quit ", UTF8StringEncoding )->stringByAppendingString( appName );
	SEL quitCb = NS::MenuItem::registerActionCallback( "appQuit", [](void*,SEL,const NS::Object* pSender){
			auto pApp = NS::Application::sharedApplication();
			pApp->terminate( pSender );
			} );

	NS::MenuItem* pAppQuitItem = pAppMenu->addItem( quitItemName, quitCb, NS::String::string( "q", UTF8StringEncoding ) );
	pAppQuitItem->setKeyEquivalentModifierMask( NS::EventModifierFlagCommand );
	pAppMenuItem->setSubmenu( pAppMenu );

	NS::MenuItem* pWindowMenuItem = NS::MenuItem::alloc()->init();
	NS::Menu* pWindowMenu = NS::Menu::alloc()->init( NS::String::string( "Window", UTF8StringEncoding ) );

	SEL closeWindowCb = NS::MenuItem::registerActionCallback( "windowClose", [](void*, SEL, const NS::Object*){
			auto pApp = NS::Application::sharedApplication();
			pApp->windows()->object< NS::Window >(0)->close();
			} );
	NS::MenuItem* pCloseWindowItem = pWindowMenu->addItem( NS::String::string( "Close Window", UTF8StringEncoding ), closeWindowCb, NS::String::string( "w", UTF8StringEncoding ) );
	pCloseWindowItem->setKeyEquivalentModifierMask( NS::EventModifierFlagCommand );

	pWindowMenuItem->setSubmenu( pWindowMenu );

	pMainMenu->addItem( pAppMenuItem );
	pMainMenu->addItem( pWindowMenuItem );

	pAppMenuItem->release();
	pWindowMenuItem->release();
	pAppMenu->release();
	pWindowMenu->release();

	return pMainMenu->autorelease();
}
// *

- (void) applicationWillFinishLaunching : (NSNotification*) notification
{
}

- (void) applicationDidFinishLaunching : (NSNotification*) notification
{
}

- (BOOL) applicationShouldTerminateAfterLastWindowClosed : (NSApplication*) sender
{
	return YES;
}

@end
*/

int
test_objc(int a)
{
	@autoreleasepool {
		id<MTLDevice> device = MTLCreateSystemDefaultDevice();

		printf("%@\n", device);
	}
	return a;
}

