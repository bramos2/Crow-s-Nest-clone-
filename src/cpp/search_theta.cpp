#pragma once
#include "../hpp/search_theta.hpp"

namespace crow {

theta_star::~theta_star() { clean_data(); }

void theta_star::set_weight(float w) { weight = w; }

float theta_star::get_weight() { return weight; }

std::vector<tile*> theta_star::get_path() { return path; }

void theta_star::clean_data() {
  for (size_t c = 0; c < nodes.size(); ++c) {
    for (size_t r = 0; r < nodes[c].size(); ++r) {
      node* curr = nodes[c][r];
      if (curr) {
        delete curr;
      }
    }
  }
}

bool theta_star::set_theta_star(tile* _start, tile* _goal, tile_map* _map) {
  if (!_start || !_goal) {
    return false;
  }

  tilemap = _map;
  nodes.resize(tilemap->get_height());
  for (auto& row : nodes) {
    row.resize(tilemap->get_width());
  }

  // loading all the nodes
  for (size_t row = 0; row < tilemap->map.size(); ++row) {
    for (size_t col = 0; col < tilemap->map[row].size(); ++col) {
      node* curr = new node(tilemap->map[row][col], nullptr);
      nodes[row][col] = curr;
    }
  }

  // connecting all neighbor nodes
  for (size_t row = 0; row < tilemap->map.size(); ++row) {
    for (size_t col = 0; col < tilemap->map[row].size(); ++col) {
      tile* curr = tilemap->map[row][col];

      // nodemap[curr]->neighbors.reserve(curr->neighbors.size());
      nodes[row][col]->neighbors.reserve(curr->neighbors.size());
      for (auto neighbor : curr->neighbors) {
        if (neighbor->is_open) {
          nodes[row][col]->neighbors.push_back(
              nodes[neighbor->row][neighbor->col]);
        }
      }
    }
  }

  finished = false;
  open.clear();
  open.resize(0);

  // initialize our start and goal pointers
  // goal = nodemap[_goal];
  goal = nodes[_goal->row][_goal->col];
  // start = nodemap[_start];
  start = nodes[_start->row][_start->col];
  start->parent = start;
  start->g = 0;
  start->h = start->tileptr->m_distance(_goal) * weight;
  start->f = start->g + start->h;

  // adding start to our open list
  open.push_back(start);
  std::push_heap(open.begin(), open.end(), compare_cost());

  return true;
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

      // TODO: REMOVE AFTER TEST
      // path.push_back(start->tileptr);

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

}  // namespace crow
