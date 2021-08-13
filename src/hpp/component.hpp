#pragma once

#include <liblava/lava.hpp>

#include "object.hpp"

namespace crow {
// forward declare for Object class due to co-dependency
class Object;

enum class Component_Type { DRAWABLE = 0, PLAYER, BOX_COLLISION, SPHERE_COLLISION, };

struct Component {
  // back pointer to the parent Object of this component (very useful)
  Object* parent;

  // virtual update function, must be included for all components
  virtual void update(const lava::app* const _app, std::vector<Object>* _objects) = 0;
  // used for differentiating different types of components
  virtual Component_Type get_type() const = 0;
};


}  // namespace crow