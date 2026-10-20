#pragma once

enum class InputAction {
	SelectRight,
	SelectLeft,
	SelectUp,
	SelectDown,

	Confirm,
	Cancel,

	Pause,

	MoveRight,
	MoveLeft,
	MoveUp,
	MoveDown,

	Lock,
	TriggerLock,

	Shoot,
	Charge,

	Count,
};

void UpdateInput(void);
bool CheckInputAction(InputAction action);
bool GetIsConnectGamePad(void);