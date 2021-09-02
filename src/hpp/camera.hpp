#pragma once

#include <liblava/lava.hpp>

#include "../hpp/map.hpp"

namespace crow {

struct camera_device_data {
  glm::mat4 projection_view;
};

auto mouse_in_world(lava::app& app, lava::camera& camera, lava::input& input)
    -> glm::vec3;

auto get_floor_point(lava::camera& camera) -> glm::vec3;

// utilizes mouse_to_ray() and ray_to_floor() to find out where in the floor the
// mouse is pointing
glm::vec3 mouse_to_floor(const lava::app* const _app);

// converts the mouse's current position on the screen to a ray that can be used
// to check for mouse interactions
glm::vec3 mouse_to_ray(const lava::app* const _app);

// takes in an arbitrary ray (pre-normalization is optional) and its position in
// space, and figures out the x and z coordinates that the ray will intersect
// the floor at
//
// important! if the ray cannot intersect the floor, then the y coordinate of
// the returned ray will always be -1. if not, then it will always be 0.
glm::vec3 ray_to_floor(glm::vec3 ray_direction, glm::vec3 ray_position);

void initialize_debug_camera(lava::camera& camera);

void update_room_camera(map_room* active_room, lava::camera& camera);

}  // namespace crow
