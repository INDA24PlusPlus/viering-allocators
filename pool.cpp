#include "pool.h"

#include <print>
#include <Windows.h>

PoolAllocator::PoolAllocator(size_t chunk_size, int pool_size) : chunk_size(chunk_size), pool_size(pool_size), sub_pools(), last_ptr(nullptr) {
	// get page size
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	this->PAGE_SIZE = si.dwPageSize;
	
	this->pool_start = VirtualAlloc(nullptr, chunk_size * pool_size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	this->curr_ptr = this->pool_start;

	sub_pools.push_back(this->pool_start);
	
	// linked list of chunks
	void* curr = this->curr_ptr;
	for (int i = 0; i < pool_size - 1; i++) {
		void* next = (char*)curr + chunk_size;
		*(void**)curr = next;
		curr = next;
	}
	*(void**)curr = nullptr;

	// set remaining chunks in current page
	this->page_remaining_chunks = (PAGE_SIZE - (chunk_size * pool_size) % PAGE_SIZE) / this->chunk_size;
}

PoolAllocator::~PoolAllocator() {
	for (int i = 0; i < sub_pools.size(); i++) {
		VirtualFree(sub_pools[i], 0, MEM_RELEASE);
	}
}

void* PoolAllocator::alloc() {
	if (this->curr_ptr == nullptr) {
		// current page is full, allocate new one
		if (this->page_remaining_chunks == 0) {
			void* addr = VirtualAlloc(nullptr, chunk_size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

			sub_pools.push_back(addr); // add to free list

			this->curr_ptr = addr;
			
			*(void**)this->last_ptr = addr;
			*(void**)addr = nullptr;

			this->page_remaining_chunks = PAGE_SIZE / chunk_size - 1;

			return addr;
		}

		// space left in current page
		void* next = (char*)last_ptr + chunk_size;
		*(void**)last_ptr = next;

		this->page_remaining_chunks--;

		this->last_ptr = next;
		return next;
	}

	// advance one chunk forward
	this->last_ptr = this->curr_ptr;
	void* chunk = this->curr_ptr;
	this->curr_ptr = *(void**)this->curr_ptr;

	return chunk;
}

void PoolAllocator::free(void* ptr) {
	*(void**)ptr = this->curr_ptr; 
	this->curr_ptr = ptr;
}