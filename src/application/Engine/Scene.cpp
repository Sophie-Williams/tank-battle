#include "Scene.h"
#include "GameResource.h"
#include "LiveObject.h"

using namespace ci;

static Scene* s_currentScene = nullptr;

Scene::Scene(const ci::Rectf& area) : _sceneArea(area) {
	_backGroundTex = GameResource::getInstance()->getTexture(TEX_ID_BACKGROUND);

	s_currentScene = this;
}

Scene::~Scene() {
	s_currentScene = nullptr;
}

void Scene::update(float t) {
	for (auto it = _drawableObjects.begin(); it != _drawableObjects.end(); it++) {
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
	for (auto it = _drawableObjects.begin(); it != _drawableObjects.end(); it++) {
		auto liveObject = dynamic_cast<LiveObject*>(it->get());
		if (liveObject) {
			liveObject->drawHeathBar();
		}
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
}


const std::list<DrawableObjectRef>& Scene::getDrawableObjects() const {
	return _drawableObjects;
}


std::list<DrawableObjectRef>::const_iterator Scene::findObjectIter(const DrawableObject* pObject) const {
	for (auto it = _drawableObjects.begin(); it != _drawableObjects.end(); it++) {
		if (pObject == it->get()) {
			return it;
		}
	}

	return _drawableObjects.end();
}

DrawableObjectRef Scene::findObjectRef(const DrawableObject* pObject) const {
	auto it = findObjectIter(pObject);
	if (it == _drawableObjects.end()) {
		return nullptr;
	}

	return *it;
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