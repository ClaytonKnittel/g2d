
#include <cassert>
#include <iostream>

#ifdef USE_OPENGL
#include <g2d/opengl/gl.h>
#elif USE_METAL
#include <g2d/metal/metal.h>
#endif

#include <check.h>

#if 0
class metal_adder{
public:

    void init_with_device(MTL::Device* device);
    void prepare_data();
    void send_compute_command();
    void random_number_generator(MTL::Buffer* buffer);
    void encode_add_command(MTL::ComputeCommandEncoder* compute_encoder);
    void verify();


private:
    MTL::Buffer* _A;
    MTL::Buffer* _B;
    MTL::Buffer* _C;

    MTL::Device* _device;
    MTL::CommandQueue* _CommandQueue;
    MTL::ComputePipelineState* _addFunctionPSO;

};

const unsigned int vector_length = 1u << 10;
auto buffer_size = vector_length*sizeof(float);

void metal_adder::random_number_generator(MTL::Buffer *buffer){
    float* data_ptr = (float*)buffer->contents();
        for (unsigned long index = 0; index < vector_length; index++){
            data_ptr[index] = (float)rand() / (float)(RAND_MAX);
        }
}

void metal_adder::encode_add_command(MTL::ComputeCommandEncoder* compute_encoder){
    compute_encoder->setComputePipelineState(_addFunctionPSO);

        compute_encoder->setBuffer(_A, 0, 0);
        compute_encoder->setBuffer(_B, 0, 1);
        compute_encoder->setBuffer(_C, 0, 2);

        MTL::Size grid_size = MTL::Size(vector_length, 1, 1);

        NS::UInteger _thread_group_size = _addFunctionPSO->maxTotalThreadsPerThreadgroup();
        if(_thread_group_size > vector_length){
            _thread_group_size = vector_length;
        }

        MTL::Size thread_group_size = MTL::Size(_thread_group_size, 1, 1);

        compute_encoder->dispatchThreads(grid_size, thread_group_size);
}

void metal_adder::init_with_device(MTL::Device *device){

    NS::Error* error;
    this->_device = device;

	const NS::String* lib_path = NS::String::string("/Users/ClaytonKnittel/Documents/g2d/build/test/libg2d_unit_testing_shaders.metallib",
			NS::StringEncoding::ASCIIStringEncoding);
    auto lib = _device->newLibrary(lib_path, &error);
    if(!lib){
        std::cerr << "Failed to load Default Library\n";
        std::exit(-1);
    }

    auto function =NS::String::string("add_vector", NS::ASCIIStringEncoding);
    auto mtl_function = lib->newFunction(function);
    if(!mtl_function){
        std::cerr<<"failed to load kernel\n";
        std::exit(-1);
    }

    _addFunctionPSO = _device->newComputePipelineState(mtl_function, &error);
    _CommandQueue   = _device->newCommandQueue();

}


void metal_adder::prepare_data(){

    _A = _device->newBuffer(buffer_size, MTL::ResourceStorageModeShared);
    _B = _device->newBuffer(buffer_size, MTL::ResourceStorageModeShared);
    _C = _device->newBuffer(buffer_size, MTL::ResourceStorageModeShared);

    random_number_generator(_A);
    random_number_generator(_B);
}

void metal_adder::send_compute_command(){
    MTL::CommandBuffer* command_buffer = _CommandQueue->commandBuffer();
    MTL::ComputeCommandEncoder* compute_encoder = command_buffer->computeCommandEncoder();

    encode_add_command(compute_encoder);
    compute_encoder->endEncoding();
    command_buffer->commit();
    command_buffer->waitUntilCompleted();

    verify();
}

void metal_adder::verify(){
    auto a = (float*)_A->contents();
    auto b = (float*)_B->contents();
    auto c = (float*)_C->contents();

    for(unsigned long i = 0; i < vector_length; ++i){
        if( c[i] != (a[i] + b[i]) ){
             std::cout << "\033[1;31m TEST FAILED \033[0m\n" ;}
    }
            std::cout << "\033[1;32m TEST PASSED \033[0m\n" ;

}

int
main()
{
#ifdef USE_OPENGL
	gl_context_t ctx;
	gl_init(&ctx, 800, 600);

	gl_set_bg_color(0xffd3b6ff);

	while (!gl_should_exit(&ctx)) {
		gl_clear(&ctx);

		gl_render(&ctx);
	}

	gl_exit(&ctx);

#elif USE_METAL
	NS::AutoreleasePool* p_pool = NS::AutoreleasePool::alloc()->init();
    MTL::Device* device = MTL::CreateSystemDefaultDevice();

    metal_adder* adder = new metal_adder();
    adder->init_with_device(device);
    adder->prepare_data();
    adder->send_compute_command();

    std::cout << " End of Computation  " << std::endl;
    p_pool->release();

#endif

	return 0;
}
#endif


#pragma region Declarations {

class Renderer
{
    public:
        Renderer( MTL::Device* pDevice );
        ~Renderer();
        void draw( MTK::View* pView );

    private:
        MTL::Device* _pDevice;
        MTL::CommandQueue* _pCommandQueue;
};

class MyMTKViewDelegate : public MTK::ViewDelegate
{
    public:
        MyMTKViewDelegate( MTL::Device* pDevice );
        virtual ~MyMTKViewDelegate() override;
        virtual void drawInMTKView( MTK::View* pView ) override;

    private:
        Renderer* _pRenderer;
};

class MyAppDelegate : public NS::ApplicationDelegate
{
    public:
        ~MyAppDelegate();

        NS::Menu* createMenuBar();

        virtual void applicationWillFinishLaunching( NS::Notification* pNotification ) override;
        virtual void applicationDidFinishLaunching( NS::Notification* pNotification ) override;
        virtual bool applicationShouldTerminateAfterLastWindowClosed( NS::Application* pSender ) override;

    private:
        NS::Window* _pWindow;
        MTK::View* _pMtkView;
        MTL::Device* _pDevice;
        MyMTKViewDelegate* _pViewDelegate = nullptr;
};

#pragma endregion Declarations }


int main( int argc, char* argv[] )
{
    NS::AutoreleasePool* pAutoreleasePool = NS::AutoreleasePool::alloc()->init();

    MyAppDelegate del;

    NS::Application* pSharedApplication = NS::Application::sharedApplication();
    pSharedApplication->setDelegate( &del );
    pSharedApplication->run();

    pAutoreleasePool->release();

    return 0;
}


#pragma mark - AppDelegate
#pragma region AppDelegate {

MyAppDelegate::~MyAppDelegate()
{
    _pMtkView->release();
    _pWindow->release();
    _pDevice->release();
    delete _pViewDelegate;
}

NS::Menu* MyAppDelegate::createMenuBar()
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

void MyAppDelegate::applicationWillFinishLaunching( NS::Notification* pNotification )
{
    NS::Menu* pMenu = createMenuBar();
    NS::Application* pApp = reinterpret_cast< NS::Application* >( pNotification->object() );
    pApp->setMainMenu( pMenu );
    pApp->setActivationPolicy( NS::ActivationPolicy::ActivationPolicyRegular );
}

void MyAppDelegate::applicationDidFinishLaunching( NS::Notification* pNotification )
{
    CGRect frame = (CGRect){ {100.0, 100.0}, {512.0, 512.0} };

    _pWindow = NS::Window::alloc()->init(
        frame,
        NS::WindowStyleMaskClosable|NS::WindowStyleMaskTitled,
        NS::BackingStoreBuffered,
        false );

    _pDevice = MTL::CreateSystemDefaultDevice();

    _pMtkView = MTK::View::alloc()->init( frame, _pDevice );
    _pMtkView->setColorPixelFormat( MTL::PixelFormat::PixelFormatBGRA8Unorm_sRGB );
    _pMtkView->setClearColor( MTL::ClearColor::Make( 1.0, 0.0, 0.0, 1.0 ) );

    _pViewDelegate = new MyMTKViewDelegate( _pDevice );
    _pMtkView->setDelegate( _pViewDelegate );

    _pWindow->setContentView( _pMtkView );
    _pWindow->setTitle( NS::String::string( "00 - Window", NS::StringEncoding::UTF8StringEncoding ) );

    _pWindow->makeKeyAndOrderFront( nullptr );

    NS::Application* pApp = reinterpret_cast< NS::Application* >( pNotification->object() );
    pApp->activateIgnoringOtherApps( true );
}

bool MyAppDelegate::applicationShouldTerminateAfterLastWindowClosed( NS::Application* pSender )
{
    return true;
}

#pragma endregion AppDelegate }


#pragma mark - ViewDelegate
#pragma region ViewDelegate {

MyMTKViewDelegate::MyMTKViewDelegate( MTL::Device* pDevice )
: MTK::ViewDelegate()
, _pRenderer( new Renderer( pDevice ) )
{
}

MyMTKViewDelegate::~MyMTKViewDelegate()
{
    delete _pRenderer;
}

void MyMTKViewDelegate::drawInMTKView( MTK::View* pView )
{
    _pRenderer->draw( pView );
}

#pragma endregion ViewDelegate }


#pragma mark - Renderer
#pragma region Renderer {

Renderer::Renderer( MTL::Device* pDevice )
: _pDevice( pDevice->retain() )
{
    _pCommandQueue = _pDevice->newCommandQueue();
}

Renderer::~Renderer()
{
    _pCommandQueue->release();
    _pDevice->release();
}

void Renderer::draw( MTK::View* pView )
{
    NS::AutoreleasePool* pPool = NS::AutoreleasePool::alloc()->init();

    MTL::CommandBuffer* pCmd = _pCommandQueue->commandBuffer();
    MTL::RenderPassDescriptor* pRpd = pView->currentRenderPassDescriptor();
    MTL::RenderCommandEncoder* pEnc = pCmd->renderCommandEncoder( pRpd );
    pEnc->endEncoding();
    pCmd->presentDrawable( pView->currentDrawable() );
    pCmd->commit();

    pPool->release();
}

#pragma endregion Renderer }

