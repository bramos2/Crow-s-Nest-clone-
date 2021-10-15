#pragma once

#include <liblava/lava.hpp>

#include <imgui.h>

namespace crow {
// default time a message will linger on screen before disappearing
constexpr float default_message_time = 3.0f;
// default time necessary to wait to interact with an object
constexpr float default_interact_wait = 3.0f;

struct message {
  std::string text;

  float time_remaining = 0;
  // if this is an interaction progress bar, then this will increase until
  // progress_max
  float progress = 0;
  // if this is ever set to a a number above 0, this message will be treated as
  // a progress bar and display as one. the progress bar representss progress
  // when interacting with any object.
  float progress_max = 0;

  message()
      : text(""),
        time_remaining(0),
        progress(0),
        progress_max(0) {}
  message(std::string _text, float _time_remaining = default_message_time,
          float _progress_max = 0)
      : text(_text),
        time_remaining(_time_remaining),
        progress(0),  // always 0
        progress_max(_progress_max) {}
  void operator=(const message &m);

  void update(float dt);
  void display(float scale, ImVec2 wh);
  // display this message along with a progress bar, indicating a WIP
  // interaction
  void display_bar(float scale, ImVec2 wh);
};
}  // namespace crow