#pragma once
#include "vector2.h"

void InitPlayUI(void);
void UpdatePlayUI(void);
void DrawPlayUI(void);

/// <summary>
/// エネルギーゲージ残量の右下座標取得
/// </summary>
Vector2 GetEnergyGaugeEndPos(void);

/// <summary>
/// ボスHPゲージ残量の右下座標取得
/// </summary>
Vector2 GetBossHpGaugeEndPos(void);

/// <summary>
/// コンボの終了を知らせる
/// </summary>
void TriggerEndCombo(void);

void TriggerDisplayEffect(void);
void TriggerDangerDisplayEffect(void);

/// <summary>
/// 危険エフェクトの削除処理
/// </summary>
void CancelDangerDisplayEffect(void);

void TriggerGaugeShakeByDamage(void);

/// <summary>
/// 開始アイコンのフレームイン開始
/// </summary>
/// <param name=""></param>
void TriggerStartIconFrameIn(void);

/// <summary>
/// 開始アイコンのフレームイン終了
/// </summary>
void TriggerStartIconFrameOut(void);

/// <summary>
/// PauseGuideの出現処理
/// </summary>
void TriggerPauseGuideFrameIn(void);

/// <summary>
/// EnergyGuideの出現処理
/// </summary>
void TriggerEnergyGuideFrameIn(void);

/// <summary>
/// ReEquipGuideの出現処理
/// </summary>
void TriggerReEquipGuideFrameIn(void);