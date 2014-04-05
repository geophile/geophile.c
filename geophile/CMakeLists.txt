cmake_minimum_required(VERSION 2.8)

add_library(geophile SHARED
  Box2.cpp
  ByteBuffer.cpp
  IntList.cpp
  IntSet.cpp
  Point2.cpp
  Region.cpp
  RegionPool.cpp
  RegionQueue.cpp
  SessionMemory.cpp
  Space.cpp
  SpatialIndex.cpp
  SpatialIndexScan.cpp
  SpatialObjectArray.cpp
  SpatialObjectTypes.cpp
  ZArray.cpp)

install(TARGETS geophile DESTINATION lib)

install(FILES 
  Box2.h
  ByteBufferOverflowException.h
  ByteBufferUnderflowException.h
  Cursor.h
  GeophileException.h
  OrderedIndex.h
  Point2.h
  Record.h
  SessionMemory.h
  Space.h
  SpatialIndex.h
  SpatialIndexFilter.h
  SpatialIndexScan.h
  SpatialObject.h
  SpatialObjectArray.h
  SpatialObjectKey.h
  SpatialObjectTypes.h
  DESTINATION include)

install(TARGETS 
  geophile
  LIBRARY DESTINATION lib)
