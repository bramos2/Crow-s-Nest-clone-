#pragma once

#include "liblava/app/camera.hpp"

namespace crow {

auto get_floor_point(lava::camera& camera) -> glm::vec3;

void initialize_debug_camera(lava::camera& camera);

}  // namespace crow
