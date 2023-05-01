#pragma once

#include <cstdint>
#include <string>

#ifdef USE_OPENGL
#include <g2d/opengl/gl.h>
#elif defined(USE_METAL)
#include <g2d/metal/g2d_window.h>
#endif

namespace g2d {

class Context {
 private:
  uint32_t width;
  uint32_t height;

  uint32_t target_framerate;

  // color_t background_color;

  std::string title;
};

}  // namespace g2d
