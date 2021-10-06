#pragma once

#include <liblava/lava.hpp>
#include <vector>

#include "../hpp/pipeline.hpp"

namespace crow {
void set_desc_layouts(crow::descriptor_layouts& desc_layouts, lava::app* app);

void create_desc_sets(crow::descriptor_layouts& desc_layouts,
                      crow::descriptor_sets& desc_sets,
                      lava::descriptor::pool::ptr descriptor_pool);

void update_desc_sets(crow::descriptor_sets& desc_sets,
                      lava::app* app, lava::buffer& camera_buffer);
} // namespace crow