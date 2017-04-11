#pragma once
#include <stdint.h>

template <typename TYPE> void Construct(void* dest) {
	new ((TYPE*)dest) TYPE;
}

template <typename TYPE> void Destruct(void* dest) {
	((TYPE*)dest)->~TYPE();
}

// ------------------------------------------------------------
// BaseStack
// ------------------------------------------------------------
template<class T>
class BaseStack {

public:
	BaseStack() : _size(0), _capacity(0), _data(0) {
		_typeSize = sizeof(T);
		_constructor = !__has_trivial_constructor(T);
		_destructor = !__has_trivial_destructor(T);
	}

	virtual ~BaseStack() {
		if (_data != 0) {
			clear();
			delete[] _data;
		}
	}

	virtual bool allowed(int size) = 0;

	void push(const T& t) {
		if (allowed(_size)) {
			if (_size + 1 > _capacity) {
				grow(_capacity * 2 + 8);
			}
			_items[_size++] = t;
		}
	}

	void pop() {
		if (_size > 0) {
			uint8_t* ptr = _data + (_size - 1) * _typeSize;
			Destruct<T>(ptr);
			--_size;
		}
	}

	const T& top() const {
		return _items[_size - 1];
	}

	T& top() {
		return _items[_size - 1];
	}

	const T& at(int index) const {
		return _items[_size - index - 1];
	}

	T& at(int index) {
		return _items[_size - index - 1];
	}

	uint32_t size() const {
		return _size;
	}

	bool empty() const {
		return _size == 0;
	}

	void clear() {
		if (_destructor) {
			uint8_t* ptr = _data;
			for (uint32_t i = 0; i < _size; ++i) {
				Destruct<T>(ptr);
				ptr += _typeSize;
			}
		}
		_size = 0;
	}
private:
	void grow(uint32_t newCapacity) {
		if (newCapacity > _capacity) {
			uint8_t* newItems = new uint8_t[newCapacity * _typeSize];
			if (_data != 0) {
				memcpy(newItems, _data, _size * _typeSize);
			}
			uint8_t* ptr = newItems + _size * _typeSize;
			if (_data != 0) {
				delete[] _data;
			}
			_data = newItems;
			_capacity = newCapacity;
		}
	}
	uint32_t _size;
	uint32_t _capacity;
	bool _constructor;
	bool _destructor;
	uint32_t _typeSize;
	union {
		uint8_t* _data;
		T* _items;
	};

};

// ------------------------------------------------------------
// Stack
// ------------------------------------------------------------
template<class T>
class Stack : public BaseStack<T> {

public:
	Stack() : BaseStack() {}
	virtual ~Stack() {}
	bool allowed(int size) {
		return true;
	}
};

// ------------------------------------------------------------
// FixedSizeStack
// ------------------------------------------------------------
template<class T>
class FixedSizeStack : public BaseStack<T> {

public:
	FixedSizeStack(int max) : BaseStack() , _max(max) {}
	virtual ~FixedSizeStack() {}
	bool allowed(int size) {
		if (size >= _max) {
			return false;
		}
		return true;
	}
private:
	int _max;
};

