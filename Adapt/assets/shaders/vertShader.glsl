#version 330 core

layout (location = 0) in vec3 aPos;      // Vertex position
layout (location = 1) in vec3 aColour;   // Vertex color

out vec3 ourColour;                      // Output color to fragment shader

uniform mat4 cameraMatrix;                  // Camera transformation matrix
uniform mat4 model;                      // Model transformation matrix

void main() {
    // Apply both model and camera transformations
    gl_Position = cameraMatrix * model * vec4(aPos, 1.0);
    ourColour = aColour;
}