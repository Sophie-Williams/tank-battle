#pragma once
#include "TankController.h"
#include <memory>
#include "../common/SyncMessageQueue.hpp"
#include "Engine/Tank.h"

class TankControllerWorker {
	std::shared_ptr<TankController> _tankController;
	SignalAny _stopSignal;
	std::shared_ptr<Tank> _tank;
private:
public:
	TankControllerWorker(const std::shared_ptr<Tank>& tank, const std::shared_ptr<TankController>& tankController);
	virtual ~TankControllerWorker();

	void run();
	void stop();
	void pause();
	void resume();
};