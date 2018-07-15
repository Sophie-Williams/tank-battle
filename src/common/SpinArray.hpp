#pragma once
template <class T, int ARRAY_SIZE>
class SpinArray {
	T _arr[ARRAY_SIZE];
	int _cap;
	int _n;

	inline T& nextInternal() {
		if (_n == ARRAY_SIZE) {
			return _arr[_n = 0];
		}
		return _arr[_n++];
	}

public:
	SpinArray(const T& defaultValue) : 
		_arr{ defaultValue },
		_n(0),
		_cap(0)
	{}

	SpinArray() :_n(0), _cap(0)
	{}

	inline T& next() {
		T& val = nextInternal();
		if (_cap < _n) {
			_cap = _n;
		}

		return val;
	}

	inline int cap() const {
		return _cap;
	}

	inline T& operator[](int i) {
		return _arr[i];
	}

	inline const T& operator[](int i) const {
		return _arr[i];
	}
};