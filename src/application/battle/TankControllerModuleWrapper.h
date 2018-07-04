#pragma once
#include "TankController.h"

class TankControllerModuleWrapper : public TankController {
private:
    class TankControllerLoader;
private:
    TankControllerLoader* _controllerLoader;
	TankController* _interface;
public:
	TankControllerModuleWrapper(const char* module);
	virtual ~TankControllerModuleWrapper();
	TankOperations giveOperations(TankPlayerContext* player);
};