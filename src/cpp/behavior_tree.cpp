#include "../hpp/behavior_tree.hpp"

#include <functional>

namespace crow {

void behavior_tree::composite_node::add_child(node* n) {
  children.push_back(n);
}

const std::vector<behavior_tree::node*>&
behavior_tree::composite_node::get_children() const {
  return children;
}

status behavior_tree::selector_node::run(float dt, crow::ai_manager& m) {
  for (auto child : get_children()) {
    status current = child->run(dt, m);
    if (current == status::PASSED || current == status::RUNNING) {
      return current;
    }
  }
  return status::FAILED;
}

status behavior_tree::sequence_node::run(float dt, crow::ai_manager& m) {
  for (auto child : get_children()) {
    status current = child->run(dt, m);
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

status behavior_tree::root_node::run(float dt, crow::ai_manager& m) {
  if (get_child()) {
    return get_child()->run(dt, m);
  }
  return status::FAILED;
}

status behavior_tree::inverter_node::run(float dt, crow::ai_manager& m) {
  const status temp = get_child()->run(dt, m);
  if (temp == status::RUNNING) {
    return temp;
  }

  if (temp == status::PASSED) {
    return status::FAILED;
  }

  return status::PASSED;
}

behavior_tree::behavior_tree() : root(new root_node) {}

behavior_tree::~behavior_tree() { delete root; }

void behavior_tree::build_tree() {
  // allocating nodes for the tree
  lnodes.resize(12);
  // 0 - 11 child nodes (12)
  for (size_t i = 0; i < 12; i++) {
    lnodes[i] = new leaf_node();
  }

  seqn.resize(3);
  // 12 - 14 sequence nodes (3)
  for (size_t i = 0; i < 3; i++) {
    seqn[i] = new sequence_node();
  }

  seln.resize(4);
  // 15 - 18 selector nodes (4)
  for (size_t i = 0; i < 4; i++) {
    seln[i] = new selector_node();
  }

  // assigning behaviors to leaf nodes
  {
    unsigned int c = 0;
    lnodes[c++]->r = &crow::has_path;
    lnodes[c++]->r = &crow::target_player;
    lnodes[c++]->r = &crow::target_console;
    lnodes[c++]->r = &crow::target_door;

    lnodes[c++]->r = &crow::has_path;
    lnodes[c++]->r = &crow::is_path_currernt;
    lnodes[c++]->r = &crow::get_path;

    lnodes[c++]->r = &crow::reached_target;
    lnodes[c++]->r = &crow::move;

    lnodes[c++]->r = &crow::is_target_door;
    lnodes[c++]->r = &crow::handle_door;
    lnodes[c++]->r = &crow::destroy_target;
  }

  // connecting nodes to form tree
  // tree diagram
  // vector index = num - 1
  //           root
  //             |
  //            (13)seq
  //    |        |        |        |
  // (16)sel  (17)sel  (18)sel  (19)sel
  // | | | |    |    |   | |    |      |
  // 1 2 3 4 (14)seq 7   8 9   (15)seq 12
  //            | |             | |
  //            5 6             10 11

  {
    unsigned int c = 0;
    unsigned int sel = 0;
    unsigned int seq = 0;

    set_root_child(seqn[seq]);
    for (size_t i = 0; i < 4; i++) {
      seqn[seq]->add_child(seln[i]);
    }
    seq++;

    // first selector
    for (size_t i = 0; i < 4; i++) {
      seln[sel]->add_child(lnodes[c++]);
    }
    sel++;

    // second selector
    seln[sel]->add_child(seqn[seq]);
    for (size_t i = 0; i < 2; i++) {
      seqn[seq]->add_child(lnodes[c++]);
    }
    seq++;
    seln[sel++]->add_child(lnodes[c++]);

    // 3rd selector
    for (size_t i = 0; i < 2; i++) {
      seln[sel]->add_child(lnodes[c++]);
    }
    sel++;

    // 4th selector
    seln[sel]->add_child(seqn[seq]);
    for (size_t i = 0; i < 2; i++) {
      seqn[seq]->add_child(lnodes[c++]);
    }
    seq++;
    seln[sel++]->add_child(lnodes[c++]);
  }
}

void behavior_tree::clean_tree() {
  for (auto& n : lnodes) {
    if (n) {
      delete n;
    }
  }

  for (auto& n : seln) {
    if (n) {
      delete n;
    }
  }

  for (auto& n : seqn) {
    if (n) {
      delete n;
    }
  }
}

void behavior_tree::set_root_child(node* n) const { root->set_child(n); }

status behavior_tree::run(float dt) const {
  status result = status::FAILED;
  if (aim) {
    result = root->run(dt, *aim);
  }
  return result;
}

status behavior_tree::leaf_node::run(float dt, crow::ai_manager& m) {
  status result = status::FAILED;
  if (r) {
    result = std::invoke(r, dt, m);
  }
  return result;
}

}  // namespace crow