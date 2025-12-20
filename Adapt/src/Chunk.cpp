#include "Chunk.h"
#include <cstddef>

// ---- Face builders ----
inline void addFaceNorth(std::vector<Chunk::Vertex>& v, float x, float y, float z) {
    v.push_back({ x, y + 1, z});
    v.push_back({ x + 1, y + 1, z});
    v.push_back({ x, y + 1, z + 1});
    v.push_back({ x + 1, y + 1, z + 1});
}

inline void addFaceSouth(std::vector<Chunk::Vertex>& v, float x, float y, float z) {
    v.push_back({ x, y, z});
    v.push_back({ x, y, z + 1});
    v.push_back({ x + 1, y, z});
    v.push_back({ x + 1, y, z + 1});
}

inline void addFaceEast(std::vector<Chunk::Vertex>& v, float x, float y, float z) {
    v.push_back({ x + 1, y, z});
    v.push_back({ x + 1, y + 1, z});
    v.push_back({ x + 1, y, z + 1});
    v.push_back({ x + 1, y + 1, z + 1});
}

inline void addFaceWest(std::vector<Chunk::Vertex>& v, float x, float y, float z) {
    v.push_back({ x, y, z});
    v.push_back({ x, y, z + 1});
    v.push_back({ x, y + 1, z});
    v.push_back({ x, y + 1, z + 1});
}

inline void addFaceUp(std::vector<Chunk::Vertex>& v, float x, float y, float z) {
    v.push_back({ x, y, z + 1});
    v.push_back({ x + 1, y, z + 1});
    v.push_back({ x, y + 1, z + 1});
    v.push_back({ x + 1, y + 1, z + 1});
}

inline void addFaceDown(std::vector<Chunk::Vertex>& v, float x, float y, float z) {
    v.push_back({ x, y, z});
    v.push_back({ x, y + 1, z});
    v.push_back({ x + 1, y, z});
    v.push_back({ x + 1, y + 1, z});
}

// ---- Chunk constructor ----
Chunk::Chunk(size_t sx, size_t sy, size_t sz)
    : sizeX(sx), sizeY(sy), sizeZ(sz)
{
    data.resize(sizeX, std::vector<std::vector<Block>>(sizeY, std::vector<Block>(sizeZ)));

    // Example: fill all blocks with Solid
    for (size_t x = 0; x < sizeX; ++x)
        for (size_t y = 0; y < sizeY; ++y)
            for (size_t z = 0; z < sizeZ; ++z)
                data[x][y][z].blockType = Block::Solid;
}

// ---- Mesh generation ----
void Chunk::generateMesh(std::vector<Vertex>& vertices, std::vector<unsigned>& indices) {
    vertices.clear();
    indices.clear();

    unsigned faceCount = 0;

    for (size_t x = 0; x < sizeX; ++x) {
        for (size_t y = 0; y < sizeY; ++y) {
            for (size_t z = 0; z < sizeZ; ++z) {

                const Block& block = data[x][y][z];
                if (!block.IsActive()) continue;

                bool north = (y + 1 < sizeY) && data[x][y + 1][z].IsActive();
                bool south = (y > 0) && data[x][y - 1][z].IsActive();
                bool east = (x + 1 < sizeX) && data[x + 1][y][z].IsActive();
                bool west = (x > 0) && data[x - 1][y][z].IsActive();
                bool up = (z + 1 < sizeZ) && data[x][y][z + 1].IsActive();
                bool down = (z > 0) && data[x][y][z - 1].IsActive();

                float fx = static_cast<float>(x);
                float fy = static_cast<float>(y);
                float fz = static_cast<float>(z);

                if (!north) { addFaceNorth(vertices, fx, fy, fz); faceCount++; }
                if (!south) { addFaceSouth(vertices, fx, fy, fz); faceCount++; }
                if (!east) { addFaceEast(vertices, fx, fy, fz); faceCount++; }
                if (!west) { addFaceWest(vertices, fx, fy, fz); faceCount++; }
                if (!up) { addFaceUp(vertices, fx, fy, fz); faceCount++; }
                if (!down) { addFaceDown(vertices, fx, fy, fz); faceCount++; }
            }
        }
    }

    // ---- Build indices ----
    indices.reserve(faceCount * 6);
    for (unsigned i = 0; i < faceCount; ++i) {
        unsigned base = i * 4;
        indices.push_back(base + 0);
        indices.push_back(base + 1);
        indices.push_back(base + 2);
        indices.push_back(base + 2);
        indices.push_back(base + 1);
        indices.push_back(base + 3);
    }
}

// ---- Static processChunk function ----
void Chunk::processChunk(const ChunkType& chunk, std::vector<Vertex>& vertices, std::vector<unsigned>& indices) {
    vertices.clear();
    indices.clear();

    if (chunk.empty() || chunk[0].empty() || chunk[0][0].empty()) {
        return;
    }

    size_t sizeX = chunk.size();
    size_t sizeY = chunk[0].size();
    size_t sizeZ = chunk[0][0].size();

    unsigned faceCount = 0;

    for (size_t x = 0; x < sizeX; ++x) {
        for (size_t y = 0; y < sizeY; ++y) {
            for (size_t z = 0; z < sizeZ; ++z) {

                const Block& block = chunk[x][y][z];
                if (!block.IsActive()) continue;

                bool north = (y + 1 < sizeY) && chunk[x][y + 1][z].IsActive();
                bool south = (y > 0) && chunk[x][y - 1][z].IsActive();
                bool east = (x + 1 < sizeX) && chunk[x + 1][y][z].IsActive();
                bool west = (x > 0) && chunk[x - 1][y][z].IsActive();
                bool up = (z + 1 < sizeZ) && chunk[x][y][z + 1].IsActive();
                bool down = (z > 0) && chunk[x][y][z - 1].IsActive();

                float fx = static_cast<float>(x);
                float fy = static_cast<float>(y);
                float fz = static_cast<float>(z);

                if (!north) { addFaceNorth(vertices, fx, fy, fz); faceCount++; }
                if (!south) { addFaceSouth(vertices, fx, fy, fz); faceCount++; }
                if (!east) { addFaceEast(vertices, fx, fy, fz); faceCount++; }
                if (!west) { addFaceWest(vertices, fx, fy, fz); faceCount++; }
                if (!up) { addFaceUp(vertices, fx, fy, fz); faceCount++; }
                if (!down) { addFaceDown(vertices, fx, fy, fz); faceCount++; }
            }
        }
    }

    // Build indices
    indices.reserve(faceCount * 6);
    for (unsigned i = 0; i < faceCount; ++i) {
        unsigned base = i * 4;
        indices.push_back(base + 0);
        indices.push_back(base + 1);
        indices.push_back(base + 2);
        indices.push_back(base + 2);
        indices.push_back(base + 1);
        indices.push_back(base + 3);
    }
}