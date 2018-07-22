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
	bool setProgramScript(const wchar_t* scriptStart, const wchar_t* scriptEnd);
	void setup(TankPlayerContext* player);
	TankOperations giveOperations(TankPlayerContext* player);
};
