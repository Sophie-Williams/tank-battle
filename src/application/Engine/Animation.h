#pragma once
#include "TexturedObject.h"

class Animation : public TexturedObject
{
protected:
	ci::ivec2 _frameSize;
	int _iterator;
	int _frameCount;
	int _loop;
	float _startAt;
	float _frameDuration;
protected:
	virtual void updateInternal(float t);
	virtual void drawInternal();
	void autoCalculateFrameCount();
public:
	Animation();
	virtual ~Animation();

	void reset();
	void start(float t);
	void stop();

	// set loop <= 0 to loop the animation forever
	void setLoop(int loop);
	// set frameCount to -1 to auto compute frame count base on image size and frame size
	void setFrameCount(int frameCount);
	// default duration = 1/30 (second)
	void setDisplayFrameDuration(float duration);
};