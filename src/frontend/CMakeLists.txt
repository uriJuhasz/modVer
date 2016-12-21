set (FE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

set (BOOGIE_COCO_DIR ${FE_DIR}/boogie/coco)

set (COCO_OUTPUTS
    ${BOOGIE_COCO_DIR}/Scanner.cpp
    ${BOOGIE_COCO_DIR}/Scanner.h
    ${BOOGIE_COCO_DIR}/Parser.cpp
    ${BOOGIE_COCO_DIR}/Parser.h
)

set (COCO_EXE C:/work/cocoR/coco.exe)

add_custom_command(
  OUTPUT  ${COCO_OUTPUTS}
  COMMAND ${COCO_EXE} ${BOOGIE_COCO_DIR}/boogiePL2.atg #-o ./boogie/coco
  MAIN_DEPENDENCY boogie/coco/boogiePL2.atg
#  WORKING_DIRECTORY boogie/coco
)


list(APPEND FRONTEND_SOURCES ${COCO_OUTPUTS}
    TextPosition.cpp TextPosition.h boogie/ParseTree.h boogie/ParseTree.cpp
)

include_directories (${CMAKE_BINARY_DIR})

add_library(frontend STATIC ${FRONTEND_SOURCES})
