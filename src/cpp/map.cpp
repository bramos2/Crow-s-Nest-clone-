
#include "../hpp/map.hpp"

namespace crow {

// void door::move_entity(const size_t entity_indx, int const room_number) {}

void room::load_entities(lava::app* app, crow::entities& entities,
                         std::vector<lava::mesh::ptr>& meshes,
                         crow::descriptor_writes_stack* writes_stack,
                         lava::descriptor::pool::ptr descriptor_pool,
                         lava::buffer& camera_buffer) {
  // first we need to know what our entity and total indices will be
  size_t initial_index = entities.current_size;
  const unsigned int total_entities = r_meshes.size() + objects.size();

  // then we need to allocate entities
  entities.allocate(total_entities);

  if (!r_meshes.empty()) {
    // loading floor
    object_indices.push_back(initial_index++);
    entities.initialize_entity(app, object_indices.back(), r_meshes[0],
                               writes_stack, descriptor_pool, camera_buffer);

    // loading walls
    if (r_meshes.size() > 1) {
      for (size_t i = 1; i < r_meshes.size(); ++i) {
        object_indices.push_back(initial_index++);
        entities.initialize_entity(app, object_indices.back(), r_meshes[i],
                                   writes_stack, descriptor_pool,
                                   camera_buffer);
      }
    }
  }

  // loading objects
  for (crow::interactible*& i : objects) {
    object_indices.push_back(initial_index++);
    entities.initialize_entity(app, object_indices.back(),
                               meshes[static_cast<size_t>(i->type)],
                               writes_stack, descriptor_pool, camera_buffer);
    glm::vec2 pos = get_tile_wpos(i->x, i->y);
    entities.set_world_position(object_indices.back(), pos.x, 0.f, pos.y);
  }
}

void room::make_room_meshes(lava::app* app) {
  lava::mesh_data cube = lava::create_mesh_data(lava::mesh_type::cube);
  cube.scale_vector({width * 2.f, 0.2f, length * 2.f});
  lava::mesh::ptr floor_mesh = lava::make_mesh();
  floor_mesh->add_data(cube);
  floor_mesh->create(app->device);
  r_meshes.push_back(floor_mesh);

  // initializing tile map TODO: MOVE THIS TO AN INIT DATA FUNCTION
  tiles = crow::tile_map(width, length);
  tiles.create_map();
  pather = crow::theta_star(&tiles);

  for (crow::interactible*& i : objects) {
    if (i && i->type != crow::object_type::DOOR) {
      tiles.map[i->y][i->x]->is_open = false;
    }
  }
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
  // prevents player from pathing to tiles that don't exist
  if (!g || !g->is_open) return result;
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

void level::load_entities(lava::app* app, crow::entities& entities,
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
// all level LOADING method definitions moved to levels.cpp

void level::clean_level(std::vector<lava::mesh::ptr>& trash) {
  selected_room = nullptr;
  for (auto& row : rooms) {
    for (auto& r : row) {
      while (!r.r_meshes.empty()) {
        trash.push_back(r.r_meshes.back());
        r.r_meshes.pop_back();
      }

      while (!r.objects.empty()) {
        crow::interactible* t = r.objects.back();
        r.objects.pop_back();
        delete t;
      }

      r.id = 0;
      r.object_indices.clear();
      r.r_meshes.clear();
      r.pather.clean_data();
      int debug = 0;
    }
  }
}

}  // namespace crow