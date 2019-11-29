#ifndef CONTROLS_H
#define CONTROLS_H

#include <glm/glm.hpp>

void setControllerWindow(GLFWwindow* winArg);
void computeMatricesFromInputs();
glm::mat4 getViewMatrix();
glm::mat4 getProjectionMatrix();

void changeSpeed();
void changeHorizMouseSpeed();
void changeVertMouseSpeed();
void changeFOV();
void changeViewType();
void resetCamera();

#endif
