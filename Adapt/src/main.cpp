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
#include <vector>
#include <chrono>

#include "Shader.h"
#include "Camera.h"
#include "Inputs.h"
#include "Chunk.h"
#include "Block.h"

// ---- Global variables ----
float deltaTime = 0.0f;
float lastFrame = 0.0f;
int fpsCount = 0;
std::chrono::steady_clock::time_point fpsStartTime;
float averageFPS = 0;
float lowestFps = -1;
float highestFps = -1;

const unsigned int screenWidth = 1920;
const unsigned int screenHeight = 1080;

const int Chunk_Size = 16;
const int Chunk_Distance = 8;
const int Half_Render = Chunk_Distance / 2;

// ---- Cube test data (can keep for reference) ----
float CubeVertices[] = {
    // Positions
    1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f,
    1.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f
};
unsigned int CubeIndices[] = {
    1, 0, 2,
    1, 2, 3
};
// ---- Voxel chunk variables ----
std::vector<Chunk::Vertex> chunkVertices;
std::vector<unsigned> chunkIndices;

// ---- Helper function to create a simple test chunk ----
ChunkType createTestChunk(int sizeX, int sizeY, int sizeZ) {
    ChunkType chunk(sizeX, std::vector<std::vector<Block>>(sizeY, std::vector<Block>(sizeZ)));
    for (int x = 0; x < sizeX; x++)
        for (int y = 0; y < sizeY; y++)
            for (int z = 0; z < sizeZ; z++) {
                chunk[x][y][z].SetActive(true);
                chunk[x][y][z].blockType = Block::Solid;
            }
    return chunk;
}

int main() {
    GLFWwindow* window;

    // ---- GLFW initialization ----
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(screenWidth, screenHeight, "Adapt", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // ---- Load window icon ----
    int width, height, channels;
    unsigned char* pixels = stbi_load("assets/textures/ObsidianAxe.png", &width, &height, &channels, 4);
    if (!pixels) {
        std::cerr << "Failed to load texture" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    GLFWimage images[1]{};
    images[0].width = width;
    images[0].height = height;
    images[0].pixels = pixels;
    glfwSetWindowIcon(window, 1, images);

    glDisable(GL_CULL_FACE);

    // ---- Shader ----
    Shader shader("assets/shaders/vertShader.glsl", "assets/shaders/fragShader.glsl");

    // ---- Cube VAO/VBO (keep for reference) ----
    unsigned int vertexArrayObject, vertexBufferObject, elementBufferObject;
    glGenVertexArrays(1, &vertexArrayObject);
    glGenBuffers(1, &vertexBufferObject);
    glGenBuffers(1, &elementBufferObject);

    glBindVertexArray(vertexArrayObject);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(CubeVertices), CubeVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferObject);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(CubeIndices), CubeIndices, GL_STATIC_DRAW);

    // Vertex layout: x,y,z
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(3*sizeof(float)), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Chunk::Vertex), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    // ---- Depth buffer ----
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);


    Camera camera(screenWidth, screenHeight, glm::vec3(8.0f, 8.0f, 25.0f));

    fpsStartTime = std::chrono::steady_clock::now();

    // ---- Create a chunk ----
    ChunkType chunk = createTestChunk(16, 16, 16);
    Chunk::processChunk(chunk, chunkVertices, chunkIndices);
    Chunk::processChunk(chunk, chunkVertices, chunkIndices);

    // ---- Voxel chunk VAO/VBO ----
    unsigned int chunkVAO, chunkVBO, chunkEBO;
    glGenVertexArrays(1, &chunkVAO);
    glGenBuffers(1, &chunkVBO);
    glGenBuffers(1, &chunkEBO);

    glBindVertexArray(chunkVAO);
    glBindBuffer(GL_ARRAY_BUFFER, chunkVBO);
    glBufferData(GL_ARRAY_BUFFER, chunkVertices.size() * sizeof(Chunk::Vertex), chunkVertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, chunkEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, chunkIndices.size() * sizeof(unsigned), chunkIndices.data(), GL_STATIC_DRAW);

    // Vertex layout: x,y,z
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Chunk::Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    // ---- Render loop ----
    while (!glfwWindowShouldClose(window)) {
        auto cycleStartTime = std::chrono::steady_clock::now();

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        float fps = 1.0f / deltaTime;
        if (lowestFps == -1 || fps < lowestFps) lowestFps = fps;
        if (highestFps == -1 || fps > highestFps) highestFps = fps;
        fpsCount++;

        auto currentTimePoint = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::seconds>(currentTimePoint - fpsStartTime).count() >= 1) {
            averageFPS = fpsCount;
            std::cout << "Average FPS: " << averageFPS << " (Lowest: " << lowestFps << ", Highest: " << highestFps << ")" << std::endl;
            fpsCount = 0;
            lowestFps = -1;
            highestFps = -1;
            fpsStartTime = currentTimePoint;
        }

        Inputs::Update(window, camera);

        glViewport(0, 0, screenWidth, screenHeight);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();

        glm::mat4 view = glm::lookAt(camera.Position, camera.Position + camera.Orientation, camera.Up);
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
        glm::mat4 cameraMatrix = projection * view;

        // ---- Draw voxel chunks ----
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(50.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Rotate the chunk
        model = glm::translate(model, glm::vec3(-8.0f, -8.0f, -8.0f)); // Center the 16x16x16 chunk

        glUniformMatrix4fv(glGetUniformLocation(shader.ID, "cameraMatrix"), 1, GL_FALSE, glm::value_ptr(cameraMatrix));
        glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));

        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);

        glBindVertexArray(chunkVAO);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(chunkIndices.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();

        auto cycleEndTime = std::chrono::steady_clock::now();
        auto cycleDuration = std::chrono::duration_cast<std::chrono::milliseconds>(cycleEndTime - cycleStartTime).count();
    }

    // ---- Cleanup ----
    glDeleteVertexArrays(1, &vertexArrayObject);
    glDeleteBuffers(1, &vertexBufferObject);
    glDeleteBuffers(1, &elementBufferObject);

    glDeleteVertexArrays(1, &chunkVAO);
    glDeleteBuffers(1, &chunkVBO);
    glDeleteBuffers(1, &chunkEBO);

    stbi_image_free(pixels);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}