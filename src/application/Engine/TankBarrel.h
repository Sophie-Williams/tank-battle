#pragma once
#include "TexturedObject.h"
#include "Animation.h"

class TankBarrel : public TexturedObject
{
protected:
	Animation _fireAnim;
public:
	TankBarrel();
	virtual ~TankBarrel();
	void fire(float t);
	void setBound(const ci::Rectf& boundRect);
protected:
	void updateInternal(float t);
	void drawInternal();
};