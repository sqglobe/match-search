# Add library wtih  the core library responsible for searching process
add_library(files-search STATIC
  include/files-search/Searcher.h
  include/files-search/MatchesCollector.h
  include/files-search/Factory.h
  
  impl/interfaces/DirectoryWalker.h
  impl/interfaces/MatchSeeker.h
  
  impl/DirectoryWalkerImpl.h
  impl/DirectoryWalkerImpl.cpp
  impl/MatchSeekerImpl.h
  impl/MatchSeekerImpl.cpp
  impl/SearcherImpl.h
  impl/SearcherImpl.cpp

  impl/Factory.cpp
)

# Add include folders with public headers available for user of that library
target_include_directories(files-search PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

# Add include folder with implementations into as internal for the library
target_include_directories(files-search PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")

