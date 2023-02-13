#include "memory_pool.h"

struct MEMORY_BLOCK * global_memory_pool[MAXIMUM_POOL] = {};

struct MEMORY_BLOCK * init_memory_block(int _index)
{
	struct MEMORY_BLOCK * _block = (struct MEMORY_BLOCK *) malloc(sizeof(struct MEMORY_BLOCK));
	_block->_start = malloc(MEMORY_BLOCK_SIZE);
	_block->_end = _block->_start + MEMORY_BLOCK_SIZE;
	_block->_p = _block->_start;
	_block->_next = NULL;
	_block->_new = NULL;
	global_memory_pool[_index] = _block;
	return _block;
}

struct MEMORY_BLOCK * Expansion(struct MEMORY_BLOCK * _block, int _multiple, int _index)
{
	int _size =  MEMORY_BLOCK_SIZE * _multiple;
	struct MEMORY_BLOCK * _block_ = (struct MEMORY_BLOCK *) malloc(sizeof(struct MEMORY_BLOCK));
	_block_->_start = malloc(_size);
	_block_->_end = _block_->_start + _size;
	_block_->_p = _block_->_start;
	_block_->_next = NULL;
	_block_->_new = NULL;
	_block->_next = _block_;
	global_memory_pool[_index]->_new = _block_;
	return _block_;
}

void * dy_malloc(size_t _size)
{
	// 获取当前所属线程 id 并对全局内存池数组长度取余来获取在数组中对应的位置下标
	int _index = ((int) pthread_self()) % MAXIMUM_POOL;
	// 从全局内存池拿对应的内存块
	struct MEMORY_BLOCK * _block = global_memory_pool[_index];
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
	// 分配内存
	void * p = _block->_p;
	_block->_p = _block->_p + _size;
	return p;
}

void dy_free()
{
	// 获取当前所属线程 id 并对全局内存池数组长度取余来获取在数组中对应的位置下标
	int _index = ((int) pthread_self()) % MAXIMUM_POOL;
	// 从全局内存池拿对应的内存块
	struct MEMORY_BLOCK * _block = global_memory_pool[_index];
	struct MEMORY_BLOCK * _block_;
	int sign = 0;
	// 循环释放链表内存
	while (1)
	{
		if (NULL == _block)
		{
			break;
		}
		if (sign == 0)
		{
			_block_ = _block->_next;
			_block->_p = _block->_start;
			memset(_block->_start, 0, MEMORY_BLOCK_SIZE);
			_block->_new = NULL;
			_block->_next = NULL;
			_block = _block_;
			sign++;
			continue;
		}
		_block_ = _block->_next;
		free(_block->_start);
		free(_block);
		_block = _block_;
	}
}