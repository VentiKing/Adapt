#pragma once

#include <string>

class Block {
public:
	enum BlockType {
		Solid,
		Liquid,
		Tranparent
	};
	~Block();
	bool IsActive();
	void SetActive(bool active);
	BlockType blockType;

	std::string BlockName;
};