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
  EXIT,
  DOOR_PANEL,
  FLOOR,
  WALL,
  SHORT_WALL,
  COUNT,
};

// when current_level == final_level, we will skip to the win screen instead of
// attempting to advance to the next level
static const int final_level = 10;

}  // namespace crow