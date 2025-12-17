#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(linker, "/NODEFAULTLIB:MSVCRT")

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <thread>
#include <chrono>

#include "Shader.h"
#include "Camera.h"
#include "Inputs.h"

float deltaTime = 0.0f;
float lastFrame = 0.0f;
int fpsCount = 0;
std::chrono::steady_clock::time_point fpsStartTime;
float averageFPS = 0;
float lowestFps = -1;
float highestFps = -1;

const int Clocks = 30;

const unsigned int screenWidth = 1920;
const unsigned int screenHeight = 1080;

bool wireFrameMode = false;

void processInput(GLFWwindow* window);

/* Cube vertices (positions and color) */
float CubeVertices[] = {
    // Positions      //color bits
    // Back face
    1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    // Front face
    0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    // Left face
    0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
    // Right face
    1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
    1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
    1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
    // Bottom face
    1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
    1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
    // Top face
    0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f
};

unsigned int CubeIndices[] = {
    // Front face
    0, 3, 1, 0, 2, 3,
    // Back face
    4, 7, 5, 4, 6, 7,
    // Left face
    8, 11, 9, 8, 10, 11,
    // Right face
    12, 15, 13, 12, 14, 15,
    // Bottom face
    16, 19, 17, 16, 18, 19,
    // Top face
    20, 23, 21, 20, 22, 23
};

int main() {
    GLFWwindow* window;

    /* Initialize the GLFW library */
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(screenWidth, screenHeight, "Adapt", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    /* Initialize glad */
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    /* Load image file */
    int width, height;
    int channels;
    unsigned char* pixels = stbi_load("assets/textures/ObsidianAxe.png", &width, &height, &channels, 4);

    if (!pixels) {
    std::cerr << "Failed to load texture" << std::endl;
    glfwDestroyWindow(window);
    glfwTerminate();
    return -1;
    }

    /* Change window icon */
    GLFWimage images[1]{};
    images[0].width = width;
    images[0].height = height;
    images[0].pixels = pixels;

    glfwSetWindowIcon(window, 1, images);

    glDisable(GL_CULL_FACE);

    /* Load shaders */
    Shader shader("assets/shaders/vertShader.glsl", "assets/shaders/fragShader.glsl");

    /* Set up the Vertex Array Object, Vertex Buffer Object, and Element Buffer Object for the cube */
    unsigned int vertexArrayObject, vertexBufferObject, elementBufferObject;
    glGenVertexArrays(1, &vertexArrayObject);
    glGenBuffers(1, &vertexBufferObject);
    glGenBuffers(1, &elementBufferObject);

    glBindVertexArray(vertexArrayObject);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(CubeVertices), CubeVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferObject);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(CubeIndices), CubeIndices, GL_STATIC_DRAW);

    /* Define vertex attributes */
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    /* Enable the Depth Buffer */
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    Camera camera(screenWidth, screenHeight, glm::vec3(0.5f, 0.5f, 5.0f));

    fpsStartTime = std::chrono::steady_clock::now();

    /* Render Loop */
    while (!glfwWindowShouldClose(window)) {
        auto cycleStartTime = std::chrono::steady_clock::now();

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        float fps = 1.0f / deltaTime;
        if (lowestFps == -1 || fps < lowestFps)
            lowestFps = fps;
        if (highestFps == -1 || fps > highestFps)
            highestFps = fps;

        fpsCount++;
        std::chrono::steady_clock::time_point currentTimePoint = std::chrono::steady_clock::now();

        if (std::chrono::duration_cast<std::chrono::seconds>(currentTimePoint - fpsStartTime).count() >= 1) {
            averageFPS = fpsCount;
            std::cout << "Average FPS: " << averageFPS << std::endl;

            lowestFps = -1;
            highestFps = -1;
            fpsCount = 0;
            fpsStartTime = currentTimePoint;
        }

        /* Process input */
        Inputs::Update(window, camera);

        /* Render here */
        glViewport(0, 0, screenWidth, screenHeight);

        glDisable(GL_CULL_FACE);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        glm::mat4 view = glm::lookAt(glm::vec3(2.0f, 2.0f, 3.0f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
        glm::mat4 cameraMatrix = projection * view;
        glUniformMatrix4fv(glGetUniformLocation(shader.ID, "cameraMatrix"), 1, GL_FALSE, glm::value_ptr(cameraMatrix));
        //std::cout << "Camera position working / set" << std::endl;
        camera.matrix(45.0f, 0.1f, 100.0f, shader, "cameraMatrix");

        /* Draw the cubes */
        //std::cout << "Shader ID: " << shader.ID << std::endl;
        GLint modelLoc = glGetUniformLocation(shader.ID, "model");
        //std::cout << "Model uniform location: " << modelLoc << std::endl;
        shader.use();
        glBindVertexArray(vertexArrayObject);
        
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));

        //std::cout << "Drawing cube" << std::endl;
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();

        auto cycleEndTime = std::chrono::steady_clock::now();
        auto cycleDuration = std::chrono::duration_cast<std::chrono::milliseconds>(cycleEndTime - cycleStartTime).count();
        //std::cout << "Cycle runtime: " << cycleDuration << " ms" << std::endl;
    }

    /* Clean up */
    glDeleteVertexArrays(1, &vertexArrayObject);
    glDeleteBuffers(1, &vertexBufferObject);
    glDeleteBuffers(1, &elementBufferObject);

    stbi_image_free(pixels);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}