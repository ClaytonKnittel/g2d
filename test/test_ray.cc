
#include <g2d/metal/metal.h>

class ViewDelegate : public MTK::ViewDelegate
{
private:
	MTL::Device* m_device;
	MTL::CommandQueue* m_command_queue;

public:
	ViewDelegate(MTL::Device* device) : m_device(device->retain()) {
		m_command_queue = device->newCommandQueue();
	}

	virtual ~ViewDelegate() override {
		m_command_queue->release();
		m_device->release();
	}

	virtual void drawInMTKView(MTK::View* view) override {
    	NS::AutoreleasePool* _pool = NS::AutoreleasePool::alloc()->init();

		MTL::CommandBuffer* cmd_buffer = m_command_queue->commandBuffer();
		MTL::RenderPassDescriptor* render_pass =
			view->currentRenderPassDescriptor();
		MTL::RenderCommandEncoder* encoder =
			cmd_buffer->renderCommandEncoder(render_pass);

		encoder->endEncoding();
		cmd_buffer->presentDrawable(view->currentDrawable());
		cmd_buffer->commit();

		_pool->release();
	}
};

class AppDelegate : public NS::ApplicationDelegate
{
private:
	MTL::Device* m_device;
	NS::Window* m_window;
	MTK::View* m_view;
	ViewDelegate* m_view_delegate;


	NS::Menu* createMenuBar()
	{
		using NS::StringEncoding::UTF8StringEncoding;

		NS::Menu* pMainMenu = NS::Menu::alloc()->init();
		NS::MenuItem* pAppMenuItem = NS::MenuItem::alloc()->init();
		NS::Menu* pAppMenu = NS::Menu::alloc()->init( NS::String::string( "Appname", UTF8StringEncoding ) );

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

public:
	AppDelegate() : m_device(nullptr), m_view(nullptr), m_view_delegate(nullptr) {}
	~AppDelegate() {
		if (m_view_delegate != nullptr) {
			delete m_view_delegate;
		}
		if (m_view != nullptr) {
			m_view->release();
		}
		if (m_window != nullptr) {
			m_window->release();
		}
		if (m_device != nullptr) {
			m_device->release();
		}
	}

	virtual void applicationWillFinishLaunching( NS::Notification* notification ) override {
		NS::Menu* menu = createMenuBar();
		NS::Application* app = static_cast<NS::Application*>(notification->object());
		app->setMainMenu(menu);
		app->setActivationPolicy(NS::ActivationPolicy::ActivationPolicyRegular);
	}

	virtual void applicationDidFinishLaunching( NS::Notification* notification ) override {

		m_device = MTL::CreateSystemDefaultDevice();

		if (m_device == nullptr) {
			fprintf(stderr, "Failed to find device\n");
			exit(-1);
		}

		m_view_delegate = new ViewDelegate(m_device);

		CGRect frame = { { 0, 0 }, { 600, 600 } };

		m_window = NS::Window::alloc()->init(
				frame,
				NS::WindowStyleMaskClosable | NS::WindowStyleMaskTitled,
				NS::BackingStoreBuffered,
				false);

		m_view = MTK::View::alloc()->init(frame, m_device);
		m_view->setClearColor(MTL::ClearColor::Make(1, 1, 0.8, 1));
		m_view->setDelegate(m_view_delegate);

		m_window->setContentView(m_view);
		m_window->setTitle(NS::String::string("test program!", NS::UTF8StringEncoding));
		m_window->makeKeyAndOrderFront(nullptr);

		NS::Application* app = static_cast<NS::Application*>(notification->object());
		app->activateIgnoringOtherApps(true);
	}

	virtual bool applicationShouldTerminateAfterLastWindowClosed( NS::Application* pSender ) override {
		return true;
	}
};

int
main()
{
	NS::AutoreleasePool* pool = NS::AutoreleasePool::alloc()->init();

	AppDelegate del;

	NS::Application* app = NS::Application::sharedApplication();
	app->setDelegate(&del);
	app->run();

    /*NS::Error* pError = nullptr;
	NS::URL* lib_path = NS::URL::alloc()->init(NS::String::string(
				"/Users/claytonknittel/VSCode/g2d/build/test/"
				"libg2d_unit_testing_shaders.metallib",
				NS::UTF8StringEncoding));
	MTL::Library* library = device->newLibrary(lib_path, &pError);
	lib_path->release();
	if (library == nullptr) {
		fprintf(stderr, "Failed to initialize library: %s\n",
				pError->localizedDescription()->utf8String());
		return -1;
	}

	MTL::Function* vertex_fn = library->newFunction(NS::String::string("vertex_main", NS::UTF8StringEncoding));
	MTL::Function* fragment_fn = library->newFunction(NS::String::string("fragment_main", NS::UTF8StringEncoding));

	MTL::RenderPipelineDescriptor* pipeline =
		MTL::RenderPipelineDescriptor::alloc()->init();
	pipeline->colorAttachments()->object(0)->setPixelFormat(
			MTL::PixelFormatBGRA8Unorm_sRGB);
	pipeline->setVertexFunction(vertex_fn);
	pipeline->setFragmentFunction(fragment_fn);

	MTL::RenderPipelineState* pipeline_state =
		device->newRenderPipelineState(pipeline, &pError);
	if (pipeline_state == nullptr) {
		fprintf(stderr, "Failed to initialize render pipeline state: %s\n",
				pError->localizedDescription()->utf8String());
		return -1;
	}*/

	/*pipeline_state->release();
	pipeline->release();
	fragment_fn->release();
	vertex_fn->release();
	library->release();*/
	pool->release();

	return 0;
}

