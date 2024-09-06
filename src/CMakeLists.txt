set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Add the core source files
set(CORE_SOURCES
    core/Engine.cpp
    core/Window.cpp
    core/Input.cpp
    core/Logger.cpp
)

# Add the graphics source files
set(GRAPHICS_SOURCES
    graphics/Renderer.cpp
)

# Create the main executable
add_executable(GraphicsEngine ${CORE_SOURCES} ${GRAPHICS_SOURCES})

# Link libraries to the executable
target_link_libraries(GraphicsEngine
    glew
    glfw
    ${OPENGL_LIBRARIES}
)
