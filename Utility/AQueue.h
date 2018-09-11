/******************************************************************************
版权所有：	深圳市理邦精密仪器有限公司
项目名称：	LT
版本号：	1.0
文件名：	AQueue.h
生成日期：	2017.02.10
作者：		何登锋
功能说明：
	用数组实现的固定长度的队列，与STL中queue的最大区别在于其长度是固定的。
	注意：不能用于存储类对象，只能存储基本类型。
******************************************************************************/
#ifndef _AQUEUE_H
#define _AQUEUE_H


#include <cstring>
#include "DebugCtrl.h"
#include <string>


/******************************************************************************
作者: 何登锋, 2017.03.03
功能描述:
	固定长度的队列。从尾部添加数据，满了之后会从头部自动删除数据，以保持队列长
度不变。不能手动删除数据，但是可以全部清空。当存放类对象的时候，效率不高。
******************************************************************************/
template <typename T>
class AQueue
{
	public:
		// 取得第一个数据元素的引用。
		T front(void);

		// 移出第一个数据元素。
		void pop_front(void);

		// 追加一个数据元素。
		int push_back(const T &el);

		// 在队列最前端追加一个元素
		int push_front(const T &el);

		// 取得队列的第一个元素。
		T back(void);

		// 移出最后一个数据元素。
		void pop_back(void);

		// 判断队列是否已满。
		bool full(void) const;

		// 判断队列是否为空。
		bool empty() const;

		// 得到队列的大小。
		unsigned size() const;
		//获得队列头的位置
		unsigned head() const;

		// 从新设定队列的大小。
		unsigned resize(unsigned cap);

		// 清除队列。
		void clear(void);

		// 读出第index数据。
		T read(unsigned index) const;

		// 取得最新的长度为Len的数据
		unsigned read(T *buff, unsigned len) const;

		unsigned read(unsigned sindex, unsigned len, T *buff) const;

		// 重载=
		AQueue<T> &operator=(const AQueue<T> &aqueue);

		// 重载[]
		T operator[](unsigned index) const;

		// 读出第index数据。
		T at(unsigned index);

		unsigned int capacity(void) const
		{
			return _capacity;
		}

		AQueue(unsigned cap);
		AQueue();
		~AQueue();

	protected:
		T *_buff;
		unsigned _capacity;     // 该队列的容量，长度最大值
		unsigned _current_size;
		unsigned _head, _tail;  // 头索引和尾索引

	private:
		T _error_data;
};


/******************************************************************************
作者: 何登锋, 2017.02.10
功能描述:
	返回队列的第一个元素，也就是最老的一个数据。
参数说明:
返回值:
	队列的第一个元素
******************************************************************************/
template <typename T>
T AQueue<T>::front(void)
{
	assert(NULL != _buff);
	if(NULL == _buff || empty())
	{
		return _error_data;
	}

	return _buff[_head];
}



/******************************************************************************
作者: 何登锋, 2017.02.10
功能描述:
	删除第一个数据。
参数说明:
返回值:
******************************************************************************/
template <typename T>
void AQueue<T>::pop_front(void)
{
	if(_current_size <= 0)
	{
		return;
	}

	_head = (_head + 1) % _capacity;
	_current_size--;
}



/******************************************************************************
作者: 何登锋, 2017.02.10
功能描述:
	返回队列的最后一个元素，也就是最新的一个数据。
参数说明:
返回值:
	队列的最后一个元素
******************************************************************************/
template <typename T>
T AQueue<T>::back(void)
{
	assert(NULL !=_buff);
	if(NULL == _buff || empty() == true)
	{
		return _error_data;
	}

	return _buff[_tail];
}



/******************************************************************************
作者: 何登锋, 2017.02.10
功能描述:
	从队列尾部添加一个元素。
参数说明:
	el : 待添加待元素。
返回值:
	-1 : 添加失败
	 0 : 正常添加
	 1 : 有环回插入，即头部元素被删除的情况。
******************************************************************************/
template <typename T>
int AQueue<T>::push_back(const T &el)
{
	assert(NULL != _buff);
	if(NULL == _buff)
	{
		return -1;
	}

	int re = 0;

	if(full() == true)
	{
		_head = (_head + 1) % _capacity;
		_current_size--;
		re = 1;
	}

	_tail = (_tail +  1) % _capacity;
	_buff[_tail] = el;
	_current_size++;

	assert(size() <= _capacity);

	return re;
}


/******************************************************************************
作者: 何登锋, 2017.03.11
功能描述:
	在队列最前端追加一个元素。
参数说明:
返回值:
******************************************************************************/
template <typename T>
int AQueue<T>::push_front(const T &el)
{
	assert(NULL != _buff);
	if(NULL == _buff)
	{
		return -1;
	}
	if(full() == true)
	{
		return 1;
	}

	_head = (_head + _capacity - 1) % _capacity;
	_buff[_head] = el;
	_current_size++;

	assert(size() <= _capacity);

	return 0;
}

/******************************************************************************
作者: 返回值, 2017.12.10
功能描述:
	删除最后一个数据。
参数说明:
返回值:
******************************************************************************/
template <typename T>
void AQueue<T>::pop_back(void)
{
	if(_current_size <= 0)
	{
		return;
	}

	_current_size--;
	_tail--;
	_tail = (_tail < 0) ? 0 : _tail;
}



/******************************************************************************
作者: 返回值, 2017.02.10
功能描述:
	判断队列是否满了。
参数说明:
返回值:
	true : 数组满
	false : 数组还没满
******************************************************************************/
template <typename T>
bool AQueue<T>::full() const
{
	return (_capacity == _current_size);
}



/******************************************************************************
作者: 何登锋, 2017.02.10
功能描述:
	判断队列是否为空。
参数说明:
返回值:
	true : 队列空
	false : 队列不空
******************************************************************************/
template <typename T>
bool AQueue<T>::empty() const
{
	return (_current_size == 0);
}



/******************************************************************************
作者: 何登锋, 2017.02.10
功能说明:
	返回队列的长度
参数说明:
返回值:
	队列目前的长度
******************************************************************************/
template <typename T>
inline unsigned AQueue<T>::size() const
{
	return _current_size;
}
/******************************************************************************
作者: hedengfeng, 2017.02.10
功能说明:
	返回队列的长度
参数说明:
返回值:
	队列目前的长度
******************************************************************************/
template <typename T>
inline unsigned AQueue<T>::head() const
{
	return _head;
}


/******************************************************************************
作者: 何登锋, 2017.02.10
功能描述:
	重新设定大小。
参数说明:
返回值:
******************************************************************************/
template <typename T>
unsigned AQueue<T>::resize(unsigned cap)
{
	assert(cap > 0);
	if(cap <= 0)
	{
		return 0;
	}

	if(_buff != NULL)
	{
		delete[] _buff;
	}

	_capacity = cap;
	try    // 出现过new不成功导致死机的情况
	{
		_buff = new T[cap];
	}
	catch(std::bad_alloc)
	{
		_buff = NULL;
		_capacity = 0;
	}

	_current_size = 0;
	_head = 0;
	_tail = -1;

	return 1;
}



/******************************************************************************
作者: 何登锋, 2017.07.31
功能描述:
	清空队列。
参数说明:
返回值:
******************************************************************************/
template <typename T>
void AQueue<T>::clear(void)
{
	_current_size = 0;
	_head = 0;
	_tail = -1;
}



/******************************************************************************
作者: 何登锋, 2017.02.10
功能描述:
	返回从指定index值起，长度为len的一段数据，返回再buff[]中
参数说明:
	sindex : 起始点的索引
	   len : 希望取得数据的长度
	  buff : 取得数据后返回于该buff中
返回值:
	0 : 失败
	>0 : 取得元素的个数
说明：
	将该函数设置未私有，因为内外部用户来说，sindex很难计算，
也没有使用这种接口的可能。--lixiaojun 2017-04-22
更改:将此函数改为公有,供插值部件提供读取一段index区间的功能.
******************************************************************************/
template <typename T>
unsigned AQueue<T>::read(unsigned sindex, unsigned len, T *buff) const
{
	IF((NULL == _buff) || (empty() == true) || (buff == NULL))
	{
		return 0;
	}

	if(((sindex + len) <= _capacity))
	{
		memcpy(buff, _buff + (sindex) % _capacity, len * sizeof(T));
	}
	else  // 如果是数据是越过数组尾部，拐弯回到数组开始处，这样要分两段来取数据
	{
		int first_part = _capacity - sindex;
		assert(first_part >= 0);

		memcpy(buff, _buff + sindex, first_part * sizeof(T));
		memcpy(buff + first_part, _buff, (len - first_part) * sizeof(T));
	}

	return len;
}



/******************************************************************************
作者: 何登锋, 2017.02.11
功能描述:
	返回该队列中指定index位置的元素。
参数说明:
	index: 指定的索引值。
返回值:
	返回指定索引值处的元素。
******************************************************************************/
template <typename T>
T AQueue<T>::read(unsigned index) const
{
	assert(index < _capacity);
	assert(NULL != _buff);

	if((NULL == _buff) || (empty() == true) || (index >= _capacity))
	{
		return _error_data;
	}

	return _buff[(index + _head) % _capacity];
}



/******************************************************************************
作者: 何登锋, 2017.02.27
功能描述:
	取得最新的长度为Len的数据，返回再buff[]中
参数说明:
	len : 希望取得数据的长度
	buff : 取得数据后返回于该buff中
返回值:
	0 : 失败
	>0 : 取得元素的个数
******************************************************************************/
template <typename T>
unsigned AQueue<T>::read(T *buff, unsigned len) const
{
	if(empty() == true)
	{
		return 0;
	}

	return _read((_tail - len + _capacity) % _capacity, len, buff);
}



/******************************************************************************
作者: 何登锋, 2017.04.28
功能说明:
参数说明:
返回值:
******************************************************************************/
template <typename T>
AQueue<T> &AQueue<T>::operator=(const AQueue<T> &aq)
{
	if (this == &aq)
	{
		return *this;
	}

	IF(_capacity != aq._capacity)
	{
		return *this;
	}

	_current_size = aq._current_size;
	_head = aq._head;
	_tail = aq._tail;

	memcpy(_buff, aq._buff, _capacity * sizeof(T));

	return *this;
}



/******************************************************************************
作者: 何登锋, 2017.09.28
功能说明:
	重载了[]，直接调用read实现。
参数说明:
返回值:
******************************************************************************/
template <typename T>
T AQueue<T>::operator[](unsigned index) const
{
	return read(index);
}



/******************************************************************************
作者: 何登锋, 2017.09.28
功能说明:
	read的另外封装。
参数说明:
返回值:
******************************************************************************/
template <typename T>
T AQueue<T>::at(unsigned index)
{
	return read(index);
}



/******************************************************************************
作者: 何登锋, 2017.02.10
功能说明:
	构造方法。
参数说明:
返回值:
******************************************************************************/
template <typename T>
AQueue<T>::AQueue(unsigned cap)
{
	_buff = new T[cap];
	_capacity = cap;
	_current_size = 0;
	_head = 0;
	_tail = -1;

	memset(&_error_data, 0, sizeof(T));
}



/******************************************************************************
作者: 何登锋, 2017.02.10
功能说明:
	构造方法。
参数说明:
返回值:
******************************************************************************/
template <typename T>
AQueue<T>::AQueue()
{
	_buff = NULL;
	_capacity = 0;
	_current_size = 0;
	_head = 0;
	_tail = -1;
	memset(&_error_data, 0, sizeof(T));
}



/******************************************************************************
作者: 何登锋, 2017.02.10
功能说明:
	析构方法。
参数说明:
返回值:
******************************************************************************/
template <typename T>
AQueue<T>::~AQueue()
{
	assert(_buff != NULL);
	delete[] _buff;
}


#endif

