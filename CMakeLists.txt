cmake_minimum_required(VERSION 3.15)
project(gameProject)

set(CMAKE_CXX_STANDARD 17)

# SDL2 path
set(SDL2_DIR "${CMAKE_SOURCE_DIR}/external/SDL2")

# Include headers
include_directories("${SDL2_DIR}/include")

# Link libraries
if (WIN32)
    set(SDL2_LIB_DIR "${SDL2_DIR}/lib/x64")
endif()

link_directories(${SDL2_LIB_DIR})

add_executable(gameProject main.cpp)

# Link SDL2
target_link_libraries(gameProject SDL2 SDL2main)

# Copy DLL to build folder
add_custom_command(TARGET gameProject POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${SDL2_LIB_DIR}/SDL2.dll"
    $<TARGET_FILE_DIR:gameProject>
)