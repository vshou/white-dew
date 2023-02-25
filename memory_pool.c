#include "memory_pool.h"

static pthread_key_t pt_key;

static pthread_once_t pt_once = PTHREAD_ONCE_INIT;

void init_pt_key()
{
	pthread_key_create(&pt_key, destroy_function);
}

void destroy_function(void * _p)
{
	MEMORY_BLOCK * _block = (MEMORY_BLOCK *) _p;
	MEMORY_BLOCK * _block_;
	while (1)
	{
		if (NULL == _block)
		{
			break;
		}
		_block_ = _block->_next;
		free(_block->_start);
		free(_block);
		_block = _block_;
	}
}

void init_memory_block(MEMORY_BLOCK * _block, MEMORY_BLOCK * _next, size_t _size)
{
	// 开辟内存
	_block->_start = malloc(_size);
	// 指针偏移
	_block->_end = _block->_start + _size;
	// 初始化属性
	_block->_cap = _size;
	_block->_p = _block->_start;
	_block->_next = _next;
	_block->_new = NULL;
}

MEMORY_BLOCK * Expansion(int _multiple)
{
	// 获取需要扩容的大小
	int _size =  MEMORY_BLOCK_SIZE * _multiple;

	// 生成内存块结构体
	MEMORY_BLOCK * _block_ = (MEMORY_BLOCK *) malloc(sizeof(MEMORY_BLOCK));

	// 初始化内存块
	init_memory_block(_block_, NULL, _size);

	return _block_;
}

void * dy_malloc(size_t _size)
{
	// 程序周期内只执行一次的初始化线程本地缓存 key 操作
	pthread_once(&pt_once, init_pt_key);
	MEMORY_BLOCK * _head = (MEMORY_BLOCK *) pthread_getspecific(pt_key);
	if (NULL == _head)
	{
		MEMORY_BLOCK * _pt_block = (MEMORY_BLOCK *) malloc(sizeof(MEMORY_BLOCK));
		init_memory_block(_pt_block, NULL, MEMORY_BLOCK_SIZE);
		_pt_block->_new = _pt_block;
		pthread_setspecific(pt_key, _pt_block);
		_head = _pt_block;
	}
	MEMORY_BLOCK * _block = _head->_new;

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
			MEMORY_BLOCK * _block_ = Expansion(2);
			_block_->_next = _block->_next;
			_block->_next = _block_;
			_block = _block_;
		}
		// 头内存块节点 _new 属性更新
		_head->_new = _block;
	} else if ((_block->_end - _block->_p) < _size && _size < MEMORY_BLOCK_SIZE)
	{
		if (NULL != _block->_next)
		{
			_block = _block->_next;
		}
		else
		{
			// _size 小于 MEMORY_BLOCK_SIZE, 则开辟 MEMORY_BLOCK_SIZE 大小的内存块
			MEMORY_BLOCK * _block_ = Expansion(1);
			_block_->_next = _block->_next;
			_block->_next = _block_;
			_block = _block_;
		}
		// 头内存块节点 _new 属性更新
		_head->_new = _block;
	}

	// 转移指针
	void * p = _block->_p;
	// 指针偏移
	_block->_p = _block->_p + _size;

	return p;
}

void dy_free()
{
	// 从线程本地存储获取头部内存块节点
	MEMORY_BLOCK * _head = (MEMORY_BLOCK *) pthread_getspecific(pt_key);
	MEMORY_BLOCK * _block = _head;
	MEMORY_BLOCK * _new_ = _block->_new;

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
	_head->_new = _head;
}
