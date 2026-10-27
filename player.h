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

enum class ModeElecShield {
	//通常時
	Standby,

	//回復状態時
	Recovery,
	//破壊状態時
	Break,
	//復活処理時
	ReEquip,
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
/// デンリョク残量取得
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


/// <summary>
/// playerの現在状態を取得
/// </summary>
/// <returns>player.nowState</returns>
PlayerState GetPlayerNowState(void);

bool GetPlayerIsInvincible(void);

bool GetPlayerIsEquipShield(void);

/// <summary>
/// ElecShieldの状態を変更
/// </summary>
/// <param name="mode">次の状態</param>
void ChangeModeElecShield(ModeElecShield mode);