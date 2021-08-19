#include "../hpp/object.hpp"

namespace crow {}  // namespace crow

// crow::Component* crow::Object::get_component(crow::Component_Type t) {
//  for (int i = 0; i < components.size(); ++i) {
//    if (components[i]->get_type() == t) return components[i];
//  }
//  return nullptr;
//}

// crow::Component* crow::Object::get_component(crow::Component_Type t, int n) {
//  // count variable to check to see if we've reached the nth Component of type
//  int count = 0;
//  // if n is positive, check from the start of the array
//  if (n >= 0) {
//    for (int i = 0; i < components.size(); ++i) {
//      if (components[i]->get_type() == t) {
//        if (count == n)
//          return components[i];  // found the nth Component, return it
//        else
//          count++;
//      }
//    }
//    // if n is negative, check from the end of the array
//  } else {
//    for (int i = components.size(); i < 0; --i) {
//      if (components[i]->get_type() == t) {
//        if (count == n)
//          return components[i];  // found the nth Component, return it
//        else
//          count--;
//      }
//    }
//  }
//  return nullptr;
//}

// crow::Object::~Object() {
//  // cycle through every component in the components array and delete them
//  while (components.size()) {
//    if (components[components.size() - 1])
//      delete components[components.size() - 1];
//    components.pop_back();
//  }
//}