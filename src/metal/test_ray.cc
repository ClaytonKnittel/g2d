
#include <simd/simd.h>
#include <sys/stat.h>

#include <g2d/g2d.h>
#include <g2d/metal/metal.h>
#include <g2d/metal/metal_shader_lib_source.h>

namespace shader_types
{
    struct VertexData
    {
        simd::float2 position;
        simd::float2 texcoord;
    };

    struct InstanceData
    {
        simd::float3x3 instanceTransform;
    };
}

class Renderer
{
private:
	static constexpr const uint32_t MAX_FRAMES_IN_FLIGHT = 3;
	static constexpr const uint32_t NUM_INSTANCES = 32;

	MTL::Device* m_device;
	MTL::CommandQueue* m_command_queue;
	MTL::RenderPipelineState* m_pipeline_state;
	MTL::Library* m_library;
	MTL::Texture* m_texture;
	MTL::Buffer* m_vertex_buf;
	MTL::Buffer* m_v_index_buf;
	MTL::Buffer* m_instance_buf[MAX_FRAMES_IN_FLIGHT];
	//MTL::Buffer* m_arg_buf;
	uint64_t m_frame_idx;

	void getShaders() {
		NS::Error* err = nullptr;

		dispatch_data_t data = dispatch_data_create(g_metal_shader_lib_source,
				g_metal_shader_lib_source_size, nullptr, ^void(void) {});
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

	void buildTextures()
	{
		const uint32_t tw = 9;
		const uint32_t th = 9;

		MTL::TextureDescriptor* texture_desc = MTL::TextureDescriptor::alloc()->init();
		texture_desc->setWidth(tw);
		texture_desc->setHeight(th);
		texture_desc->setPixelFormat(MTL::PixelFormatRGBA8Unorm);
		texture_desc->setTextureType(MTL::TextureType2D);
		texture_desc->setStorageMode(MTL::StorageModeManaged);
		texture_desc->setUsage(MTL::ResourceUsageSample | MTL::ResourceUsageRead);

		m_texture = m_device->newTexture(texture_desc);
		texture_desc->release();

		uint8_t* texture_data = (uint8_t*) malloc(tw * th * 4);
		for (uint64_t y = 0; y < th; y++) {
			for (uint64_t x = 0; x < tw; x++) {
				float dx = 2 * ((float) x - (float) (tw - 1) / 2.f) / (tw - 1);
				float dy = 2 * ((float) y - (float) (th - 1) / 2.f) / (th - 1);

				float dc = tanhf(dx * dx + dy * dy);
				uint8_t dc_norm = (uint8_t) (dc * 0xff);

				texture_data[4 * (x + tw * y) + 0] = 0x30;
				texture_data[4 * (x + tw * y) + 1] = dc_norm;
				texture_data[4 * (x + tw * y) + 2] = 1 - dc_norm;
				texture_data[4 * (x + tw * y) + 3] = 0xff;
			}
		}

		m_texture->replaceRegion(MTL::Region(0, 0, tw, th), 0, texture_data, tw * 4);

		free(texture_data);
	}

	void buildBuffers() {
		const float s = 0.5f;

		shader_types::VertexData verts[] =
		{
			// Positions   Tex Coords
			{ { -s, -s }, { 0.f, 0.f } },
			{ { +s, -s }, { 1.f, 0.f } },
			{ { +s, +s }, { 1.f, 1.f } },
			{ { -s, +s }, { 0.f, 1.f } }
		};

		uint16_t v_index[] = {
			0, 1, 2,
			2, 3, 0
		};

		m_vertex_buf = m_device->newBuffer(sizeof(verts), MTL::ResourceStorageModeManaged);
		m_v_index_buf = m_device->newBuffer(sizeof(v_index), MTL::ResourceStorageModeManaged);

		memcpy(m_vertex_buf->contents(), verts, sizeof(verts));
		memcpy(m_v_index_buf->contents(), v_index, sizeof(v_index));

		m_vertex_buf->didModifyRange(NS::Range::Make(0, m_vertex_buf->length()));
		m_v_index_buf->didModifyRange(NS::Range::Make(0, m_v_index_buf->length()));

		const size_t instance_buf_len = NUM_INSTANCES * sizeof(shader_types::InstanceData);

		for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			m_instance_buf[i] = m_device->newBuffer(instance_buf_len,
					MTL::ResourceStorageModeManaged);
		}

		/*MTL::Function* vertex_fn = m_library->newFunction(NS::String::string("rayVertex",
					NS::UTF8StringEncoding));
		MTL::ArgumentEncoder* arg_encoder = vertex_fn->newArgumentEncoder(0);
		m_arg_buf = m_device->newBuffer(arg_encoder->encodedLength(), MTL::ResourceStorageModeManaged);
		if (m_arg_buf == nullptr) {
			printf("Failed to initialize arg buf\n");
			exit(-1);
		}

		arg_encoder->setArgumentBuffer(m_arg_buf, 0);
		arg_encoder->setBuffer(m_vertex_buf, 0, 0);
		arg_encoder->setBuffer(m_v_index_buf, 0, 1);

		m_arg_buf->didModifyRange(NS::Range::Make(0, m_arg_buf->length()));

		arg_encoder->release();
		vertex_fn->release();*/
	}

public:
	Renderer(MTL::Device* device) : m_device(device), m_frame_idx(0)
	{
		m_command_queue = device->newCommandQueue();
		getShaders();
		buildTextures();
		buildBuffers();
	}

	~Renderer()
	{
		//m_arg_buf->release();
		m_vertex_buf->release();
		m_v_index_buf->release();
		for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			m_instance_buf[i]->release();
		}
		m_texture->release();
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

		uint64_t frame = m_frame_idx++;
		MTL::Buffer* instance_buf_ptr = m_instance_buf[frame % MAX_FRAMES_IN_FLIGHT];

		const float scl = 2.f / NUM_INSTANCES;
		shader_types::InstanceData* instance_buf =
			reinterpret_cast<shader_types::InstanceData*>(instance_buf_ptr->contents());
		for (uint64_t i = 0; i < NUM_INSTANCES; i++) {
			float angle = 4 * (float) i / (float) NUM_INSTANCES * 2.f * M_PI + (float) frame / 11.3f;
			float frac = (float) i / (float) NUM_INSTANCES;
			float x = (frac * 2.f - 1.f) + (1.f / (float) NUM_INSTANCES);
			float y = sinf((frac + (float) frame / 200.f) * 2.f * M_PI);

			instance_buf[i].instanceTransform = (simd::float3x3) {
				(simd::float3) { scl * sinf(angle),  scl * cosf(angle), 0.f },
				(simd::float3) { scl * cosf(angle), -scl * sinf(angle), 0.f },
				(simd::float3) { x,                  y,                 1.f },
			};
		}
		instance_buf_ptr->didModifyRange(NS::Range::Make(0, instance_buf_ptr->length()));

		encoder->setRenderPipelineState(m_pipeline_state);
		encoder->setVertexBuffer(m_vertex_buf, 0, 0);
		encoder->setVertexBuffer(instance_buf_ptr, 0, 1);
		encoder->setFragmentTexture(m_texture, 0);
		//encoder->setVertexBuffer(m_arg_buf, 0, 0);
		//encoder->useResource(m_vertex_buf, MTL::ResourceUsageRead);
		//encoder->useResource(m_color_buf, MTL::ResourceUsageRead);

		//
		// void drawIndexedPrimitives( PrimitiveType primitiveType,
		//                             NS::UInteger indexCount,
		//                             IndexType indexType,
		//                             const class Buffer* pIndexBuffer,
		//                             NS::UInteger indexBufferOffset,
		//                             NS::UInteger instanceCount );
		encoder->drawIndexedPrimitives(MTL::PrimitiveType::PrimitiveTypeTriangle,
				NS::UInteger(6),
				MTL::IndexType::IndexTypeUInt16,
				m_v_index_buf,
				NS::UInteger(0),
				NS::UInteger(NUM_INSTANCES));

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
call_test_ray()
{
	NS::AutoreleasePool* pool = NS::AutoreleasePool::alloc()->init();

	AppDelegate del;

	NS::Application* app = NS::Application::sharedApplication();
	app->setDelegate(&del);
	app->run();

	pool->release();

	return 0;
}

