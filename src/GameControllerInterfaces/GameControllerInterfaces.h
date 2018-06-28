#pragma once

#ifdef _WIN32
#define TEMPLATE_EXTERN
#ifdef EXPORT_GAME_CONTROLLER_INTERFACES
#define GAME_CONTROLLER_INTERFACE __declspec(dllexport)
#else
#define GAME_CONTROLLER_INTERFACE __declspec(dllimport)
#endif // EXPORT_GAME_CONTROLLER_INTERFACES
#else
#define TEMPLATE_EXTERN extern
#define GAME_CONTROLLER_API
#endif // _WIN32

typedef int GameObjectId;
typedef int GameObjectType;
typedef unsigned int TankOperations;

#define TANK_NULL_OPERATION (0)
#define FREEZE_COMMAND 0xFF000000
#define IS_NULL_COMMAND(commands) ((commands & FREEZE_COMMAND) == 0)

//enum class GameObjectType : int
//{
//	Tank,
//	Barrier,
//	Bullet
//};


#pragma pack(push, 1)

struct RawPoint {
	float x;
	float y;
};

struct RawRay {
	RawPoint start;
	RawPoint dir;
};

template <class T>
struct RawArray {
	int elmCount;
	T* data;
};

struct RawObject {
	// object id
	GameObjectId id;
	// shape of object, defined as polygon
	RawArray<RawPoint> shape;
};

#pragma pack(pop)

#include <malloc.h>

template <class T>
void initRawArray(RawArray<T>& arr) {
	arr.data = 0;
	arr.elmCount = 0;
}

template <class T>
void freeRawArray(RawArray<T>& arr) {
	if(arr.data) free(arr.data);
}

template <class T>
void recreateRawArray(RawArray<T>& arr, int n) {
	if (n > 0) {
		arr.data = (T*)malloc(sizeof(T) * n);
	}
}

//template GAME_CONTROLLER_INTERFACE RawArray<RawPoint> createRawArray<RawPoint>(int n);
//template GAME_CONTROLLER_INTERFACE RawArray<RawPoint> createRawArray<RawPoint>(int n);
//template GAME_CONTROLLER_INTERFACE RawArray<GameObjectId> createRawArray<GameObjectId>(int n);
//
//template GAME_CONTROLLER_INTERFACE void freeRawArray<RawObject>(RawArray<RawObject>& arr);
//template GAME_CONTROLLER_INTERFACE void freeRawArray<RawPoint>(RawArray<RawPoint>& arr);
//template GAME_CONTROLLER_INTERFACE void freeRawArray<GameObjectId>(RawArray<GameObjectId>& arr);