#ifndef _G2D_WINDOW_H
#define _G2D_WINDOW_H

#include <g2d/metal/mtl_buffer.h>

namespace g2d {

namespace metal {

class Window {
 public:
  Window(float w, float h, const char* title);

  void start();

  void* objc_window();

  MTLBuffer newBuffer(std::size_t length);

 private:
  void* objc_window_;
};

} /* namespace metal */

} /* namespace g2d */

#endif /* _G2D_WINDOW_H */
