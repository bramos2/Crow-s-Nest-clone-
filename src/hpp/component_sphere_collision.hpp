#pragma once

//#include "component.hpp"

namespace crow {

struct Component_Sphere_Collision : Component {
  glm::vec3 position;
  float radius;

  // begin collision checking methods

  // checks collision with another sphere (takes a pointer)
  bool check_collision(const Component_Sphere_Collision* const o) const {
    return glm::distance(position, o->position) < radius + o->radius;
  }

  // end collision checking methods

  inline Component_Type get_type() const {
    return Component_Type::SPHERE_COLLISION;
  }
  // does nothing
  void update(const lava::app* const _app, std::vector<Object>* _objects) {}
};
}  // namespace crow
