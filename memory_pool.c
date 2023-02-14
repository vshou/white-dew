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
	_block->_p = _block->_start;
	_block->_next = _next;
	_block->_new = NULL;
}

struct MEMORY_BLOCK * Expansion()
{

	// 生成内存块结构体
	struct MEMORY_BLOCK * _block_ = (struct MEMORY_BLOCK *) malloc(sizeof(struct MEMORY_BLOCK));
	// 初始化内存块
	init_memory_block(_block_, NULL, MEMORY_BLOCK_SIZE);
	return _block_;
}

void * dy_malloc(size_t _size)
{
	// 获取当前所属线程 id 并对全局内存池数组长度取余来获取在数组中对应的位置下标
	int _index = ((int) pthread_self()) % MAXIMUM_POOL;
	// 从全局内存池拿对应的内存块
	struct MEMORY_BLOCK * _block = global_memory_pool[_index]->_new;

	if ((_block->_end - _block->_p) < _size)
	{
		if (NULL == _block->_next)
		{
			// 如果 _size 大于 MEMORY_BLOCK_SIZE, 则直接开辟一个两倍 _size 大小的内存块
			struct MEMORY_BLOCK * _block_ = Expansion();
			_block_->_next = _block->_next;
			_block->_next = _block_;
			_block = _block_;
		}
		// 头内存块节点 _new 属性更新
		global_memory_pool[_index]->_new = _block;
	}

	// 若当前内存块剩余内存不够用, 则开辟新的内存块进行扩容, 开辟之前应先判断一下需要的内存大小 _size 是否大于 MEMORY_BLOCK_SIZE (每次开辟内存块的大小)
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
	// 循环释放链表内存
	while (NULL != _block)
	{
		// 重置内存内的内容, 防止意想不到的问题
		// memset(_block->_start, 0, _block->_end - _block->_start);
		_block->_p = _block->_start;
		_block->_next = _block->_next;
		_block->_new = NULL;
		_block = _block->_next;
	}
	global_memory_pool[_index]->_new = global_memory_pool[_index];
}