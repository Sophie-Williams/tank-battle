#pragma once
typedef unsigned int TankCommands;
#define TANK_NULL_COMMAND (0)
#define FREEZE_COMMAND 0xFF000000
#define IS_NULL_COMMAND(commands) ((commands & FREEZE_COMMAND) == 0)

class TankCommandsBuilder
{
	TankCommands& _commands;
public:
	TankCommandsBuilder(TankCommands& initCommands);

	void freeze();
	void move(char moveDir);
	void turn(char turnDir);
	void spinGun(char spinDir);

	char getMovingDir() const;
	char getTurnDir() const;
	char getSpinningGunDir() const;
};
