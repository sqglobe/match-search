find_package(Qt6 REQUIRED COMPONENTS Test)
find_package(GTest REQUIRED)

# Enable Qt6 MOC
set(CMAKE_AUTOMOC ON)

# Library includes all dependencies for tests
add_library(gui-tests-deps INTERFACE)
target_link_libraries(gui-tests-deps INTERFACE Qt::Test GTest::gmock gui files-search-mocks files-search)
# Enable access in tests into implementation folder
target_include_directories(gui-tests-deps INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/../src")
# Add current `include` directory into the include folders
target_include_directories(gui-tests-deps INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")
# Enable current source directory as include folder for local mocks
target_include_directories(gui-tests-deps INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")

# Function creates a new testing executable and registers it with CTest
# based on the source file name
function(add_qt_test SOURCE_FILE)
  get_filename_component(TARGET_NAME ${SOURCE_FILE} NAME_WE)
  add_executable(${TARGET_NAME} ${SOURCE_FILE})
  add_test(NAME ${TARGET_NAME} COMMAND ${TARGET_NAME})
  target_link_libraries(${TARGET_NAME} PRIVATE gui-tests-deps)
endfunction()

# Add tests
add_qt_test(PlainTextMatchCollectorTest.cpp)
add_qt_test(MainWindowTest.cpp)
add_qt_test(SearchWorkerTest.cpp)
add_qt_test(MainWindowIntegrationTest.cpp)

# Setup label for unit-tests
set_property(TEST PlainTextMatchCollectorTest PROPERTY LABELS UNIT_TEST)
set_property(TEST MainWindowTest PROPERTY LABELS UNIT_TEST)
set_property(TEST SearchWorkerTest PROPERTY LABELS UNIT_TEST)

# Add label for integration test
set_property(TEST MainWindowIntegrationTest PROPERTY LABELS INTEGRATION_TEST)
             