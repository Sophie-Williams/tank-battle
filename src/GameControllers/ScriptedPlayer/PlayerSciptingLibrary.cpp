#include "PlayerSciptingLibrary.h"
#include "../common/GameUtil.hpp"
#include "TankCommandsBuilder.h"

#include <function/CdeclFunction.hpp>
#include <memory>
#include <typeinfo>       // operator typeid

using namespace ffscript;
using namespace std;

typedef char MovingDir;
typedef char TurningDir;

void addMove(TankOperations& operations, MovingDir dir) {
	TankCommandsBuilder commandBuilder(operations);
	commandBuilder.move(dir);
}

void addTurn(TankOperations& operations, TurningDir dir) {
	TankCommandsBuilder commandBuilder(operations);
	commandBuilder.turn(dir);
}

void addSpinGun(TankOperations& operations, TurningDir dir) {
	TankCommandsBuilder commandBuilder(operations);
	commandBuilder.spinGun(dir);
}

void freeze(TankOperations& operations) {
	TankCommandsBuilder commandBuilder(operations);
	commandBuilder.freeze();
}

void fire(TankOperations& operations) {
	TankCommandsBuilder commandBuilder(operations);
	commandBuilder.fire();
}

template <class ...Args>
class ArgumentFunctionCounter {
public:
	static constexpr int ArgumentSize = sizeof...(Args);
};

#define SIZE_OF_ARGS(...) ArgumentFunctionCounter<__VA_ARGS__>::ArgumentSize
#define REGIST_GLOBAL_FUNCTION1(helper, func, returnType , ...) helper.registFunction(#func, #__VA_ARGS__, new BasicFunctionFactory<SIZE_OF_ARGS(__VA_ARGS__)>(EXP_UNIT_ID_USER_FUNC, FUNCTION_PRIORITY_USER_FUNCTION, #returnType, new CdeclFunction2<returnType, __VA_ARGS__>(func), helper.getSriptCompiler()))


ConstOperandBase* createMovingConsant(MovingDir cosnt_val) {
	return new CConstOperand<MovingDir>(cosnt_val, "MovingDir");
}

ConstOperandBase* createTurningConsant(TurningDir cosnt_val) {
	return new CConstOperand<TurningDir>(cosnt_val, "TurningDir");
}

ConstOperandBase* createTankOperationConsant(TankOperations cosnt_val) {
	return new CConstOperand<TankOperations>(cosnt_val, "TankOperations");
}

void PlayerSciptingLibrary::loadLibrary(ScriptCompiler* scriptCompiler) {
	
	// register types
	TYPE_TANKOPERATIONS = scriptCompiler->registType("TankOperations");
	scriptCompiler->setTypeSize(TYPE_TANKOPERATIONS, 4);

	auto type = scriptCompiler->registType("MovingDir");
	scriptCompiler->setTypeSize(type, 1);

	type = scriptCompiler->registType("TurningDir");
	scriptCompiler->setTypeSize(type, 1);

	// register contants
	auto NO_MOVE = make_shared<CdeclFunction<ConstOperandBase*, MovingDir>>(createMovingConsant);
	NO_MOVE->pushParam((void*)0);
	scriptCompiler->setConstantMap("NO_MOVE", NO_MOVE);

	auto MOVE_FORWARD = make_shared<CdeclFunction<ConstOperandBase*, MovingDir>>(createMovingConsant);
	MOVE_FORWARD->pushParam((void*)1);
	scriptCompiler->setConstantMap("MOVE_FORWARD", MOVE_FORWARD);

	auto NULL_OPERATION = make_shared<CdeclFunction<ConstOperandBase*, TankOperations>>(createTankOperationConsant);
	NULL_OPERATION->pushParam((void*)TANK_NULL_OPERATION);
	scriptCompiler->setConstantMap("NULL_OPERATION", NULL_OPERATION);

	// register global functions
	FunctionRegisterHelper helper(scriptCompiler);
	REGIST_GLOBAL_FUNCTION1(helper, freeze, void, TankOperations&);
	REGIST_GLOBAL_FUNCTION1(helper, addMove, void, TankOperations&, MovingDir);
}