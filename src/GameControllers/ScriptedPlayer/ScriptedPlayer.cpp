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
#include <DefaultPreprocessor.h>
#include <RawStringLib.h>
#include <MathLib.h>
#include <GeometryLib.h>
#include <CLamdaProg.h>

#include <memory>

using namespace ffscript;
using namespace std;

class MyComplicationLogger : public CompilationLogger {
	TankController* _controller;
public:
	MyComplicationLogger(TankController* controller) : _controller(controller) {
	}

	void log(MessageType type, const wchar_t* message) {
		string str;
		if (type == MESSAGE_INFO) {
			str.append("[INFO]   ");
		}
		else if (type == MESSAGE_WARNING) {
			str.append("[WARNING]");
		}
		else if (type == MESSAGE_ERROR) {
			str.append("[ERROR]  ");
		}
		str.append(convertToAscii(message));
		str.append(1, '\n');
		GameInterface::getInstance()->printMessage(_controller->getName(), str.c_str());
	}
};

class ScriptedPlayer::ScriptedPlayerImpl {
	CLamdaProg* _program;
	ScriptRunner* _updateRunner;
	int _setupFunctionId = -1;
	int _cleanupFunctionId = -1;
	float _lastPrintSize = -1;

	ScriptingLib::PlayerContextSciptingLibrary _myScriptLib;
	MyComplicationLogger _compicationLoger;
	string _errorMessage;
public:
	ScriptedPlayerImpl(TankController* controller) : _program(nullptr), _updateRunner(nullptr), _compicationLoger(controller) {
		//wstring scriptWstr = std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t>{}.from_bytes(script);
		_myScriptLib.setController(controller);
	}

	const char* setProgramScipt(const wchar_t* scriptStart, const wchar_t* scriptEnd) {
		CompilerSuite _compiler;
		_compiler.getCompiler()->setLogger(&_compicationLoger);
		// initialize compiler and specify 5mb of stack memory
		_compiler.initialize(5 * 1024 * 1024);

		auto& rootScope = _compiler.getGlobalScope();
		auto scriptCompiler = rootScope->getCompiler();
		includeRawStringToCompiler(scriptCompiler);
		includeMathToCompiler(scriptCompiler);
		includeGeoLibToCompiler(scriptCompiler);
		
		_myScriptLib.loadLibrary(scriptCompiler);

		scriptCompiler->beginUserLib();
		_compiler.setPreprocessor(std::make_shared<DefaultPreprocessor>());

		scriptCompiler->setErrorText("");

		if (_updateRunner) delete _updateRunner;
		_updateRunner = nullptr;

		Program* program = nullptr;
		try {
			program = _compiler.compileProgram(scriptStart, scriptEnd);
		}
		catch (exception& e) {
			_errorMessage = e.what();
			scriptCompiler->setErrorText(_errorMessage);
			int line, column;
			_compiler.getLastCompliedPosition(line, column);
			string message("error at line = ");
			message += std::to_string(line);
			message.append(", column =");
			message += std::to_string(column);
			message += '\n';
			GameInterface::getInstance()->printMessage(_myScriptLib.getController()->getName(), message.c_str());
		}
		if (program) {
			auto functionIdOfMainFunction = scriptCompiler->findFunction("update", "float");
			if (functionIdOfMainFunction >= 0) {
				auto functionFactory = scriptCompiler->getFunctionFactory(functionIdOfMainFunction);
				_updateRunner = new ScriptRunner(program, functionIdOfMainFunction);
			}
			else {
				_errorMessage = "function 'void update(float)' is not found";
			}

			_setupFunctionId = scriptCompiler->findFunction("setup", "");
			_cleanupFunctionId = scriptCompiler->findFunction("cleanup", "");

			if(_setupFunctionId < 0) {
				GameInterface::getInstance()->printMessage(_myScriptLib.getController()->getName(), "function 'void setup()' is not found\n");
			}
			if (_cleanupFunctionId < 0) {
				GameInterface::getInstance()->printMessage(_myScriptLib.getController()->getName(), "function 'void cleanup()' is not found\n");
			}
			_program = rootScope->detachScriptProgram(program);
		}
		else {
			_errorMessage = scriptCompiler->getLastError();
		}

		if (_updateRunner == nullptr) {
			return _errorMessage.c_str();
		}

		return nullptr;
	}

	~ScriptedPlayerImpl(){
		if (_program) {
			delete _program;
		}
	}

	void setup(TankPlayerContext* player){
		_program->runGlobalCode();
		_myScriptLib.resetCommand();
		_myScriptLib.setContext(player);

		if (_setupFunctionId >= 0) {
			try {
				auto& context = _program->getGlobalContext();
				Context::makeCurrent(context.get());
				ScriptRunner setupRunner(_program->getProgram(), _setupFunctionId);
				setupRunner.runFunction(nullptr);
			}
			catch (std::exception&e) {
				string message = "setup funtion failed to execute:";
				message += e.what();
				message += "\n";
				GameInterface::getInstance()->printMessage(_myScriptLib.getController()->getName(), message.c_str());
			}
		}
	}

	void cleanup(TankPlayerContext* player) {
		_myScriptLib.setContext(player);

		if (_cleanupFunctionId >= 0) {
			try {
				auto& context = _program->getGlobalContext();
				Context::makeCurrent(context.get());
				ScriptRunner setupRunner(_program->getProgram(), _cleanupFunctionId);
				setupRunner.runFunction(nullptr);
			}
			catch (std::exception&e) {
				string message = "clean funtion failed to execute:";
				message += e.what();
				message += "\n";
				GameInterface::getInstance()->printMessage(_myScriptLib.getController()->getName(), message.c_str());
			}
		}

		_program->cleanupGlobalMemory();
	}

	TankOperations giveOperations(TankPlayerContext* player) {
		_myScriptLib.setContext(player);

		if (_program && _updateRunner) {
			float t = GameInterface::getInstance()->getTime();
			//if (t - _lastPrintSize > 1.0f) {
			//	auto context = _program->getGlobalContext();
			//	auto totalSize = context->getTotalAllocatedSize();
			//	auto offset = context->getCurrentOffset();

			//	string message = "script allocated:";
			//	message += std::to_string(totalSize);
			//	message += "\nscript offset:";
			//	message += std::to_string(offset);
			//	message += "\n";
			//	GameInterface::getInstance()->printMessage(_myScriptLib.getController()->getName(), message.c_str());
			//	_lastPrintSize = t;
			//}

			try {
				ScriptParamBuffer paramsBuffer(t);
				auto& context = _program->getGlobalContext();
				Context::makeCurrent(context.get());
				_updateRunner->runFunction(&paramsBuffer);
				return _myScriptLib.getOperations();
			}
			catch (std::exception&e) {
				string message = "update funtion failed to execute:";
				message += e.what();
				message += "\n";
				GameInterface::getInstance()->printMessage(_myScriptLib.getController()->getName(), message.c_str());
			}
		}

		return TANK_NULL_OPERATION;
	}
};

ScriptedPlayer::ScriptedPlayer() : _pImpl(nullptr)
{
	_pImpl = new ScriptedPlayerImpl(this);
}

ScriptedPlayer::~ScriptedPlayer()
{
	if (_pImpl) {
		delete _pImpl;
	}
}

const char* ScriptedPlayer::setProgramScript(const wchar_t* scriptStart, const wchar_t* scriptEnd) {
	return _pImpl->setProgramScipt(scriptStart, scriptEnd);
}

const char* ScriptedPlayer::setProgramScript(const char* file) {
	auto wstr = readCodeFromUtf8File(file);

	return setProgramScript(wstr.c_str(), wstr.c_str() + wstr.size());
}

void ScriptedPlayer::setup(TankPlayerContext* player) {
	_pImpl->setup(player);
}

void ScriptedPlayer::cleanup(TankPlayerContext* player) {
	_pImpl->cleanup(player);
}

TankOperations ScriptedPlayer::giveOperations(TankPlayerContext* player) {
	return _pImpl->giveOperations(player);
	//return TANK_NULL_OPERATION;
}