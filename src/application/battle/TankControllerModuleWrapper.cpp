#include <Windows.h>

#include "TankControllerModuleWrapper.h"

typedef TankController* (*FCreateController)();

class TankControllerModuleWrapper::TankControllerLoader {
	HMODULE _hLib;
	FCreateController _createControllerFx;
public:
	TankControllerLoader(const char* module) {
		_hLib = LoadLibraryA(module);
		if (_hLib) {
			_createControllerFx = (FCreateController)GetProcAddress(_hLib, "createController");
		}
		else {
			_createControllerFx = nullptr;
		}
	}
	~TankControllerLoader() {
		if (_hLib) {
			FreeLibrary(_hLib);
		}
	}

	TankController* createController() {
		if (_createControllerFx) {
			return _createControllerFx();
		}

		return nullptr;
	}
};

TankControllerModuleWrapper::TankControllerModuleWrapper(const char* module)
{
	_controllerLoader = new TankControllerLoader(module);
	_interface = _controllerLoader->createController();
}

TankControllerModuleWrapper::~TankControllerModuleWrapper() {
	if (_interface) {
		delete _interface;
	}
	if (_controllerLoader) {
		delete _controllerLoader;
	}
}

void TankControllerModuleWrapper::setup(TankPlayerContext* player) {
	if (_interface) {
		return _interface->setup(player);
	}
}

TankOperations TankControllerModuleWrapper::giveOperations(TankPlayerContext* player) {
	if (_interface) {
		return _interface->giveOperations(player);
	}
	return TANK_NULL_OPERATION;
}