#pragma once

#include <filesystem>

namespace match_search::testing {
/**
 * Create a temporary folder `name` and returns a path to this folder.
 * Throws an exception on error.
 */
std::filesystem::path temporaryFolder(std::string_view name);
}  // namespace match_search::testing