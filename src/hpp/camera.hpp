#pragma once

#include <liblava/lava.hpp>

namespace crow {

struct camera_device_data {
  glm::mat4 projection_view;
};

auto mouse_in_world(lava::app& app, lava::camera& camera, lava::input& input)
    -> glm::vec3;

auto get_floor_point(lava::camera& camera) -> glm::vec3;

void initialize_debug_camera(lava::camera& camera);

}  // namespace crow
