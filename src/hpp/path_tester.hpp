#pragma once
#include <iostream>
#include <vector>
#include <format>
#include <liblava/app.hpp>
#include "../hpp/tile.hpp"
#include "../hpp/search_theta.hpp"

struct path_tester {
  void set_test() {
    tile_map map(20, 20);
    map.create_map();
    for (size_t i = 0; i < 19; i++) {
      map.map[i][9]->is_open = false;
    }

    for (size_t i = 1; i < 19; i++) {
      map.map[9][i]->is_open = false;
    }

    path::theta_star ts;
    ts.set_weight(1);
    ts.set_theta_star(map.map[0][0], map.map[8][10], &map);
    ts.search_theta_star();
    std::vector<tile*> path = ts.get_path();
    for (size_t i = 0; i < path.size(); i++) {
      path[i]->in_path = true;
    }
  }

  void RenderMap(std::vector<std::vector<tile*>> map) {

    for (size_t i = 0; i < map.size(); i++) {
      for (size_t j = 0; j < map[i].size(); j++) {
        if (map[i][j]->in_path) {
          fmt::print("@");
        } else {
          if (map[i][j]->is_open) {
            fmt::print("O");
          } else {
            fmt::print("#");
          }
        }
      }
      std::cout << std::endl;
    }
  }
};