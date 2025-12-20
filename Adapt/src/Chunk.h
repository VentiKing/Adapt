#pragma once
#include <vector>
#include "Block.h"

using ChunkType = std::vector<std::vector<std::vector<Block>>>;

class Chunk {
public:
    struct Vertex {
        float x, y, z;
        float r, g, b;  // ADD COLOR DATA
    };

    Chunk(size_t sx, size_t sy, size_t sz);
    void generateMesh(std::vector<Vertex>& vertices, std::vector<unsigned>& indices);

    static void processChunk(const ChunkType& chunk, std::vector<Vertex>& vertices, std::vector<unsigned>& indices);

    ChunkType data;

private:
    size_t sizeX, sizeY, sizeZ;
};