cmake_minimum_required(VERSION 2.8)

link_directories($(geophile_BINARY_DIR)/geophile)

add_executable(unittest
  RecordArray.cpp
  unittest.cpp)
target_link_libraries(unittest geophile)

add_executable(example1
  RecordArray.cpp
  example1.cpp)
target_link_libraries(example1 geophile)
