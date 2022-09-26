
#include <g2d/metal/g2d_window.h>

extern void* allocAndInitG2DWindow(float w, float h, const char* title) __asm__(
    "_allocAndInitG2DWindow");
extern void startMetalExecution(void* g2d_window) __asm__(
    "_startMetalExecution");

namespace g2d {

namespace metal {

MetalWindow::MetalWindow(float w, float h, const char* title)
    : objc_window_(allocAndInitG2DWindow(w, h, title)) {}

void MetalWindow::start() {
  startMetalExecution(objc_window_);
}

} /* namespace metal */
} /* namespace g2d */
