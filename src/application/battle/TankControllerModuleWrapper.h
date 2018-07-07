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
	
	void setup(TankPlayerContext* player);
	TankOperations giveOperations(TankPlayerContext* player);
};