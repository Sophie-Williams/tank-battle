/******************************************************************
* File:        GameComponent.cpp
* Description: implement GameResource class. A class manages resource
*              of the game.
* Author:      Vincent Pham
*
* Copyright (c) 2018 VincentPT.
** Distributed under the MIT License (http://opensource.org/licenses/MIT)
**
*
**********************************************************************/

#include "GameResource.h"
#include "cinder/app/App.h"
#include "../common/ILogger.h"

using namespace ci;
using namespace std;

GameResource* GameResource::createInstance() {
	return getInstance();
}

GameResource* GameResource::getInstance() {
	static GameResource* s_Instance = nullptr;
	if (s_Instance == nullptr) {
		s_Instance = new GameResource();
	}

	return s_Instance;
}

GameResource::GameResource() {

}

GameResource::~GameResource() {
}

void GameResource::setTexture(int id, ci::gl::Texture2dRef tex) {
	_textureMap[id] = tex;
}

void GameResource::setTexture(int id, const std::string& file) {
	try {
		auto img = loadImage(ci::app::loadAsset((file)));
		auto tex = gl::Texture2d::create(img);

		setTexture(id, tex);
	}
	
	catch (ci::app::AssetLoadExc& e) {
		ILogger::getInstance()->logV(LogLevel::Error, "load asset error: ", e.what() );
	}
	
}

ci::gl::Texture2dRef GameResource::getTexture(int id) {
	auto it = _textureMap.find(id);
	if (it != _textureMap.end()) {
		return it->second;
	}

	return nullptr;
}