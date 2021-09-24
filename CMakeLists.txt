cmake_minimum_required(VERSION 3.7)
project(inferior_osyncstream)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/modules)
add_subdirectory(sample)
add_subdirectory(test)

add_custom_target(ci
    DEPENDS run_sample
)
if((CMAKE_BUILD_TYPE STREQUAL "Debug") AND ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU") AND INFERIOR_OSYNCSTREAM_ENABLE_COVERAGE)
    add_dependencies(ci test_coverage)
else()
    add_dependencies(ci run_test)
endif()
