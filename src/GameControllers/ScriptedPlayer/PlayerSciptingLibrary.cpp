#include "PlayerSciptingLibrary.h"
#include "../common/GameUtil.hpp"
#include "TankCommandsBuilder.h"

using namespace ffscript;
using namespace std;

void addMove(TankOperations& operations, char dir) {
	TankCommandsBuilder commandBuilder(operations);
	commandBuilder.move(dir);
}

void addTurn(TankOperations& operations, char dir) {
	TankCommandsBuilder commandBuilder(operations);
	commandBuilder.turn(dir);
}

void addSpinGun(TankOperations& operations, char dir) {
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

void PlayerSciptingLibrary::loadLibrary(ScriptCompiler* scriptCompiler) {
	
	// register types
	TYPE_TANKOPERATIONS = scriptCompiler->registType("TankOperations");
	scriptCompiler->setTypeSize(TYPE_TANKOPERATIONS, 4);

	// register global functions
	FunctionRegisterHelper helper(scriptCompiler);
	REGIST_GLOBAL_FUNCTION1(helper, addMove, void, TankOperations&, char);
}