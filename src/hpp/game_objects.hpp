#pragma once

#include <liblava/lava.hpp>

#include <vector>

namespace crow {
// meshes need to be loaded in the order objects are listed here
enum class object_type {
  PLAYER = 0,
  ENEMY = 1,
  SD_CONSOLE,
  POWER_CONSOLE,
  DOOR,
  DOOR_PANEL,
  FLOOR,
  WALL,
  SHORT_WALL,
  COUNT,
};

}  // namespace crow