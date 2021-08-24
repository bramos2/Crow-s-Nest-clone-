#pragma once
#include "../hpp/search_theta.hpp"

namespace path {

theta_star::~theta_star() {
  for (size_t c = 0; c < nodes.size(); ++c) {
    for (size_t r = 0; r < nodes[c].size(); ++r) {
      node* curr = nodes[c][r];
      delete curr;
    }
  }
}

void theta_star::set_weight(float w) { weight = w; }

float theta_star::get_weight() { return weight; }

std::vector<tile*> theta_star::get_path() { return path; }

void theta_star::set_theta_star(tile* _start, tile* _goal, tile_map* _map) {
  finished = false;
  tilemap = _map;
  nodes.resize(tilemap->get_width());
  for (auto& col : nodes) {
    col.resize(tilemap->get_height());
  }
  std::unordered_map<tile*, node*>
      nodemap;  // temporary map to assign correct start and goal node pointers
  // making all nodes
  for (size_t col = 0; col < tilemap->map.size(); ++col) {
    for (size_t row = 0; row < tilemap->map[col].size(); ++row) {
      node* curr = new node(tilemap->map[col][row], nullptr);
      nodes[col][row] = curr;
      nodemap[curr->tileptr] = curr;
    }
  }

  // connecting all neighbor nodes
  for (size_t col = 0; col < tilemap->get_width(); ++col) {
    for (size_t row = 0; row < tilemap->get_height(); row++) {
      tile* curr = tilemap->map[col][row];

      nodemap[curr]->neighbors.reserve(curr->neighbors.size());
      for (auto neighbor : curr->neighbors) {
        if (neighbor->is_open) {
          nodemap[curr]->neighbors.push_back(nodemap[neighbor]);
        }
      }
    }
  }

  assert(_start != NULL && _goal != NULL);

  // initialize our start and goal pointers
  goal = nodemap[_goal];

  start = nodemap[_start];
  start->parent = start;
  start->g = 0;
  start->h = start->tileptr->m_distance(_goal) * weight;
  start->f = start->g + start->h;

  // adding start to our open list
  open.push_back(start);
  std::push_heap(open.begin(), open.end(), compare_cost());
}

void theta_star::search_theta_star() {
  // loop until the list is empty
  while (!open.empty()) {
    node* curr_node = open.front();  // retrieving current head
    std::pop_heap(open.begin(), open.end(), compare_cost());  // head to back
    open.pop_back();  // removing back from list

    // checking to see if we reached the goal
    if (curr_node->tileptr == goal->tileptr) {
      // finished, make path and send
      goal->parent = curr_node->parent;
      goal->g = curr_node->g;

      node* cpath = goal;
      while (cpath != cpath->parent) {
        path.push_back(cpath->tileptr);
        cpath = cpath->parent;
      }

      //TODO: REMOVE AFTER TEST
      path.push_back(start->tileptr);

      return;
    }

    closed.push_back(curr_node);
    for (node* neighbor : curr_node->neighbors) {
      // checking if neighbor is in closed
      if (std::find(closed.begin(), closed.end(), neighbor) == closed.end()) {
        if (std::find(open.begin(), open.end(), neighbor) == open.end()) {
          neighbor->g = std::numeric_limits<float>::infinity();
          neighbor->parent = nullptr;
        }
        update_vertex(curr_node, neighbor);
      }
    }
  }
}

void theta_star::update_vertex(node* s, node* neighbor) {
  if (s->parent->tileptr->line_of_sight(neighbor->tileptr, tilemap->map)) {
    if (s->parent->g + s->parent->tileptr->m_distance(neighbor->tileptr) <
        neighbor->g) {
      neighbor->g =
          s->parent->g + s->parent->tileptr->m_distance(neighbor->tileptr);
      neighbor->parent = s->parent;
      if (std::find(open.begin(), open.end(), neighbor) != open.end()) {
        open.erase(std::remove(open.begin(), open.end(), neighbor),
                   open.end());  // removing from open
      }
      open.push_back(neighbor);  // add neighbor into open queue
      std::push_heap(open.begin(), open.end(), compare_cost());
    }

  } else {
    if (s->g + s->tileptr->m_distance(neighbor->tileptr) < neighbor->g) {
      neighbor->g = s->g + s->tileptr->m_distance(neighbor->tileptr);
      neighbor->parent = s;
      if (std::find(open.begin(), open.end(), neighbor) != open.end()) {
        open.erase(std::remove(open.begin(), open.end(), neighbor),
                   open.end());  // removing from open
      }
      open.push_back(neighbor);  // add neighbor back into open queue
      std::push_heap(open.begin(), open.end(), compare_cost());
    }
  }
}

}  // namespace path

