#pragma once
#include "Engine/DrawableObject.h"
#include "BattlePlatform.h"

class ObjectViewContainer : public GameComponent {
protected:
	std::weak_ptr<DrawableObject> _ownerObject;
	std::list<std::shared_ptr<SnapshotObject>> _modelSnapshotObjects;
	float _lastUpdate;
	bool _enableSnapshot;
public:
	ObjectViewContainer(const DrawableObjectRef& object);
	~ObjectViewContainer();

	void update(float t);
	const std::list<std::shared_ptr<SnapshotObject>>& getModelViewObjects() const;
	void enableSnapshot(bool enable);
};