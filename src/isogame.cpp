#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include "vao.h"
#include "shader.h"
#include <cassert>
#include "fps_camera.h"
#include "primitives.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#ifdef ENABLE_IMGUI
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#endif

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 1024;

glm::mat4 const projection{ glm::perspective(glm::radians(45.0f),
    (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f) };

float delta{};
glm::vec3 light_pos{ 5.0f, 0.0f, -5.0f };
glm::vec3 light_color{ 1.0f };

bool focus_gui{};

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (focus_gui)
        return;

    cam_process_mouse(xpos, ypos);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        focus_gui = true;
    }

    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        cam_reset_last_xy(window);
        focus_gui = false;
    }

    cam_process_keyboard(window, delta);
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
    vao objectVao{ "33" , cube_with_normals };

    shader lightShader{ "light.vert", "light.frag"};
    shader objectShader{ "object.vert", "object.frag"};

    cam_reset_last_xy(window);

    float last{};
    while (!glfwWindowShouldClose(window))
    {
        float const current{ (float)glfwGetTime() };
        delta = current - last;
        last = current;

        glfwPollEvents();

        processInput(window);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        auto const view{ cam_get_view() };

        light_pos.x = (((float)cos(glfwGetTime()) + 1) / 2.0f) * 60.0f;
        light_pos.z = (float)cos(glfwGetTime() * 10.0f);
        light_pos.y = (float)sin(glfwGetTime() * 10.0f);

        // Light Source
        lightShader.bind();
        lightVao.bind();

        lightShader.set_vec3("light_color", light_color);
        lightShader.set_mat("view", view);
        lightShader.set_mat("projection", projection);
        
        glm::mat4 model{ 1.0f };
        model = glm::translate(model, light_pos);
        model = glm::scale(model, glm::vec3(0.2f));
        lightShader.set_mat("model", model);

        glDrawArrays(GL_TRIANGLES, 0, 36);

        // Object
        objectVao.bind();
        objectShader.bind();

        objectShader.set_mat("view", view);
        objectShader.set_mat("projection", projection);

        objectShader.set_vec3("mat.ambient", { 1.0f, 0.5f, 0.31f });
        objectShader.set_vec3("mat.diffuse", { 1.0f, 0.5f, 0.31f });
        objectShader.set_vec3("mat.specular", glm::vec3{ 1.0f });
        objectShader.set_float("mat.shininess", 32.0f);

        glm::vec3 diffuseColor = light_color * glm::vec3(0.8f);
        glm::vec3 ambientColor = light_color * glm::vec3(0.2f);

        objectShader.set_vec3("light.ambient", ambientColor);
        objectShader.set_vec3("light.diffuse", diffuseColor);
        objectShader.set_vec3("light.specular", glm::vec3{0.5f});
        objectShader.set_vec3("light.pos", light_pos);

        for (int i = 0; i < 20; ++i)
        {
            glm::mat4 object_model{ 1.0f };
            object_model = glm::translate(object_model, {i*3, 0, 0});
            objectShader.set_mat("model", object_model);
            glDrawArrays(GL_TRIANGLES, 0, 36);

            object_model = glm::mat4{ 1.0f };
            object_model = glm::translate(object_model, { i * 3, 0, -3 });
            objectShader.set_mat("model", object_model);
            glDrawArrays(GL_TRIANGLES, 0, 36);

            object_model = glm::mat4{ 1.0f };
            object_model = glm::translate(object_model, { i * 3, 0, 3 });
            objectShader.set_mat("model", object_model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
#ifdef ENABLE_IMGUI
        imgui_start();

        ImGui::Begin("Main Wnd");
        ImGui::BeginDisabled();
        ImGui::Checkbox("Focus GUI ... press (G) or (I) to escape.", &focus_gui);
        ImGui::EndDisabled();
        ImGui::SliderFloat3("Light Pos", glm::value_ptr(light_pos), -1000.0f, 1000.0f);
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



void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}