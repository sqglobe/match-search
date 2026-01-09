# Library with testing helpers

find_package(GTest REQUIRED)

add_library(match-search-testing STATIC
  impl/tmp_folder.cpp
)

target_sources(match-search-testing
  PUBLIC
   FILE_SET HEADERS
   BASE_DIRS 
     include
   FILES
     include/match_search/testing/expected_matcher.h
     include/match_search/testing/tmp_folder.h
)

# Add include folder with implementations into as internal for the library
target_include_directories(match-search-testing PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")

target_link_libraries(match-search-testing  GTest::gmock)