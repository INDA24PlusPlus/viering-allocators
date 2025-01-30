#include "sus.h"

#include <Windows.h>

SusAllocator::SusAllocator() : ptr(nullptr) { }

SusAllocator::~SusAllocator() {
	while (ptr) {
		SusBlock* next = ptr->next;
		VirtualFree(this->ptr, 0, MEM_RELEASE);
		this->ptr = next;
	}
}

void* SusAllocator::alloc(size_t size) {
	SusBlock** best = &ptr;
	SusBlock** curr = &ptr;

	// find best block that fits
	while (*curr) {
		if ((*curr)->size >= size && (!*best || (*curr)->size < (*best)->size)) {
			best = curr;
		}

		curr = &((*curr)->next);
	}

	// found a block
	if (*best) {
		SusBlock* target = *best;
		*best = target->next;
		return (void*)(target + 1);
	}

	// didn't find a block, allocate a new one
	SusBlock* block = (SusBlock*)VirtualAlloc(nullptr, sizeof(SusBlock) + size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

	block->size = size;
	block->next = nullptr;

	return (void*)(block + 1);
}

void SusAllocator::free(void* ptr) {
	SusBlock* block = ((SusBlock*)ptr) - 1;
	block->next = this->ptr;
	this->ptr = block;
}
