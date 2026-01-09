find_package(GTest REQUIRED)

# Add executable with tests
add_executable(files-search-tests
     DirectoryWalkerTest.cpp
     MatchSeekerTest.cpp
     SearcherTest.cpp
)

# Link all required libraries
target_link_libraries(files-search-tests  GTest::gtest_main files-search-mocks match-search-testing)

# Enable private include directories for tests
target_include_directories(files-search-tests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../src")

# Discover all test-cases and register them within CTest
gtest_discover_tests(files-search-tests 
                     PROPERTIES  LABELS UNIT_TEST)