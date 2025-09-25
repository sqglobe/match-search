find_package(Qt6 REQUIRED COMPONENTS Test)
find_package(GTest REQUIRED)

# Enable Qt6 MOC
set(CMAKE_AUTOMOC ON)

# Library includes all dependencies for tests
add_library(gui-tests-deps STATIC)
target_link_libraries(gui-tests-deps PUBLIC Qt::Test GTest::gtest GTest::gmock gui files-search-mocks files-search)
# Enable access in tests into implementation folder
target_include_directories(gui-tests-deps PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../src")
# Add current `include` directory into the include folders
target_include_directories(gui-tests-deps PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
# Enable current source directory as include folder for local mocks
target_include_directories(gui-tests-deps PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
# Add main.cpp as a source file, so any target that links this lib will have it transitively
target_sources(gui-tests-deps PUBLIC main.cpp)

# Add executable with tests
add_executable(gui-unit-test
     PlainTextMatchCollectorTest.cpp
     MainWindowTest.cpp
     SearchWorkerTest.cpp
)

target_link_libraries(gui-unit-test gui-tests-deps)

# Discover all test-cases and register them within CTest
gtest_discover_tests(gui-unit-test
                     DISCOVERY_MODE PRE_TEST
                     PROPERTIES  
                       LABELS UNIT_TEST)

# Add executable with integration tests
add_executable(gui-integration-test
     MainWindowIntegrationTest.cpp
)

target_link_libraries(gui-integration-test gui-tests-deps)

# Discover all test-cases and register them within CTest
gtest_discover_tests(gui-integration-test
                     DISCOVERY_MODE PRE_TEST
                     PROPERTIES  
                        LABELS INTEGRATION_TEST)