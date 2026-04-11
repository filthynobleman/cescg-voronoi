cmake_minimum_required(VERSION 3.22)
project(CESCGVoronoi LANGUAGES C CXX)

# Configure output locations
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
    string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG )
    set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "${CMAKE_BINARY_DIR}/lib")
    set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "${CMAKE_BINARY_DIR}/lib")
    set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "${CMAKE_BINARY_DIR}/bin")
endforeach( OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES )

# Include dependencies
include_directories("${CMAKE_SOURCE_DIR}/deps/stb")
include_directories("${CMAKE_SOURCE_DIR}/deps/glm")

# Find OpenMP, if available
find_package(OpenMP)



# Include project headers
include_directories("${CMAKE_SOURCE_DIR}/include")

# Compile project library
add_library(CESCG STATIC    "${CMAKE_SOURCE_DIR}/src/cescg/image.cpp"
                            "${CMAKE_SOURCE_DIR}/src/cescg/randgen.cpp"
                            "${CMAKE_SOURCE_DIR}/src/cescg/bbox.cpp"
                            "${CMAKE_SOURCE_DIR}/src/cescg/halfplane.cpp"
                            "${CMAKE_SOURCE_DIR}/src/cescg/polygon.cpp"
                            "${CMAKE_SOURCE_DIR}/src/cescg/vororegion.cpp"
                            "${CMAKE_SOURCE_DIR}/src/cescg/voronoi.cpp"
                            "${CMAKE_SOURCE_DIR}/src/cescg/rasterizer.cpp")
set_target_properties(CESCG PROPERTIES CXX_STANDARD 17)
if (OpenMP_FOUND)
    target_link_libraries(CESCG OpenMP::OpenMP_CXX)
endif()


# Compile test applications
add_executable(TestImage "${CMAKE_SOURCE_DIR}/src/test/test_image.cpp")
set_target_properties(TestImage PROPERTIES CXX_STANDARD 17)
if (OpenMP_FOUND)
    target_link_libraries(TestImage CESCG OpenMP::OpenMP_CXX)
else()
    target_link_libraries(TestImage CESCG)
endif()

add_executable(TestVoronoi "${CMAKE_SOURCE_DIR}/src/test/test_voronoi.cpp")
set_target_properties(TestVoronoi PROPERTIES CXX_STANDARD 17)
if (OpenMP_FOUND)
    target_link_libraries(TestVoronoi CESCG OpenMP::OpenMP_CXX)
else()
    target_link_libraries(TestVoronoi CESCG)
endif()

# add_executable(TestVoronoiCentroidal "${CMAKE_SOURCE_DIR}/src/test/test_voronoi_centroidal.cpp")
# set_target_properties(TestVoronoiCentroidal PROPERTIES CXX_STANDARD 17)
# if (OpenMP_FOUND)
#     target_link_libraries(TestVoronoiCentroidal CESCG OpenMP::OpenMP_CXX)
# else()
#     target_link_libraries(TestVoronoiCentroidal CESCG)
# endif()