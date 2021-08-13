#pragma once

#include "component.hpp"

namespace crow {

struct Component_Box_Collision : Component {
  glm::vec3 position;
  glm::vec3 size;

  // quick methods to obtain the bounds of the aabb

  float top() const { return position.y + (size.y / 2); }
  float bottom() const { return position.y - (size.y / 2); }
  float right() const { return position.x + (size.x / 2); }
  float left() const { return position.x - (size.x / 2); }
  float front() const { return position.z + (size.z / 2); }  // towards the camera
  float back() const {  return position.z - (size.z / 2); }  // away from the camera


  // begin collision checking methods

  // checks collision with another AABB (takes a pointer)
  bool check_collision(const Component_Box_Collision* const o) const {
    return left() < o->right() && right() > o->left() && bottom() < o->top() &&
           top() > o->bottom() && back() < o->front() && front() > o->back();
  }

  // end collision checking methods

  
  inline Component_Type get_type() const { return Component_Type::BOX_COLLISION; }
  // does nothing
  void update(const lava::app* const _app, std::vector<Object>* _objects) {}
};
}  // namespace crow