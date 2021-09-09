#pragma once

#include <vector>

enum class status { FAILED = 0, PASSED, RUNNING };

// a basic behavior tree
// TODO: ADD DECORATOR NODES
struct behavior_tree {
  struct node {
    virtual status run() = 0;
  };

  struct composite_node : public node {
    void add_child(node* n) { children.push_back(n); }
    const std::vector<node*>& get_children() const { return children; }

   private:
    std::vector<node*> children;
  };

  struct selector_node : public composite_node {
    virtual status run() override {
      for (auto child : get_children()) {
        status current = child->run();
        if (current == status::PASSED || current == status::RUNNING) {
          return current;
        }
      }
      return status::FAILED;
    }
  };

  struct sequence_node : public composite_node {
    virtual status run() override {
      for (auto child : get_children()) {
        status current = child->run();
        if (current == status::FAILED || current == status::RUNNING) {
          return current;
        }
      }
      return status::PASSED;
    }
  };

  class decorator_node : public node {
    node* child;

   protected:
    node* get_child() const { return child; }

   public:
    void set_child(node* n) { child = n; }
  };

  class root_node : public decorator_node {
    friend class behavior_tree;
    virtual status run() {
      if (get_child()) {
        return get_child()->run();
      }
      return status::FAILED;
    }
  };

  class inverter_node : public decorator_node {
    virtual status run() override {
      const status temp = get_child()->run();
      if (temp == status::RUNNING) {
        return temp;
      }

      if (temp == status::PASSED) {
        return status::FAILED;
      }

      return status::PASSED;
    }
  };

  behavior_tree();
  ~behavior_tree();

 private:
  root_node* root;

 public:
  void set_root_child(node* n) const { root->set_child(n); }
  status run() const { return root->run(); }
};

behavior_tree::behavior_tree() : root(new root_node) {}

behavior_tree::~behavior_tree() {}