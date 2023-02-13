#include "memory_pool.h"

struct MEMORY_BLOCK * global_memory_pool[MAXIMUM_POOL] = {};

struct MEMORY_BLOCK * init_memory_block(int _index)
{
	// 生成内存块结构体
	struct MEMORY_BLOCK * _block = (struct MEMORY_BLOCK *) malloc(sizeof(struct MEMORY_BLOCK));
	// 开辟内存块
	_block->_start = malloc(MEMORY_BLOCK_SIZE);
	// 初始化属性
	_block->_end = _block->_start + MEMORY_BLOCK_SIZE;
	_block->_p = _block->_start;
	_block->_next = NULL;
	_block->_new = NULL;
	// 头内存块节点加入到容器
	global_memory_pool[_index] = _block;
	return _block;
}

struct MEMORY_BLOCK * Expansion(struct MEMORY_BLOCK * _block, int _multiple, int _index)
{
	// 获取需要扩容的大小
	int _size =  MEMORY_BLOCK_SIZE * _multiple;
	// 生成内存块结构体
	struct MEMORY_BLOCK * _block_ = (struct MEMORY_BLOCK *) malloc(sizeof(struct MEMORY_BLOCK));
	// 开辟内存块
	_block_->_start = malloc(_size);
	// 初始化属性
	_block_->_end = _block_->_start + _size;
	_block_->_p = _block_->_start;
	_block_->_next = NULL;
	_block_->_new = NULL;
	_block->_next = _block_;
	// 头内存块节点 _new 属性更新
	global_memory_pool[_index]->_new = _block_;
	return _block_;
}

void * dy_malloc(size_t _size)
{
	// 获取当前所属线程 id 并对全局内存池数组长度取余来获取在数组中对应的位置下标
	int _index = ((int) pthread_self()) % MAXIMUM_POOL;
	// 从全局内存池拿对应的内存块
	struct MEMORY_BLOCK * _block = global_memory_pool[_index];
	// 如果头内存块还没初始化则进行初始化操作
	if (NULL == _block)
	{
		_block = init_memory_block(_index);
	}
	// 如果 _block->_new 不为 NULL, 则代表链表下已经挂载了比它更新的内存块, 将 _block 替换为最新内存块地址
	if (NULL != _block->_new)
	{
		_block = _block->_new;
	}
	// 若当前内存块剩余内存不够用, 则开辟新的内存块进行扩容, 开辟之前应先判断一下需要的内存大小 _size 是否大于 MEMORY_BLOCK_SIZE (每次开辟内存块的大小)
	if ((_block->_end - _block->_p) < _size && _size > MEMORY_BLOCK_SIZE)
	{
		// 如果 _size 大于 MEMORY_BLOCK_SIZE, 则直接开辟一个两倍 _size 大小的内存块
		_block = Expansion(_block, 2, _index);
	}
	if ((_block->_end - _block->_p) < _size && _size < MEMORY_BLOCK_SIZE)
	{
		// _size 小于 MEMORY_BLOCK_SIZE, 则开辟 MEMORY_BLOCK_SIZE 大小的内存块
		_block = Expansion(_block, 1, _index);
	}
	// 转移指针
	void * p = _block->_p;
	// 指针偏移
	_block->_p = _block->_p + _size;
	return p;
}

void dy_free()
{
	// 获取当前所属线程 id 并对全局内存池数组长度取余来获取在数组中对应的位置下标
	int _index = ((int) pthread_self()) % MAXIMUM_POOL;
	// 从全局内存池拿对应的内存块
	struct MEMORY_BLOCK * _block = global_memory_pool[_index];
	// 过渡用的指针变量
	struct MEMORY_BLOCK * _block_;
	int sign = 0;
	// 循环释放链表内存
	while (1)
	{
		if (NULL == _block)
		{
			break;
		}
		// 如果是头节点则不释放内存, 只需要修改指针变量即可, 以便复用进而减少不必要的初始化操作
		if (sign == 0)
		{
			_block_ = _block->_next;
			_block->_p = _block->_start;
			// 覆盖掉之前内存里的内容, 防止意外发生
			memset(_block->_start, 0, MEMORY_BLOCK_SIZE);
			_block->_new = NULL;
			_block->_next = NULL;
			_block = _block_;
			sign++;
			continue;
		}
		// 传递指针过渡
		_block_ = _block->_next;
		// 释放内存
		free(_block->_start);
		free(_block);
		_block = _block_;
	}
}