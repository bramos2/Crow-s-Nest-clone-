#include "../hpp/map.hpp"

#include "../hpp/camera.hpp"

namespace crow {

void map_room::set_active(lava::app* app, lava::mesh::ptr& mesh_ptr,
                          lava::camera& camera) {
  if (/*!mesh_ptr*/ !this->room_mesh) {
    this->room_mesh = lava::make_mesh();
    this->room_mesh->add_data(this->room_mesh_data);
    this->room_mesh->create(app->device);
    // mesh_ptr->destroy();
    // mesh_ptr->set_data(nullptr);

    /*mesh_ptr = lava::make_mesh();
    mesh_ptr->add_data(room_mesh_data);
    mesh_ptr->create(app->device);*/
  } else {
    /*lava::mesh::ptr new_mesh_ptr = lava::make_mesh();
    new_mesh_ptr->add_data(room_mesh_data);
    new_mesh_ptr->create(app->device);
    mesh_ptr->destroy();
    mesh_ptr = new_mesh_ptr;*/
    // mesh_ptr->set_data(room_mesh_data);
  }
  crow::update_room_camera(this, camera);
  // load tilemap
}

glm::vec2 map_room::get_tile_wpos(std::int_fast32_t const x,
                                  std::int_fast32_t const y) {
  return floor_tiles.get_tile_wpos(x, y);
}

glm::vec2 map_room::get_tile_wpos(tile* const tile) {
  return floor_tiles.get_tile_wpos(tile);
}

tile* map_room::get_tile_at(glm::vec2 const pos) {
  return floor_tiles.get_tile_at(pos);
}

std::vector<glm::vec2> map_room::get_path(glm::vec2 start, glm::vec2 goal) {
  std::vector<glm::vec2> result;
  // converting given positions into tiles
  tile* s = get_tile_at(start);
  tile* g = get_tile_at(goal);
  crow::theta_star path_finder;
  path_finder.set_weight(1.2f);
  // initializing the path finder, returns false if s or g are null
  if (path_finder.set_theta_star(s, g, &floor_tiles)) {
    path_finder.search_theta_star();
    std::vector<tile*> path = path_finder.get_path();
    // converting path from the search into world space coordinates
    if (path.size() > 0) {
      for (size_t i = 0; i < path.size(); ++i) {
        result.push_back(get_tile_wpos(path[i]));
      }
    }
  }

  /* if (floor_tiles.map.size() > 0) {
  // tile map test
  // TODO: MOVE TO A TEST BENCH
   glm::vec2 test_min = floor_tiles.min_room_pos;
   glm::vec2 test_mid = {0.0f, 0.0f};
   glm::vec2 test_max = floor_tiles.max_room_pos - 1.0f;
   int debug = 0;
   tile* tile_min = get_tile_at(test_min);
   tile* tile_mid = get_tile_at(test_mid);
   tile* tile_max = get_tile_at(test_max);
   debug = 0;
   test_min = get_tile_wpos(tile_min);
   test_mid = get_tile_wpos(tile_mid);
   test_max = get_tile_wpos(tile_max);
   debug = 0;
  }*/
  return result;
}

}  // namespace crow
