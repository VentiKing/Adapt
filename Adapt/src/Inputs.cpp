#include <iostream>

#include "Inputs.h"
#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>

/* Global Variables input state */
static bool wireFrameMode = false;
static bool isFullscreen = false;
static bool f11Pressed = false;
static bool lPressed = false;

static int windowedWidth = 1920;
static int windowedHeight = 1080;
static int windowedX = 0;
static int windowedY = 0;

namespace Inputs {
    void Update(GLFWwindow* window, Camera& camera) {
        /* ---------- Application Controls ---------- */

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        // Fullscreen toggle (F11)
        if (glfwGetKey(window, GLFW_KEY_F11) == GLFW_PRESS && !f11Pressed) {
            f11Pressed = true;

            if (!isFullscreen) {
                GLFWmonitor* monitor = glfwGetPrimaryMonitor();
                const GLFWvidmode* mode = glfwGetVideoMode(monitor);

                glfwGetWindowPos(window, &windowedX, &windowedY);
                glfwGetWindowSize(window, &windowedWidth, &windowedHeight);

                glfwSetWindowMonitor(
                    window,
                    monitor,
                    0, 0,
                    mode->width,
                    mode->height,
                    mode->refreshRate
                );

                isFullscreen = true;
            }
            else {
                glfwSetWindowMonitor(
                    window,
                    nullptr,
                    windowedX,
                    windowedY,
                    windowedWidth,
                    windowedHeight,
                    0
                );

                isFullscreen = false;
            }
        }
        if (glfwGetKey(window, GLFW_KEY_F11) == GLFW_RELEASE) {
            f11Pressed = false;
        }
        // Wireframe toggle (L)
        if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS && !lPressed) {
            lPressed = true;
            wireFrameMode = !wireFrameMode;

            glPolygonMode(
                GL_FRONT_AND_BACK,
                wireFrameMode ? GL_LINE : GL_FILL
            );
        }
        if (glfwGetKey(window, GLFW_KEY_L) == GLFW_RELEASE) {
            lPressed = false;
        }

        /* ---------- Camera Keyboard Movement ---------- */

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            camera.Position += camera.speed * camera.Orientation;

        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            camera.Position -= camera.speed * camera.Orientation;

        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            camera.Position -= (camera.speed / 2.0f) *
            glm::normalize(glm::cross(camera.Orientation, camera.Up));

        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            camera.Position += (camera.speed / 2.0f) *
            glm::normalize(glm::cross(camera.Orientation, camera.Up));

        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
            camera.Position += camera.speed * camera.Up;

        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
            camera.Position -= camera.speed * camera.Up;

        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            camera.speed = 0.004f;
        else
            camera.speed = 0.001f;

        /* ---------- Mouse Look ---------- */

        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

        if (camera.firstClick) {
            glfwSetCursorPos(window, camera.width / 2, camera.height / 2);
            camera.firstClick = false;
        }

        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);

        float rotX = camera.sensitivity *
            (float)(mouseY - (camera.height / 2)) / camera.height;

        float rotY = camera.sensitivity *
            (float)(mouseX - (camera.width / 2)) / camera.width;

        glm::vec3 newOrientation =
            glm::rotate(
                camera.Orientation,
                glm::radians(-rotX),
                glm::normalize(glm::cross(camera.Orientation, camera.Up))
            );

        if (!(
            glm::angle(newOrientation, camera.Up) <= glm::radians(5.0f) ||
            glm::angle(newOrientation, -camera.Up) <= glm::radians(5.0f)
            ))
        {
            camera.Orientation = newOrientation;
        }

        camera.Orientation =
            glm::rotate(
                camera.Orientation,
                glm::radians(-rotY),
                camera.Up
            );

        glfwSetCursorPos(window, camera.width / 2, camera.height / 2);
    }
}