#pragma once

template<class T>
struct Array {

	T* data;
	int size;
	int capacity;
	int num;

	Array() : data(nullptr), size(0), capacity(0), num(0) {}
	~Array() {
		if (data != 0) {
			delete[] data;
		}
	}

	void* alloc(int sz) {
		if (size + sz > capacity) {
			int d = capacity * 2 + 8;
			if (d < sz) {
				d = sz * 2 + 8;
			}
			resize(d);
		}
		auto res = data + size;
		size += sz;
		int d = sz / 4;
		if (d == 0) {
			d = 1;
		}
		num += d;
		return res;
	}
	void resize(int newCap) {
		if (newCap > capacity) {
			char* tmp = new T[newCap];
			if (data != nullptr) {
				memcpy(tmp, data, size);
				delete[] data;
			}
			capacity = newCap;
			data = tmp;
		}
	}
	int append(const T& s) {
		if (size + 1 > capacity) {
			resize(capacity + 9);
		}
		int ret = size;
		data[size++] = s;
		return ret;
	}
	int append(const T* s, int num) {
		if (size + num > capacity) {
			resize(capacity + num + 9);
		}
		int ret = size;
		for (int i = 0; i < num; ++i) {
			data[size++] = s[i];
		}
		return ret;
	}
	const T& get(int index) const {
		return data[index];
	}

};