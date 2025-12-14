#include "Chunk.h"
#include <iostream>

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
    for (size_t X = 0; X < Chunk.size(); ++X) {
        for (size_t Y = 0; Y < Chunk[X].size(); ++Y) {
            for (size_t Z = 0; Z < Chunk[X][Y].size(); ++Z) {
                const Block& block = Chunk[X][Y][Z];

                if (!block.hasNeighbourNorth) {
                    addFaceNorth();
                }

                if (!block.hasNeighbourSouth) {
                    addFaceSouth();
                }

                if (!block.hasNeighbourEast) {
                    addFaceEast();
                }

                if (!block.hasNeighbourWest) {
                    addFaceWest();
                }
            }
        }
    }
}