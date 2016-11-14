project(modVer)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_STANDARD 14)
SET(CMAKE_CXX_FLAGS "-std=c++14 -pedantic -Wall")

list(APPEND SRC_LIST src/common/Log.cpp )
list(APPEND SRC_LIST src/frontend/parser/ParserBase.cpp src/frontend/parser/TextPos.cpp)
list(APPEND SRC_LIST src/frontend/boogie/manual/Parser.cpp)
list(APPEND SRC_LIST test/AVLTree.bpl)

aux_source_directory(. SRC_LIST)
include_directories(src)
add_executable(${PROJECT_NAME} ${SRC_LIST})
