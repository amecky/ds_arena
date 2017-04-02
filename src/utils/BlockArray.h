#pragma once
#include <stdint.h>

const int MAX_BLOCKS = 16;

struct BlockArray {

	char* data;
	uint32_t size;
	uint32_t capacity;
	uint32_t total_capacity;
	int _sizes[MAX_BLOCKS];
	int _indices[MAX_BLOCKS];
	int _num_blocks;

	BlockArray();

	~BlockArray();

	void init(int* sizes, int num);

	void* get_ptr(int index);

	bool resize(uint32_t new_size);

	void remove(int index);

	void swap(int oldIndex, int newIndex);
	
};
