#pragma once
#include "TankController.h"

#include <string>

class TankControllerModuleWrapper : public TankController {
private:
    class TankControllerLoader;
	class TankControllerLoaderForScript;
private:
    TankControllerLoader* _controllerLoader;
	TankController* _interface;
public:
	TankControllerModuleWrapper(const char* module);
	virtual ~TankControllerModuleWrapper();
	
	void setup(TankPlayerContext* player);
	TankOperations giveOperations(TankPlayerContext* player);
};