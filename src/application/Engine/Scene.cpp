#include "Scene.h"

using namespace ci;

static Scene* s_currentScene = nullptr;

Scene::Scene(const ci::Rectf& area) : _sceneArea(area) {
	auto img = loadImage("E:\\Projects\\TankBattle\\src\\application\\assets\\background.jpg");
	_backGroundTex = gl::Texture2d::create(img);

	s_currentScene = this;
}

Scene::~Scene() {
	s_currentScene = nullptr;
}

void Scene::update(float t) {
	for (auto it = _gameObjects.begin(); it != _gameObjects.end(); it++) {
		(*it)->update(t);
	}
}

void Scene::draw() {
	// clipping by using ortho projection
	CameraOrtho orthoCam;
	orthoCam.setOrtho(_sceneArea.x1, _sceneArea.x2, _sceneArea.y1, _sceneArea.y2, 1, -1);
	gl::ScopedProjectionMatrix scopeMatrices;
	gl::setProjectionMatrix(orthoCam.getProjectionMatrix());

	//gl::color(0, 1, 0);
	//gl::drawSolidCircle(_sceneArea.getCenter(), _sceneArea.getWidth() / 2  + 20 );

	gl::draw(_backGroundTex, _sceneArea);

	for (auto it = _gameObjects.begin(); it != _gameObjects.end(); it++) {
		gl::ScopedModelMatrix modelMatrixScope;
		(*it)->draw();
	}
}

void Scene::setBackgroundColor(const ci::ColorA8u& backGround) {
	_backGround = backGround;
}

const ci::ColorA8u& Scene::getBackgroundColor() const {
	return _backGround;
}

void Scene::addGameObject(GameObjectRef gameObjectRef) {
	_gameObjects.push_back(gameObjectRef);
}

Scene* Scene::createScene(const ci::Rectf& area) {
	if (s_currentScene == nullptr) {
		return new Scene(area);
	}

	return s_currentScene;
}

Scene* Scene::getCurrentScene() {
	return s_currentScene;
}