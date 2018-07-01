#pragma once
#include "TankController.h"
#include <memory>
#include "../common/SyncMessageQueue.hpp"
#include "Engine/Tank.h"
#include "TankCamera.h"
#include "Radar.h"

class TankControllerWorker {
	std::shared_ptr<TankController> _tankController;
	SignalAny _stopSignal;
	std::shared_ptr<Tank> _tank;
	std::shared_ptr<TankCamera> _tankCamera;
	std::shared_ptr<Radar> _radar;
private:
public:
	TankControllerWorker(const std::shared_ptr<Tank>& tank, const std::shared_ptr<TankController>& tankController);
	virtual ~TankControllerWorker();

	// this method will be invoked in game UI thread
	void setUp();

	void run();
	void stop();
	void pause();
	void resume();
};