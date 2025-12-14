#include "Camera.h"

Camera::Camera(int width, int height, glm::vec3 position) {
	Camera::width = width;
	Camera::height = height;
	Position = position;
}

void Camera::matrix(float FOVdegree, float nearPlane, float farPlane, Shader& shader, const char* uniform) {
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);

	view = glm::lookAt(Position, Position + Orientation, Up);
    projection = glm::perspective(glm::radians(FOVdegree), (float)width / (float)height, nearPlane, farPlane);

	glUniformMatrix4fv(glGetUniformLocation(shader.ID, uniform), 1, GL_FALSE, glm::value_ptr(projection * view));
}

/* Obtain inputs from keyboard and mouse */
void Camera::Inputs(GLFWwindow* window) {
    /* Keyboard Inputs */
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        Position += speed * Orientation;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        Position += speed * -Orientation;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        Position += (speed / 2) * -glm::normalize(glm::cross(Orientation, Up));
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        Position += (speed / 2) * glm::normalize(glm::cross(Orientation, Up));;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        Position += speed * Up;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        Position += speed * -Up;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        speed = 0.004f;
    }
    else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE) {
        speed = 0.001f;
    }

    /* Mouse Inputs */
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

    /* Center the mouse on first click */
    if (firstClick) {
        glfwSetCursorPos(window, (width / 2), (height / 2));
        firstClick = false;
    }

    /* Get current mouse position */
    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);

    /* Calculate rotation along X and Y axes */
    float rotX = sensitivity * (float)(mouseY - (height / 2)) / height;
    float rotY = sensitivity * (float)(mouseX - (width / 2)) / width;

    /* Adjust orientation based on the mouse movement */
    glm::vec3 newOrientation = glm::rotate(Orientation, glm::radians(-rotX), glm::normalize(glm::cross(Orientation, Up)));

    /*Limit the rotation so that the camera doesn't flip */
    if (!((glm::angle(newOrientation, Up) <= glm::radians(5.0f)) or (glm::angle(newOrientation, -Up) <= glm::radians(5.0f)))) {
        Orientation = newOrientation;
    }

    /* Apply Y-axis rotation */
    Orientation = glm::rotate(Orientation, glm::radians(-rotY), Up);

    /* Reset mouse position to the center after each frame */
    glfwSetCursorPos(window, (width / 2), (height / 2));
}