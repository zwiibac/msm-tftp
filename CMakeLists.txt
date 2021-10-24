cmake_minimum_required(VERSION 3.10)

project(ZwiibacTftp C CXX)

include(CTest)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 20)

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost 1.75 REQUIRED log)
include_directories(${Boost_INCLUDE_DIRS})

add_subdirectory(src)
add_subdirectory(external)



