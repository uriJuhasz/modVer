add_subdirectory(common)
add_subdirectory(frontend)

set(SOURCES ${SOURCES} main.cpp)
add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} frontend common)
