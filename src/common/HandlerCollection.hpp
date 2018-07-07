#pragma once
#include <map>

typedef int HandlerId;

template <class T>
class HandlerCollection
{
	HandlerId _idGenerator;
	std::map<HandlerId, T> _handlerMap;
public:
	HandlerCollection() {
		_idGenerator = 0;
	}
	~HandlerCollection() {

	}

	HandlerId operator+=(const T& handler) {
		_handlerMap[_idGenerator++] = handler;
		return _idGenerator;
	}

	HandlerId operator+=(T&& handler) {
		_handlerMap[_idGenerator++] = handler;
		return _idGenerator;
	}

	void operator-=(HandlerId id) {
		_handlerMap.erase(id);
	}

	template <class ...Types>
	void operator()(Types... args) {
		for (auto it = _handlerMap.begin(); it != _handlerMap.end(); it++) {
			(it->second) (args...);
		}
	}

	bool isEmpty() {
		return _handlerMap.size() == 0;
	}
};

