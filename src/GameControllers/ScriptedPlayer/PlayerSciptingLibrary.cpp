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

#include <RawStringLib.h>

using namespace ffscript;
using namespace std;

// use for random functions
std::default_random_engine generator;

template <class ...Args>
class ArgumentFunctionCounter {
public:
	static constexpr int ArgumentSize = sizeof...(Args);
};


class TankPlayerFunctionFactory : public UserFunctionFactory
{
	function<DFunction2*()> _makeNative;
public:
	TankPlayerFunctionFactory(function<DFunction2*()>&& makeNative, ScriptCompiler* scriptCompiler, const char* returnType, int paramSize) :
		_makeNative(makeNative),
		UserFunctionFactory(scriptCompiler, returnType, paramSize) {}
	virtual ~TankPlayerFunctionFactory() {}

	DFunction2Ref TankPlayerFunctionFactory::createNative() {
		return DFunction2Ref(_makeNative());
	}
};

#define SIZE_OF_ARGS(...) ArgumentFunctionCounter<__VA_ARGS__>::ArgumentSize

#define REGIST_GLOBAL_FUNCTION00(helper, nativeFunc, scriptFunc, returnType) \
	helper.registFunction(\
		scriptFunc, "",\
		new TankPlayerFunctionFactory(\
			[](){ return new CdeclFunction2<returnType>(nativeFunc);},\
			helper.getSriptCompiler(), \
			#returnType,\
			0\
		)\
	)

#define REGIST_GLOBAL_FUNCTION01(helper, func, returnType) REGIST_GLOBAL_FUNCTION00(helper, func, #func, returnType)

#define REGIST_GLOBAL_FUNCTION10(helper, nativeFunc, scriptFunc, returnType , ...) helper.registFunction(scriptFunc, #__VA_ARGS__, new BasicFunctionFactory<SIZE_OF_ARGS(__VA_ARGS__)>(EXP_UNIT_ID_USER_FUNC, FUNCTION_PRIORITY_USER_FUNCTION, #returnType, new CdeclFunction2<returnType, __VA_ARGS__>(nativeFunc), helper.getSriptCompiler()))
#define REGIST_GLOBAL_FUNCTION11(helper, func, returnType , ...) REGIST_GLOBAL_FUNCTION10(helper, func, #func, returnType, __VA_ARGS__)

#define REGIST_CONTEXT_FUNCTION0(helper, func, returnType) \
	helper.registFunction(\
		#func, "",\
		new TankPlayerFunctionFactory(\
			[this](){ return new MFunction2<returnType, PlayerContextSciptingLibrary>(this, &PlayerContextSciptingLibrary::func);},\
			helper.getSriptCompiler(), \
			#returnType,\
			0\
		)\
	)

#define REGIST_CONTEXT_FUNCTION1(helper, func, returnType , ...) helper.registFunction(#func, #__VA_ARGS__, new BasicFunctionFactory<SIZE_OF_ARGS(__VA_ARGS__)>(EXP_UNIT_ID_USER_FUNC, FUNCTION_PRIORITY_USER_FUNCTION, #returnType, new MFunction2<returnType, PlayerContextSciptingLibrary, __VA_ARGS__>(this, &PlayerContextSciptingLibrary::func), helper.getSriptCompiler()))

namespace ScriptingLib {
	typedef RawString String;
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
		_commandBuilder.move(dir);
	}

	void PlayerContextSciptingLibrary::turn(TurningDir dir) {
		_commandBuilder.turn(dir);
	}

	void PlayerContextSciptingLibrary::rotateGun(RotatingDir dir) {
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
	void println(String& rws) {
		auto str = convertToAscii(rws.elms, rws.size);
		str.append(1, '\n');
		GameInterface::getInstance()->printMessage(str.c_str());
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

	void printMovingDir(MovingDir dir) {
		std::string str = moveToText(dir);
		str.append(1, '\n');
		GameInterface::getInstance()->printMessage(str.c_str());
	}

	void printTurningDir(TurningDir dir) {
		std::string str = turnToText(dir);
		str.append(1, '\n');
		GameInterface::getInstance()->printMessage(str.c_str());
	}

	void printRotatingDir(RotatingDir dir) {
		std::string str = rotateToTex(dir);
		str.append(1, '\n');
		GameInterface::getInstance()->printMessage(str.c_str());
	}

	///////////////////////////////////////////////////////////////////////////////////////////
	void PlayerContextSciptingLibrary::loadContextFunctions(ScriptCompiler* scriptCompiler) {
		FunctionRegisterHelper helper(scriptCompiler);
		REGIST_CONTEXT_FUNCTION0(helper, freeze, void);
		REGIST_CONTEXT_FUNCTION0(helper, fire, void);
		REGIST_CONTEXT_FUNCTION0(helper, keepPreviousState, void);
		REGIST_CONTEXT_FUNCTION1(helper, move, void, MovingDir);
		REGIST_CONTEXT_FUNCTION1(helper, turn, void, TurningDir);
		REGIST_CONTEXT_FUNCTION1(helper, rotateGun, void, RotatingDir);
		REGIST_CONTEXT_FUNCTION0(helper, getMovingDir, MovingDir);
		REGIST_CONTEXT_FUNCTION0(helper, getTurnDir, TurningDir);
		REGIST_CONTEXT_FUNCTION0(helper, getRotatingGunDir, RotatingDir);

		REGIST_CONTEXT_FUNCTION0(helper, gun, Ray);
		REGIST_CONTEXT_FUNCTION0(helper, health, float);
		REGIST_CONTEXT_FUNCTION0(helper, geometry, GeometryInfo);
		REGIST_CONTEXT_FUNCTION0(helper, movingSpeed, float);
		REGIST_CONTEXT_FUNCTION0(helper, turningSpeed, float);
		REGIST_CONTEXT_FUNCTION1(helper, isAlly, bool, GameObjectId);
		REGIST_CONTEXT_FUNCTION1(helper, isEnemy, bool, GameObjectId);
	}

	void loadGlobalFunctions(ScriptCompiler* scriptCompiler) {
		FunctionRegisterHelper helper(scriptCompiler);

		auto& basicTypes = scriptCompiler->getTypeManager()->getBasicTypes();
		int iTypeString = basicTypes.TYPE_RAWSTRING;

		// type string object
		ScriptType typeString(iTypeString, scriptCompiler->getType(iTypeString));

		REGIST_GLOBAL_FUNCTION11(helper, println, void, String&);
		REGIST_GLOBAL_FUNCTION01(helper, random, int);
		REGIST_GLOBAL_FUNCTION11(helper, random, int, int, int);

		helper.registFunction("String", "MovingDir", new ConvertToStringFactory(scriptCompiler, createStringNativeFunc<MovingDir>(moveToString), typeString));
		helper.registFunction("String", "TurningDir", new ConvertToStringFactory(scriptCompiler, createStringNativeFunc<TurningDir>(turnToString), typeString));
		helper.registFunction("String", "RotatingDir", new ConvertToStringFactory(scriptCompiler, createStringNativeFunc<RotatingDir>(rotateToString), typeString));

		REGIST_GLOBAL_FUNCTION10(helper, printMovingDir, "println", void, MovingDir);
		REGIST_GLOBAL_FUNCTION10(helper, printTurningDir, "println", void, TurningDir);
		REGIST_GLOBAL_FUNCTION10(helper, printRotatingDir, "println", void, RotatingDir);
	}

	PlayerContextSciptingLibrary::PlayerContextSciptingLibrary() : _commandBuilder(_tankOperations), _temporaryPlayerContex(nullptr){
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