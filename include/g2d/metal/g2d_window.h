#ifndef _G2D_WINDOW_H
#define _G2D_WINDOW_H

namespace g2d {

namespace metal {

class MetalWindow {
 public:
  MetalWindow(float w, float h, const char* title);

  void start();

 private:
  void* objc_window_;
};

} /* namespace metal */

} /* namespace g2d */

#endif /* _G2D_WINDOW_H */
