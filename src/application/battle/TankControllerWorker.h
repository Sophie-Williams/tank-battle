#pragma once
#include "TankController.h"
#include <memory>
#include "../common/SyncMessageQueue.hpp"
#include "Engine/Tank.h"
#include "TankCamera.h"
#include "Radar.h"
#include <thread>
#include <mutex>
#include <string>

class TankControllerWorker {
	std::shared_ptr<TankController> _tankController;
	SignalAny _stopSignal;
	std::shared_ptr<Tank> _tank;
	std::shared_ptr<TankCamera> _tankCamera;
	std::shared_ptr<Radar> _radar;
	std::thread worker;
	SignalAny* _pWaitForReadySignal;

	std::list<ColissionRawInfo> _collisions;
	std::mutex _collsionsMutex;
	HandlerId _tankCollisionHandlerId;
	std::string _name;
	float _totalProcessingTime;
	int _frameCount;
private:
	// this method will be invoked in game UI thread
	void setUp();
	void loop();
	void onTankCollision(DrawableObjectRef other, const CollisionInfo& poistion, float t);
	void enablePeripherals(bool enable);
public:
	TankControllerWorker(const std::shared_ptr<Tank>& tank, const std::shared_ptr<TankController>& tankController);
	virtual ~TankControllerWorker();

	void run();
	bool stopAndWait(int milisecond);
	void pause();
	void resume();

	void setSignalWaiter(SignalAny* pSignal);
	const std::shared_ptr<Tank>& getAssociatedTank() const;
	const std::string& getName() const;
	void setName(std::string& name);
	float getAverageProcessingTimeOfController() const;
};