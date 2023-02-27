#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#ifndef WHITE_DEW_MEMORY_POOL_H
#define WHITE_DEW_MEMORY_POOL_H


// 开辟内存块的固定大小
#define MEMORY_BLOCK_SIZE 4096

// 内存块结构体(链表型态)
typedef struct MEMORY_BLOCK
{
	size_t _cap;					// 内存块容量
	void * _p;						// 需要返回给使用者的指针
	void * _start;					// 内存块的起始地址
	void * _end;					// 内存块的结束地址
	struct MEMORY_BLOCK * _next;	// 指向下一内存块的指针
	struct MEMORY_BLOCK * _new;		// 当前内存块链表最新的内存块指针
} MEMORY_BLOCK;

// 线程本地缓存 key
static pthread_key_t pt_key;

// 初始化一次控制参数
static pthread_once_t pt_once;

// 初始化线程本地缓存 key
void init_pt_key();

// 线程本地缓存销毁回调
void destroy_function(void * _p);

// 开辟一块内存块
void * dy_malloc(size_t _size);

// 初始化内存块属性
void init_memory_block(MEMORY_BLOCK * _block, MEMORY_BLOCK * _next, size_t _size);

// 内存块链表扩容
MEMORY_BLOCK * Expansion(int _multiple);

// 释放线程所占内存
// 说明: 适用于线程池情况, 手动创建线程执行时不需要调用, 会在线程销毁时自动调用回调函数释放内存
void dy_free();

#endif //WHITE_DEW_MEMORY_POOL_H
