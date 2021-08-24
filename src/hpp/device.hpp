#pragma once

#include <liblava/app.hpp>

namespace crow {

auto create_logical_device(lava::device_manager& manager) -> lava::device::ptr;

}
