#pragma once
#include "vector2.h"
#include "timer.h"

enum class ModeStartEffect {
	//プレイヤーの生成を待つ処理
	WaitPlayerSpawn,
	//Readyの表示
	Ready,
	//Goの表示
	Go,

	Standby
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