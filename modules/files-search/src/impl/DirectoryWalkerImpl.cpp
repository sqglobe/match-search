#include "impl/DirectoryWalkerImpl.h"

#include <fstream>

using namespace files_search;

std::expected<void, std::string>
DirectoryWalkerImpl::walk(const std::filesystem::path &path,
                          const MatchSeeker &seeker,
                          MatchesCollector &collector) const {
  try {
    for (const auto &dir_entry :
         std::filesystem::recursive_directory_iterator(path)) {
      if (dir_entry.is_regular_file()) {
        auto file = std::ifstream(dir_entry.path());
        if (file.is_open()) {
          collector.startFile(dir_entry.path().c_str());
          seeker.seek(file, collector);
        } else {
          collector.failed(dir_entry.path().c_str());
        }
      }
    }
    return {};
  } catch (std::filesystem::filesystem_error &err) {
    return std::unexpected(err.code().message());
  }
}