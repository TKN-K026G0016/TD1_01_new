#pragma once
#include "vector2.h"

//エフェクトの描画タイプ
enum class TypeDrawEffect {
	Obj,
	UI,
};

enum class TypeSmoke {
	EnergyGauge,
	EnemyBomb,
};

void InitEffect();
void UpdateEffect();
void DrawEffect();
void DrawEffectAsUI();

void CreateJemEffect();


/// <summary>
/// ゲージエフェクトの出現処理
/// </summary>
/// <param name="pos">生成座標</param>
void CreateGageEffect(Vector2 pos, unsigned int color);

/// <summary>
/// 煙エフェクトの生成処理
/// </summary>
/// <param name="pos">生成座標</param>
void CreateSmokeEffect(Vector2 pos, TypeDrawEffect drawType, TypeSmoke smokeType);

/// <summary>
/// ElecShockエフェクト生成処理
/// </summary>
/// <param name="pos">生成座標</param>
void CreateElecShockEffect(Vector2 pos);

/// <summary>
/// ElecShock(チャージ)エフェクト生成処理
/// </summary>
/// <param name="pos">生成座標</param
void CreateElecShockChargedEffect(Vector2 pos);

void CreateElecLine(Vector2 startPos, Vector2 endPos, bool charged);

/// <summary>
/// ChargeOrbエフェクト生成処理
/// </summary>
/// <param name="pos">生成座標</param>
void CreateChargeOrbEffect(Vector2 pos);

/// <summary>
/// ChargeChipエフェクト生成処理
/// </summary>
void CreateChargeChipEffect();

/// <summary>
/// 爆発エフェクトの生成処理
/// </summary>
/// <param name="pos">生成座標</param>
/// <param name="color">色</param>
void CreateExplodeEffect(Vector2 pos, unsigned int color);

/// <summary>
///三角形のエフェクト生成処理　
/// </summary>
/// <param name="pos">生成座標</param>
/// <param name="color">表示する色</param>
void CreateTriangleEffect(Vector2 pos, unsigned int color);

/// <summary>
/// 三角形のエフェクト(等速)生成処理
/// </summary>
/// <param name="pos">生成座標</param>
/// <param name="sizeValue">サイズ</param>
/// <param name="moveSpeed">移動速度</param>
/// <param name="color">色</param>
void CreateTriangleEffect2(Vector2 pos, float sizeValue, float moveSpeed, unsigned int color);

/// <summary>
/// 拡大する円のエフェクト生成処理
/// </summary>
/// <param name="pos">生成座標</param>
/// <param name="endSizeValue">最終的なサイズ</param>
/// <param name="easingSpeed">イージング速度(defaultは0.0f)</param>
/// <param name="color">色</param>
void CreateCircleEffect(Vector2 pos, float endSizeValue, float easingSpeed, unsigned int color);

/// <summary>
/// playerのダメージエフェクト生成処理
/// </summary>
/// <param name="pos">生成座標</param>
void CreatePlayerDamageEffect(Vector2 pos);

/// <summary>
/// enemyのダメージエフェクト生成処理
/// </summary>
/// <param name="pos"></param>
void CreateEnemyDamageEffect(Vector2 pos);
//void CreateEnemyDamageEffect(Vector2 pos);

/// <summary>
/// playerの移動エフェクト生成処理
/// </summary>
/// <param name="pos">生成座標</param>
/// <param name="moveVec">対象の移動方向</param>
void CreatePlayerMoveEffect(Vector2 pos, Vector2 moveVec);

/// <summary>
/// 回復エフェクト生成開始処理
/// </summary>
void TriggerCreateRecoveryEffect(void);

/// <summary>
/// ジェムの破壊エフェクト生成処理
/// </summary>
/// <param name="pos">生成座標</param>
void CreateBreakGemEffect(Vector2 pos);

void CreateBombExplosionEffect(Vector2 pos, float endSizeValue, unsigned int color);

void CreateBarrierBreakEffect(Vector2 pos, unsigned int color);

void CreateFlashPanel(void);

unsigned int GetPlayerColor(void);

unsigned int GetEnemyColor(void);