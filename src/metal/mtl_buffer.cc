#include <g2d/metal/g2d_window.h>
#include <g2d/metal/mtl_buffer.h>

extern void* newBufferWithLength(void* g2d_window, std::size_t length) __asm__(
    "_newBufferWithLength");
extern void writeToMTLBuffer(void* mtl_buffer_id, std::size_t pos, void* bytes,
                             std::size_t len) __asm__("_writeToMTLBuffer");
extern void releaseMTLBuffer(void* mtl_buffer_id) __asm__("_releaseMTLBuffer");

namespace g2d {

namespace metal {

MTLBuffer::~MTLBuffer() {
  releaseMTLBuffer(mtl_buffer_);
}

void MTLBuffer::write(std::size_t pos, void* bytes, std::size_t len) {
  writeToMTLBuffer(mtl_buffer_, pos, bytes, len);
}

MTLBuffer::MTLBuffer(Window& window, std::size_t length) {
  void* mtl_buffer = newBufferWithLength(window.objc_window(), length);
  mtl_buffer_ = mtl_buffer;
}

}  // namespace metal

}  // namespace g2d