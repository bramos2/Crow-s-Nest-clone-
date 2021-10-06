
#include "../hpp/map.hpp"

namespace crow {

void door::move_entity(const size_t entity_indx, int const room_number) {}

void room::load_entities(lava::app* app, crow::entities2& entities,
                         std::vector<lava::mesh::ptr>& meshes,
                         crow::descriptor_writes_stack* writes_stack,
                         lava::descriptor::pool::ptr descriptor_pool,
                         lava::buffer& camera_buffer) {
  // first we need to know what our entity and total indices will be
  size_t initial_index = entities.current_size;
  const unsigned int total_entities = room_meshes.size();
  //+room_objects.size();

  // then we need to allocate entities
  entities.allocate(total_entities);

  if (!room_meshes.empty()) {
    // loading floor
    object_indices.push_back(initial_index++);
    entities.initialize_entity(app, object_indices.back(), room_meshes[0],
                               writes_stack, descriptor_pool, camera_buffer);

    // loading walls
    if (room_meshes.size() > 1) {
      for (size_t i = 1; i < room_meshes.size(); ++i) {
        object_indices.push_back(initial_index++);
        entities.initialize_entity(app, object_indices.back(), room_meshes[i],
                                   writes_stack, descriptor_pool,
                                   camera_buffer);
      }
    }
  }

  // loading objects
  /*if (!room_objects.empty()) {
    for (const auto& obj : room_objects) {
      object_indices.push_back(initial_index++);
      entities.initialize_entity(app, object_indices.back(),
                                 meshes[static_cast<size_t>(obj.second)],
                                 writes_stack, descriptor_pool, camera_buffer);
    }
  }*/
}

void room::make_room_meshes(lava::app* app) {
  lava::mesh_data cube = lava::create_mesh_data(lava::mesh_type::cube);
  cube.scale_vector({width * 2.f, 0.2f, length * 2.f});
  lava::mesh::ptr floor_mesh = lava::make_mesh();
  floor_mesh->add_data(cube);
  floor_mesh->create(app->device);
  room_meshes.push_back(floor_mesh);

  // initializing tile map
  tiles = crow::tile_map(width, length);
  tiles.create_map();
  pather = crow::theta_star(&tiles);
}

auto room::get_tile_wpos(unsigned int const x, unsigned int const y)
    -> glm::vec2 {
  return tiles.get_tile_wpos(x, y);
}

auto room::get_tile_wpos(tile* const tile) -> glm::vec2 {
  return tiles.get_tile_wpos(tile);
}

auto room::get_tile_at(glm::vec2 const pos) -> tile* {
  return tiles.get_tile_at(pos);
}

auto room::get_path(glm::vec2 start, glm::vec2 goal) -> std::vector<glm::vec2> {
  std::vector<glm::vec2> result;
  // converting given positions into tiles
  tile* s = get_tile_at(start);
  tile* g = get_tile_at(goal);
  // crow::theta_star path_finder;
  // pather.set_weight(1.2f);
  // initializing the path finder, returns false if s or g are null
  if (pather.set_theta_star(s, g)) {
    pather.search_theta_star();
    std::vector<tile*> path = pather.get_path();
    // converting path from the search into world space coordinates
    if (!path.empty()) {
      for (size_t i = 0; i < path.size(); ++i) {
        result.push_back(get_tile_wpos(path[i]));
      }
    }
  }
  return result;
}

void level::load_entities(lava::app* app, crow::entities2& entities,
                          std::vector<lava::mesh::ptr>& meshes,
                          crow::descriptor_writes_stack* writes_stack,
                          lava::descriptor::pool::ptr descriptor_pool,
                          lava::buffer& camera_buffer) {
  for (auto& row : rooms) {
    for (auto& r : row) {
      if (r.id != 0) {
        r.load_entities(app, entities, meshes, writes_stack, descriptor_pool,
                        camera_buffer);
      }
    }
  }
}

void level::load_level(std::string filepath) {}

void level::test_level(lava::app* app) {
  x = y = 5;
  id = 1;
  starting_room = 1;
  exit_room = 8;
  rooms.resize(y);
  for (auto& row : rooms) {
    row.resize(x);
  }

  rooms[0][0].id = 1;
  rooms[0][0].make_room_meshes(app);
  // pushing worker index for desmostrations
  rooms[0][0].object_indices.push_back(0);
  rooms[0][1].id = 2;
  rooms[0][1].make_room_meshes(app);
  rooms[1][1].id = 3;
  rooms[1][1].make_room_meshes(app);
  rooms[2][1].id = 4;
  rooms[2][1].make_room_meshes(app);
  rooms[2][0].id = 5;
  // changin the dimmensions of this room
  rooms[2][0].width = rooms[2][0].length = 25;
  rooms[2][0].make_room_meshes(app);
  // pushing ai index for demostrations
  rooms[2][0].object_indices.push_back(1);
  rooms[3][1].id = 6;
  rooms[3][1].make_room_meshes(app);
  rooms[2][2].id = 7;
  rooms[2][2].make_room_meshes(app);
  rooms[2][3].id = 8;
  rooms[2][3].make_room_meshes(app);
}

void level::clean_level(std::vector<lava::mesh::ptr>& trash) {
  selected_room = nullptr;
  for (auto& row : rooms) {
    for (auto& r : row) {
      while (!r.room_meshes.empty()) {
        trash.push_back(r.room_meshes.back());
        r.room_meshes.pop_back();
      }

      r.id = 0;
      r.object_indices.clear();
      r.room_meshes.clear();
      r.pather.clean_data();
      int debug = 0;
    }
  }
}

}  // namespace crow









//#include "../hpp/map.hpp"
//
//#include "../hpp/camera.hpp"
//
//namespace crow {
//
//void map_room::set_active(lava::app* app, lava::mesh::ptr& mesh_ptr,
//                          lava::camera& camera) {
//  if (/*!mesh_ptr*/ !this->room_mesh) {
//    this->room_mesh = lava::make_mesh();
//    this->room_mesh->add_data(this->room_mesh_data);
//    this->room_mesh->create(app->device);
//    // mesh_ptr->destroy();
//    // mesh_ptr->set_data(nullptr);
//
//    /*mesh_ptr = lava::make_mesh();
//    mesh_ptr->add_data(room_mesh_data);
//    mesh_ptr->create(app->device);*/
//  } else {
//    /*lava::mesh::ptr new_mesh_ptr = lava::make_mesh();
//    new_mesh_ptr->add_data(room_mesh_data);
//    new_mesh_ptr->create(app->device);
//    mesh_ptr->destroy();
//    mesh_ptr = new_mesh_ptr;*/
//    // mesh_ptr->set_data(room_mesh_data);
//  }
//  //crow::update_room_camera(this, camera);
//  // load tilemap
//}
//
//glm::vec2 map_room::get_tile_wpos(unsigned int const x, unsigned int const y) {
//  return floor_tiles.get_tile_wpos(x, y);
//}
//
//glm::vec2 map_room::get_tile_wpos(tile* const tile) {
//  return floor_tiles.get_tile_wpos(tile);
//}
//
//tile* map_room::get_tile_at(glm::vec2 const pos) {
//  return floor_tiles.get_tile_at(pos);
//}
//
//std::vector<glm::vec2> map_room::get_path(glm::vec2 start, glm::vec2 goal) {
//  std::vector<glm::vec2> result;
//  // converting given positions into tiles
//  tile* s = get_tile_at(start);
//  tile* g = get_tile_at(goal);
//  crow::theta_star path_finder;
//  path_finder.set_weight(1.2f);
//  // initializing the path finder, returns false if s or g are null
//  if (path_finder.set_theta_star(s, g)) {
//    path_finder.search_theta_star();
//    std::vector<tile*> path = path_finder.get_path();
//    // converting path from the search into world space coordinates
//    if (path.size() > 0) {
//      for (size_t i = 0; i < path.size(); ++i) {
//        result.push_back(get_tile_wpos(path[i]));
//      }
//    }
//  }
//
//  /* if (floor_tiles.map.size() > 0) {
//  // tile map test
//  // TODO: MOVE TO A TEST BENCH
//   glm::vec2 test_min = floor_tiles.min_room_pos;
//   glm::vec2 test_mid = {0.0f, 0.0f};
//   glm::vec2 test_max = floor_tiles.max_room_pos - 1.0f;
//   int debug = 0;
//   tile* tile_min = get_tile_at(test_min);
//   tile* tile_mid = get_tile_at(test_mid);
//   tile* tile_max = get_tile_at(test_max);
//   debug = 0;
//   test_min = get_tile_wpos(tile_min);
//   test_mid = get_tile_wpos(tile_mid);
//   test_max = get_tile_wpos(tile_max);
//   debug = 0;
//  }*/
//  return result;
//}
//
//}  // namespace crow
