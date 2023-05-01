#pragma once

#include "g2d/common/object.h"

namespace g2d {

class ObjManager {
 public:
  ObjManager() = default;

  void insert(Object);
};

}  // namespace g2d