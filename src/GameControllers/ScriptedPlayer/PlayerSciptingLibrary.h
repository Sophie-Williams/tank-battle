#pragma once
#include <CompilerSuite.h>
#include <GameControllerInterfaces.h>
#include "TankCommandsBuilder.h"
#include "PlayerContext.h"

namespace ScriptingLib {
	typedef char MovingDir;
	typedef char TurningDir;
	typedef char RotatingDir;
	typedef RawRay Ray;
	typedef RawPoint Point;

	class PlayerContextSciptingLibrary
	{
	private:
		TankOperations _tankOperations;
		TankCommandsBuilder _commandBuilder;
		TankPlayerContext* _temporaryPlayerContex;
	private:
		// operation functions
		void move(MovingDir dir);
		void turn(TurningDir dir);
		void rotateGun(RotatingDir dir);
		void freeze();
		void fire();
		void keepPreviousState();

		MovingDir getMovingDir();
		TurningDir getTurnDir();
		RotatingDir getRotatingGunDir();

		// getter functions of player's information
		Ray gun();
		float health();
		GeometryInfo geometry();
		float movingSpeed();
		float turningSpeed();
		bool isAlly(GameObjectId id);
		bool isEnemy(GameObjectId id);

	private:
		void loadContextFunctions(ffscript::ScriptCompiler* scriptCompiler);
	public:
		PlayerContextSciptingLibrary();
		void setContext(TankPlayerContext* context);
		void loadLibrary(ffscript::ScriptCompiler* scriptCompiler);
		void resetCommand();
		TankOperations getOperations() const;
	};
}