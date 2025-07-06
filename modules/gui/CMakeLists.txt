# Library supplied gui interface

add_subdirectory(src)

# That makes possible to exclude tests in order to
# accelerate building
if (BUILD_TESTING)
  add_subdirectory( tests )
endif()
