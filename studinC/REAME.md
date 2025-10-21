# Custom Memory Allocator in C

This project implements a simple custom memory allocator in C, similar to the standard `malloc`, `free`, `calloc`, and `realloc` functions. The allocator manages memory blocks using a linked list and provides thread safety with mutex locks.

## Functions Explained

- **malloc(size_t size)**  
  Allocates a block of memory of the requested size. If a suitable free block is found, it reuses it; otherwise, it requests more memory from the system.

- **free(void *block)**  
  Frees a previously allocated memory block, making it available for future allocations.

- **calloc(size_t num, size_t nsize)**  
  Allocates memory for an array of elements, initializing all bytes to zero. It checks for overflow and uses `malloc` internally.

- **realloc(void *block, size_t size)**  
  Changes the size of an existing memory block. If the new size is larger, it allocates a new block, copies the data, and frees the old block.

These functions are thread-safe and use a simple linked list to keep track of memory blocks.