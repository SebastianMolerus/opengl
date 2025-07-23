# CMakeList.txt : CMake project for isogame, include source and define
# project specific logic here.
#

add_definitions(-DSHADERS_ROOT="${CMAKE_CURRENT_SOURCE_DIR}/shaders")
add_definitions(-DTEXTURES_ROOT="${CMAKE_CURRENT_SOURCE_DIR}/textures")
add_definitions(-DMODELS_ROOT="${CMAKE_CURRENT_SOURCE_DIR}/models")
add_definitions(-DENABLE_IMGUI)

add_executable (${PROJECT_NAME} "main.cpp" "vao.cpp" "shader.cpp" "texture.cpp" "fps_camera.cpp" "primitives.h" "mesh.cpp" "model.cpp")

target_compile_definitions(${PROJECT_NAME} PRIVATE "DEBUG=$<IF:$<CONFIG:Debug>,1,0>")

target_link_libraries(${PROJECT_NAME} PRIVATE glfw)
target_link_libraries(${PROJECT_NAME} PRIVATE glad::glad)
target_link_libraries(${PROJECT_NAME} PRIVATE glm::glm)
target_link_libraries(${PROJECT_NAME} PRIVATE imgui::imgui)
target_link_libraries(${PROJECT_NAME} PRIVATE assimp::assimp)


if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
