cmake_minimum_required(VERSION 2.8)

set(GEOPHILE_INCLUDES "$(CMAKE_INSTALL_PREFIX)/include")

include_directories($(GEOPHILE_INCLUDES))
link_directories($(geophile_BINARY_DIR)/geophile $(geophile_BINARY_DIR)/geophiletest)

# index_unittest
add_executable(index_unittest
  index_unittest.cpp)
target_link_libraries(index_unittest geophiletest geophile)

# core_unittest
add_executable(core_unittest
  core_unittest.cpp)
target_link_libraries(core_unittest geophile)

