#include "PlayerSciptingLibrary.h"
#include "../common/GameUtil.hpp"

#include <function/CdeclFunction.hpp>
#include <function/MemberFunction2.hpp>
#include "BasicFunction.h"

#include <memory>
#include <functional>
#include <random>

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
#define REGIST_GLOBAL_FUNCTION1(helper, func, returnType , ...) helper.registFunction(#func, #__VA_ARGS__, new BasicFunctionFactory<SIZE_OF_ARGS(__VA_ARGS__)>(EXP_UNIT_ID_USER_FUNC, FUNCTION_PRIORITY_USER_FUNCTION, #returnType, new CdeclFunction2<returnType, __VA_ARGS__>(func), helper.getSriptCompiler()))

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
		return _temporaryPlayerContex->isEnemy(id);
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

	PlayerContextSciptingLibrary::PlayerContextSciptingLibrary() : _commandBuilder(_tankOperations) {
		resetCommand();
	}

	void PlayerContextSciptingLibrary::resetCommand() {
		_tankOperations = TANK_NULL_OPERATION;
	}

	TankOperations PlayerContextSciptingLibrary::getOperations() const {
		return _tankOperations;
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
		auto NO_MOVE = make_shared<CdeclFunction<ConstOperandBase*, MovingDir>>(createMovingConsant);
		NO_MOVE->pushParam((void*)0);
		scriptCompiler->setConstantMap("NO_MOVE", NO_MOVE);

		auto MOVE_FORWARD = make_shared<CdeclFunction<ConstOperandBase*, MovingDir>>(createMovingConsant);
		MOVE_FORWARD->pushParam((void*)1);
		scriptCompiler->setConstantMap("MOVE_FORWARD", MOVE_FORWARD);

		auto MOVE_BACKWARD = make_shared<CdeclFunction<ConstOperandBase*, MovingDir>>(createMovingConsant);
		MOVE_BACKWARD->pushParam((void*)-1);
		scriptCompiler->setConstantMap("MOVE_BACKWARD", MOVE_BACKWARD);
		// turning consants
		auto NO_TURN = make_shared<CdeclFunction<ConstOperandBase*, TurningDir>>(createTurningConsant);
		NO_TURN->pushParam((void*)0);
		scriptCompiler->setConstantMap("NO_TURN", NO_TURN);

		auto TURN_LEFT = make_shared<CdeclFunction<ConstOperandBase*, TurningDir>>(createTurningConsant);
		TURN_LEFT->pushParam((void*)1);
		scriptCompiler->setConstantMap("TURN_LEFT", TURN_LEFT);

		auto TURN_RIGHT = make_shared<CdeclFunction<ConstOperandBase*, TurningDir>>(createTurningConsant);
		TURN_RIGHT->pushParam((void*)-1);
		scriptCompiler->setConstantMap("TURN_RIGHT", TURN_RIGHT);

		// turning consants
		auto NO_ROTATE = make_shared<CdeclFunction<ConstOperandBase*, RotatingDir>>(createRotatingConsant);
		NO_ROTATE->pushParam((void*)0);
		scriptCompiler->setConstantMap("NO_ROTATE", NO_ROTATE);

		auto ROTATE_LEFT = make_shared<CdeclFunction<ConstOperandBase*, RotatingDir>>(createRotatingConsant);
		ROTATE_LEFT->pushParam((void*)1);
		scriptCompiler->setConstantMap("ROTATE_LEFT", ROTATE_LEFT);

		auto ROTATE_RIGHT = make_shared<CdeclFunction<ConstOperandBase*, RotatingDir>>(createRotatingConsant);
		ROTATE_RIGHT->pushParam((void*)-1);
		scriptCompiler->setConstantMap("ROTATE_RIGHT", ROTATE_RIGHT);

		// register global functions
		loadContextFunctions(scriptCompiler);
	}
}