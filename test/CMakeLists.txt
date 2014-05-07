cmake_minimum_required(VERSION 2.8)

include_directories(../geophile)
link_directories($(geophile_BINARY_DIR)/geophile)

add_executable(unittest
  unittest.cpp)
target_link_libraries(unittest geophile)
