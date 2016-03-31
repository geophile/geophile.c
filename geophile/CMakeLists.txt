cmake_minimum_required(VERSION 2.8)

# Enable the following line to get line numbers from valgrind
# set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -O0 -ggdb")

add_library(geophile SHARED
  Box2.cpp
  ByteBuffer.cpp
  IntList.cpp
  IntSet.cpp
  Point2.cpp
  Region.cpp
  RegionPool.cpp
  RegionQueue.cpp
  SessionMemoryBase.cpp
  Space.cpp
  SpatialObjectTypes.cpp
  ZArray.cpp)

add_library(geophiletest SHARED
  TestSpatialObject.cpp
  testbase.cpp)

# install(TARGETS geophile geophiletest DESTINATION lib)

install(FILES 
  geophile.h
  Box2.h
  BufferingSpatialObjectReferenceManager.h
  ByteBuffer.h
  ByteBufferOverflowException.h
  ByteBufferUnderflowException.h
  Cursor.h
  GeophileException.h
  InlineSpatialObjectReferenceManager.h
  InMemorySpatialObjectReferenceManager.h
  OrderedIndex.h
  OutputArray.h
  OutputArrayBase.h
  Point2.h
  Record.h
  RegionComparison.h
  SessionMemoryBase.h
  SessionMemory.h
  Space.h
  SpatialIndex.h
  SpatialIndexFilter.h
  SpatialIndexScan.h
  SpatialObject.h
  SpatialObjectKey.h
  SpatialObjectReferenceManager.h
  SpatialObjectPointer.h
  SpatialObjectTypes.h
  Z.h
  ZArray.h
  util.h
  # Testing
  OrderedIndexFactory.h
  TestSpatialObject.h
  testbase.h
  DESTINATION include/geophile)

install(TARGETS 
  geophile
  geophiletest
  LIBRARY DESTINATION lib)
