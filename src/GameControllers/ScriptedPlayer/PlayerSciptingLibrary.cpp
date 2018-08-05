#include "PlayerSciptingLibrary.h"
#include "../common/GameUtil.hpp"
#include "GameInterface.h"

#include <function/CdeclFunction.hpp>
#include <function/MemberFunction2.hpp>
#include <BasicFunction.h>
#include <Utils.h>

#include <memory>
#include <functional>
#include <random>

using namespace ffscript;
using namespace std;

// use for random functions
std::default_random_engine generator;

#define SIZE_OF_ARGS(...) ArgumentFunctionCounter<__VA_ARGS__>::count

#define REGIST_GLOBAL_FUNCTION1(helper, nativeFunc, scriptFunc, returnType,...) \
	helper.registFunction(\
		scriptFunc, #__VA_ARGS__,\
		createUserFunctionFactoryCdecl<returnType, __VA_ARGS__>(helper.getSriptCompiler(), #returnType, nativeFunc)\
	)

#define REGIST_GLOBAL_FUNCTION2(helper, func, returnType, ...) REGIST_GLOBAL_FUNCTION1(helper, func, #func, returnType, __VA_ARGS__)

#define REGIST_CONTEXT_FUNCTION1(helper, nativeFunc, scriptFunc, returnType,...) \
	helper.registFunction(\
		scriptFunc, #__VA_ARGS__,\
		createUserFunctionFactoryMember<PlayerContextSciptingLibrary, returnType, __VA_ARGS__>(helper.getSriptCompiler(), this, #returnType, &PlayerContextSciptingLibrary::nativeFunc)\
	)

#define REGIST_CONTEXT_FUNCTION2(helper, func, returnType, ...) REGIST_CONTEXT_FUNCTION1(helper, func, #func, returnType, __VA_ARGS__)

namespace ScriptingLib {
	///////////////////////////////////////////////////////////////////////////////////////////
	ConstOperandBase* createMovingConsant(MovingDir cosnt_val) {
		return new CConstOperand<MovingDir>(cosnt_val, "MovingDir");
	}

	ConstOperandBase* createTurningConsant(TurningDir cosnt_val) {
		return new CConstOperand<TurningDir>(cosnt_val, "TurningDir");
	}

	ConstOperandBase* createRotatingConsant(RotatingDir cosnt_val) {
		return new CConstOperand<RotatingDir>(cosnt_val, "RotatingDir");
	}

	///////////////////////////////////////////////////////////////////////////////////////////
	void PlayerContextSciptingLibrary::move(MovingDir dir) {
		if (abs(dir) > 1) {;
			GameInterface::getInstance()->printMessage(_theController->getName(),
				"warning!!! Moving direction is out of bound\n");
		}
		_commandBuilder.move(dir);
	}

	void PlayerContextSciptingLibrary::turn(TurningDir dir) {
		if (abs(dir) > 1) {
			;
			GameInterface::getInstance()->printMessage(_theController->getName(),
				"warning!!! Turning direction is out of bound\n");
		}
		_commandBuilder.turn(dir);
	}

	void PlayerContextSciptingLibrary::rotateGun(RotatingDir dir) {
		if (abs(dir) > 1) {
			;
			GameInterface::getInstance()->printMessage(_theController->getName(),
				"warning!!! Rotation direction is out of bound\n");
		}
		_commandBuilder.spinGun(dir);
	}

	void PlayerContextSciptingLibrary::freeze() {
		_commandBuilder.freeze();
	}

	void PlayerContextSciptingLibrary::fire() {
		_commandBuilder.fire();
	}

	void PlayerContextSciptingLibrary::keepPreviousState() {
		_tankOperations = TANK_NULL_OPERATION;
	}

	MovingDir PlayerContextSciptingLibrary::getMovingDir() {
		return _commandBuilder.getMovingDir();
	}

	TurningDir PlayerContextSciptingLibrary::getTurnDir() {
		return _commandBuilder.getTurnDir();
	}

	RotatingDir PlayerContextSciptingLibrary::getRotatingGunDir() {
		return _commandBuilder.getSpinningGunDir();
	}

	///////////////////////////////////////////////////////////////////////////////////////////
	Ray PlayerContextSciptingLibrary::gun() {
		return _temporaryPlayerContex->getMyGun();
	}

	float PlayerContextSciptingLibrary::health() {
		return _temporaryPlayerContex->getMyHealth();
	}

	GeometryInfo PlayerContextSciptingLibrary::geometry() {
		return _temporaryPlayerContex->getMyGeometry();
	}

	float PlayerContextSciptingLibrary::movingSpeed() {
		return _temporaryPlayerContex->getMovingSpeed();
	}

	float PlayerContextSciptingLibrary::turningSpeed() {
		return _temporaryPlayerContex->getTurningSpeed();
	}

	bool PlayerContextSciptingLibrary::isAlly(GameObjectId id) {
		return _temporaryPlayerContex->isAlly(id);
	}

	bool PlayerContextSciptingLibrary::isEnemy(GameObjectId id) {
		_temporaryPlayerContex->getCameraSnapshot();
		return _temporaryPlayerContex->isEnemy(id);
	}

	//////////////////////////////////////////////////////////////////////////////////////////
	void PlayerContextSciptingLibrary::println(String& rws) {
		auto str = convertToAscii(rws.elms, rws.size);
		str.append(1, '\n');
		GameInterface::getInstance()->printMessage(_theController->getName(), str.c_str());
	}

	void PlayerContextSciptingLibrary::println(wstring& s) {
		auto str = convertToAscii(s.c_str(), (int)s.size());
		str.append(1, '\n');
		GameInterface::getInstance()->printMessage(_theController->getName(), str.c_str());
	}

	void PlayerContextSciptingLibrary::println(string& s) {
		auto str = s;
		str.append(1, '\n');
		GameInterface::getInstance()->printMessage(_theController->getName(), str.c_str());
	}

	int random(int a, int b) {
		std::uniform_int_distribution<int> distribution(a, b);
		return distribution(generator);
	}

	int random() {
		return random(INT_MIN, INT_MAX);
	}

	const char* moveToText(MovingDir dir) {
		if (dir == 0) {
			return "NO_MOVE";
		}
		else if (dir == 1) {
			return "MOVE_FORWARD";
			
		}
		else if (dir == -1) {
			return "MOVE_BACKWARD";
		}
		return "UNKNOWN";
	}

	RawString moveToString(MovingDir dir) {
		RawString rws;
		constantConstructor(rws, moveToText(dir));
		return rws;
	}

	const char* turnToText(TurningDir dir) {
		if (dir == 0) {
			return "NO_TURN";
		}
		else if (dir == 1) {
			return "TURN_LEFT";

		}
		else if (dir == -1) {
			return "TURN_RIGHT";
		}
		return "UNKNOWN";
	}

	RawString turnToString(TurningDir dir) {
		RawString rws;
		constantConstructor(rws, turnToText(dir));
		return rws;
	}

	const char* rotateToTex(RotatingDir dir) {
		if (dir == 0) {
			return "NO_ROTATE";
		}
		else if (dir == 1) {
			return "ROTATE_LEFT";

		}
		else if (dir == -1) {
			return "ROTATE_RIGHT";
		}
		return "UNKNOWN";
	}

	RawString rotateToString(RotatingDir dir) {
		RawString rws;
		constantConstructor(rws, rotateToTex(dir));
		return rws;
	}

	void PlayerContextSciptingLibrary::printMovingDir(MovingDir dir) {
		std::string str = moveToText(dir);
		str.append(1, '\n');
		GameInterface::getInstance()->printMessage(_theController->getName(), str.c_str());
	}

	void PlayerContextSciptingLibrary::printTurningDir(TurningDir dir) {
		std::string str = turnToText(dir);
		str.append(1, '\n');
		GameInterface::getInstance()->printMessage(_theController->getName(), str.c_str());
	}

	void PlayerContextSciptingLibrary::printRotatingDir(RotatingDir dir) {
		std::string str = rotateToTex(dir);
		str.append(1, '\n');
		GameInterface::getInstance()->printMessage(_theController->getName(), str.c_str());
	}

	float getTime() {
		return GameInterface::getInstance()->getTime();
	}

	///////////////////////////////////////////////////////////////////////////////////////////
	void PlayerContextSciptingLibrary::loadContextFunctions(ScriptCompiler* scriptCompiler) {
		FunctionRegisterHelper helper(scriptCompiler);
		REGIST_CONTEXT_FUNCTION2(helper, freeze, void);
		REGIST_CONTEXT_FUNCTION2(helper, fire, void);
		REGIST_CONTEXT_FUNCTION2(helper, keepPreviousState, void);
		REGIST_CONTEXT_FUNCTION2(helper, move, void, MovingDir);
		REGIST_CONTEXT_FUNCTION2(helper, turn, void, TurningDir);
		REGIST_CONTEXT_FUNCTION2(helper, rotateGun, void, RotatingDir);
		REGIST_CONTEXT_FUNCTION2(helper, getMovingDir, MovingDir);
		REGIST_CONTEXT_FUNCTION2(helper, getTurnDir, TurningDir);
		REGIST_CONTEXT_FUNCTION2(helper, getRotatingGunDir, RotatingDir);

		REGIST_CONTEXT_FUNCTION2(helper, gun, Ray);
		REGIST_CONTEXT_FUNCTION2(helper, health, float);
		REGIST_CONTEXT_FUNCTION2(helper, geometry, GeometryInfo);
		REGIST_CONTEXT_FUNCTION2(helper, movingSpeed, float);
		REGIST_CONTEXT_FUNCTION2(helper, turningSpeed, float);
		REGIST_CONTEXT_FUNCTION2(helper, isAlly, bool, GameObjectId);
		REGIST_CONTEXT_FUNCTION2(helper, isEnemy, bool, GameObjectId);
	}

	void PlayerContextSciptingLibrary::loadGlobalFunctions(ScriptCompiler* scriptCompiler) {
		FunctionRegisterHelper helper(scriptCompiler);

		auto& basicTypes = scriptCompiler->getTypeManager()->getBasicTypes();
		int iTypeString = basicTypes.TYPE_RAWSTRING;

		// type string object
		ScriptType typeString(iTypeString, scriptCompiler->getType(iTypeString));

		REGIST_CONTEXT_FUNCTION2(helper, println, void, String&);
		REGIST_CONTEXT_FUNCTION2(helper, println, void, wstring&);
		REGIST_CONTEXT_FUNCTION2(helper, println, void, string&);
		REGIST_GLOBAL_FUNCTION2(helper, random, int);
		REGIST_GLOBAL_FUNCTION2(helper, random, int, int, int);
		REGIST_GLOBAL_FUNCTION2(helper, getTime, float);

		REGIST_GLOBAL_FUNCTION1(helper, moveToString, "String", String, MovingDir);
		REGIST_GLOBAL_FUNCTION1(helper, turnToString, "String", String, TurningDir);
		REGIST_GLOBAL_FUNCTION1(helper, rotateToString, "String", String, RotatingDir);

		REGIST_CONTEXT_FUNCTION1(helper, printMovingDir, "println", void, MovingDir);
		REGIST_CONTEXT_FUNCTION1(helper, printTurningDir, "println", void, TurningDir);
		REGIST_CONTEXT_FUNCTION1(helper, printRotatingDir, "println", void, RotatingDir);
	}

	PlayerContextSciptingLibrary::PlayerContextSciptingLibrary() : 
		_commandBuilder(_tankOperations), _temporaryPlayerContex(nullptr), _theController(nullptr) {
		resetCommand();
	}

	void PlayerContextSciptingLibrary::resetCommand() {
		_tankOperations = TANK_NULL_OPERATION;
	}

	TankOperations PlayerContextSciptingLibrary::getOperations() const {
		return _tankOperations;
	}

	void PlayerContextSciptingLibrary::setContext(TankPlayerContext* context) {
		_temporaryPlayerContex = context;
	}

	void PlayerContextSciptingLibrary::setController(TankController* controler) {
		_theController = controler;
	}

	TankController* PlayerContextSciptingLibrary::getController() const {
		return _theController;
	}

	inline void addDirConstant(ScriptCompiler* scriptCompiler, char dir,
		const char* (*fDir2Text)(char), ConstOperandBase* (*fCreateDirConsant)(char)) {
		auto createConstantFuncObj = make_shared<CdeclFunction<ConstOperandBase*, MovingDir>>(fCreateDirConsant);
		createConstantFuncObj->pushParam((void*)dir);
		scriptCompiler->setConstantMap(fDir2Text(dir), createConstantFuncObj);
	}

	void PlayerContextSciptingLibrary::loadLibrary(ScriptCompiler* scriptCompiler) {
		// register types
		auto type = scriptCompiler->registType("MovingDir");
		scriptCompiler->setTypeSize(type, 1);

		type = scriptCompiler->registType("TurningDir");
		scriptCompiler->setTypeSize(type, 1);

		type = scriptCompiler->registType("RotatingDir");
		scriptCompiler->setTypeSize(type, 1);

		type = scriptCompiler->registType("GameObjectId");
		scriptCompiler->setTypeSize(type, sizeof(GameObjectId));

		auto& typeManager = scriptCompiler->getTypeManager();
		auto& basicTypes = typeManager->getBasicTypes();

		ScriptType typeFloat(basicTypes.TYPE_FLOAT, scriptCompiler->getType(basicTypes.TYPE_FLOAT).c_str());

		// register struct Point must be same as RawPoint
		StructClass* pointStruct = new StructClass(scriptCompiler, "Point");
		pointStruct->addMember(typeFloat, "x");
		pointStruct->addMember(typeFloat, "y");
		auto iTypePoint = scriptCompiler->registStruct(pointStruct);
		ScriptType typePoint(iTypePoint, scriptCompiler->getType(iTypePoint).c_str());

		// register struct Ray must be same as RawRay
		StructClass* rayStruct = new StructClass(scriptCompiler, "Ray");
		rayStruct->addMember(typePoint, "start");
		rayStruct->addMember(typePoint, "dir");
		auto iTypeRay = scriptCompiler->registStruct(rayStruct);

		// register struct GeometryInfo must be same as GeometryInfo of C++ type
		StructClass* geometryInfoStruct = new StructClass(scriptCompiler, "GeometryInfo");
		geometryInfoStruct->addMember(typePoint, "coord");
		geometryInfoStruct->addMember(typeFloat, "rotation");
		auto iGeometryInfoType = scriptCompiler->registStruct(geometryInfoStruct);

		// register contants
		// moving contants
		addDirConstant(scriptCompiler, 0, moveToText, createMovingConsant);
		addDirConstant(scriptCompiler, 1, moveToText, createMovingConsant);
		addDirConstant(scriptCompiler, -1, moveToText, createMovingConsant);

		// turning consants
		addDirConstant(scriptCompiler, 0, turnToText, createTurningConsant);
		addDirConstant(scriptCompiler, 1, turnToText, createTurningConsant);
		addDirConstant(scriptCompiler, -1, turnToText, createTurningConsant);
		
		// turning consants
		addDirConstant(scriptCompiler, 0, rotateToTex, createRotatingConsant);
		addDirConstant(scriptCompiler, 1, rotateToTex, createRotatingConsant);
		addDirConstant(scriptCompiler, -1, rotateToTex, createRotatingConsant);

		// register global functions
		loadGlobalFunctions(scriptCompiler);

		// register global functions
		loadContextFunctions(scriptCompiler);
	}
}