#pragma once
#include "GameControllerInterfaces.h"
#include "TankCommandsBuilder.h"
#include "PlayerContext.h"
#include "TankController.h"
#include "../GameControllers.h"

class GAME_CONTROLLER_API ScriptedPlayer : public TankController
{	
	class ScriptedPlayerImpl;
protected:
	ScriptedPlayerImpl * _pImpl;
public:
	ScriptedPlayer();
	virtual ~ScriptedPlayer();
	const char* setProgramScript(const wchar_t* scriptStart, const wchar_t* scriptEnd);
	const char* setProgramScript(const char* file);
	void setup(TankPlayerContext* player);
	void cleanup(TankPlayerContext* player);
	TankOperations giveOperations(TankPlayerContext* player);
};
