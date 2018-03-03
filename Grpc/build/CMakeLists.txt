cmake_minimum_required(VERSION 3.0)
project(GrpcApp)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -std=c++11 -O0 -Wall -g2 -ggdb")

message(------------------------------------------------)
message(project_name: ${PROJECT_NAME})
message(project_src_dir: ${PROJECT_SOURCE_DIR})
message(project_bin_dir: ${PROJECT_BINARY_DIR})

add_subdirectory(../Grpc bin/mygrpc)
add_subdirectory(../Lib bin/baselib)

set(MAIN_SRC ../src/main.cpp) 
add_executable(appGrpc ${MAIN_SRC}) 
target_link_libraries(appGrpc base grpc++ protobuf mygrpc boost_system)

include_directories("../Grpc/include")

