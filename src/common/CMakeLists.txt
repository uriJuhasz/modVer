include_directories(..)
set(COMMON_SOURCES Log.cpp Log.h data_types.h data_types.cpp)

add_library(common ${COMMON_SOURCES})
