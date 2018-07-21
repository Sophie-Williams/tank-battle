#pragma once
#include "GameControllerInterfaces.h"
#include "TankCommandsBuilder.h"
#include "PlayerContext.h"
#include "TankController.h"

class ScriptedPlayer : public TankController
{	
	class ScriptedPlayerImpl;
protected:
	ScriptedPlayerImpl * _pImpl;
public:
	ScriptedPlayer();
	virtual ~ScriptedPlayer();
	bool setProgramScript(const char* script);
	void setup(TankPlayerContext* player);
	TankOperations giveOperations(TankPlayerContext* player);
};
