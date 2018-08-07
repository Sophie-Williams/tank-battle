#pragma once
#include <CompilerSuite.h>
#include <GameControllerInterfaces.h>
#include "TankCommandsBuilder.h"
#include "PlayerContext.h"
#include "TankController.h"
#include <string>
#include <RawStringLib.h>
#include <ffscript.h>

namespace ScriptingLib {
	typedef char MovingDir;
	typedef char TurningDir;
	typedef char RotatingDir;
	typedef RawRay Ray;
	typedef RawPoint Point;
	typedef ffscript::RawString String;

	class PlayerContextSciptingLibrary
	{
	private:
		TankOperations _tankOperations;
		TankCommandsBuilder _commandBuilder;
		TankPlayerContext* _temporaryPlayerContex;
		TankController* _theController;
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

		const SnapshotTimeObjectPoints* getRadarSnapshot() const;
		const SnapshotObjectPoints* getCameraSnapshot() const;
		const SnapshotColissions* getCollisions() const;

	private:
		void registerGeometryTypes(ffscript::ScriptCompiler* scriptCompiler);
		void loadContextFunctions(ffscript::ScriptCompiler* scriptCompiler);
		void loadGlobalFunctions(ffscript::ScriptCompiler* scriptCompiler);
		void printMovingDir(MovingDir dir);
		void printTurningDir(TurningDir dir);
		void printRotatingDir(RotatingDir dir);
		void printCollisionPosition(CollisionPosition dir);
		void println(String& rws);
		void println(std::wstring& s);
		void println(std::string& s);


	public:
		PlayerContextSciptingLibrary();
		void setContext(TankPlayerContext* context);
		void setController(TankController* controler);
		TankController* getController() const;
		void loadLibrary(ffscript::ScriptCompiler* scriptCompiler);
		void resetCommand();
		TankOperations getOperations() const;
	};
}