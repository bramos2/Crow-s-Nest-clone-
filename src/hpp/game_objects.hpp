#pragma once

#include <liblava/lava.hpp>

#include <vector>

namespace crow {

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
};

}  // namespace crow