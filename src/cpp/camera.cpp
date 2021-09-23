#include "../hpp/camera.hpp"

#include <glm/gtx/vector_angle.hpp>

#include "../../debug_camera_control/debug_camera_control.hpp"

// #include <fmt/core.h>
#include <liblava/lava.hpp>

#include <glm/gtx/euler_angles.inl>

namespace crow {

auto mouse_in_world(lava::app& app, lava::camera& camera, lava::input& input)
    -> glm::vec3 {
  auto [mouse_x, mouse_y] = input.get_mouse_position();
  unsigned window_width = app.window.get_framebuffer_size().x;
  unsigned window_height = app.window.get_framebuffer_size().y;
  // Bring mouse from viewport into normalized device coordinates.
  float ndc_x = (2.f * mouse_x) / window_width - 1.f;
  float ndc_y = 1.f - (2.f * mouse_y) / window_height;
  // Make a ray in projection space from the NDC coordinates.
  glm::vec4 clip =
      glm::vec4(ndc_x, ndc_y, -1.f,
                1.f);  // z == -1.f because space is in left-hand coordinates.
  // "Un-project" coordinates into eye space.
  glm::vec4 eye = glm::inverse(camera.get_projection()) * clip;
  eye = glm::vec4(eye.x, eye.y, -1.f, 0);
  // Bring ray into world space.
  glm::vec3 world = glm::vec3(glm::inverse(camera.get_view()) *
                              eye);  // Truncate the W element.
  world = glm::normalize(world);
  return world;
}

auto get_floor_point(lava::camera& camera) -> glm::vec3 {
  glm::mat4 const view = camera.get_view();
  glm::vec3 const translation = camera.position;
  glm::vec3 const rotation = camera.rotation;  // X, Y, and Z angles in degrees.
  float height = translation.y;                // Assume the floor is at y == 0.

  glm::vec3 up = glm::vec3{0, 1, 0};
  // float pitch = -glm::acos(glm::dot(front, up));
  float pitch = rotation.x + 90;
  float theta = 90 - pitch;
  float forward_magnitude = translation.y / glm::sin(glm::radians(theta)) *
                            glm::sin(glm::radians(90.f));

  glm::vec3 front;
  front.x =
      -glm::cos(glm::radians(rotation.x)) * glm::sin(glm::radians(rotation.y));
  front.y = glm::sin(glm::radians(rotation.x));
  front.z =
      glm::cos(glm::radians(rotation.x)) * glm::cos(glm::radians(rotation.y));
  front = glm::normalize(front);

  auto result = front * forward_magnitude + translation;
  return result;
}

glm::vec3 mouse_to_floor(const lava::app* const _app) {
  glm::vec3 ray_origin = {_app->camera.position.x, _app->camera.position.y,
                          _app->camera.position.z};

  glm::vec3 ray_direction = mouse_to_ray(_app);
  glm::vec3 final_position = ray_to_floor(ray_direction, ray_origin);

  return final_position;  //{clicked_position.x, invalid, clicked_position.z};
}

glm::vec3 mouse_to_ray(const lava::app* const _app) {
  // we need the width and heights of the window for this
  glm::vec2 wh = _app->window.get_size();
  // we need the mouse position too
  lava::mouse_position_ref _mouse_pos = _app->input.get_mouse_position();

  // first thing we gotta get is the position of the pixel we clicked with
  // the mouse and move it into ndc
  glm::vec4 mouse_position_ndc = {(_mouse_pos.x / wh.x * 2 - 1),
                                  (_mouse_pos.y / wh.y * 2 - 1), -1, 1};
  // then we multiply this point by the inverse of the projection matrix to
  // move it into camera space
  glm::mat4 inverse_projection_matrix =
      glm::inverse(_app->camera.get_projection());
  glm::vec4 mouse_position_cam = inverse_projection_matrix * mouse_position_ndc;
  // blank out these last two values so they dont screw us up
  mouse_position_cam.z = -1;
  mouse_position_cam.w = 0;
  // continue backpedaling by multiplying by the inverse view matrix
  // to move it into world space
  glm::mat4 inverse_view_matrix = glm::inverse(_app->camera.get_view());
  glm::vec4 mouse_position_wld = inverse_view_matrix * mouse_position_cam;
  // the variables of the ray itself; its origin and its direction
  glm::vec4 ray_direction = glm::normalize(mouse_position_wld);

  return ray_direction;
}

glm::vec3 ray_to_floor(glm::vec3 ray_direction, glm::vec3 ray_position) {
  glm::vec4 final_position = {ray_position.x, ray_position.y, ray_position.z,
                              0};

  // "invalid" float: if the clicked position does not return a valid position,
  // this float will be set to -1. in any method taking in the resultant of this
  // method, check for this -1. that means the position returned by this method
  // cannot be used.
  float invalid = 0;
  // ray is pointing at the floor
  if (ray_direction.y < 0 && ray_position.y > 0 ||
      ray_direction.y > 0 && ray_position.y < 0) {
    final_position.x -= ray_position.y * (ray_direction.x / ray_direction.y);
    // although unnecessaary, the y position is calculated
    final_position.y -=
        ray_position.y;  // * (ray_direction.y / ray_direction.y);
    final_position.z -= ray_position.y * (ray_direction.z / ray_direction.y);
    // ray is not pointing at the floor and thus will never intersect
  } else {
    invalid = -1;
  }

  return {final_position.x, invalid, final_position.z};
}

void initialize_debug_camera(lava::camera& camera) {
  camera.rotation_speed = 250;
  camera.movement_speed += 10;
  camera.position = lava::v3(0.0f, -4.036f, 8.304f);
  camera.rotation = lava::v3(0, 0, 0);
  // These globals are defined in debug_camera_control.hpp
  camera.set_movement_keys(debug_key_up, debug_key_down, debug_key_left,
                           debug_key_right);
}

void update_room_camera(crow::map_room* active_room, lava::camera& camera) {
  camera.position = active_room->cam_pos;
  camera.rotation = active_room->cam_rotation;
}

}  // namespace crow
