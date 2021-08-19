#pragma once
#include <algorithm>
#include <assert.h>
#include <unordered_map>
#include <vector>

#include "../hpp/tile.hpp"

namespace path {

class theta_star {
  bool finished;  // unused at the moment
  float weight;   // heuristic weight

  struct node {
    tile* tileptr;
    node* parent;
    std::vector<node*> neighbors;
    float g;
    float h;
    float f;  // may not need to be here

    node() = default;
    ~node() = default;
    node(tile* _tile, node* _parent)
        : g(0), h(0), f(0), tileptr(_tile), parent(_parent){};
  };

  // comparator used to sorting the heap to pop the current head in open list
  struct compare_cost {
    bool operator()(node* const a, node* const b) const {
      if (a->f > b->f) return true;
      if (a->f < b->f) return false;
      if (a->f == b->f) {
        if (a->g > b->g)
          return true;
        else
          return false;
      }
    }
  };

  std::vector<node*> open;
  std::vector<node*> closed;
  std::vector<tile*> path;
  node* start;
  node* goal;
  tile_map* tilemap;
  std::vector<std::vector<node*>> nodes;

 public:
  theta_star() = default;
  ~theta_star() = default;

  // sets heuristic weight
  void set_weight(float w);

  // returns current value of the heuristic weight
  float get_weight();

  // initializes data for search
  void set_theta_star(tile* _start, tile* _goal, tile_map* _map);

  // searches for a path between the initialized start and goal nodes
  void search_theta_star();

  // returns a vector with the path from the goal to our position
  std::vector<tile*> get_path();

 private:
  // theta* part of the algorithm
  void update_vertex(node* s, node* neighbor);
};
}  // namespace path