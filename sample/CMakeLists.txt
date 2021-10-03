cmake_minimum_required(VERSION 3.7)
enable_language(CXX)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/../cmake/modules)

#
# C++ version
#
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON) #...is required...
set(CMAKE_CXX_EXTENSIONS OFF) #...without compiler extensions like gnu++11

#
# Set our project name
#
project(sample)

#
# Source files
#
set(sample_src
    "./sample.cpp"
)

#
# find libraries and include
#

# find thread library
find_package(Threads REQUIRED)

#
# Compile
#
add_executable(sample ${sample_src})
target_include_directories(sample PRIVATE "../include")
target_link_libraries(sample Threads::Threads)

#
# add Run task
#
add_custom_target(run_sample
    COMMAND sample
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    DEPENDS sample
    COMMENT "Execute produced executable."
)
