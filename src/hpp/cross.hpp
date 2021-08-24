#include <liblava/file.hpp>

#include <string>

namespace crow {

auto find_exe_path() -> std::string;

auto find_spv_path(std::string& spv_name) -> std::string;

auto get_spirv_data(std::string file_name) -> lava::file_data;

}  // namespace crow
