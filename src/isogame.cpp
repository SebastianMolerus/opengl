#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include "vao.h"
#include "shader.h"
#include "texture.h"
#include <cassert>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define ENABLE_IMGUI

#ifdef ENABLE_IMGUI
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#endif

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

const unsigned int SCR_WIDTH = 1024;
const unsigned int SCR_HEIGHT = 768;

glm::mat4 const projection{ glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f) };
glm::vec3 const camera_up = glm::vec3{ 0.0f, 1.0f, 0.0f };

glm::vec3 camera_pos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 camera_front{0.0f, 0.0f, -1.0f};

glm::vec3 light_pos(1.2f, 1.0f, -2.0f);
glm::vec3 light_color(1.0f);

float delta{};
float yaw{-90.0f};
float pitch{};

bool focus_imgui{ false };

float lastX = SCR_WIDTH / 2.0f, lastY = SCR_HEIGHT / 2.0f;

float const cube[] = {
        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,

        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,

         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,

        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f
};

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    static bool firstMouse{ true };
    if (focus_imgui) {
        firstMouse = true;
        return;
    }
    
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

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

GLFWwindow* init()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    assert(window != NULL);

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    int const glad{ gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) };
    assert(glad != 0);

    glEnable(GL_DEPTH_TEST);

    return window;
}

#ifdef ENABLE_IMGUI
void init_imgui(GLFWwindow* window)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();
}

void imgui_start()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void imgui_stop()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
#endif

int main()
{
    GLFWwindow* window{ init() };
#ifdef ENABLE_IMGUI
    init_imgui(window);
#endif

    vao lightVao{ "3" , cube };
    vao objectVao{ "3" , cube };

    shader lightShader{ "1.vert", "light.frag"};
    shader objectShader{ "1.vert", "object.frag"};

    float last{};
    while (!glfwWindowShouldClose(window))
    {
        float const current{ (float)glfwGetTime() };
        delta = current - last;
        last = current;

        glfwPollEvents();

        processInput(window);
        glm::mat4 const view{ glm::lookAt(camera_pos, camera_pos + camera_front, camera_up) };

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 model{ 1.0f };
        model = glm::rotate(model, (float)glfwGetTime(), glm::vec3{0, 1.0f, 0.0});
        model = glm::translate(model, light_pos);
        model = glm::scale(model, glm::vec3(0.2f));

        lightShader.bind();
        lightVao.bind();

        lightShader.set_mat("model", model);
        lightShader.set_mat("view", view);
        lightShader.set_mat("projection", projection);

        lightShader.set_vec3("light_color", light_color);

        glDrawArrays(GL_TRIANGLES, 0, 36);

        objectVao.bind();
        objectShader.bind();

        objectShader.set_mat("model", glm::mat4{ 1.0f });
        objectShader.set_mat("view", view);
        objectShader.set_mat("projection", projection);

        objectShader.set_vec3("object_color", glm::vec3(1.0f, 0.5f, 0.31f));
        objectShader.set_vec3("light_color", light_color);

        glDrawArrays(GL_TRIANGLES, 0, 36);

#ifdef ENABLE_IMGUI
        imgui_start();
        //ImGui::ShowDemoWindow(); // Show demo window! :)

        ImGui::Begin("Main Wnd");
        ImGui::BeginDisabled();
        ImGui::Checkbox("Gui focus (G)", &focus_imgui);
        ImGui::SameLine();
        bool p{ !focus_imgui };
        ImGui::Checkbox("Free Cam (I)", &p);
        ImGui::EndDisabled();
        ImGui::Text("Camera Pos X:%f Y:%f Z:%f", camera_pos.x, camera_pos.y, camera_pos.z);
        ImGui::SliderFloat3("Light Pos", glm::value_ptr(light_pos), -100.0f, 100.0f);
        ImGui::SliderFloat3("Light Color", glm::value_ptr(light_color), 0.0, 1.0f);
        ImGui::End();

        imgui_stop();
#endif
        glfwSwapBuffers(window);
    }

#ifdef ENABLE_IMGUI
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
#endif
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        focus_imgui = true;
    }

    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        focus_imgui = false;
    }

    if (focus_imgui)
        return;

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

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}