#pragma once

#include <vector>
#include "../hpp/enemy_behaviors.hpp"
#include "../hpp/super_matrix.hpp"

namespace crow {

//class ai_manager;

// a basic behavior tree
// TODO: ADD DECORATOR NODES
struct behavior_tree {

    crow::ai_manager* aim = nullptr;
    super_matrix e_matrix;

  struct node {
    virtual status run(float dt, crow::ai_manager& m) = 0;
    status (*r)(float, crow::ai_manager&){nullptr};
  };

   struct leaf_node : public crow::behavior_tree::node {
    virtual status run(float dt, crow::ai_manager& m) override;
  };

  struct composite_node : public behavior_tree::node {
    void add_child(behavior_tree::node* n);
    const std::vector<behavior_tree::node*>& get_children() const;

   private:
    std::vector<behavior_tree::node*> children;
  };

  struct selector_node : public behavior_tree::composite_node {
    virtual status run(float dt, crow::ai_manager& m) override;
  };

  struct sequence_node : public behavior_tree::composite_node {
    virtual status run(float dt, crow::ai_manager& m) override;
  };

  class decorator_node : public behavior_tree::node {
    behavior_tree::node* child;

   protected:
    behavior_tree::node* get_child() const;

   public:
    void set_child(behavior_tree::node* n);
  };

  class root_node : public behavior_tree::decorator_node {
    friend struct behavior_tree;
    virtual status run(float dt, crow::ai_manager& m);
  };

  class inverter_node : public behavior_tree::decorator_node {
    virtual status run(float dt, crow::ai_manager& m) override;
  };



  behavior_tree();
  ~behavior_tree();
  
  void build_tree();
  void clean_tree();

 private:
  behavior_tree::root_node* root;
  std::vector<leaf_node*> lnodes;
  std::vector<selector_node*> seln;
  std::vector<sequence_node*> seqn;
  std::vector<inverter_node*> invn;

 public:
  void set_root_child(behavior_tree::node* n) const;
  status run(float dt) const;
};

}  // namespace crow