#include "ScriptedPlayer.h"
#include "../GameControllers.h"
#include "../common/GameUtil.hpp"
#include "GameInterface.h"
#include "TankCommandsBuilder.h"
#include "PlayerSciptingLibrary.h"

#include <string>
#include <locale>
#include <codecvt>

#include <CompilerSuite.h>
#include <ScriptTask.h>
#include <Utils.h>

#include <memory>

using namespace ffscript;
using namespace std;

extern "C" {
	GAME_CONTROLLER_API ScriptedPlayer* createController() {
		return new ScriptedPlayer();
	}
}

class ScriptedPlayer::ScriptedPlayerImpl {
	GlobalScopeRef rootScope;
	Program* _program;
	int _functionIdOfMainFunction;
	shared_ptr<ScriptTask> _scriptTask;
	TankPlayerContext* _temporaryPlayerContex;
	PlayerSciptingLibrary _myScriptLib;
public:
	ScriptedPlayerImpl(const char* script) : _functionIdOfMainFunction(-1) {
		wstring scriptWstr = std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t>{}.from_bytes(script);
		CompilerSuite compiler;
		compiler.initialize(1024);

		rootScope = compiler.getGlobalScope();
		auto scriptCompiler = rootScope->getCompiler();

		_myScriptLib.loadLibrary(scriptCompiler);		
		
		_program = compiler.compileProgram(scriptWstr.c_str(), scriptWstr.c_str() + scriptWstr.size());

		if (_program) {
			_functionIdOfMainFunction = scriptCompiler->findFunction("frame", "");
			if (_functionIdOfMainFunction >= 0) {
				auto functionFactory = scriptCompiler->getFunctionFactory(_functionIdOfMainFunction);
				auto& returnType = functionFactory->getReturnType();

				// return type of frame function must be TankOperations
				if (returnType.iType() != _myScriptLib.TYPE_TANKOPERATIONS) {
					_functionIdOfMainFunction = -1;
				}

				_scriptTask = make_shared<ScriptTask>(_program);
			}
		}
	}

	bool isValidProgram() {
		return _functionIdOfMainFunction >= 0;
	}

	~ScriptedPlayerImpl(){
		if (_program) {
			delete _program;
		}
	}

	TankOperations giveOperations(TankPlayerContext* player) {
		// run function and allow maxium 5mb stack size
		_temporaryPlayerContex = player;
		try {
			_scriptTask->runFunction(5 * 1024 * 1024, _functionIdOfMainFunction, nullptr);
			auto pResult = (TankOperations*)_scriptTask->getTaskResult();
			return *pResult;
		}
		catch (std::exception&e) {			
		}

		return TANK_NULL_OPERATION;
	}
};

ScriptedPlayer::ScriptedPlayer() : _pImpl(nullptr)
{
}

ScriptedPlayer::~ScriptedPlayer()
{
	if (_pImpl) {
		delete _pImpl;
	}
}

bool ScriptedPlayer::setProgramScript(const char* script) {
	if (_pImpl) {
		delete _pImpl;
	}
	
	_pImpl = new  ScriptedPlayerImpl(script);
	if (_pImpl->isValidProgram() == false) {
		return false;
	}

	return false;
}

void ScriptedPlayer::setup(TankPlayerContext*) {
}

TankOperations ScriptedPlayer::giveOperations(TankPlayerContext* player) {
	return _pImpl->giveOperations(player);
}