#pragma once
#include <algorithm>
#include <assert.h>
#include <vector>

#include "../hpp/tile.hpp"

namespace crow {

class theta_star {
  bool finished = false;  // unused at the moment
  float weight = 1.2f;    // heuristic weight

  struct node {
    tile* tileptr = nullptr;
    node* parent = nullptr;
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
      if (a->f > b->f) {
        return true;
      } else if (a->f < b->f) {
        return false;
      } else {
        if (a->g > b->g) {
          return true;
        } else {
          return false;
        }
      }
      return false;
    }
  };

  std::vector<node*> open;
  std::vector<node*> closed;
  std::vector<tile*> path;
  node* start = nullptr;
  node* goal = nullptr;
  tile_map* tilemap = nullptr;
  std::vector<std::vector<node*>> nodes;

 public:
  theta_star();
  theta_star(const theta_star& ts);
  theta_star(crow::tile_map* t_map);
  ~theta_star();

  // sets heuristic weight
  void set_weight(float w);

  // returns current value of the heuristic weight
  float get_weight();

  // initializes data for search
  bool set_theta_star(tile* _start, tile* _goal);

  // searches for a path between the initialized start and goal nodes
  void search_theta_star();

  // returns a vector with the path from the goal to our position
  std::vector<tile*> get_path();

  void clean_data();

 private:
  // theta* part of the algorithm
  void update_vertex(node* s, node* neighbor);
};

}  // namespace crow
