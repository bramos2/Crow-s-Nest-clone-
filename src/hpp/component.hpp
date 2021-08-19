#pragma once

namespace crow {

struct component_interface {};

struct component_player : component_interface {
  void update();
};

struct component_sphynx : component_interface {
  void update();
};

}  // namespace crow