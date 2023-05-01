#pragma once

#include <cstddef>

namespace g2d {

namespace metal {

// Forward-declare Window.
class Window;

class MTLBuffer {
  friend class g2d::metal::Window;

 public:
  ~MTLBuffer();

  // Disable copy constructor/assignment.
  MTLBuffer(const MTLBuffer&) = delete;
  MTLBuffer& operator=(const MTLBuffer&) = delete;

  void write(std::size_t pos, void* bytes, std::size_t len);

 private:
  // TODO add options
  MTLBuffer(Window& window, std::size_t length);

  // Pointer to Obj-C MTL::Buffer object
  void* mtl_buffer_;
};

}  // namespace metal

}  // namespace g2d