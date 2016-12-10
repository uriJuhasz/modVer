project(modVer)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_STANDARD 14)
SET(CMAKE_CXX_FLAGS "-std=c++14 -pedantic -Wall")

list(APPEND SRC_LIST src/common/Log.cpp )
list(APPEND SRC_LIST src/frontend/parser/ParserBase.cpp src/frontend/parser/TextPos.cpp)
list(APPEND SRC_LIST src/frontend/boogie/coco/Parser.cpp src/frontend/boogie/coco/Scanner.cpp)
#list(APPEND SRC_LIST src/frontend/boogie/manual/Parser.cpp src/frontend/boogie/AST.h)
#list(APPEND SRC_LIST test/AVLTree.bpl)

aux_source_directory(. SRC_LIST)
include_directories(src)
include_directories(SYSTEM ../../workspace4.7/boost_1_62_0)
add_executable(${PROJECT_NAME} ${SRC_LIST})

add_custom_command(OUTPUT src/frontend/boogie/coco/Scanner.cpp 
                          src/frontend/boogie/coco/Scanner.h
                          src/frontend/boogie/coco/Parser.cpp
                          src/frontend/boogie/coco/Parser.h
                   COMMAND C:/work/cocoR/coco.exe boogiePL2.atg
                   MAIN_DEPENDENCY src/frontend/boogie/coco/boogiePL2.atg
                   WORKING_DIRECTORY src/frontend/boogie/coco
                    )

set(CMAKE_VERBOSE_MAKEFILE on)

