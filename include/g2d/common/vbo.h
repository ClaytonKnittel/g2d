#pragma once

#include <cstddef>

namespace g2d {

class VBO {
 public:
  VBO();

  void write(std::size_t pos, void* bytes, std::size_t len);

 private:
#ifdef USE_METAL
  metal::MTLBuffer buffer_;
#else
  opengl::VBO buffer_;
#endif
};

}  // namespace g2d
