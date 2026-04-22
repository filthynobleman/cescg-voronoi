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



# Define project's environment variables
set(CESCG_SAMPLES_DIR "${CMAKE_SOURCE_DIR}/samples")
set(CESCG_OUTPUT_DIR "${CMAKE_BINARY_DIR}/output")

# Configure all project headers (moves them into the build directory)
configure_file("${CMAKE_SOURCE_DIR}/include/cescg/bbox.hpp" "${CMAKE_BINARY_DIR}/include/cescg/bbox.hpp")
configure_file("${CMAKE_SOURCE_DIR}/include/cescg/grid.hpp" "${CMAKE_BINARY_DIR}/include/cescg/grid.hpp")
configure_file("${CMAKE_SOURCE_DIR}/include/cescg/halfplane.hpp" "${CMAKE_BINARY_DIR}/include/cescg/halfplane.hpp")
configure_file("${CMAKE_SOURCE_DIR}/include/cescg/image.hpp" "${CMAKE_BINARY_DIR}/include/cescg/image.hpp")
configure_file("${CMAKE_SOURCE_DIR}/include/cescg/polygon.hpp" "${CMAKE_BINARY_DIR}/include/cescg/polygon.hpp")
configure_file("${CMAKE_SOURCE_DIR}/include/cescg/randgen.hpp" "${CMAKE_BINARY_DIR}/include/cescg/randgen.hpp")
configure_file("${CMAKE_SOURCE_DIR}/include/cescg/rasterizer.hpp" "${CMAKE_BINARY_DIR}/include/cescg/rasterizer.hpp")
configure_file("${CMAKE_SOURCE_DIR}/include/cescg/utils.hpp.in" "${CMAKE_BINARY_DIR}/include/cescg/utils.hpp")
configure_file("${CMAKE_SOURCE_DIR}/include/cescg/voronoi.hpp" "${CMAKE_BINARY_DIR}/include/cescg/voronoi.hpp")
configure_file("${CMAKE_SOURCE_DIR}/include/cescg/vororegion.hpp" "${CMAKE_BINARY_DIR}/include/cescg/vororegion.hpp")

# Include project headers
include_directories("${CMAKE_BINARY_DIR}/include")

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


# Compile demos
add_executable(Mosaicking "${CMAKE_SOURCE_DIR}/src/demos/mosaicking.cpp")
set_target_properties(Mosaicking PROPERTIES CXX_STANDARD 17)
if (OpenMP_FOUND)
    target_link_libraries(Mosaicking CESCG OpenMP::OpenMP_CXX)
else()
    target_link_libraries(Mosaicking CESCG)
endif()

add_executable(FrontPropagation "${CMAKE_SOURCE_DIR}/src/demos/frontprop.cpp")
set_target_properties(FrontPropagation PROPERTIES CXX_STANDARD 17)
if (OpenMP_FOUND)
    target_link_libraries(FrontPropagation CESCG OpenMP::OpenMP_CXX)
else()
    target_link_libraries(FrontPropagation CESCG)
endif()

add_executable(Stippling "${CMAKE_SOURCE_DIR}/src/demos/stippling.cpp")
set_target_properties(Stippling PROPERTIES CXX_STANDARD 17)
if (OpenMP_FOUND)
    target_link_libraries(Stippling CESCG OpenMP::OpenMP_CXX)
else()
    target_link_libraries(Stippling CESCG)
endif()

add_executable(Texture "${CMAKE_SOURCE_DIR}/src/demos/texture.cpp")
set_target_properties(Texture PROPERTIES CXX_STANDARD 17)
if (OpenMP_FOUND)
    target_link_libraries(Texture CESCG OpenMP::OpenMP_CXX)
else()
    target_link_libraries(Texture CESCG)
endif()

# Compile test applications
# add_executable(TestImage "${CMAKE_SOURCE_DIR}/src/test/test_image.cpp")
# set_target_properties(TestImage PROPERTIES CXX_STANDARD 17)
# if (OpenMP_FOUND)
#     target_link_libraries(TestImage CESCG OpenMP::OpenMP_CXX)
# else()
#     target_link_libraries(TestImage CESCG)
# endif()

# add_executable(TestVoronoi "${CMAKE_SOURCE_DIR}/src/test/test_voronoi.cpp")
# set_target_properties(TestVoronoi PROPERTIES CXX_STANDARD 17)
# if (OpenMP_FOUND)
#     target_link_libraries(TestVoronoi CESCG OpenMP::OpenMP_CXX)
# else()
#     target_link_libraries(TestVoronoi CESCG)
# endif()

# add_executable(TestVoronoiCentroidal "${CMAKE_SOURCE_DIR}/src/test/test_voronoi_centroidal.cpp")
# set_target_properties(TestVoronoiCentroidal PROPERTIES CXX_STANDARD 17)
# if (OpenMP_FOUND)
#     target_link_libraries(TestVoronoiCentroidal CESCG OpenMP::OpenMP_CXX)
# else()
#     target_link_libraries(TestVoronoiCentroidal CESCG)
# endif()