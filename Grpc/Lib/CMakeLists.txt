cmake_minimum_required(VERSION 3.0)
project(base)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fPIC")
set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -std=c++11 -fPIC -O0 -Wall -g2 -ggdb")

message(------------------------------------------------)
find_package(Boost
             1.57.0
             COMPONENTS
             REQUIRED)

message(project_name: ${PROJECT_NAME})
message(project_src_dir: ${PROJECT_SOURCE_DIR})
message(project_bin_dir: ${PROJECT_BINARY_DIR})

add_subdirectory(Thread)
add_subdirectory(Semaphore)
add_subdirectory(Timer)

#reference the objects using an expression of the form $<TARGET_OBJECTS:objlib> as a source
add_library(${PROJECT_NAME} 
            SHARED
            $<TARGET_OBJECTS:thread>
            $<TARGET_OBJECTS:semaphore>
            $<TARGET_OBJECTS:timer>)

#install(TARGETS ${PROJECT_NAME} DESTINATION ../build/bin)

