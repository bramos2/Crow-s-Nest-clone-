#include "../hpp/cross.hpp"

#include <array>

namespace crow {

// http://www.cplusplus.com/forum/general/11104/
#ifdef WIN32
#include <windows.h>
auto get_exe_path() -> std::string {
  std::array<char, MAX_PATH> result{};
  std::string full_path = std::string(
      result.data(), GetModuleFileName(NULL, result.data(), MAX_PATH));
  return std::string(full_path.substr(0, full_path.find_last_of('\\\\'))) + "/";
}
#else
#include <linux/limits.h>
#include <unistd.h>
auto get_exe_path() -> std::string {
  std::array<char, PATH_MAX - NAME_MAX> result{};
  std::string full_path = std::string(result.data());
  return std::string(full_path.substr(0, full_path.find_last_of('/'))) + "/";
}
#endif

}  // namespace crow
