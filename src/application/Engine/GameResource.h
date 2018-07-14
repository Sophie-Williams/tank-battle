#pragma once
#include "cinder/gl/gl.h"
#include <map>
#include <string>

class GameResource
{
private:
	std::map<int, ci::gl::Texture2dRef> _textureMap;
protected:
	GameResource();
public:
	static GameResource* createInstance();
	static GameResource* getInstance();
	virtual ~GameResource();

	void setTexture(int id, ci::gl::Texture2dRef);
	void setTexture(int id, const std::string& file);
	ci::gl::Texture2dRef getTexture(int id);
};


enum ResourceId : int {
	TEX_ID_BULLET = 0,
	TEX_ID_EXPLOSION,
	TEX_ID_TANKBODY,
	TEX_ID_TANKBARREL,
	TEX_ID_TANKSHOT,
	TEX_ID_BACKGROUND,
};
