#include "fps_camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

namespace
{
	glm::vec3 const camera_up = glm::vec3{ 0.0f, 1.0f, 0.0f };
	glm::vec3 camera_pos = glm::vec3(0.0f, 0.0f, 3.0f);
	glm::vec3 camera_front{ 0.0f, 0.0f, -1.0f };

    float yaw{ -90.0f }; // around Y movement
    float pitch{};       // around X movement

    float last_x{};
    float last_y{};
}

void cam_reset_last_xy(GLFWwindow* window)
{
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    last_x = (float)x;
    last_y = (float)y;
}

void cam_process_keyboard(GLFWwindow* window, float delta)
{
    const float cameraSpeed{ 5.0f * delta };

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera_pos += camera_front * cameraSpeed;

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera_pos -= camera_front * cameraSpeed;

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera_pos += glm::cross(camera_front, camera_up) * cameraSpeed;

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera_pos -= glm::cross(camera_front, camera_up) * cameraSpeed;

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera_pos += camera_up * cameraSpeed;

    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        camera_pos -= camera_up * cameraSpeed;
}

void cam_process_mouse(double xpos, double ypos)
{
    float xoffset = (float)xpos - last_x;
    float yoffset = last_y - (float)ypos;
    last_x = (float)xpos;
    last_y = (float)ypos;

    const float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 dir{};
    dir.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    dir.y = sin(glm::radians(pitch));
    dir.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    camera_front = glm::normalize(dir);
}

glm::mat4 cam_get_view()
{
    return glm::lookAt(camera_pos, camera_pos + camera_front, camera_up);
}
