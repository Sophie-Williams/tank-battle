/******************************************************************
* File:        GameControllerInterfaces.h
* Description: declare common interfaces and functions that used
*              in plugged-in C++ player controller library or script.
* Author:      Vincent Pham
*
* Copyright (c) 2018 VincentPT.
** Distributed under the MIT License (http://opensource.org/licenses/MIT)
**
*
**********************************************************************/

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
#define GAME_CONTROLLER_INTERFACE
#endif // _WIN32

typedef int GameObjectId;
typedef int GameObjectType;
typedef unsigned int TankOperations;

#define TANK_NULL_OPERATION (0)
#define FREEZE_COMMAND 0x80000000
#define IS_NULL_COMMAND(commands) ((commands & FREEZE_COMMAND) == 0)

//enum class GameObjectType : int
//{
//	Tank,
//	Barrier,
//	Bullet
//};

enum class CollisionPosition : char
{
	Unknown = 0,
	Front,
	Right,
	Bottom,
	Left
};


#pragma pack(push, 1)

struct RawPoint {
	float x;
	float y;
};

struct ColissionRawInfo {
	CollisionPosition collisionPosition;
	bool isExplosion;
};

struct GeometryInfo {
	RawPoint coord;
	float rotation;
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

struct RawTimeObject {
	// object id
	GameObjectId id;
	// float detected time
	float t;
	// shape of object, defined as polygon
	RawArray<RawPoint> shape;
};

#pragma pack(pop)

#if defined(__MACH__)
#include <stdlib.h>
#else
#include <malloc.h>
#endif

template <class T>
void initRawArray(RawArray<T>& arr) {
	arr.data = 0;
	arr.elmCount = 0;
}

template <class T>
void freeRawArray(RawArray<T>& arr) {
	if (arr.data) {
		free(arr.data);
		arr.data = nullptr;
	}
	arr.elmCount = 0;
}

template <class T>
void recreateRawArray(RawArray<T>& arr, int n) {
	if (n > 0) {
		arr.data = (T*)malloc(sizeof(T) * n);
	}
	else {
		arr.data = nullptr;
	}
	arr.elmCount = n;
}

//template GAME_CONTROLLER_INTERFACE RawArray<RawPoint> createRawArray<RawPoint>(int n);
//template GAME_CONTROLLER_INTERFACE RawArray<RawPoint> createRawArray<RawPoint>(int n);
//template GAME_CONTROLLER_INTERFACE RawArray<GameObjectId> createRawArray<GameObjectId>(int n);
//
//template GAME_CONTROLLER_INTERFACE void freeRawArray<RawObject>(RawArray<RawObject>& arr);
//template GAME_CONTROLLER_INTERFACE void freeRawArray<RawPoint>(RawArray<RawPoint>& arr);
//template GAME_CONTROLLER_INTERFACE void freeRawArray<GameObjectId>(RawArray<GameObjectId>& arr);

GAME_CONTROLLER_INTERFACE RawPoint operator-(const RawPoint& P);
GAME_CONTROLLER_INTERFACE RawPoint operator+(const RawPoint& P, const RawPoint& Q);
GAME_CONTROLLER_INTERFACE const RawPoint& operator+=(RawPoint& P, const RawPoint& Q);
GAME_CONTROLLER_INTERFACE RawPoint operator-(const RawPoint& P, const RawPoint& Q);
GAME_CONTROLLER_INTERFACE const RawPoint& operator-=(RawPoint& P, const RawPoint& Q);
GAME_CONTROLLER_INTERFACE float operator*(const RawPoint& P, const RawPoint& Q);

GAME_CONTROLLER_INTERFACE RawPoint operator*(const RawPoint& P, float k);
GAME_CONTROLLER_INTERFACE const RawPoint& operator*=(RawPoint& P, float k);
GAME_CONTROLLER_INTERFACE RawPoint operator/(const RawPoint& P, float k);
GAME_CONTROLLER_INTERFACE const RawPoint& operator/=(RawPoint& P, float k);
