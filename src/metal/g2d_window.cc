
#include <g2d/metal/g2d_window.h>

extern void* allocAndInitG2DWindow(
    void* self_ptr, float w, float h,
    const char* title) __asm__("_allocAndInitG2DWindow");
extern void startMetalExecution(void* g2d_window) __asm__(
    "_startMetalExecution");
extern void registerRenderCallbackObjc(
    void* g2d_window, g2d::metal::RenderCallbackFn
                          callback) __asm__("_registerRenderCallbackObjc");

namespace g2d {

namespace metal {

Window::Window(float w, float h, const char* title)
    : objc_window_(allocAndInitG2DWindow(this, w, h, title)) {}

void Window::start() {
  startMetalExecution(objc_window_);
}

void* Window::objc_window() {
  return objc_window_;
}

MTLBuffer Window::newBuffer(std::size_t length) {
  return MTLBuffer(*this, length);
}

void Window::registerRenderCallback(RenderCallbackFn callback) {
  registerRenderCallbackObjc(objc_window_, callback);
}

} /* namespace metal */
} /* namespace g2d */
