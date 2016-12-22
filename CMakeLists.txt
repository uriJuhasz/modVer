project(modVer)
cmake_minimum_required(VERSION 2.8)

set (modVer_VERSION_MAJOR 1)
set (modVer_VERSION_MINOR 0)

set (CMAKE_VERBOSE_MAKEFILE on)


#set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic -Wall -std=c++17")

find_package(Boost 1.62.0 COMPONENTS)
include_directories(SYSTEM ${Boost_INCLUDE_DIRS} )
include_directories( ${CMAKE_SOURCE_DIR} )
include_directories( ${CMAKE_SOURCE_DIR}/src )


add_subdirectory(src)


