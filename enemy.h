#pragma once
#include "vector2.h"
#include "texture.h"
#include "timer.h"
#include "texture.h"

//ザコ敵の種類
enum class EnemyType {
	None,

	//移動→射撃
	EnemyShooter,
	//ボス防御
	EnemyGuardian,
	//移動のみ
	EnemyBasic,
	//隊列移動
	EnemyFormation,
	//小雑魚召喚
	EnemyM,
	//小雑魚
	EnemySmall,
	//小ザコの群体
	EnemySmallGroup,
	//小雑魚隊列
	EnemySmallFarmation,
	//ボム
	EnemyBomb,
	//長距離射撃
	EnemySniper,


	Count
};
#pragma region 構造体:EnemyShooter

struct EnemyShooter {
	Vector2 pos = { 0, 0 };
	Vector2 size = { 64, 64 };

	//生存フラグ
	bool isAlive = false;

	int hp = 30;
	int hpMax = 30;

	float hitRadius = 20.0f;

	//<射撃処理関係>
	Timer shootTimer = { 300, 0 };

	//ショックの生成無視状態
	bool isIgnoreShock = false;
	//ショック生成無視タイマー
	Timer ignoreSpawnShockTimer = { 20, 0 };

	//<フラッシュ関係>
	bool isFlash = false;
	Timer flashTimer = { 7, 0 };
	//<ヒットストップ関係>
	bool isHitStop = false;
	Timer hitStopTimer = { 10, 0 };


	//<移動関係>
	// 視界
	float viewRange = 700.0f;
	//速度
	Vector2 velocity = { 0.0f, 0.0f };
	//スピード
	float nowSpeed = 0.0f;
	//加速度
	float accleretionSpeed = 0.05f;
	//スピード上限
	float moveSpeedLimit = 2.0f;
	//減速度
	float decelerationSpeed = 0.12f;
	Vector2 inputVec = { 0, 0 };

	//生成演出
	float spawnEasingT = 0.0f;
	float scale = 1.0f;
	int alpha = 0;

	Texture texture = {};
};
#pragma endregion

#pragma region 構造体:EnemyGuardian

struct EnemyGuardian {
	Vector2 pos = { 0, 0 };
	Vector2 size = { 64, 64 };

	//生存フラグ
	bool isAlive = false;

	int hp = 60;
	int hpMax = 60;

	float hitRadius = 20.0f;

	//<射撃処理関係>
	Timer shootTimer = { 75, 0 };

	//ショックの生成無視状態
	bool isIgnoreShock = false;
	//ショック生成無視タイマー
	Timer ignoreSpawnShockTimer = { 20, 0 };

	//<フラッシュ関係>
	bool isFlash = false;
	Timer flashTimer = { 7, 0 };

	//生成演出
	float spawnEasingT = 0.0f;
	float scale = 1.0f;
	int alpha = 0;

	Texture texture = {};
};
 
#pragma endregion

#pragma region 構造体:EnemyBasic

struct EnemyBasic {
	Vector2 pos = { 0, 0 };
	Vector2 size = { 64, 64 };

	//生存フラグ
	bool isAlive = false;

	int hp = 20;
	int hpMax = 20;

	float hitRadius = 30.0f;

	//攻撃力
	float pow = 25.0f;

	//移動速度
	float moveSpeed = 1.5f;
	float moveTheta = 0;
	bool thetaUpDate = false;

	//ショックの生成無視状態
	bool isIgnoreShock = false;
	//ショック生成無視タイマー
	Timer ignoreSpawnShockTimer = { 20, 0 };

	//<フラッシュ関係>
	bool isFlash = false;
	Timer flashTimer = { 10, 0 };
	//<ヒットストップ関係>
	bool isHitStop = false;
	Timer hitStopTimer = { 10, 0 };

	//生成演出
	float spawnEasingT = 0.0f;
	float scale = 1.0f;
	int alpha = 0;

	Texture texture = {};
};

#pragma endregion

#pragma region 構造体:EnemyM

struct EnemyM {
	Vector2 pos = { 0, 0 };
	Vector2 size = { 64, 64 };

	//生存フラグ
	bool isAlive = false;

	int hp = 60;
	int hpMax = 60;

	float hitRadius = 20.0f;

	//<射撃処理関係>
	Timer shootTimer = { 30, 0 };

	//ショックの生成無視状態
	bool isIgnoreShock = false;
	//ショック生成無視タイマー
	Timer ignoreSpawnShockTimer = { 20, 0 };

	//<フラッシュ関係>
	bool isFlash = false;
	Timer flashTimer = { 7, 0 };

	//生成演出
	float spawnEasingT = 0.0f;
	float scale = 1.0f;
	int alpha = 0;

	Texture texture = {};
};

#pragma endregion

#pragma region 構造体:EnemySmall

struct EnemySmall {
	Vector2 pos = { 0, 0 };
	Vector2 size = { 32, 32 };

	//生存フラグ
	bool isAlive = false;

	int hp = 5;
	int hpMax = 5;

	float hitRadius = 20.0f;

	float pow = 15.0f;

	//<移動関係>
	//速度
	Vector2 velocity = { 0.0f, 0.0f };
	//スピード
	float nowSpeed = 0.0f;
	//加速度
	float accleretionSpeed = 0.05f;
	//スピード上限
	float moveSpeedLimit = 4.0f;
	//減速度
	float decelerationSpeed = 0.12f;
	//方向転換の比率
	float turnRate = 0.5f;
	//移動方向変更フラグ
	bool velocityUpDate = true;


	//ショックの生成無視状態
	bool isIgnoreShock = false;
	//ショック生成無視タイマー
	Timer ignoreSpawnShockTimer = { 20, 0 };
	//<ヒットストップ関係>
	bool isHitStop = false;
	Timer hitStopTimer = { 10, 0 };

	//<フラッシュ関係>
	bool isFlash = false;
	Timer flashTimer = { 7, 0 };

	//生成演出
	float spawnEasingT = 0.0f;
	float scale = 1.0f;
	int alpha = 0;

	Texture texture = {};
};

#pragma endregion

#pragma region 構造体:EnemyBomb

struct EnemyBomb {
	Vector2 pos = { 0, 0 };
	Vector2 size = { 128, 128 };
	//<生存関係>
	bool isAlive = false;

	int hp = 60;
	int hpMax = 60;

	//当たり判定の大きさ
	float hitRadius = 64.0f;

	float pow = 40.0f;
	
	int alphaBurstAreaReady = 255;
	int alphaBurstAreaNormal = 15;

	//<爆発関係>
	//爆発までの時間
	Timer burst = { 0,0 };
	//爆発範囲
	float burstRadius = 384.0f;
	Vector2 bustSize = { 768,768 };
	//<移動関係>
	//速度
	Vector2 velocity = { 0.0f, 0.0f };
	//スピード
	float nowSpeed = 0.0f;
	//加速度
	float accleretionSpeed = 0.05f;
	//スピード上限
	float moveSpeedLimit = 6.0f;
	//減速度
	float decelerationSpeed = 0.1f;

	//ショックの生成無視状態
	bool isIgnoreShock = false;
	//ショック生成無視タイマー
	Timer ignoreSpawnShockTimer = { 20, 0 };

	//<フラッシュ関係>
	bool isFlash = false;
	Timer flashTimer = { 7, 0 };

	Timer createSmokeTimer = { 20, 0 };
	Timer createSmokeReadyBurstTimer = { 7,0 };
	Vector2 createSmokePosOffset = { 0, 50 };

	//生成演出
	float spawnEasingT = 0.0f;
	float scale = 1.0f;
	int alpha = 0;

	Texture texture = {};
	Texture burstTexture = {};
};

#pragma endregion

#pragma region 構造体:EnemySniper

struct EnemySniper {
	Vector2 pos = { 0, 0 };
	Vector2 size = { 64, 64 };

	//生存フラグ
	bool isAlive = false;

	int hp = 30;
	int hpMax = 30;

	float hitRadius = 20.0f;

	//<射撃処理関係>
	Timer shootLazerTimer = { 300, 0 };
	Timer shootingLazerTimer = { 60, 0 };

	//ショックの生成無視状態
	bool isIgnoreShock = false;
	//ショック生成無視タイマー
	Timer ignoreSpawnShockTimer = { 20, 0 };

	//<フラッシュ関係>
	bool isFlash = false;
	Timer flashTimer = { 7, 0 };
	//<ヒットストップ関係>
	bool isHitStop = false;
	Timer hitStopTimer = { 10, 0 };

	//生成演出
	float spawnEasingT = 0.0f;
	float scale = 1.0f;
	int alpha = 0;

	Texture texture = {};
};
#pragma endregion


void InitEnemy(void);
void UpdateEnemy(void);
void DrawEnemy(void);

/// <summary>
/// ザコ敵の生成処理
/// </summary>
/// <param name="pos">生成座標</param>
/// <param name="type"生成する種類></param>
void SpawnEnemy(Vector2 pos, EnemyType type);

EnemyShooter* GetEnemyShooterArray(void);

int GetEnemyShooterLimit(void);

/// <summary>
/// Enemy1へのダメージ処理
/// </summary>
/// <param name="enemy"></param>
/// <param name="pow"></param>
void DamageEnemyShooter(EnemyShooter& enemy, int pow, bool hitStopSwitch);

EnemyGuardian* GetEnemyGuardianArray(void);

int GetEnemyGuardianLimit(void);

/// <summary>
/// Enemy2へのダメージ処理
/// </summary>
/// <param name="enemy"></param>
/// <param name="pow"></param>
void DamageEnemyGuardian(EnemyGuardian& enemy, int pow);


EnemyBasic* GetEnemyBasicArray(void);

int GetEnemyBasicLimit(void);

/// <summary>
/// EnemyBasicへのダメージ処理
/// </summary>
/// <param name="enemy">enemyBasic</param>
/// <param name="pow">ダメージ</param>
void DamageEnemyBasic(EnemyBasic& enemy, int pow, bool hitStopSwitch);

EnemyM* GetEnemyMArray(void);

int GetEnemyMLimit(void);

/// <summary>
/// EnemyMへのダメージ処理
/// </summary>
/// <param name="enemy"></param>
/// <param name="pow"></param>
void DamageEnemyM(EnemyM& enemy, int pow);

EnemySmall* GetEnemySmallArray(void);

int GetEnemySmallLimit(void);

/// <summary>
/// EnemySmollへのダメージ処理
/// </summary>
/// <param name="enemy"></param>
/// <param name="pow"></param>
void DamageEnemySmall(EnemySmall&enemy, int pow);

EnemyBomb* GetEnemyBombArray(void);

int GetEnemyBombLimit(void);

/// <summary>
/// EnemyBombへのダメージ処理
/// </summary>
/// <param name="enemy"></param>
/// <param name="pow"></param>
void DamageEnemyBomb(EnemyBomb& enemy, int pow);

EnemySniper* GetEnemySniperArray(void);

int GetEnemySniperLimit(void);

/// <summary>
/// EnemyBombへのダメージ処理
/// </summary>
/// <param name="enemy"></param>
/// <param name="pow"></param>
void DamageEnemySniper(EnemySniper& enemy, int pow, bool hitStopSwitch);

void DeadAllEnemy(void);