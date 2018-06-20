#pragma once

typedef int ObjectId;

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
	ObjectId id;
	// shape of object, defined as polygon
	RawArray<RawPoint> shape;
};

#pragma pack(pop)

class PlayerContext {
public:
	PlayerContext();
	virtual ~PlayerContext();

	virtual RawArray<RawObject> takeRadarSnapshot() = 0;
	virtual RawArray<RawObject> takeCameraSnapshot() = 0;
	virtual RawRay getMyGun() = 0;
	virtual float getMyHealth() = 0;
};

class GameInterface {
public:
	GameInterface();
	virtual ~GameInterface();
	static GameInterface* getInstance();

	virtual float getObjectHealth(ObjectId id) = 0;
};