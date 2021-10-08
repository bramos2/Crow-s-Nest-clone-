#pragma once

#include <vector>

enum class status { FAILED = 0, PASSED, RUNNING };

// a basic behavior tree
// TODO: ADD DECORATOR NODES
struct behavior_tree {
  struct node {
    virtual status run() = 0;
  };

  struct composite_node : public behavior_tree::node {
    void add_child(behavior_tree::node* n);
    const std::vector<behavior_tree::node*>& get_children() const;

   private:
    std::vector<behavior_tree::node*> children;
  };

  struct selector_node : public behavior_tree::composite_node {
    virtual status run() override;
  };

  struct sequence_node : public behavior_tree::composite_node {
    virtual status run() override;
  };

  class decorator_node : public behavior_tree::node {
    behavior_tree::node* child;

   protected:
    behavior_tree::node* get_child() const;

   public:
    void set_child(behavior_tree::node* n);
  };

  class root_node : public behavior_tree::decorator_node {
    friend class behavior_tree;
    virtual status run();
  };

  class inverter_node : public behavior_tree::decorator_node {
    virtual status run() override;
  };

  behavior_tree();
  ~behavior_tree();

 private:
  behavior_tree::root_node* root;

 public:
  void set_root_child(behavior_tree::node* n) const;
  status run() const;
};
