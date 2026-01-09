find_package(GTest REQUIRED)

# Add executable with tests
add_executable(match-search-testing-tests
    ExpectedMatcherTest.cpp
    TmpFolderTest.cpp
)

# Link all required libraries
target_link_libraries(match-search-testing-tests  GTest::gtest_main match-search-testing)

# Enable private include directories for tests
target_include_directories(match-search-testing-tests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../src")

# Discover all test-cases and register them within CTest
gtest_discover_tests(match-search-testing-tests 
                     PROPERTIES  LABELS UNIT_TEST)