/*#include "World.h"
#include "Chunk.h"
#include "Shader.h"
#include "Camera.h"
#include <glad/glad.h>

#include <vector>

const int Chunk_Size = 16;
const int Chunk_Distance = 8;
const int Half_Render = Chunk_Distance / 2;

std::vector<Chunk::Vertex> chunkVertices;
std::vector<unsigned> chunkIndices;

void World::GenerateWorld() {

    glm::ivec3 cameraChunk;
    cameraChunk.x = floor(camera.Position.x / Chunk_Size);
    cameraChunk.y = floor(camera.Position.y / Chunk_Size);
    cameraChunk.z = floor(camera.Position.z / Chunk_Size);

    for (int x = 0; x < Chunk_Distance; x++) {
        for (int y = 0; y < 1; y++) {
            for (int z = 0; z < Chunk_Distance; z++) {
                glm::ivec3 chunkPos;
                chunkPos.x = cameraChunk.x + x - Half_Render;
                chunkPos.y = cameraChunk.y - 1;
                chunkPos.z = cameraChunk.z + z - Half_Render;

                glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(chunkPos) * (float)Chunk_Size);

                glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
                glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(chunkIndices.size()), GL_UNSIGNED_INT, 0);
            }
        }
    }
}*/