#pragma once
#include <string>

class Block {
public:
    enum BlockType {
        Solid,
        Liquid,
        Transparent
    };

    bool isActive;

    // Constructor
    Block(BlockType type = Solid, const std::string& name = "Block")
        : blockType(type), BlockName(name), active(true) {
    }

    // Destructor
    ~Block() = default;

    // Check if the block is active (for meshing)
    bool IsActive() const { return active; }

    // Activate or deactivate the block
    void SetActive(bool a) { active = a; }

    BlockType blockType;       // Type of the block
    std::string BlockName;     // Optional name
private:
    bool active;               // Internal active flag
};