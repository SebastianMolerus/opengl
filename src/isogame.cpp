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

const unsigned int SCR_WIDTH = 1024;
const unsigned int SCR_HEIGHT = 768;

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
    vao objectVao{ "332" , cube_normals_tex };

    shader lightShader{ "light.vert", "light.frag"};
    shader objectShader{ "object.vert", "object.frag"};

    objectShader.set_mat("projection", projection);

    objectShader.sampler_to_texture("mat.diffuse", "container_diff.png", GL_RGBA);
    objectShader.sampler_to_texture("mat.specular", "container_spec.png", GL_RGBA);
    objectShader.set_float("mat.shininess", 32);

    objectShader.set_vec3("directional_light.ambient", glm::vec3{ 0.1f });
    objectShader.set_vec3("directional_light.diffuse", glm::vec3{ 0.3f });
    objectShader.set_vec3("directional_light.specular", glm::vec3{ 1.0f });
    objectShader.set_vec3("directional_light.direction", -0.3, -1.0f, 0.0f);

    objectShader.set_vec3("point_light.ambient", glm::vec3{ 0.2f });
    objectShader.set_vec3("point_light.diffuse", glm::vec3{ 0.5f });
    objectShader.set_vec3("point_light.specular", glm::vec3{ 1.0f });
    objectShader.set_float("point_light.constant", 1.0f);
    objectShader.set_float("point_light.linear", 0.09f);
    objectShader.set_float("point_light.quadratic", 0.032f);

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
        glm::vec3 cubePositions[] = {
            glm::vec3(0.0f,  0.0f,  0.0f),
            glm::vec3(2.0f,  5.0f, -15.0f),
            glm::vec3(-1.5f, -2.2f, -2.5f),
            glm::vec3(-3.8f, -2.0f, -12.3f),
            glm::vec3(2.4f, -0.4f, -3.5f),
            glm::vec3(-1.7f,  3.0f, -7.5f),
            glm::vec3(1.3f, -2.0f, -2.5f),
            glm::vec3(1.5f,  2.0f, -2.5f),
            glm::vec3(1.5f,  0.2f, -1.5f),
            glm::vec3(-1.3f,  1.0f, -1.5f)
        };


        objectVao.bind();
        objectShader.bind();

        objectShader.set_mat("view", view);
        objectShader.set_vec3("view_pos", cam_get_pos());
        objectShader.set_vec3("point_light.position", light_pos);

        for (unsigned int i = 0; i < 10; i++)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            float angle = 20.0f * i;
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            objectShader.set_mat("model", model);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

#ifdef ENABLE_IMGUI
        imgui_start();

        ImGui::Begin("Main Wnd");
        ImGui::BeginDisabled();
        ImGui::Checkbox("Focus GUI ... press (G) or (I) to escape.", &focus_gui);
        ImGui::EndDisabled();
        ImGui::SliderFloat3("Light Pos", glm::value_ptr(light_pos), -20.0f, 20.0f);
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