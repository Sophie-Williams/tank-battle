#include "TankControllerWorker.h"
#include "Engine/GameEngine.h"

TankControllerWorker::TankControllerWorker(const std::shared_ptr<TankController>& tankController) :
	_tankController(tankController),
	_stopSignal(false)
{
}

TankControllerWorker::~TankControllerWorker() {}

long long getCurrentTimeStamp() {
	using namespace std::chrono;
	auto now = system_clock::now();
	auto now_ms = time_point_cast<milliseconds>(now);
	return now_ms.time_since_epoch().count();
}

void TankControllerWorker::run() {
	constexpr unsigned int requestControlInterval = 200;
	unsigned int timeLeft = 0;
	bool signal = false;

	TankOperations tankCommands = TANK_NULL_OPERATION;
	GameEngine* gameEngine = GameEngine::getInstance();
	TankPlayerContext* playerContext = nullptr;
	do
	{
		auto t1 = getCurrentTimeStamp();
		tankCommands = _tankController->giveOperations(playerContext);
		if (!IS_NULL_COMMAND(tankCommands) && _tank && _tank->isAvailable()) {
			auto tank = _tank;
			auto task = [tank, tankCommands](float t) {
				if (tank->isAvailable() == false) return;

				auto scopeCommands = tankCommands;
				TankCommandsBuilder commandsBuilder(scopeCommands);
				tank->move(commandsBuilder.getMovingDir(), t);
				tank->turn(commandsBuilder.getTurnDir(), t);
				tank->spinBarrel(commandsBuilder.getSpinningGunDir(), t);
			};

			gameEngine->postTask(task);
		}
		auto t2 = getCurrentTimeStamp();

		timeLeft = (unsigned int)(t2 - t1);
		timeLeft = timeLeft > requestControlInterval ? 0 : requestControlInterval - timeLeft;
		// check if a stop signal was sent then exit the loop
	} while (_stopSignal.waitSignal(signal, timeLeft) == false);
}

void TankControllerWorker::stop() {
}

void TankControllerWorker::pause() {
}

void TankControllerWorker::resume() {
}