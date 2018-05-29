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

	for (auto it = _drawableObjects.begin(); it != _drawableObjects.end(); it++) {
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

void Scene::addDrawbleObject(DrawableObjectRef drawableObject) {
	_drawableObjects.push_back(drawableObject);
	addGameObject(drawableObject);
}

void Scene::addGameObject(GameObjectRef gameObjectRef) {
	_gameObjects.push_back(gameObjectRef);
}

void Scene::removeGameObject(GameObjectRef gameObjectRef) {
	auto it = findObjectIter(gameObjectRef.get());
	if (it == _gameObjects.end()) {
		return;
	}

	_gameObjects.erase(it);
}

std::list<GameObjectRef>::const_iterator Scene::findObjectIter(const GameObject* pObject) const {
	for (auto it = _gameObjects.begin(); it != _gameObjects.end(); it++) {
		if (pObject == it->get()) {
			return it;
		}
	}

	return _gameObjects.end();
}

GameObjectRef Scene::findObjectRef(const GameObject* pObject) const {
	auto it = findObjectIter(pObject);
	if (it == _gameObjects.end()) {
		return nullptr;
	}

	return *it;
}

const std::list<GameObjectRef>& Scene::getObjects() const {
	return _gameObjects;
}

std::list<GameObjectRef>& Scene::getObjects() {
	return _gameObjects;
}

const std::list<DrawableObjectRef>& Scene::getDrawableObjects() const {
	return _drawableObjects;
}

std::list<DrawableObjectRef>& Scene::getDrawableObjects() {
	return _drawableObjects;
}

const ci::Rectf& Scene::getSceneArea() const {
	return _sceneArea;
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