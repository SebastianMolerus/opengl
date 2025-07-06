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

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

float mix_value{ 0.1f };

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

    int const glad{ gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) };
    assert(glad != 0);

    glEnable(GL_DEPTH_TEST);

    return window;
}

int main()
{
    GLFWwindow* window{ init() };

    float vertices[] = {
        // positions        // texture coords
        -1.0f, 0.0f, 0.0f, 0.0, 0.5,
        0.0f, 0.5f, 0.0f,  0.5, 0.75,
        1.0f, 0.0f, 0.0f,   1.0, 0.5,
        0.0f, -0.5f, 0.0f, 0.5, 0.25,
        0.0f, 0.0f, 0.0f, 0.5, 0.5
    };

    unsigned indices[] = {
        0, 4, 1,
        4, 1, 2,
        4, 2, 3,
        4, 3, 0
    };

    vao vao1{ "32", vertices };
    vao1.ebo(indices);

    texture t;
    t.another_texture("grass.png", GL_RGBA);

    shader s{ "1.vert", "1.frag", &t };

    s.sampler_to_texture("tile_texture", "grass.png");

    glm::mat4 view = glm::mat4(1.0f);
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -1.0f));

    glm::mat4 projection = glm::mat4(1.0f);
    projection = glm::ortho(-400.0f, 400.0f, -300.0f, 300.0f, 0.1f, 100.0f);

    s.set_mat("view", view);
    s.set_mat("projection", projection);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE | GL_FILL);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        constexpr float scale_factor{ 32.0f };

        for (int x = 0; x < 10; ++x) {
            float x_dir = scale_factor - (x * scale_factor);
            float y_dir = scale_factor / 2.0f - (x * scale_factor / 2.0f);
            for (int y = 0; y < 10; ++y)
            {
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, { x_dir, y_dir, 0.0f });
                model = glm::scale(model, { scale_factor, scale_factor, 0.0f });
                s.set_mat("model", model);
                glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);
                x_dir += scale_factor;
                y_dir -= scale_factor / 2.0f;
            }
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        mix_value += 0.01f;
        if (mix_value > 1.0f)
            mix_value = 1.0f;
    }

    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        mix_value -= 0.01f;
        if (mix_value < 0.0f)
            mix_value = 0.0f;
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}