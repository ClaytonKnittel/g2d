
#include <simd/simd.h>
#include <sys/stat.h>

#include <g2d/metal/metal.h>
#include "test_shader_source.h"

class Renderer
{
private:
	MTL::Device* m_device;
	MTL::CommandQueue* m_command_queue;
	MTL::RenderPipelineState* m_pipeline_state;
	MTL::Library* m_library;
	MTL::Buffer* m_vertex_buf;
	MTL::Buffer* m_color_buf;
	MTL::Buffer* m_arg_buf;

	void getShaders() {
		NS::Error* err = nullptr;

		dispatch_data_t data = dispatch_data_create(g_shader_source,
				g_shader_source_size, nullptr, ^void(void) {});
		m_library = m_device->newLibrary(data, &err);
		if (m_library == nullptr) {
			printf("%s\n", err->localizedDescription()->utf8String());
		}

		MTL::Function* vertex_fn = m_library->newFunction(NS::String::string("rayVertex",
					NS::UTF8StringEncoding));
		MTL::Function* fragment_fn = m_library->newFunction(NS::String::string("rayFragment",
					NS::UTF8StringEncoding));

		if (vertex_fn == nullptr) {
			printf("couldn't find rayVertex\n");
			exit(-1);
		}
		if (fragment_fn == nullptr) {
			printf("couldn't find rayFragment\n");
			exit(-1);
		}

		MTL::RenderPipelineDescriptor* desc = MTL::RenderPipelineDescriptor::alloc()->init();
		desc->setVertexFunction(vertex_fn);
		desc->setFragmentFunction(fragment_fn);
		desc->colorAttachments()->object(0)->setPixelFormat(MTL::PixelFormat::PixelFormatBGRA8Unorm_sRGB);

		m_pipeline_state = m_device->newRenderPipelineState(desc, &err);
		if (m_pipeline_state == nullptr) {
			printf("%s", err->localizedDescription()->utf8String());
		}

		desc->release();
		fragment_fn->release();
		vertex_fn->release();
	}

	void buildBuffers() {
		const size_t NumVertices = 3;

		simd::float3 positions[NumVertices] =
		{
			{ -0.8f,  0.8f, 0.0f },
			{  0.0f, -0.8f, 0.0f },
			{ +0.8f,  0.8f, 0.0f }
		};

		simd::float3 colors[NumVertices] =
		{
			{  1.0, 0.3f, 0.2f },
			{  0.8f, 1.0, 0.0f },
			{  0.8f, 0.0f, 1.0 }
		};

		m_vertex_buf = m_device->newBuffer(sizeof(positions), MTL::ResourceStorageModeManaged);
		m_color_buf = m_device->newBuffer(sizeof(colors), MTL::ResourceStorageModeManaged);

		memcpy(m_vertex_buf->contents(), positions, sizeof(positions));
		memcpy(m_color_buf->contents(), colors, sizeof(colors));

		m_vertex_buf->didModifyRange(NS::Range::Make(0, m_vertex_buf->length()));
		m_color_buf->didModifyRange(NS::Range::Make(0, m_color_buf->length()));

		MTL::Function* vertex_fn = m_library->newFunction(NS::String::string("rayVertex",
					NS::UTF8StringEncoding));
		MTL::ArgumentEncoder* arg_encoder = vertex_fn->newArgumentEncoder(0);
		m_arg_buf = m_device->newBuffer(arg_encoder->encodedLength(), MTL::ResourceStorageModeManaged);
		if (m_arg_buf == nullptr) {
			printf("Failed to initialize arg buf\n");
			exit(-1);
		}

		arg_encoder->setArgumentBuffer(m_arg_buf, 0);
		arg_encoder->setBuffer(m_vertex_buf, 0, 0);
		arg_encoder->setBuffer(m_color_buf, 0, 1);

		m_arg_buf->didModifyRange(NS::Range::Make(0, m_arg_buf->length()));

		arg_encoder->release();
		vertex_fn->release();
	}

public:
	Renderer(MTL::Device* device) : m_device(device)
	{
		m_command_queue = device->newCommandQueue();
		getShaders();
		buildBuffers();
	}

	~Renderer()
	{
		m_arg_buf->release();
		m_vertex_buf->release();
		m_color_buf->release();
		m_library->release();
		m_pipeline_state->release();
		m_command_queue->release();
		m_device->release();
	}

	void Render(MTK::View* view) {
    	NS::AutoreleasePool* _pool = NS::AutoreleasePool::alloc()->init();

		MTL::CommandBuffer* cmd_buffer = m_command_queue->commandBuffer();
		MTL::RenderPassDescriptor* render_pass =
			view->currentRenderPassDescriptor();
		MTL::RenderCommandEncoder* encoder =
			cmd_buffer->renderCommandEncoder(render_pass);

		/*static uint64_t _cnt = 0;
		uint64_t cnt = ++_cnt;
		double val = sin((double) cnt / 100.);
		((float*) m_color_buf->contents())[0] = (float) val;
		m_color_buf->didModifyRange( NS::Range::Make( 0, sizeof(float) ) );*/

		encoder->setRenderPipelineState(m_pipeline_state);
		encoder->setVertexBuffer(m_arg_buf, 0, 0);
		encoder->useResource(m_vertex_buf, MTL::ResourceUsageRead);
		encoder->useResource(m_color_buf, MTL::ResourceUsageRead);
		encoder->drawPrimitives(MTL::PrimitiveType::PrimitiveTypeTriangle, NS::UInteger(0), NS::UInteger(3));

		encoder->endEncoding();
		cmd_buffer->presentDrawable(view->currentDrawable());
		cmd_buffer->commit();

		_pool->release();
	}
};

class ViewDelegate : public MTK::ViewDelegate
{
private:
	Renderer renderer;

public:
	ViewDelegate(MTL::Device* device) : renderer(device->retain()) {
	}

	virtual ~ViewDelegate() override {
	}

	virtual void drawInMTKView(MTK::View* view) override {
		renderer.Render(view);
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
		m_view->setColorPixelFormat(MTL::PixelFormat::PixelFormatBGRA8Unorm_sRGB);
		printf("target framerate: %d\n", m_view->preferredFramesPerSecond());
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

	pool->release();

	return 0;
}

