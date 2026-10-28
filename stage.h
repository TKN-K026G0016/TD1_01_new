#pragma once
#include "vector2.h"
#include "timer.h"
#include "tool.h"

enum class ModeCameraZoom {
	Standby,
	ZoomIn,
	ZoomOut,
};

enum class ModeCameraShake {
	Standby,

	S,
	M,
	L,

	Count
};

enum class ModeStartEffect {
	//プレイヤーの生成を待つ処理
	WaitPlayerSpawn,
	//Readyの表示
	Ready,
	//Goの表示
	Go,

	Standby
};

enum class ModeBackground {
	Standby,
	Combo100,
	Combo500,
	Combo1000,
};

void InitStage(void);
void UpdateStage(void);
void DrawStage(void);

/// <summary>
/// 可動域の取得
/// </summary>
/// <param name="pos">可動域を格納する配列</param>
void GetMovablePos(Vector2 pos[2]);

/// <summary>
/// カメラの座標取得
/// </summary>
/// <param name=""></param>
/// <returns></returns>
Vector2 GetCameraPos(void);

/// <summary>
/// カメラの拡大率取得
/// </summary>
/// <param name=""></param>
/// <returns></returns>
float GetCameraZoom(void);

float GetCameraZoomEasingT(void);

EasingMode GetCameraZoomEasingMode(void);

/// <summary>
/// カメラのズーム状態変更
/// </summary>
/// <param name="nextMode">次の状態</param>
void ChangeModeCameraZoom(ModeCameraZoom nextMode);

/// <summary>
/// カメラの振動状態変更
/// </summary>
/// <param name="nextMode">次の状態</param>
void ChangeModeCameraShake(ModeCameraShake nextMode);

/// <summary>
/// 現在の時間を取得
/// </summary>
TimeData GetGameTimeData(void);

/// <summary>
/// ボスラウンド開始時間を取得
/// </summary>
TimeData GetBossRoundTimeData(void);

ModeStartEffect GetNowModeStart(void);

bool GetIsWarningBoss(void);

/// <summary>
/// 背景のフラッシュ処理
/// </summary>
/// <param name="mode">次の状態</param>
void TriggerBackgroundFlash(ModeBackground mode);