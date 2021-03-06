#pragma once

#include <cstdint>
#include <vector>

namespace crow
{
	using binary_blob_t = std::vector<uint8_t>;

	binary_blob_t load_binary_blob(const char* path);
}