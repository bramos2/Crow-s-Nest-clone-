#include "../hpp/message.hpp"

namespace crow {
void message::operator=(const message &m) {
  text = m.text;
  time_remaining = m.time_remaining;
  progress = m.progress;
  progress_max = m.progress_max;
}

void message::update(float dt) {
  if (progress < progress_max) {
    progress += dt;
    // processing for when the progress bar has finished
    if (progress >= progress_max) {
      progress = progress_max;
    }
  } else {
    time_remaining -= dt;
  }
}

void message::display(float scale, ImVec2 wh) {
  if (time_remaining <= 0) return;

  // if this is a progress bar, use the other draw method, not this one.
  if (progress_max > 0) {
    display_bar(scale, wh);
    return;
  }

  // ImGui::SetWindowFontScale(wh.x / 960.f * scale);
  float text_size =
      wh.x / 960.f * ImGui::GetFontSize() * text.size() / 2 * scale;

  ImVec2 popup_window_xy = {wh.x / 2.0f - ((text_size / 2.0f)) - wh.x * 0.05f,
                            wh.y * 0.16f};
  ImVec2 popup_window_wh = {text_size + wh.x * 0.1f, wh.y * 0.048f * scale};
  ImGui::SetNextWindowPos(popup_window_xy, ImGuiCond_Always);
  ImGui::SetNextWindowSize(popup_window_wh, ImGuiCond_Always);
  ImGui::Begin("popup_wind", 0,
               ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoCollapse |
                   ImGuiWindowFlags_NoResize);

  ImGui::SetWindowFontScale(wh.x / 960.f * scale);
  ImGui::SetCursorPosX(popup_window_wh.x * 0.05f);
  ImGui::Text(text.c_str());
  ImGui::End();
}

void message::display_bar(float scale, ImVec2 wh) {
  // ImGui::SetWindowFontScale(wh.x / 960.f * scale);
  float text_size =
      wh.x / 960.f * ImGui::GetFontSize() * text.size() / 2 * scale;

  ImVec2 popup_window_xy = {wh.x / 2.0f - ((text_size / 2.0f)) - wh.x * 0.05f,
                            wh.y * 0.16f};
  ImVec2 popup_window_wh = {text_size + wh.x * 0.1f,
                            wh.y * 0.048f * scale + wh.y * 0.15f};
  ImGui::SetNextWindowPos(popup_window_xy, ImGuiCond_Always);
  ImGui::SetNextWindowSize(popup_window_wh, ImGuiCond_Always);
  ImGui::Begin("popup_wind_p", 0,
               ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoCollapse |
                   ImGuiWindowFlags_NoResize);

  ImGui::SetWindowFontScale(wh.x / 960.f * scale);
  ImGui::SetCursorPosX(popup_window_wh.x * 0.05f);
  ImGui::Text(text.c_str());

  // draw the bar using a button, just because we can
  // TODO::load a texture instead of something as hilarious as a BUTTON

  ImVec2 prog_bar_wh = {(popup_window_wh.x * 0.92f) * (progress / progress_max),
                        popup_window_wh.y * 0.45f};
  ImGui::NewLine();                                 // positioning
  ImGui::SetCursorPosX(popup_window_wh.x * 0.04f);  // positioning
  if (progress == progress_max)
    ImGui::Button("COMPLETE", prog_bar_wh);
  else
    ImGui::Button("", prog_bar_wh);

  ImGui::End();
}
}  // namespace crow