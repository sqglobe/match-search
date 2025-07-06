# This library contains core logic related to file searching

# Add subdirectory with library sources
add_subdirectory(src)

# If tests are required add subdirectories with tests and mocks
if (BUILD_TESTING)
  add_subdirectory( tests )
  add_subdirectory( mocks )
endif()
