# CMakeList.txt : CMake project for isogame, include source and define
# project specific logic here.
#

add_definitions(-DSHADERS_ROOT="${CMAKE_CURRENT_SOURCE_DIR}/shaders")
add_definitions(-DTEXTURES_ROOT="${CMAKE_CURRENT_SOURCE_DIR}/textures")

add_executable (isogame "isogame.cpp" "vao.cpp" "vao.h" "shader.h" "shader.cpp" "texture.cpp" )

target_link_libraries(isogame PRIVATE glfw)
target_link_libraries(isogame PRIVATE glad::glad)
target_link_libraries(isogame PRIVATE glm::glm)
target_link_libraries(isogame PRIVATE imgui::imgui)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET isogame PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
