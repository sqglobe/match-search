find_package(GTest REQUIRED)

# Library with mocks for interfaces from `files-search`
# It's interface type, because there is no cpp-files
add_library(files-search-mocks INTERFACE)

# Add include directory to make it possible using its header in tests
target_include_directories(files-search-mocks INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")

# Link required libraries for convenient usage in tests
target_link_libraries(files-search-mocks INTERFACE GTest::gmock files-search)

