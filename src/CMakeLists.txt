# CMakeList.txt : CMake project for isogame, include source and define
# project specific logic here.
#

add_definitions(-DSHADERS_ROOT="${CMAKE_CURRENT_SOURCE_DIR}/shaders")
add_definitions(-DTEXTURES_ROOT="${CMAKE_CURRENT_SOURCE_DIR}/textures")
add_definitions(-DENABLE_IMGUI)

add_executable (isogame "isogame.cpp" "vao.cpp" "vao.h" "shader.h" "shader.cpp" "texture.cpp" "fps_camera.cpp" "fps_camera.h" "primitives.h")

target_link_libraries(isogame PRIVATE glfw)
target_link_libraries(isogame PRIVATE glad::glad)
target_link_libraries(isogame PRIVATE glm::glm)
target_link_libraries(isogame PRIVATE imgui::imgui)
target_link_libraries(isogame PRIVATE assimp::assimp)


if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET isogame PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
