#include "../hpp/map.hpp"

namespace crow {
void map_room::set_active(lava::app* app, lava::mesh::ptr& mesh_ptr) {
  if (!mesh_ptr) {
    //mesh_ptr->destroy();
    //mesh_ptr->set_data(nullptr);
    mesh_ptr = lava::make_mesh();
    mesh_ptr->add_data(room_mesh_data);
    mesh_ptr->create(app->device);
  } else {
    mesh_ptr->set_data(room_mesh_data);
    //mesh_ptr->create(app->device);
  }
}
}  // namespace crow
