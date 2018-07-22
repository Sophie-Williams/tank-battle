#pragma once
#include <CompilerSuite.h>
#include <GameControllerInterfaces.h>
#include "TankCommandsBuilder.h"

typedef char MovingDir;
typedef char TurningDir;

class PlayerContextSciptingLibrary
{
private:
	TankOperations _tankOperations;
	TankCommandsBuilder _commandBuilder;
private:

	void setMove(MovingDir dir);
	void setTurn(TurningDir dir);
	void setGunTurn(TurningDir dir);
	void freeze();
	void fire();
	void keepPreviousState();

private:
	void loadContextFunctions(ffscript::ScriptCompiler* scriptCompiler);
public:
	PlayerContextSciptingLibrary();
	void loadLibrary(ffscript::ScriptCompiler* scriptCompiler);
	void resetCommand();
	TankOperations getOperations() const;
};
