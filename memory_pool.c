#include "memory_pool.h"

struct MEMORY_BLOCK * global_memory_pool[MAXIMUM_POOL] = {};


struct MEMORY_BLOCK * init_head_memory_block(int _index)
{
	// 生成内存块结构体
	struct MEMORY_BLOCK * _block = (struct MEMORY_BLOCK *) malloc(sizeof(struct MEMORY_BLOCK));
	// 初始化内存块
	init_memory_block(_block, NULL, MEMORY_BLOCK_SIZE);
	_block->_new = _block;
	// 头内存块节点加入到容器
	global_memory_pool[_index] = _block;
	return _block;
}

void init_memory_block(struct MEMORY_BLOCK * _block, struct MEMORY_BLOCK * _next, size_t _size)
{
	_block->_start = malloc(_size);
	_block->_end = _block->_start + _size;
	_block->_cap = _size;
	_block->_p = _block->_start;
	_block->_next = _next;
	_block->_new = NULL;
}

struct MEMORY_BLOCK * Expansion(int _multiple)
{
	// 获取需要扩容的大小
	int _size =  MEMORY_BLOCK_SIZE * _multiple;

	// 生成内存块结构体
	struct MEMORY_BLOCK * _block_ = (struct MEMORY_BLOCK *) malloc(sizeof(struct MEMORY_BLOCK));

	// 初始化内存块
	init_memory_block(_block_, NULL, _size);

	return _block_;
}

void * dy_malloc(size_t _size)
{
	// 获取当前所属线程 id 并对全局内存池数组长度取余来获取在数组中对应的位置下标
	int _index = ((int) pthread_self()) % MAXIMUM_POOL;

	// 从全局内存池拿对应的内存块
	struct MEMORY_BLOCK * _block = global_memory_pool[_index]->_new;

	// 若当前内存块剩余内存不够用, 则开辟新的内存块进行扩容, 开辟之前应先判断一下需要的内存大小 _size 是否大于 MEMORY_BLOCK_SIZE (每次开辟内存块的大小)
	if ((_block->_end - _block->_p) < _size && _size > MEMORY_BLOCK_SIZE)
	{
		if (NULL != _block->_next && _size <= _block->_cap)
		{
			_block = _block->_next;
		}
		else
		{
			// 如果 _size 大于 MEMORY_BLOCK_SIZE, 则直接开辟一个两倍 _size 大小的内存块
			struct MEMORY_BLOCK * _block_ = Expansion(2);
			_block_->_next = _block->_next;
			_block->_next = _block_;
			_block = _block_;
		}
		// 头内存块节点 _new 属性更新
		global_memory_pool[_index]->_new = _block;
	} else if ((_block->_end - _block->_p) < _size && _size < MEMORY_BLOCK_SIZE)
	{
		if (NULL != _block->_next)
		{
			_block = _block->_next;
		}
		else
		{
			// _size 小于 MEMORY_BLOCK_SIZE, 则开辟 MEMORY_BLOCK_SIZE 大小的内存块
			struct MEMORY_BLOCK * _block_ = Expansion(1);
			_block_->_next = _block->_next;
			_block->_next = _block_;
			_block = _block_;
		}
		// 头内存块节点 _new 属性更新
		global_memory_pool[_index]->_new = _block;
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
	struct MEMORY_BLOCK * _new_ = _block->_new;
	// 循环释放链表内存
	while (1)
	{
		// 重置内存内的内容, 防止意想不到的问题
		memset(_block->_start, 0, _block->_cap);
		_block->_p = _block->_start;
		// 如果当前已经是本次线程操作内最新的内存块, 则结束循环
		if (_block == _new_)
		{
			break;
		}
		_block = _block->_next;
	}
	// 初始化头节点的 _new 属性
	global_memory_pool[_index]->_new = global_memory_pool[_index];
}