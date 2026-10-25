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

enum class ModeViberation {
	Standby,

	S,
	M,
	L,

	Count
};

void InitInput(void);
void UpdateInput(void);
bool CheckInputAction(InputAction action);
bool GetIsConnectGamePad(void);

/// <summary>
/// ゲームパッドの振動開始処理
/// </summary>
/// <param name="mode"></param>
void TriggerViberationGamePad(ModeViberation mode);