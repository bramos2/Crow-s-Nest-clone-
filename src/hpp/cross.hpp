#include <string>

namespace crow {

auto find_exe_path() -> std::string;

auto find_shader_path(std::string spv_name) -> std::string;

}  // namespace crow
