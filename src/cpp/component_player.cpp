#include "../hpp/component_player.hpp"

void crow::Component_Player::update(const lava::app* const _app,
                                    std::vector<Object>* _objects) {
  left_click(_app, _objects);
}

void crow::Component_Player::left_click(const lava::app* const _app,
                                    std::vector<Object>* _objects) {
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
  glm::mat4 inverse_projection_matrix = glm::inverse(_app->camera.get_projection());
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
  glm::vec4 ray_origin = {_app->camera.position.x, _app->camera.position.y,
                          _app->camera.position.z, 0};
  // final selected position
  glm::vec4 clicked_position = {_app->camera.position.x, _app->camera.position.y,
                                _app->camera.position.z, 0};

  // ray cast beta cast
  bool invalid = false;
  if (ray_direction.y < 0 && ray_origin.y > 0) {
    while (clicked_position.y > 0) {
      clicked_position += ray_direction;
    }
  } else if (ray_direction.y > 0 && ray_origin.y < 0) {
    while (clicked_position.y < 0) {
      clicked_position += ray_direction;
    }
  } else {
    invalid = true;
  }

  // debug output
  printf("mouse: (%f, %f)\ndeduced mouse position: (%f, %f, %f) invalid = %i\n",
         _mouse_pos.x, _mouse_pos.y, clicked_position.x, clicked_position.y,
         clicked_position.z, invalid);
}
