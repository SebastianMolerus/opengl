#ifndef __FPS_CAMERA_H__
#define __FPS_CAMERA_H__

#include <glm/glm.hpp>

struct GLFWwindow;

void cam_reset_last_xy(GLFWwindow*);

void cam_process_keyboard(GLFWwindow*, float);

void cam_process_mouse(double xpos, double ypos);

glm::mat4 cam_get_view();

glm::vec3 cam_get_pos();

#endif