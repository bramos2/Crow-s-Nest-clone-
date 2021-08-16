#pragma once
#include <algorithm>
#include <assert.h>
#include <cstdint>
#include <limits>
#include <queue>
#include <unordered_map>
#include <vector>

#include "tile.hpp"

namespace path {

class theta_star {
 private:
  bool finished;

 public:
  theta_star() = default;
  ~theta_star() = default;

  struct node {
    tile* ntile;
    node* parent;
    node* child;
    // int_fast8_t index;
    std::vector<node*> neighbors;

    double given_cost;
    double heuristic;
    double final_cost;

    node() = default;
    ~node() = default;
    node(tile* _tile, node* _parent) : ntile(_tile), parent(_parent){};
  };

 private:
  struct compare_cost {
    bool operator()(const node* a, const node* b) const {
      if (a->final_cost > b->final_cost) return true;
      if (a->final_cost < b->final_cost) return false;
      if (a->final_cost == b->final_cost) {
        if (a->given_cost > b->given_cost)
          return true;
        else
          return false;
      }
    }
  };

  std::vector<node*> open;
  std::vector<node*> closed;
  // std::vector<node*> successors;
  node* start;
  node* goal;
  node* solution_node;
  tile_map* tilemap;
  std::vector<std::vector<node*>> nodes;
  std::unordered_map<tile*, node*> nodemap;

  /*std::priority_queue<plannerNode*, std::queue<plannerNode*>, comparator>
   * open;*/

  void update_vertex(node* s, node* neighbor) {}

  void search_theta_star() {}

  void set_theta_star(tile* _start, tile* _goal, tile_map* _map) {}

  bool line_of_sight(node* a, node* b) {}

  float distance(node* a, node* b) {}
};

theta_star::theta_star() {}

theta_star::~theta_star() {}

void theta_star::set_theta_star(tile* _start, tile* _goal, tile_map* _map) {
  finished = false;
  tilemap = _map;
  nodes.resize(tilemap->width);
  for (auto col : tilemap->map) {
    col.resize(tilemap->height);
  }
  // making all nodes
  for (size_t col = 0; col < tilemap->width; ++col) {
    for (size_t row = 0; row < tilemap->height; row++) {
      node* curr = new node(tilemap->map[col][row], nullptr);
      nodes[col][row] = curr;
      nodemap[curr->ntile] = curr;
    }
  }
  // connecting all neighbor nodes
  for (size_t col = 0; col < tilemap->width; ++col) {
    for (size_t row = 0; row < tilemap->height; row++) {
      tile* curr = tilemap->map[col][row];

      nodemap[curr]->neighbors.reserve(curr->neighbors.size());
      for (auto neighbor : curr->neighbors) {
        nodemap[curr]->neighbors.push_back(nodemap[neighbor]);
      }
    }
  }

  assert(_start != NULL && _goal != NULL);

  goal = new node;
  goal->ntile = _goal;

  start = new node(_start, start);
  start->given_cost = 0;
  start->heuristic;  // = distance from goal to start
  start->final_cost = start->given_cost + start->heuristic;

  open.push_back(start);
  std::push_heap(open.begin(), open.end(), compare_cost());  
}

void theta_star::search_theta_star() {
  while (!open.empty()) {
    node* curr_node = open.front();
    std::pop_heap(open.begin(), open.end(), compare_cost());
    open.pop_back();

    if (curr_node->ntile == goal->ntile) {
      // finished, make path and send
      goal->parent = curr_node->parent;
      goal->given_cost = curr_node->given_cost;
    }

    closed.push_back(curr_node);
    for (node* neighbor : curr_node->neighbors) {
      // checking if neighbor is in closed
      if (std::find(closed.begin(), closed.end(), neighbor) == closed.end()) {
        if (std::find(open.begin(), open.end(), neighbor) == open.end()) {
          neighbor->given_cost = std::numeric_limits<float>::infinity();
          neighbor->parent = nullptr;
        }
        update_vertex(curr_node, neighbor);
      }
    }
  }
}

void theta_star::update_vertex(node* s, node* neighbor) {
  if (line_of_sight(s->parent, neighbor)) {
    if (s->parent->given_cost + distance(s->parent, neighbor) <
        neighbor->given_cost) {
      neighbor->given_cost =
          s->parent->given_cost + distance(s->parent, neighbor);
      neighbor->parent = s->parent;
      if (std::find(open.begin(), open.end(), neighbor) != open.end()) {
        open.erase(std::remove(open.begin(), open.end(), neighbor), open.end()); //removing from open
      }
      open.push_back(neighbor); //add neighbor into open queue
      std::push_heap(open.begin(), open.end(), compare_cost());
    }

  } else {
    // if gScore(s) + c(s, neighbor) < gScore(neighbor)
    if (s->given_cost + distance(s, neighbor) < neighbor->given_cost) {
      neighbor->given_cost = s->given_cost + distance(s, neighbor);
      neighbor->parent = s;
      if (std::find(open.begin(), open.end(), neighbor) != open.end()) {
        open.erase(std::remove(open.begin(), open.end(), neighbor), open.end()); //removing from open
      }
      open.push_back(neighbor); //add neighbor back into open queue
      std::push_heap(open.begin(), open.end(), compare_cost());
    }
  }
}

bool theta_star::line_of_sight(node* a, node* b) { return false; }

}  // namespace path