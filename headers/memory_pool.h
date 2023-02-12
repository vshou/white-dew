#include <stdlib.h>

#ifndef WHITE_DEW_MEMORY_POOL_H
#define WHITE_DEW_MEMORY_POOL_H

// 全局内存池的大小
#define MAXIMUM_POOL 5
// 开辟内存块的固定大小
#define MEMORY_BLOCK_SIZE 4096

// 内存块结构体(链表型态)
struct MEMORY_BLOCK
{
	void * _p;						// 需要返回给使用者的指针
	void * _start;					// 内存块的起始地址
	void * _end;					// 内存块的结束地址
	struct MEMORY_BLOCK * next;		// 指向下一内存块的指针
};

// 全局内存池
struct MEMORY_BLOCK * global_memory_pool[MAXIMUM_POOL] = {};

// 初始化内存块
struct MEMORY_BLOCK * init_memory_block();

// 开辟一块内存块
void * dy_malloc(size_t _size);

// 内存块链表扩容
void Expansion(const struct MEMORY_BLOCK * _block);

// 释放内存
void dy_free();

#endif //WHITE_DEW_MEMORY_POOL_H
