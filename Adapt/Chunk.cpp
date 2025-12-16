/*#include "Chunk.h"
#include "Block.h"
#include <iostream>
#include <vector>
#include <cstddef>

using ChunkType = std::vector<std::vector<std::vector<Block>>>;

void addFaceNorth() {
    std::cout << "Adding north face.\n";
}

void addFaceSouth() {
    std::cout << "Adding south face.\n";
}

void addFaceEast() {
    std::cout << "Adding east face.\n";
}

void addFaceWest() {
    std::cout << "Adding west face.\n";
}

void processChunk(const ChunkType& Chunk) {
    size_t sizeX = Chunk.size();
    if (sizeX == 0) return;
    size_t sizeY = Chunk[0].size();
    if (sizeY == 0) return;
    size_t sizeZ = Chunk[0][0].size();
    if (sizeZ == 0) return;

    for (size_t X = 0; X < sizeX; ++X) {
        for (size_t Y = 0; Y < sizeY; ++Y) {
            for (size_t Z = 0; Z < sizeZ; ++Z) {
                const Block& block = Chunk[X][Y][Z];
                if (!block.IsActive() || block.blockType == Block::Transparent) continue;

                bool northSolid = (Y + 1 < sizeY) && Chunk[X][Y + 1][Z].IsActive() && Chunk[X][Y + 1][Z].blockType == Block::Solid;
                bool southSolid = (Y > 0) && Chunk[X][Y - 1][Z].IsActive() && Chunk[X][Y - 1][Z].blockType == Block::Solid;
                bool eastSolid = (X + 1 < sizeX) && Chunk[X + 1][Y][Z].IsActive() && Chunk[X + 1][Y][Z].blockType == Block::Solid;
                bool westSolid = (X > 0) && Chunk[X - 1][Y][Z].IsActive() && Chunk[X - 1][Y][Z].blockType == Block::Solid;
                bool upSolid = (Z + 1 < sizeZ) && Chunk[X][Y][Z + 1].IsActive() && Chunk[X][Y][Z + 1].blockType == Block::Solid;
                bool downSolid = (Z > 0) && Chunk[X][Y][Z - 1].IsActive() && Chunk[X][Y][Z - 1].blockType == Block::Solid;

                if (northSolid && southSolid && eastSolid && westSolid && upSolid && downSolid) {
                    continue;
                }

                if (!northSolid) addFaceNorth();
                if (!southSolid) addFaceSouth();
                if (!eastSolid) addFaceEast();
                if (!westSolid) addFaceWest();
            }
        }
    }
}*/