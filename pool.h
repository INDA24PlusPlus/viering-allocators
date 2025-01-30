#pragma once

#include <cstdlib>
#include <cstdint>
#include <vector>

class PoolAllocator {
public:
	PoolAllocator(size_t chunk_size, int pool_size);
	~PoolAllocator();

	void* alloc();
	void free(void* ptr);

private:
	size_t chunk_size;
	int pool_size;

	void* pool_start;
	void* curr_ptr;
	void* last_ptr;

	int page_remaining_chunks;

	std::vector<void*> sub_pools;

	int PAGE_SIZE;
};