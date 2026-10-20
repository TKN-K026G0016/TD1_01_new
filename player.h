#pragma once
#include "vector2.h"

enum class PlayerLaserLevel {
	Level0 = 0,
	Level1 = 1,
	Level2 = 2,

	Count
};

enum class PlayerState {
	Normal,
	DeadEnergy,

	Count
};

void InitPlayer(void);
void UpdatePlayer(void);
void DrawPlayer(void);

/// <summary>
/// playerの座標取得
/// </summary>
Vector2 GetPlayerPos(void);

/// <summary>
/// playerの当たり判定の大きさ取得
/// </summary>
/// <returns>player.hitRadius</returns>
float GetPlayerHitRadius(void);

/// <summary>
/// playerの体力取得
/// </summary>
/// <returns>player.remainLife</returns>
int GetPlayerHp(void);

/// <summary>
/// playerの生存フラグ取得
/// </summary>
/// <returns>player.isAlive</returns>
bool GetPlayerIsAlive(void);

/// <summary>
/// 
/// </summary>
/// <param name=""></param>
/// <returns></returns>
float GetPlayerRemainEnergy(void);

/// <summary>
/// エネルギーの最大値取得
/// </summary>
/// <param name=""></param>
/// <returns></returns>
float GetPlayerEnergyLimit(void);

/// <summary>
/// 現在のレーザーの火力倍率取得
/// </summary>
/// <param name=""></param>
/// <returns></returns>
float GetPlayerLaserPowRate(void);

/// <summary>
/// エネルギー回復処理
/// </summary>
/// <param name="recoveryValue">回復量</param>
void RecoveryEnergy(float recoveryValue);

/// <summary>
/// 射撃位置取得
/// </summary>
/// <returns>shootPos</returns>
Vector2 GetPlayerShootPos(void);

/// <summary>
/// playerの方向取得
/// </summary>
/// <returns>player.rotateTheta</returns>
float GetPlayerRotateTheta(void);

/// <summary>
/// playerのレベル取得
/// </summary>
/// <returns>player.laserLevel</returns>
PlayerLaserLevel GetPlayerNowLaserLevel(void);

/// <summary>
/// playerの被弾処理
/// </summary>
/// <param name="hitPos">接触した相手の座標</param>
/// <param name="damage">ダメージ量</param>
void PlayerDamage(Vector2 hitPos, float damage);

/// <summary>
/// プレイヤーをノックバックさせる処理
/// </summary>
/// <param name="hitPos">接触した相手の座標</param>
void PlayerKnockBack(Vector2 hitPos);

bool GetDodgeCloseSwitch(void);

/// <summary>
/// ギリ避け判定の大きさ取得
/// </summary>
/// <returns>player.dodgeCloseRadius</returns>
float GetPlayerDodgeCloseRadius(void);

/// <summary>
/// ギリ避け成功時の処理
/// </summary>
/// <param name="eBulletPos">敵弾の座標</param>
void TriggerDodgeClose(Vector2 eBulletPos);

/// <summary>
/// playerの現在状態を取得
/// </summary>
/// <returns>player.nowState</returns>
PlayerState GetPlayerNowState(void);

/// <summary>
/// 体力ゲージ制のスイッチ確認
/// </summary>
bool GetLifeGaugeSwitch(void);