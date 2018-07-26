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


void PlayerContextSciptingLibrary::setMove(MovingDir dir) {
	_commandBuilder.move(dir);
}

void PlayerContextSciptingLibrary::setTurn(TurningDir dir) {
	_commandBuilder.turn(dir);
}

void PlayerContextSciptingLibrary::setGunTurn(TurningDir dir) {
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

ConstOperandBase* createMovingConsant(MovingDir cosnt_val) {
	return new CConstOperand<MovingDir>(cosnt_val, "MovingDir");
}

ConstOperandBase* createTurningConsant(TurningDir cosnt_val) {
	return new CConstOperand<TurningDir>(cosnt_val, "TurningDir");
}

void PlayerContextSciptingLibrary::loadContextFunctions(ScriptCompiler* scriptCompiler) {
	FunctionRegisterHelper helper(scriptCompiler);
	REGIST_CONTEXT_FUNCTION0(helper, freeze, void);
	REGIST_CONTEXT_FUNCTION0(helper, fire, void);
	REGIST_CONTEXT_FUNCTION1(helper, setMove, void, MovingDir);
	REGIST_CONTEXT_FUNCTION1(helper, setTurn, void, TurningDir);
	REGIST_CONTEXT_FUNCTION1(helper, setGunTurn, void, TurningDir);
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

	// register global functions
	loadContextFunctions(scriptCompiler);
}