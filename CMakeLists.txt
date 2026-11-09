cmake_minimum_required(VERSION 3.15)
project(OpenAkinator LANGUAGES C)

set(PROGRAM_NAME open-akinator)

set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

set(SRC_FILES
    ${SOURCE_DIR}/main.c
    ${SOURCE_DIR}/game.c
    ${SOURCE_DIR}/util.c
)

add_executable(${PROGRAM_NAME} ${SRC_FILES})

target_compile_options(${PROGRAM_NAME} PRIVATE -Wall -Wextra -pedantic)

target_include_directories(${PROGRAM_NAME} PRIVATE ${INCLUDE_DIR} ${SOURCE_DIR})

target_link_libraries(${PROGRAM_NAME} PRIVATE m)

add_executable(debug ${SRC_FILES})

target_compile_definitions(debug PRIVATE DEBUG)

target_compile_options(debug PRIVATE 
    -Wall 
    -Wextra 
    -pedantic
    -static 
    -fno-builtin-strlen 
    -g
)

target_include_directories(debug PRIVATE ${INCLUDE_DIR} ${SOURCE_DIR})

target_link_libraries(debug PRIVATE m)