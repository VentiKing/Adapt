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
    BlockType blockType;
    std::string BlockName;

    Block() : isActive(true), blockType(Solid), BlockName("") {}
    ~Block();

    bool IsActive() const;
    void SetActive(bool active);
};