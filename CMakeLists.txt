cmake_minimum_required( VERSION 2.6 )

project( GeometryShader )

set( CMAKE_BUILD_TYPE Debug)

# Debug Normal
add_subdirectory( "${CMAKE_SOURCE_DIR}/DebugNormal" 
  "${CMAKE_BINARY_DIR}/DebugNormal-${CMAKE_BUILD_TYPE}" )
# Culling
add_subdirectory( "${CMAKE_SOURCE_DIR}/Culling" 
  "${CMAKE_BINARY_DIR}/Culling-${CMAKE_BUILD_TYPE}" )
# Splitting
add_subdirectory( "${CMAKE_SOURCE_DIR}/Splitting" 
  "${CMAKE_BINARY_DIR}/Splitting-${CMAKE_BUILD_TYPE}" )
