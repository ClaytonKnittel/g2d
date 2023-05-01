#pragma once

namespace g2d {

class Object {
 public:
  Object() = default;

  Object(const Object&) = delete;
  Object& operator=(const Object&) = delete;
};

}  // namespace g2d