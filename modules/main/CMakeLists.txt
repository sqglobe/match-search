# Executable that will be used by user

add_executable(match-search main.cpp)
target_link_libraries(match-search PRIVATE gui files-search)

install(TARGETS match-search)