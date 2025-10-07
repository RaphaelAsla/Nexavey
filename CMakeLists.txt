cmake_minimum_required(VERSION 3.10)
project(Nexavey)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(src/engine)
include_directories(src/engine/systems)
include_directories(libs/tinyobjloader)
include_directories(libs/imageloader)

file(GLOB_RECURSE CORE_SOURCES src/engine/*.cpp src/engine/*.hpp)

add_executable(Nexavey
    src/main.cpp
	${CORE_SOURCES}
)

target_link_libraries(Nexavey
    glfw
    vulkan
    dl
    pthread
    X11
    Xxf86vm
    Xrandr
    Xi
)

add_custom_target(compile_shaders ALL
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/compile_shaders.sh ${SHADER_DIR} ${SHADER_OUT_DIR}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Compiling all shaders"
    VERBATIM
)

add_dependencies(Nexavey compile_shaders)

add_custom_target(clean_shaders
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${SHADER_OUT_DIR}
    COMMENT "Cleaning compiled shaders"
)

add_custom_target(run
    COMMAND ${CMAKE_COMMAND} -E env LD_LIBRARY_PATH=${CMAKE_BINARY_DIR}/src/engine ${CMAKE_BINARY_DIR}/Nexavey
    DEPENDS Nexavey
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
