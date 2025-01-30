#pragma once

#include <cstdlib>
#include <cstdint>
#include <vector>

struct SusBlock {
	size_t size;
	SusBlock* next;
};

class SusAllocator {
public:
	SusAllocator();
	~SusAllocator();

	void* alloc(size_t size);
	void free(void* ptr);

public:
	SusBlock* ptr;
};