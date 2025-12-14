#include "Block.h"

Block::~Block() {
}

bool Block::IsActive() const {
    return isActive;
}

void Block::SetActive(bool active) {
    isActive = active;
}