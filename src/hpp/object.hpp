#pragma once

#include <liblava/lava.hpp>

#include "component.hpp"

namespace crow {
// forward declare for Component struct due to co-dependency
struct Component;
// another forward declare for similar reasons
enum class Component_Type;

class Object {
 public:
  // all components hosted by this Object 
  std::vector<crow::Component*> components;

  // returns the first component found of the specified type (will need to re-cast the pointer after getting)
  Component* get_component(crow::Component_Type t);
  // returns the nth component found of the specified type (will need to re-cast the pointer after getting)
  // note: if n is negative, then it will check from the back rather than front of the array
  Component* get_component(crow::Component_Type t, int n);
  ~Object();
};

}  // namespace crow