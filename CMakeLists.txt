cmake_minimum_required(VERSION 3.14)

project(Kitsune_Engine)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(freetype REQUIRED)

find_package(Bullet REQUIRED)
include_directories(${BULLET_INCLUDE_DIR})

add_executable(${PROJECT_NAME} main.cpp stb_image.hpp ft2build.hpp)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/v8_workspace/v8/v8/include)
target_link_libraries(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/v8_workspace/v8/v8/out/x64.release/obj/libv8_monolith.a)
target_link_libraries(${PROJECT_NAME} OpenGL::GL glfw freetype BulletDynamics BulletCollision LinearMath)