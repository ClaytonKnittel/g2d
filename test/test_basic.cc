
#include <iostream>

#ifdef USE_OPENGL
#include <g2d/opengl/gl.h>
#elif USE_METAL
#include <g2d/metal/metal.h>
#endif

#include <check.h>

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

