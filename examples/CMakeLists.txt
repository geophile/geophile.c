cmake_minimum_required(VERSION 2.8)

set(GEOPHILE_INCLUDES "$(CMAKE_INSTALL_PREFIX)/include")
# Enable the following line to get line numbers from valgrind
# set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -O0 -ggdb")

include_directories("$(GEOPHILE_INCLUDES)")
link_directories($(geophile_BINARY_DIR)/geophile)

# example1

add_executable(example1
  example1.cpp)

target_link_libraries(example1 geophile)

# example2

add_executable(example2
  example2.cpp)

target_link_libraries(example2 geophile)

# example3

add_executable(example3
  example3.cpp)

target_link_libraries(example3 geophile)

