#include "match_search/testing/tmp_folder.h"

#include <stdlib.h>

#include <format>
#include <stdexcept>

namespace fs = std::filesystem;
namespace match_search::testing {
fs::path temporaryFolder(std::string_view name) {
  auto path = fs::temp_directory_path() / std::format("{}_XXXXXX", name);
  std::string asString = path.native();
  if (const auto *res = ::mkdtemp(&asString[0]); res == nullptr) {
    throw std::runtime_error("Failed to create a temporary folder");
  }
  return fs::path(asString);
}
}  // namespace match_search::testing