#pragma once
#include "GameControllerInterfaces.h"

class GAME_CONTROLLER_INTERFACE TankCommandsBuilder
{
	TankOperations& _commands;
public:
	TankCommandsBuilder(TankOperations& initCommands);

	void freeze();
	void fire();
	void move(char moveDir);
	void turn(char turnDir);
	void spinGun(char spinDir);

	char getMovingDir() const;
	char getTurnDir() const;
	char getSpinningGunDir() const;
	bool hasFire() const;
};
