cmake_minimum_required(VERSION 2.8)

include_directories($<CMAKE_INSTALL_PREFIX>/geophile/include)
link_directories($(geophile_BINARY_DIR)/geophile)

# example1

add_executable(example1
  example1.cpp)

target_link_libraries(example1 geophile)

# example2

add_executable(example2
  example2.cpp)

target_link_libraries(example2 geophile)
