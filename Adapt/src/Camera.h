#pragma once

#ifndef CAMERA_CLASS_H
#define CAMERA_CLASS_H
#define GLM_ENABLE_EXPERIMENTAL

#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtx\rotate_vector.hpp>
#include <glm\gtx\vector_angle.hpp>
#include "shader.h"

class Camera {
public:
	/* Stores the main variables */
	glm::vec3 Position;
	glm::vec3 Orientation = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);

	/* Prevents Camera Jumping */
	bool firstClick = true;

	int width, height;

	/* Adjusts speed of the camera while looking around */
	float speed = 100.0f;
	float sensitivity = 100.0f;

	/* Camera Constructor */
	Camera(int width, int height, glm::vec3 position);
	
	void matrix(float FOVdegree, float nearPlane, float farPlane, Shader& shader, const char* uniform);
	void Inputs(GLFWwindow* window);
};

#endif