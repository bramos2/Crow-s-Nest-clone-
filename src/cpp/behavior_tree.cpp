#include "../hpp/behavior_tree.hpp"

void behavior_tree::composite_node::add_child(node* n) {
  children.push_back(n);
}

const std::vector<behavior_tree::node*>&
behavior_tree::composite_node::get_children() const {
  return children;
}

status behavior_tree::selector_node::run() {
  for (auto child : get_children()) {
    status current = child->run();
    if (current == status::PASSED || current == status::RUNNING) {
      return current;
    }
  }
  return status::FAILED;
}

status behavior_tree::sequence_node::run() {
  for (auto child : get_children()) {
    status current = child->run();
    if (current == status::FAILED || current == status::RUNNING) {
      return current;
    }
  }
  return status::PASSED;
}

behavior_tree::node* behavior_tree::decorator_node::get_child() const {
  return child;
}

void behavior_tree::decorator_node::set_child(node* n) { child = n; }

status behavior_tree::root_node::run() {
  if (get_child()) {
    return get_child()->run();
  }
  return status::FAILED;
}

status behavior_tree::inverter_node::run() {
  const status temp = get_child()->run();
  if (temp == status::RUNNING) {
    return temp;
  }

  if (temp == status::PASSED) {
    return status::FAILED;
  }

  return status::PASSED;
}

behavior_tree::behavior_tree() : root(new root_node) {}

behavior_tree::~behavior_tree() {}

void behavior_tree::set_root_child(node* n) const { root->set_child(n); }

status behavior_tree::run() const { return root->run(); }
