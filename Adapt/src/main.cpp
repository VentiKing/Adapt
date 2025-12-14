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
#include "Chunk.h"

#define STB_IMAGE_IMPLEMENTATION

float deltaTime = 0.0f;
float lastFrame = 0.0f;
int fpsCount = 0;
std::chrono::steady_clock::time_point fpsStartTime;
float avgFps = 0;
float lowestFps = -1;
float highestFps = -1;

const int clocks = 30;

const unsigned int screenWidth = 1920;
const unsigned int screenHeight = 1080;

void processInput(GLFWwindow* window);

/* Cube vertices (positions and color) */
float CubeVertices[] = {
    // Positions       color
    // Front face
    1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    // Back face
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

void globalFunction() {
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

    cChunk Chunk();
    globalFunction();

    /* Load image file */
    int width, height;
    int channels;   // Number of color components
    unsigned char* pixels = stbi_load("assets/textures/ObsidianAxe.png", &width, &height, &channels, 4);

    if (!pixels) { // Check if image loading succeeded
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

    glEnable(GL_CULL_FACE);  // Enable face culling


    /* Load shaders */
    Shader shader("assets/shaders/vertShader.glsl", "assets/shaders/fragShader.glsl");

    

    /* Set up the VAO, VBO, and EBO for the cube */
    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(CubeVertices), CubeVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(CubeIndices), CubeIndices, GL_STATIC_DRAW);

    /* Define vertex attributes */
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0); // Unbind VAO

    /* Enable the Depth Buffer */
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);     // pixels that are closer (less distance away) should be drawn over

    Camera camera(screenWidth, screenHeight, glm::vec3(0.0f, 0.0f, 2.0f));

    fpsStartTime = std::chrono::steady_clock::now();

    /* Render Loop */
    while (!glfwWindowShouldClose(window)) {
        // Start measuring time for this cycle
        auto cycleStartTime = std::chrono::steady_clock::now();

        // Delta Time
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // FPS Calculations
        float fps = 1.0f / deltaTime;
        if (lowestFps == -1 || fps < lowestFps)
            lowestFps = fps;
        if (highestFps == -1 || fps > highestFps)
            highestFps = fps;

        fpsCount++;
        std::chrono::steady_clock::time_point currentTimePoint = std::chrono::steady_clock::now();

        if (std::chrono::duration_cast<std::chrono::seconds>(currentTimePoint - fpsStartTime).count() >= 1) {
            avgFps = fpsCount;
            std::cout << "Average FPS: " << avgFps << std::endl;

            lowestFps = -1;
            highestFps = -1;
            fpsCount = 0;
            fpsStartTime = currentTimePoint;
        }

        /* Process input */
        processInput(window);

        /* Render here */
        glViewport(0, 0, screenWidth, screenHeight);  // Sets viewport to window size

        glCullFace(GL_BACK);  // Cull back face for optimization
        glFrontFace(GL_CW);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);  // Clear background color
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // Clear color and depth buffers

        camera.Inputs(window);  // Handle camera input
        camera.matrix(45.0f, 0.1f, 100.0f, shader, "camMatrix");  // Update projection and view matrices

        /* Draw the cubes matrix */
        shader.use();  // Use the shader program
        glBindVertexArray(VAO);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();

        /* End measuring time for this cycle and log the duration */
        auto cycleEndTime = std::chrono::steady_clock::now();
        auto cycleDuration = std::chrono::duration_cast<std::chrono::milliseconds>(cycleEndTime - cycleStartTime).count();
        std::cout << "Cycle runtime: " << cycleDuration << " ms" << std::endl;
    }

    /* Clean up */
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    stbi_image_free(pixels);  // Free loaded image memory
    glfwDestroyWindow(window);  // Destroy the window
    glfwTerminate();  // Terminate GLFW
    return 0;
}

/* Process inputs */
void processInput(GLFWwindow* window) {
    static bool isFullscreen = false; // Track fullscreen state
    static int windowedWidth = 1920, windowedHeight = 1080; // Store windowed dimensions
    static int windowedX = 0, windowedY = 0; // Store windowed position
    static bool f11Pressed = false; // Track F11 key press
    static bool lPressed = false; // Track L key press
    static bool wireframeMode = false; // Track wireframe mode state

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    } else if (glfwGetKey(window, GLFW_KEY_F11) == GLFW_PRESS && !f11Pressed) {
        f11Pressed = true; // Mark key as pressed
        if (!isFullscreen) {
            GLFWmonitor* monitor = glfwGetPrimaryMonitor();
            const GLFWvidmode* mode = glfwGetVideoMode(monitor);
            glfwGetWindowPos(window, &windowedX, &windowedY);
            glfwGetWindowSize(window, &windowedWidth, &windowedHeight);
            glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
            isFullscreen = true;
        }
        else {
            glfwSetWindowMonitor(window, nullptr, windowedX, windowedY, screenWidth, screenHeight, 0);
            isFullscreen = false;
        }
    } else if (glfwGetKey(window, GLFW_KEY_F11) == GLFW_RELEASE) {
        f11Pressed = false;
    }
}