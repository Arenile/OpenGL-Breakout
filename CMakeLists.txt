cmake_minimum_required(VERSION 3.10)
#add_subdirectory(include/glad/)

# set the project name
project(Breakout VERSION 0.1)

set(GLAD "${CMAKE_CURRENT_LIST_DIR}/include/glad/gl.h" "${CMAKE_CURRENT_LIST_DIR}/src/gl.c")
set(STB_IMAGE "${CMAKE_CURRENT_LIST_DIR}/include/stb_image.h")

# add the executable
add_executable(Breakout main.cpp Game.cpp Game.hpp GameLevel.hpp GameLevel.cpp BallObject.cpp BallObject.hpp GameObject.cpp GameObject.hpp ResourceManager.cpp ResourceManager.hpp SpriteRenderer.cpp SpriteRenderer.hpp Shader.cpp Shader.hpp Texture.cpp Texture.hpp ${GLAD} ${STB_IMAGE})

target_link_libraries(Breakout PUBLIC glfw)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)