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
    void add_child(node* n);
    const std::vector<node*>& get_children() const;

   private:
    std::vector<node*> children;
  };

  struct selector_node : public composite_node {
    virtual status run() override;
  };

  struct sequence_node : public composite_node {
    virtual status run() override;
  };

  class decorator_node : public node {
    node* child;

   protected:
    node* get_child() const;

   public:
    void set_child(node* n);
  };

  class root_node : public decorator_node {
    friend class behavior_tree;
    virtual status run();
  };

  class inverter_node : public decorator_node {
    virtual status run() override;
  };

  behavior_tree();
  ~behavior_tree();

 private:
  root_node* root;

 public:
  void set_root_child(node* n) const;
  status run() const;
};

