#include <Windows.h>
#include "TankControllerModuleWrapper.h"
#include "../GameControllers/ScriptedPlayer/ScriptedPlayer.h"

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
	virtual ~TankControllerLoader() {
		if (_hLib) {
			FreeLibrary(_hLib);
		}
	}

	virtual TankController* createController() {
		if (_createControllerFx) {
			return _createControllerFx();
		}

		return nullptr;
	}
};

class TankControllerModuleWrapper::TankControllerLoaderForScript : public TankControllerModuleWrapper::TankControllerLoader {
	const std::wstring& _script;
public:
	TankControllerLoaderForScript(const char* module, const std::wstring& script) :
		TankControllerLoader(module),
		_script(script)
	{}

	virtual ~TankControllerLoaderForScript() {
	}

	virtual TankController* createController() {
		TankController* controller = TankControllerLoader::createController();
		ScriptedPlayer* sciptedPlayer = dynamic_cast<ScriptedPlayer*>(controller);
		if (sciptedPlayer) {
			//bool res = sciptedPlayer->setProgramScript(_script.c_str(), _script.c_str() + _script.size());
			//if (res == false) {
			//	delete sciptedPlayer;
			//	return nullptr;
			//}
		}

		return controller;
	}
};

TankControllerModuleWrapper::TankControllerModuleWrapper(const char* module)
{
	_controllerLoader = new TankControllerLoader(module);
	_interface = _controllerLoader->createController();
}

TankControllerModuleWrapper::TankControllerModuleWrapper(const char* module, const std::wstring& script) {
	_controllerLoader = new TankControllerLoaderForScript(module, script);
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