#include "enemy.h"
#include "player.h"
#include "stage.h"
#include "enemy_bullet.h"
#include "elec_bullet.h"
#include "gem.h"
#include "boss_enemy.h"
#include "effect.h"
#include "se.h"

#include "vector2.h"
#include "timer.h"
#include "tool.h"
#include "texture.h"

#define _USE_MATH_DEFINES
#include <math.h>

//カラー関係
static constexpr unsigned int kNormalColor = 0xE01068FF;
static constexpr unsigned int kFlashColor = WHITE;

#pragma region データ: 生成演出

static constexpr float kSpawnEasingSpeed = 0.03f;
static constexpr EasingMode kSpawnEasingMode = EasingMode::EaseOut;

static constexpr float kScaleMin = 1.0f;
static constexpr float kScaleMax = 3.0f;

static constexpr int kAlphaMin = 0;
static constexpr int kAlphaMax = 255;

#pragma endregion

#pragma region データ: EnemyShooter

constexpr int kEnemyShooterLimit = 20;
static EnemyShooter enemyShooter[kEnemyShooterLimit];

struct EnemyShooterData {
	//ザイス
	Vector2 size = { 64, 64 };

	//体力上限
	int hpMax = 45;
	float hitRadius = 20.0f;

	//射撃関係
	int shootTime = 300;

	// 視界
	float viewRange = 700.0f;

	//加速度
	float accleretionSpeed = 0.05f;
	//スピード上限
	float moveSpeedLimit = 2.0f;
	//減速度
	float decelerationSpeed = 0.12f;

	//ジェムの生成量
	int createGemLimit = 4;
	//撃破時のスコア
	int defeatScore = 150;
};
EnemyShooterData enemyShooterData;



#pragma endregion

#pragma region データ: EnemyGuardian
constexpr int kEnemyGuardianLimit = 10;
static EnemyGuardian enemyGuardian[kEnemyGuardianLimit];

struct EnemyGuardianData {
	Vector2 size = { 64, 64 };

	int hpMax = 360;
	float hitRadius = 20.0f;

	int shootTime = 75;

	//ジェムの生成量
	int createGemLimit = 1;
	//撃破時のスコア
	int defeatScore = 150;
};
EnemyGuardianData enemyGuardianData;

#pragma endregion

#pragma region データ: EnemyBasic
constexpr int kEnemyBasicLimit = 50;
static EnemyBasic enemyBasic[kEnemyBasicLimit];

struct EnemyBasicData {
	Vector2 size = { 64, 64 };

	int hpMax = 20;

	float hitRadius = 30.0f;

	//攻撃力
	float pow = 25.0f;

	float moveSpeed = 1.5f;

	//Formationの生成間隔
	int formationRange = 75;

	//ジェムの生成量
	int createGemLimitNormal = 2;
	//ジェムの生成量（隊列）
	int createGemLimitFormation = 1;

	//撃破時のスコア
	int defeatScore = 100;
};
EnemyBasicData enemyBasicData;



#pragma endregion

#pragma region データ: EnemyM
constexpr int kEnemyMLimit = 4;
static EnemyM enemyM[kEnemyMLimit];

struct EnemyMData {
	Vector2 size = { 64, 64 };

	int hpMax = 60;
	float hitRadius = 20.0f;

	int shootTime = 30;

	//ジェムの生成量
	int enemyMGemLimit = 4;
	//撃破時のスコア
	int defeatScore = 150;
};
EnemyMData enemyMData;


#pragma endregion

#pragma region データ: EnemySmall
constexpr int kEnemySmallLimit = 50;
static EnemySmall enemySmall[kEnemySmallLimit];

struct EnemySmallData {
	Vector2 size = { 32, 32 };

	int hpMax = 5;

	float hitRadius = 20.0f;

	float pow = 15.0f;

	//加速度
	float accleretionSpeed = 0.05f;
	//スピード上限
	float moveSpeedLimit = 4.0f;
	//減速度
	float decelerationSpeed = 0.12f;
	//方向転換の比率
	float turnRate = 0.5f;

	//ジェムの生成量
	int createGemLimit = 1;
	//Formationの生成間隔
	int smallFormationRange = 50;
	//撃破時のスコア
	int defeatScore = 50;
};
EnemySmallData enemySmallData;



#pragma endregion

#pragma region データ: EnemySmallGroup

//散らばる範囲(最大)
constexpr int kScatterLengthMax = 100;
//散らばる範囲(最小)
constexpr int kScatterLengthMin = 0;

#pragma endregion

#pragma region データ: EnemyBomb
constexpr int kEnemyBombLimit = 10;
static EnemyBomb enemyBomb[kEnemyBombLimit];

struct EnemyBombData {
	Vector2 size = { 128, 128 };

	int hpMax = 60;
	//当たり判定の大きさ
	float hitRadius = 64.0f;

	float pow = 40.0f;

	int alphaBurstAreaReady = 255;
	int alphaBurstAreaNormal = 15;

	//爆発範囲
	float burstRadius = 384.0f;
	Vector2 bustSize = { 768,768 };

	//加速度
	float accleretionSpeed = 0.05f;
	//スピード上限
	float moveSpeedLimit = 6.0f;
	//減速度
	float decelerationSpeed = 0.1f;

	Vector2 createSmokePosOffset = { 0, 50 };

	//ジェムの生成量
	int createGemLimit = 3;
	//撃破時のスコア
	int defeatScore = 500;
};
EnemyBombData enemyBombData;

#pragma endregion

#pragma region データ: EnemySniper
constexpr int kEnemySniperLimit = 10;
static EnemySniper enemySniper[kEnemySniperLimit];

struct EnemySniperData {
	Vector2 size = { 64, 64 };

	int hpMax = 75;
	float hitRadius = 20.0f;

	//攻撃のスパン
	int shootLaserTime = 300;
	//連続発射のスパン
	int shootingLaserTime = 8;
	//連続発射の回数
	int burstLimit = 7;

	//ジェムの生成量
	int createGemLimit = 4;
	//撃破スコア
	int defeatScore = 300;
};
EnemySniperData enemySniperData;



#pragma endregion



#pragma region 関数: enemyShooter

/// <summary>
/// enemyShooterの初期化関数
/// </summary>
static void InitEnemyShooter(void) {
	for (int i = 0; i < kEnemyShooterLimit; i++) {
		enemyShooter[i] = {};
		enemyShooter[i].shootTimer.time = enemyShooterData.shootTime;
		enemyShooter[i].texture = GetTexture(TextureType::EnemyShooter);
	}
}

/// <summary>
/// enemyShooterの発射処理
/// </summary>
static void ShootEnemyBulletShooter(void) {
	for (int i = 0; i < kEnemyShooterLimit; i++) {
		if (!enemyShooter[i].isAlive) continue;

		enemyShooter[i].shootTimer.count++;
		if (enemyShooter[i].shootTimer.count >= enemyShooter[i].shootTimer.time) {
			enemyShooter[i].shootTimer.count = 0;

			Vector2 playerPos = GetPlayerPos();

			float disX = playerPos.x - enemyShooter[i].pos.x;
			float disY = playerPos.y - enemyShooter[i].pos.y;
			float shootTheta = atan2f(disY, disX);
			ShootEnemyBullet(enemyShooter[i].pos, shootTheta);

		}

	}
}

/// <summary>
/// enemyShooterの移動処理
/// </summary>
static void MoveEnemyShooter(void) {
	Vector2 playerPos = GetPlayerPos();
	for (int i = 0;i < kEnemyShooterLimit;i++) {
		if (!enemyShooter[i].isAlive)continue;
		if (enemyShooter[i].isHitStop) continue;

		float diffX = playerPos.x - enemyShooter[i].pos.x;
		float diffY = playerPos.y - enemyShooter[i].pos.y;
		float distance = sqrtf(diffX * diffX + diffY * diffY);
		if (distance > enemyShooterData.viewRange) {
			enemyShooter[i].velocity = { diffX / distance,diffY / distance };
			enemyShooter[i].nowSpeed += enemyShooterData.accleretionSpeed;
			if (enemyShooter[i].nowSpeed > enemyShooterData.moveSpeedLimit) {
				enemyShooter[i].nowSpeed = enemyShooterData.moveSpeedLimit;
			}
			enemyShooter[i].pos.x += enemyShooter[i].velocity.x * enemyShooter[i].nowSpeed;
			enemyShooter[i].pos.y += enemyShooter[i].velocity.y * enemyShooter[i].nowSpeed;
		}
		else {
			enemyShooter[i].nowSpeed -= enemyShooterData.decelerationSpeed;
			if (enemyShooter[i].nowSpeed < 0) {
				enemyShooter[i].nowSpeed = 0;
			}
			//ShootEnemyBulletShooter();
		}
	}
}

/// <summary>
/// デンゲキショックの生成を受け付けない状態のカウント
/// </summary>
/// <param name=""></param>
static void CountIgnoreShockEnemyShooter(void) {
	for (int i = 0; i < kEnemyShooterLimit; i++) {
		if (!enemyShooter[i].isAlive) continue;
		if (!enemyShooter[i].isIgnoreShock) continue;


		enemyShooter[i].ignoreSpawnShockTimer.count++;
		if (enemyShooter[i].ignoreSpawnShockTimer.count >= enemyShooter[i].ignoreSpawnShockTimer.time) {
			enemyShooter[i].ignoreSpawnShockTimer.count = 0;
			enemyShooter[i].isIgnoreShock = false;
		}
	}
}

/// <summary>
/// フラッシュ演出の処理
/// </summary>
static void UpdateFlashTimerEnemyShooter(void) {
	for (int i = 0; i < kEnemyShooterLimit; i++) {
		if (!enemyShooter[i].isAlive) continue;
		if (!enemyShooter[i].isFlash) continue;

		enemyShooter[i].flashTimer.count++;
		if (enemyShooter[i].flashTimer.count >= enemyShooter[i].flashTimer.time) {
			enemyShooter[i].flashTimer.count = 0;
			enemyShooter[i].isFlash = false;
		}

	}
}

/// <summary>
/// ヒットストップ演出の処理
/// </summary>
static void UpdateHitStopTimerEnemyShooter(void) {
	for (int i = 0; i < kEnemyShooterLimit; i++) {
		if (!enemyShooter[i].isAlive) continue;
		if (!enemyShooter[i].isHitStop) continue;

		enemyShooter[i].hitStopTimer.count++;
		if (enemyShooter[i].hitStopTimer.count >= enemyShooter[i].hitStopTimer.time) {
			enemyShooter[i].hitStopTimer.count = 0;
			enemyShooter[i].isHitStop = false;
		}

	}
}

/// <summary>
/// enemyShooterの生成演出処理
/// </summary>
static void UpdateSpawnEffectEnemyShooter(void) {
	for (int i = 0; i < kEnemyShooterLimit; i++) {
		if (!enemyShooter[i].isAlive) continue;
		if (enemyShooter[i].spawnEasingT >= 1.0f) continue;
	
		enemyShooter[i].spawnEasingT += kSpawnEasingSpeed;
		if (enemyShooter[i].spawnEasingT >= 1.0f) {
			enemyShooter[i].spawnEasingT = 1.0f;
		}

		enemyShooter[i].scale = UpdateValueByEasing(kScaleMax, kScaleMin, enemyShooter[i].spawnEasingT, kSpawnEasingMode);
		enemyShooter[i].alpha = UpdateValueByEasing(kAlphaMin, kAlphaMax, enemyShooter[i].spawnEasingT, kSpawnEasingMode);
	}
}

/// <summary>
/// enemyShooterの消去処理
/// </summary>
static void DeadEnemyShooter(int index) {
	enemyShooter[index].isAlive = false;
	enemyShooter[index].hp = enemyShooterData.hpMax;

	CreateExplodeEffect(enemyShooter[index].pos,WHITE);
}

/// <summary>
/// enemyShooterの死亡判定処理
/// </summary>
static void CheckDeadEnemyShooter(void) {
	for (int i = 0; i < kEnemyShooterLimit; i++) {
		if (!enemyShooter[i].isAlive) continue;

		if (enemyShooter[i].hp <= 0) {
			DeadEnemyShooter(i);

			//ジェムの生成処理
			for (int j = 0; j < enemyShooterData.createGemLimit;j++) {
				SpawnGem(enemyShooter[i].pos, GemType::S);
			}

			AddScoreByDefeatEnemy(enemyShooterData.defeatScore);
		}
	}
}

/// <summary>
/// enemyShooterの描画処理
/// </summary>
static void DrawEnemyShooter(void) {
	for (int i = 0; i < kEnemyShooterLimit; i++) {
		if (!enemyShooter[i].isAlive) continue;

		Vector2 size = { enemyShooterData.size.x * enemyShooter[i].scale, enemyShooterData.size.y * enemyShooter[i].scale };

		Texture texture = enemyShooter[i].texture;
		if (enemyShooter[i].isFlash) {
			texture.color = kFlashColor;
		}
		else {
			texture.color = kNormalColor;
		}
		texture.color = (texture.color & 0xFFFFFF00) | enemyShooter[i].alpha;

		DrawTextureObj(texture, enemyShooter[i].pos, size);
	}
}

#pragma endregion

#pragma region 関数: enemyGuardian

static void InitEnemyGuardian(void) {
	for (int i = 0;i < kEnemyGuardianLimit;i++) {
		enemyGuardian[i] = {};
		enemyGuardian[i].shootTimer.time = enemyGuardianData.shootTime;
		enemyGuardian[i].texture = GetTexture(TextureType::EnemyGuardian);
	}
}

static void ShootEnemyBulletguardian(void) {
	for (int i = 0; i < kEnemyGuardianLimit; i++) {
		if (!enemyGuardian[i].isAlive) continue;

		enemyGuardian[i].shootTimer.count++;
		if (enemyGuardian[i].shootTimer.count >= enemyGuardian[i].shootTimer.time) {
			enemyGuardian[i].shootTimer.count = 0;

			Vector2 playerPos = GetPlayerPos();

			float disX = playerPos.x - enemyGuardian[i].pos.x;
			float disY = playerPos.y - enemyGuardian[i].pos.y;
			float shootTheta = atan2f(disY, disX);
			ShootEnemyBullet(enemyGuardian[i].pos, shootTheta);
		}
	}
}

/// <summary>
/// デンゲキショックの生成を受け付けない状態のカウント
/// </summary>
/// <param name=""></param>
static void CountIgnoreShockEnemyGuardian(void) {
	for (int i = 0; i < kEnemyGuardianLimit; i++) {
		if (!enemyGuardian[i].isAlive) continue;
		if (!enemyGuardian[i].isIgnoreShock) continue;


		enemyGuardian[i].ignoreSpawnShockTimer.count++;
		if (enemyGuardian[i].ignoreSpawnShockTimer.count >= enemyGuardian[i].ignoreSpawnShockTimer.time) {
			enemyGuardian[i].ignoreSpawnShockTimer.count = 0;
			enemyGuardian[i].isIgnoreShock = false;
		}
	}
}

/// <summary>
/// enemyShooterの生成演出処理
/// </summary>
static void UpdateSpawnEffectEnemyGuardian(void) {
	for (int i = 0; i < kEnemyGuardianLimit; i++) {
		if (!enemyGuardian[i].isAlive) continue;
		if (enemyGuardian[i].spawnEasingT >= 1.0f) continue;

		enemyGuardian[i].spawnEasingT += kSpawnEasingSpeed;
		if (enemyGuardian[i].spawnEasingT >= 1.0f) {
			enemyGuardian[i].spawnEasingT = 1.0f;
		}

		enemyGuardian[i].scale = UpdateValueByEasing(kScaleMax, kScaleMin, enemyGuardian[i].spawnEasingT, kSpawnEasingMode);
		enemyGuardian[i].alpha = UpdateValueByEasing(kAlphaMin, kAlphaMax, enemyGuardian[i].spawnEasingT, kSpawnEasingMode);
	}
}

static void UpdateFlashTimerEnemyGuardian(void) {
	for (int i = 0; i < kEnemyGuardianLimit; i++) {
		if (!enemyGuardian[i].isAlive) continue;
		if (!enemyGuardian[i].isFlash) continue;

		enemyGuardian[i].flashTimer.count++;
		if (enemyGuardian[i].flashTimer.count >= enemyGuardian[i].flashTimer.time) {
			enemyGuardian[i].flashTimer.count = 0;
			enemyGuardian[i].isFlash = false;
		}
	}
}

static void DeadEnemyGuardian(int index) {
	enemyGuardian[index].isAlive = false;
	enemyGuardian[index].hp = enemyGuardianData.hpMax;

	CreateExplodeEffect(enemyGuardian[index].pos, WHITE);

	AddScoreByDefeatEnemy(enemyGuardianData.defeatScore);
}

static void CheckDeadEnemyGuardian(void) {
	for (int i = 0; i < kEnemyGuardianLimit; i++) {
		if (!enemyGuardian[i].isAlive) continue;

		if (enemyGuardian[i].hp <= 0) {
			DeadEnemyGuardian(i);

			//ジェムの生成処理
			for (int j = 0; j < enemyGuardianData.createGemLimit; j++) {
				SpawnGem(enemyGuardian[i].pos, GemType::S);
			}
		}
	}
}

static void DrawEnemyGuardian(void) {
	for (int i = 0; i < kEnemyGuardianLimit; i++) {
		if (!enemyGuardian[i].isAlive) continue;

		Vector2 size = { enemyGuardianData.size.x * enemyGuardian[i].scale, enemyGuardianData.size.y * enemyGuardian[i].scale };

		Texture texture = enemyGuardian[i].texture;
		if (enemyGuardian[i].isFlash) {
			texture.color = kFlashColor;
		}
		else {
			texture.color = kNormalColor;
		}
		texture.color = (texture.color & 0xFFFFFF00) | enemyGuardian[i].alpha;

		DrawTextureObj(texture, enemyGuardian[i].pos, size);
	}
}

static void Attack2EnemyGuardianMove(void) {
	Attack2Enemy2Pos attack2Pos;
	int bossEnemyNouStates = GetBossEnemyNowStates();
	Timer attack2Timer = GetAttack2Timer();
	Vector2 bossEnemyPos = GetBossEnemyPos();
	float bossEnemyRotateTheta = GetBossEnemyRotateTheta();
	int enemyCount = 0;
	if (bossEnemyNouStates == Attack2) {

		if (attack2Timer.count == 0) {
			for (int i = 0;i < kEnemyGuardianLimit;i++) {
				if (enemyGuardian[i].isAlive)continue;
				enemyGuardian[i].isAlive = true;

				enemyGuardian[i].spawnEasingT = 0.0f;
				enemyGuardian[i].scale = kScaleMax;
				enemyGuardian[i].alpha = kAlphaMin;

				enemyCount++;
				if (enemyCount == 6)break;
			}
		}
		for (int i = 0;i < kEnemyGuardianLimit;i++) {
			if (!enemyGuardian[i].isAlive)continue;
			enemyGuardian[i].pos.x = cosf(bossEnemyRotateTheta + attack2Pos.theta[enemyCount]) * attack2Pos.dstance[enemyCount] + bossEnemyPos.x;
			enemyGuardian[i].pos.y = sinf(bossEnemyRotateTheta + attack2Pos.theta[enemyCount]) * attack2Pos.dstance[enemyCount] + bossEnemyPos.y;
			enemyCount++;
			if (enemyCount == 6)break;
		}
	}
	else if (bossEnemyNouStates != Attack2) {
		for (int i = 0;i < kEnemyGuardianLimit;i++) {
			enemyGuardian[i].isAlive = false;
			enemyGuardian[i].hp = enemyGuardianData.hpMax;
		}
	}
}

#pragma endregion

#pragma region 関数: enemyBasic

static void InitEnemyBasic(void) {
	for (int i = 0; i < kEnemyBasicLimit; i++) {
		enemyBasic[i] = {};

		enemyBasic[i].texture = GetTexture(TextureType::EnemyBasic);
	}
}

/// <summary>
/// enemyBasicの移動処理
/// </summary>
/// <param name=""></param>
static void MoveEnemyBasic(void) {
	for (int i = 0; i < kEnemyBasicLimit; i++) {
		if (!enemyBasic[i].isAlive) continue;
		if (enemyBasic[i].isHitStop) continue;

		if (enemyBasic[i].thetaUpDate) {

			Vector2 targetPos = GetPlayerPos();
			float disX = targetPos.x - enemyBasic[i].pos.x;
			float disY = targetPos.y - enemyBasic[i].pos.y;/*
			float disLength = sqrtf(disX * disX + disY * disY);
			if (disLength != 0.0f) {
				disX /= disLength;
				disY /= disLength;
			}
			Vector2 moveVec = { disX, disY };

			enemyBasic[i].pos.x += moveVec.x * enemyBasic[i].moveSpeed;
			enemyBasic[i].pos.y += moveVec.y * enemyBasic[i].moveSpeed;*/
			enemyBasic[i].moveTheta = atan2f(disY, disX);
		}
		enemyBasic[i].pos.x += cosf(enemyBasic[i].moveTheta) * enemyBasicData.moveSpeed;
		enemyBasic[i].pos.y += sinf(enemyBasic[i].moveTheta) * enemyBasicData.moveSpeed;
		if ((enemyBasic[i].pos.x < -640 || enemyBasic[i].pos.x>3200 || enemyBasic[i].pos.y < -360 || enemyBasic[i].pos.y>1800)&&!enemyBasic[i].thetaUpDate) {
			Vector2 targetPos = GetPlayerPos();
			float disX = targetPos.x - enemyBasic[i].pos.x;
			float disY = targetPos.y - enemyBasic[i].pos.y;
			enemyBasic[i].moveTheta = atan2f(disY, disX);
		}
	}
}

/// <summary>
/// デンゲキショックの生成を受け付けない状態のカウント
/// </summary>
/// <param name=""></param>
static void CountIgnoreShockEnemyBasic(void) {
	for (int i = 0; i < kEnemyBasicLimit; i++) {
		if (!enemyBasic[i].isAlive) continue;
		if (!enemyBasic[i].isIgnoreShock) continue;


		enemyBasic[i].ignoreSpawnShockTimer.count++;
		if (enemyBasic[i].ignoreSpawnShockTimer.count >= enemyBasic[i].ignoreSpawnShockTimer.time) {
			enemyBasic[i].ignoreSpawnShockTimer.count = 0;
			enemyBasic[i].isIgnoreShock = false;
		}
	}
}

/// <summary>
/// フラッシュの経過時間計測処理
/// </summary>
/// <param name=""></param>
static void UpdateFlashTimerEnemyBasic(void) {
	for (int i = 0; i < kEnemyBasicLimit; i++) {
		if (!enemyBasic[i].isAlive) continue;
		if (!enemyBasic[i].isFlash) continue;

		enemyBasic[i].flashTimer.count++;
		if (enemyBasic[i].flashTimer.count >= enemyBasic[i].flashTimer.time) {
			enemyBasic[i].flashTimer.count = 0;
			enemyBasic[i].isFlash = false;
		}
	}
}

static void UpdateHitStopTimerEnemyBasic(void) {
	for (int i = 0; i < kEnemyBasicLimit; i++) {
		if (!enemyBasic[i].isAlive) continue;
		if (!enemyBasic[i].isHitStop) continue;

		enemyBasic[i].hitStopTimer.count++;
		if (enemyBasic[i].hitStopTimer.count >= enemyBasic[i].hitStopTimer.time) {
			enemyBasic[i].hitStopTimer.count = 0;
			enemyBasic[i].isHitStop = false;
		}
	}
}

/// <summary>
/// enemyBasicの生成演出処理
/// </summary>
static void UpdateSpawnEffectEnemyBasic(void) {
	for (int i = 0; i < kEnemyBasicLimit; i++) {
		if (!enemyBasic[i].isAlive) continue;
		if (enemyBasic[i].spawnEasingT >= 1.0f) continue;

		enemyBasic[i].spawnEasingT += kSpawnEasingSpeed;
		if (enemyBasic[i].spawnEasingT >= 1.0f) {
			enemyBasic[i].spawnEasingT = 1.0f;
		}

		enemyBasic[i].scale = UpdateValueByEasing(kScaleMax, kScaleMin, enemyBasic[i].spawnEasingT, kSpawnEasingMode);
		enemyBasic[i].alpha = UpdateValueByEasing(kAlphaMin, kAlphaMax, enemyBasic[i].spawnEasingT, kSpawnEasingMode);
	}
}

static void DeadEnemyBasic(int index) {
	enemyBasic[index].isAlive = false;
	enemyBasic[index].hp = enemyBasicData.hpMax;

	CreateExplodeEffect(enemyBasic[index].pos, WHITE);

	AddScoreByDefeatEnemy(enemyBasicData.defeatScore);
}

static void CheckDeadEnemyBasic(void) {
	for (int i = 0; i < kEnemyBasicLimit; i++) {
		if (!enemyBasic[i].isAlive) continue;

		if (enemyBasic[i].hp <= 0) {
			DeadEnemyBasic(i);

			//ジェムの生成処理
			if (enemyBasic[i].thetaUpDate) {
				for (int j = 0;j < enemyBasicData.createGemLimitNormal;j++) {
					SpawnGem(enemyBasic[i].pos, GemType::S);
				}
			}
			else {
				for (int j = 0;j < enemyBasicData.createGemLimitFormation;j++) {
					SpawnGem(enemyBasic[i].pos, GemType::S);
				}

			}
		}
	}
}

static void DrawEnemyBasic(void) {
	for (int i = 0; i < kEnemyBasicLimit; i++) {
		if (!enemyBasic[i].isAlive) continue;

		Vector2 size = { enemyBasicData.size.x * enemyBasic[i].scale, enemyBasicData.size.y * enemyBasic[i].scale };

		Texture texture = enemyBasic[i].texture;
		if (enemyBasic[i].isFlash) {
			texture.color = WHITE;
		}
		else {
			texture.color = kNormalColor;
		}

		texture.color = (texture.color & 0xFFFFFF00) | enemyBasic[i].alpha;

		DrawTextureObj(texture, enemyBasic[i].pos, size);
	}
}


#pragma endregion

#pragma region 関数: enemyM

static void InitEnemyM(void) {
	for (int i = 0;i < kEnemyMLimit;i++) {
		enemyM[i] = {};
		enemyM[i].shootTimer.time = enemyMData.shootTime;
		enemyM[i].texture = GetTexture(TextureType::EnemyGuardian);
	}
}

static void ShootEnemySmall(void) {
	for (int i = 0; i < kEnemyMLimit; i++) {
		if (!enemyM[i].isAlive) continue;

		enemyM[i].shootTimer.count++;
		if (enemyM[i].shootTimer.count >= enemyM[i].shootTimer.time) {
			enemyM[i].shootTimer.count = 0;
			for (int j = 0;j < kEnemySmallLimit;j++) {
				if (enemySmall[j].isAlive)continue;

				enemySmall[j].isAlive = true;
				enemySmall[j].velocity = { ToFloat(GetRand(0,20)),ToFloat(GetRand(0,20)) };
				enemySmall[j].pos = enemyM[i].pos;
				break;
			}
		}
	}
}

/// <summary>
/// デンゲキショックの生成を受け付けない状態のカウント
/// </summary>
/// <param name=""></param>
static void CountIgnoreShockEnemyM(void) {
	for (int i = 0; i < kEnemyMLimit; i++) {
		if (!enemyM[i].isAlive) continue;
		if (!enemyM[i].isIgnoreShock) continue;


		enemyM[i].ignoreSpawnShockTimer.count++;
		if (enemyM[i].ignoreSpawnShockTimer.count >= enemyM[i].ignoreSpawnShockTimer.time) {
			enemyM[i].ignoreSpawnShockTimer.count = 0;
			enemyM[i].isIgnoreShock = false;
		}
	}
}

static void UpdateFlashTimerEnemyM(void) {
	for (int i = 0; i < kEnemyMLimit; i++) {
		if (!enemyM[i].isAlive) continue;
		if (!enemyM[i].isFlash) continue;

		enemyM[i].flashTimer.count++;
		if (enemyM[i].flashTimer.count >= enemyM[i].flashTimer.time) {
			enemyM[i].flashTimer.count = 0;
			enemyM[i].isFlash = false;
		}
	}
}

/// <summary>
/// enemyShooterの生成演出処理
/// </summary>
static void UpdateSpawnEffectEnemyM(void) {
	for (int i = 0; i < kEnemyMLimit; i++) {
		if (!enemyM[i].isAlive) continue;
		if (enemyM[i].spawnEasingT >= 1.0f) continue;

		enemyM[i].spawnEasingT += kSpawnEasingSpeed;
		if (enemyM[i].spawnEasingT >= 1.0f) {
			enemyM[i].spawnEasingT = 1.0f;
		}

		enemyM[i].scale = UpdateValueByEasing(kScaleMax, kScaleMin, enemyM[i].spawnEasingT, kSpawnEasingMode);
		enemyM[i].alpha = UpdateValueByEasing(kAlphaMin, kAlphaMax, enemyM[i].spawnEasingT, kSpawnEasingMode);
	}
}

static void DeadEnemyM(int index) {
	enemyM[index].isAlive = false;
	enemyM[index].hp = enemyMData.hpMax;

	CreateExplodeEffect(enemyM[index].pos, WHITE);

	AddScoreByDefeatEnemy(enemyMData.defeatScore);
}

static void CheckDeadEnemyM(void) {
	for (int i = 0; i < kEnemyMLimit; i++) {
		if (!enemyM[i].isAlive) continue;

		if (enemyM[i].hp <= 0) {
			DeadEnemyM(i);

			//ジェムの生成処理
			SpawnGem(enemyM[i].pos, GemType::S);
		}
	}
}

static void DrawEnemyM(void) {
	for (int i = 0; i < kEnemyMLimit; i++) {
		if (!enemyM[i].isAlive) continue;

		Vector2 size = { enemyMData.size.x * enemyM[i].scale, enemyMData.size.y * enemyM[i].scale };

		Texture texture = enemyM[i].texture;
		if (enemyM[i].isFlash) {
			texture.color = kFlashColor;
		}
		else {
			texture.color = kNormalColor;
		}

		texture.color = (texture.color & 0xFFFFFF00) | enemyM[i].alpha;

		DrawTextureObj(texture, enemyM[i].pos, size);
	}
}

#pragma endregion

#pragma region 関数: enemySmall

static void InitEnemySmall(void) {
	for (int i = 0; i < kEnemySmallLimit; i++) {
		enemySmall[i] = {};

		enemySmall[i].texture = GetTexture(TextureType::EnemySmall);
	}
}

/// <summary>
/// enemySmollの移動処理
/// </summary>
/// <param name=""></param>
static void MoveEnemySmall(void) {
	for (int i = 0; i < kEnemySmallLimit; i++) {
		if (!enemySmall[i].isAlive) continue;
		if (enemySmall[i].isHitStop) continue;

		//移動方向更新処理
		if (enemySmall[i].velocityUpDate) {
			Vector2 targetPos = GetPlayerPos();
			Vector2 moveVec = { targetPos.x - enemySmall[i].pos.x,   targetPos.y - enemySmall[i].pos.y };

			float disLength = sqrtf(moveVec.x * moveVec.x + moveVec.y * moveVec.y);
			if (disLength != 0.0f) {
				moveVec.x /= disLength;
				moveVec.y /= disLength;
			}

			//移動ベクトル(ゆっくり曲がる)
			enemySmall[i].velocity.x = (1 - enemySmallData.turnRate) * enemySmall[i].velocity.x + moveVec.x * enemySmallData.turnRate;
			enemySmall[i].velocity.y = (1 - enemySmallData.turnRate) * enemySmall[i].velocity.y + moveVec.y * enemySmallData.turnRate;
		}

		//加速処理
		enemySmall[i].nowSpeed += enemySmallData.accleretionSpeed;
		if (enemySmall[i].nowSpeed > enemySmallData.moveSpeedLimit) {
			enemySmall[i].nowSpeed = enemySmallData.moveSpeedLimit;
		}

		//座標に反映
		enemySmall[i].pos.x += enemySmall[i].velocity.x * enemySmall[i].nowSpeed;
		enemySmall[i].pos.y += enemySmall[i].velocity.y * enemySmall[i].nowSpeed;
	}
}

/// <summary>
/// デンゲキショックの生成を受け付けない状態のカウント
/// </summary>
/// <param name=""></param>
static void CountIgnoreShockEnemySmall(void) {
	for (int i = 0; i < kEnemySmallLimit; i++) {
		if (!enemySmall[i].isAlive) continue;
		if (!enemySmall[i].isIgnoreShock) continue;


		enemySmall[i].ignoreSpawnShockTimer.count++;
		if (enemySmall[i].ignoreSpawnShockTimer.count >= enemySmall[i].ignoreSpawnShockTimer.time) {
			enemySmall[i].ignoreSpawnShockTimer.count = 0;
			enemySmall[i].isIgnoreShock = false;
		}
	}
}

/// <summary>
/// 
/// </summary>
/// <param name=""></param>
static void UpdateFlashTimerEnemySmall(void) {
	for (int i = 0; i < kEnemySmallLimit; i++) {
		if (!enemySmall[i].isAlive) continue;
		if (!enemySmall[i].isFlash) continue;

		enemySmall[i].flashTimer.count++;
		if (enemySmall[i].flashTimer.count >= enemySmall[i].flashTimer.time) {
			enemySmall[i].flashTimer.count = 0;
			enemySmall[i].isFlash = false;
		}
	}
}

/// <summary>
/// enemySmallの生成演出処理
/// </summary>
static void UpdateSpawnEffectEnemySmall(void) {
	for (int i = 0; i < kEnemySmallLimit; i++) {
		if (!enemySmall[i].isAlive) continue;
		if (enemySmall[i].spawnEasingT >= 1.0f) continue;

		enemySmall[i].spawnEasingT += kSpawnEasingSpeed;
		if (enemySmall[i].spawnEasingT >= 1.0f) {
			enemySmall[i].spawnEasingT = 1.0f;
		}

		enemySmall[i].scale = UpdateValueByEasing(kScaleMax, kScaleMin, enemySmall[i].spawnEasingT, kSpawnEasingMode);
		enemySmall[i].alpha = UpdateValueByEasing(kAlphaMin, kAlphaMax, enemySmall[i].spawnEasingT, kSpawnEasingMode);
	}
}

static void DeadEnemySmall(int index) {
	enemySmall[index].isAlive = false;
	enemySmall[index].hp = enemySmallData.hpMax;

	CreateExplodeEffect(enemySmall[index].pos, WHITE);

	AddScoreByDefeatEnemy(enemySmallData.defeatScore);
	
}

static void CheckDeadEnemySmall(void) {
	for (int i = 0; i < kEnemySmallLimit; i++) {
		if (!enemySmall[i].isAlive) continue;

		if (enemySmall[i].hp <= 0) {
			DeadEnemySmall(i);

			//ジェムの生成処理
			for (int j = 0;j < enemySmallData.createGemLimit;j++) {
				SpawnGem(enemySmall[i].pos, GemType::S);
			}
		}
	}
}

static void DrawEnemySmall(void) {
	for (int i = 0; i < kEnemySmallLimit; i++) {
		if (!enemySmall[i].isAlive) continue;

		Vector2 size = { enemySmallData.size.x * enemySmall[i].scale, enemySmallData.size.y * enemySmall[i].scale };

		Texture texture = enemySmall[i].texture;
		if (enemySmall[i].isFlash) {
			texture.color = kFlashColor;
		}
		else {
			texture.color = kNormalColor;
		}

		texture.color = (texture.color & 0xFFFFFF00) | enemySmall[i].alpha;

		DrawTextureObj(texture, enemySmall[i].pos, size);
	}
}


#pragma region 関数: 生成処理

/// <summary>
/// 生成時の出現座標作成
/// </summary>
/// <param name="centerPos">中心座標</param>
/// <returns>出現座標</returns>
static Vector2 CreateScatterPos(Vector2 centerPos) {
	float scatterLength = ToFloat(GetRand(kScatterLengthMin, kScatterLengthMax));

	float theta = ToFloat(GetRand(0, 360)) * (ToFloat(M_PI) / 180.0f);

	Vector2 pos;
	pos.x = centerPos.x + scatterLength * cosf(theta);
	pos.y = centerPos.y + scatterLength * sinf(theta);

	return pos;
}

#pragma endregion

#pragma endregion

#pragma region 関数: enemyBomb

static void InitEnemyBomb(void) {
	for (int i = 0;i < kEnemyBombLimit;i++) {
		enemyBomb[i] = {};

		enemyBomb[i].texture = GetTexture(TextureType::BossBomb);
		enemyBomb[i].burstTexture = GetTexture(TextureType::BossBomBurstRange);
	}
}


static void MoveEnemyBomb(void) {
	for (int i = 0;i < kEnemyBombLimit;i++) {

		if (enemyBomb[i].isAlive == true) {
			enemyBomb[i].nowSpeed -= enemyBombData.decelerationSpeed;
			if (enemyBomb[i].nowSpeed <= 0) {
				enemyBomb[i].nowSpeed = 0;
			}
			enemyBomb[i].pos.x += enemyBomb[i].velocity.x * enemyBomb[i].nowSpeed;
			enemyBomb[i].pos.y += enemyBomb[i].velocity.y * enemyBomb[i].nowSpeed;
			if (enemyBomb[i].pos.x <= 0) {
				enemyBomb[i].pos.x = 0;
				enemyBomb[i].velocity.x *= -1;
			}
			else if (enemyBomb[i].pos.x >= 2560) {
				enemyBomb[i].pos.x = 2560;
				enemyBomb[i].velocity.x *= -1;
			}
			if (enemyBomb[i].pos.y <= 0) {
				enemyBomb[i].pos.y = 0;
				enemyBomb[i].velocity.y *= -1;
			}
			else if (enemyBomb[i].pos.y >= 1440) {
				enemyBomb[i].pos.y = 1440;
				enemyBomb[i].velocity.y *= -1;
			}

			int remainTime =
				enemyBomb[i].burst.time -
				enemyBomb[i].burst.count;

			if (remainTime > 90) {
				enemyBomb[i].createSmokeTimer.count++;
				if (enemyBomb[i].createSmokeTimer.count >= enemyBomb[i].createSmokeTimer.time) {
					enemyBomb[i].createSmokeTimer.count = 0;
					Vector2 createPos = { enemyBomb[i].pos.x + enemyBombData.createSmokePosOffset.x, enemyBomb[i].pos.y + enemyBombData.createSmokePosOffset.y };
					CreateSmokeEffect(createPos, TypeDrawEffect::Obj, TypeSmoke::EnemyBomb);
				}
			} else {
				enemyBomb[i].createSmokeReadyBurstTimer.count++;
				if (enemyBomb[i].createSmokeReadyBurstTimer.count >= enemyBomb[i].createSmokeReadyBurstTimer.time) {
					enemyBomb[i].createSmokeReadyBurstTimer.count = 0;
					Vector2 createPos = { enemyBomb[i].pos.x + enemyBombData.createSmokePosOffset.x, enemyBomb[i].pos.y + enemyBombData.createSmokePosOffset.y };
					CreateSmokeEffect(createPos, TypeDrawEffect::Obj, TypeSmoke::EnemyBomb);
				}
			}

		}
	}
}

static void DeadEnemyBomb(int index) {
	enemyBomb[index].isAlive = false;
	enemyBomb[index].hp = enemyBombData.hpMax;

	CreateExplodeEffect(enemyBomb[index].pos, WHITE);
}

static void BurstEnemyBomb(void) {
	Vector2 playerPos = GetPlayerPos();
	int gemLimit = GetGemLimit();
	Gem* gem = GetGemArray();
	for (int i = 0;i < kEnemyBombLimit;i++) {
		enemyBomb[i].burst.count++;

		if (enemyBomb[i].isAlive) {
			if (enemyBomb[i].burst.count == enemyBomb[i].burst.time) {
				float PlayerDiffX = enemyBomb[i].pos.x - playerPos.x;
				float PlayerDiffY = enemyBomb[i].pos.y - playerPos.y;
				float playerDistance = sqrtf(PlayerDiffX * PlayerDiffX + PlayerDiffY * PlayerDiffY);
				if (playerDistance <= enemyBombData.burstRadius) {
					PlayerDamage(enemyBomb[i].pos, enemyBombData.pow);
				}
				CreateBombExplosionEffect(enemyBomb[i].pos, 768, WHITE);
				for (int j = 0;j < gemLimit;j++) {
					if (!gem[j].isAlive)continue;

					float GemDiffX = enemyBomb[i].pos.x - gem[j].pos.x;
					float GemDiffY = enemyBomb[i].pos.y - gem[j].pos.y;
					float gemDistance = sqrtf(GemDiffX * GemDiffX + GemDiffY * GemDiffY);
					if (gemDistance <= enemyBombData.burstRadius) {
						BreakGem(j);
					}
				}
				DeadEnemyBomb(i);
				PlaySe(SeType::ExploseEnemyBomb);
				ChangeModeCameraShake(ModeCameraShake::L);
			}
		}
	}
}


/// <summary>
/// デンゲキショックの生成を受け付けない状態のカウント
/// </summary>
/// <param name=""></param>
static void CountIgnoreShockEnemyBomb(void) {
	for (int i = 0; i < kEnemyBombLimit; i++) {
		if (!enemyBomb[i].isAlive) continue;
		if (!enemyBomb[i].isIgnoreShock) continue;


		enemyBomb[i].ignoreSpawnShockTimer.count++;
		if (enemyBomb[i].ignoreSpawnShockTimer.count >= enemyBomb[i].ignoreSpawnShockTimer.time) {
			enemyBomb[i].ignoreSpawnShockTimer.count = 0;
			enemyBomb[i].isIgnoreShock = false;
		}
	}
}

static void UpdateFlashTimerEnemyBomb(void) {
	for (int i = 0; i < kEnemyBombLimit; i++) {
		if (!enemyBomb[i].isAlive) continue;
		if (!enemyBomb[i].isFlash) continue;

		enemyBomb[i].flashTimer.count++;
		if (enemyBomb[i].flashTimer.count >= enemyBomb[i].flashTimer.time) {
			enemyBomb[i].flashTimer.count = 0;
			enemyBomb[i].isFlash = false;
		}
	}
}

/// <summary>
/// enemyBombの生成演出処理
/// </summary>
static void UpdateSpawnEffectEnemyBomb(void) {
	for (int i = 0; i < kEnemyBombLimit; i++) {
		if (!enemyBomb[i].isAlive) continue;
		if (enemyBomb[i].spawnEasingT >= 1.0f) continue;

		enemyBomb[i].spawnEasingT += kSpawnEasingSpeed;
		if (enemyBomb[i].spawnEasingT >= 1.0f) {
			enemyBomb[i].spawnEasingT = 1.0f;
		}

		enemyBomb[i].scale = UpdateValueByEasing(kScaleMax, kScaleMin, enemyBomb[i].spawnEasingT, kSpawnEasingMode);
		enemyBomb[i].alpha = UpdateValueByEasing(kAlphaMin, kAlphaMax, enemyBomb[i].spawnEasingT, kSpawnEasingMode);
	}
}

static void CheckDeadEnemyBomb(void) {
	for (int i = 0; i < kEnemyBombLimit; i++) {
		if (!enemyBomb[i].isAlive) continue;

		if (enemyBomb[i].hp <= 0) {
			DeadEnemyBomb(i);

			//ジェムの生成処理
			for (int j = 0; j < enemyBombData.createGemLimit; j++) {
				SpawnGem(enemyBomb[i].pos, GemType::S);
			}

			AddScoreByDefeatEnemy(enemyBombData.defeatScore);

		}
	}
}

static void DrawEnemyBomb(void) {
	for (int i = 0; i < kEnemyBombLimit; i++) {
		if (!enemyBomb[i].isAlive) continue;

		Vector2 size = { enemyBombData.size.x * enemyBomb[i].scale, enemyBombData.size.y * enemyBomb[i].scale };

		Texture texture = enemyBomb[i].texture;
		if (enemyBomb[i].isFlash) {
			texture.color = kFlashColor;
		}
		else {
			texture.color = kNormalColor;
		}
		texture.color = (texture.color & 0xFFFFFF00) | enemyBomb[i].alpha;

		//爆発範囲の描画
		if (enemyBomb[i].burst.count <= enemyBomb[i].burst.time - 90) {
			Texture burstTexture = enemyBomb[i].burstTexture;
			burstTexture.color = (burstTexture.color & 0xFFFFFF00) | enemyBombData.alphaBurstAreaNormal;

			DrawTextureObj(burstTexture, enemyBomb[i].pos, enemyBombData.bustSize);
		}
		else {
			for (int j = 0;j < 45;j++) {
				if (enemyBomb[i].burst.time - enemyBomb[i].burst.count == j * 2) {
					Texture burstTexture = enemyBomb[i].burstTexture;
					burstTexture.color = (burstTexture.color & 0xFFFFFF00) | enemyBombData.alphaBurstAreaReady;
					DrawTextureObj(enemyBomb[i].burstTexture, enemyBomb[i].pos, enemyBombData.bustSize);
				}
			}
		}
		DrawTextureObj(texture, enemyBomb[i].pos, size);
	}
}


#pragma endregion

#pragma region 関数: enemySniper

static void InitEnemySniper(void) {
	for (int i = 0; i < kEnemySniperLimit; i++) {
		enemySniper[i] = {};
		enemySniper[i].shootTimer.time = enemySniperData.shootLaserTime;
		enemySniper[i].burstShootTimer.time = enemySniperData.shootingLaserTime;
		enemySniper[i].texture = GetTexture(TextureType::EnemySniper);
	}
}

static void ShootEnemyBulletSniper(void) {
	for (int i = 0; i < kEnemySniperLimit; i++) {
		if (!enemySniper[i].isAlive) continue;
			Vector2 playerPos = GetPlayerPos();
			float disX = playerPos.x - enemySniper[i].pos.x;
			float disY = playerPos.y - enemySniper[i].pos.y;
			enemySniper[i].theta = atan2f(disY, disX);
		//攻撃のスパン
		enemySniper[i].shootTimer.count++;
		if (enemySniper[i].shootTimer.count >= enemySniper[i].shootTimer.time) {

			//連続射撃のスパン
			enemySniper[i].burstShootTimer.count++;
			if (enemySniper[i].burstShootTimer.count >= enemySniper[i].burstShootTimer.time) {

			
				//射撃処理
				ShootEnemyBullet(enemySniper[i].pos, enemySniper[i].theta);

				enemySniper[i].burstShootTimer.count = 0;
				enemySniper[i].burstCount++;
				
			}

			//一定回数射撃したら
			if (enemySniper[i].burstCount >= enemySniperData.burstLimit) {
				enemySniper[i].shootTimer.count = 0;
				enemySniper[i].burstCount = 0;
			}

		}

	}
}

/// <summary>
/// デンゲキショックの生成を受け付けない状態のカウント
/// </summary>
/// <param name=""></param>
static void CountIgnoreShockEnemySniper(void) {
	for (int i = 0; i < kEnemySniperLimit; i++) {
		if (!enemySniper[i].isAlive) continue;
		if (!enemySniper[i].isIgnoreShock) continue;


		enemySniper[i].ignoreSpawnShockTimer.count++;
		if (enemySniper[i].ignoreSpawnShockTimer.count >= enemySniper[i].ignoreSpawnShockTimer.time) {
			enemySniper[i].ignoreSpawnShockTimer.count = 0;
			enemySniper[i].isIgnoreShock = false;
		}
	}
}

static void UpdateFlashTimerEnemySniper(void) {
	for (int i = 0; i < kEnemySniperLimit; i++) {
		if (!enemySniper[i].isAlive) continue;
		if (!enemySniper[i].isFlash) continue;

		enemySniper[i].flashTimer.count++;
		if (enemySniper[i].flashTimer.count >= enemySniper[i].flashTimer.time) {
			enemySniper[i].flashTimer.count = 0;
			enemySniper[i].isFlash = false;
		}

	}
}

static void UpdateHitStopTimerEnemySniper(void) {
	for (int i = 0; i < kEnemySniperLimit; i++) {
		if (!enemySniper[i].isAlive) continue;
		if (!enemySniper[i].isHitStop) continue;

		enemySniper[i].hitStopTimer.count++;
		if (enemySniper[i].hitStopTimer.count >= enemySniper[i].hitStopTimer.time) {
			enemySniper[i].hitStopTimer.count = 0;
			enemySniper[i].isHitStop = false;
		}

	}
}

/// <summary>
/// enemySniperの生成演出処理
/// </summary>
static void UpdateSpawnEffectEnemySniper(void) {
	for (int i = 0; i < kEnemySniperLimit; i++) {
		if (!enemySniper[i].isAlive) continue;
		if (enemySniper[i].spawnEasingT >= 1.0f) continue;

		enemySniper[i].spawnEasingT += kSpawnEasingSpeed;
		if (enemySniper[i].spawnEasingT >= 1.0f) {
			enemySniper[i].spawnEasingT = 1.0f;
		}

		enemySniper[i].scale = UpdateValueByEasing(kScaleMax, kScaleMin, enemySniper[i].spawnEasingT, kSpawnEasingMode);
		enemySniper[i].alpha = UpdateValueByEasing(kAlphaMin, kAlphaMax, enemySniper[i].spawnEasingT, kSpawnEasingMode);
	}
}

static void DeadEnemySniper(int index) {
	enemySniper[index].isAlive = false;
	enemySniper[index].hp = enemySniperData.hpMax;

	CreateExplodeEffect(enemySniper[index].pos,WHITE);
}

static void CheckDeadEnemySniper(void) {
	for (int i = 0; i < kEnemySniperLimit; i++) {
		if (!enemySniper[i].isAlive) continue;

		if (enemySniper[i].hp <= 0) {
			DeadEnemySniper(i);

			//ジェムの生成処理
			for (int j = 0; j < enemySniperData.createGemLimit; j++) {
				SpawnGem(enemySniper[i].pos, GemType::S);
			}

			AddScoreByDefeatEnemy(enemySniperData.defeatScore);
		}
	}
}

static void DrawEnemySniper(void) {
	for (int i = 0; i < kEnemySniperLimit; i++) {
		if (!enemySniper[i].isAlive) continue;

		Vector2 size = { enemySniperData.size.x * enemySniper[i].scale, enemySniperData.size.y * enemySniper[i].scale };

		Texture texture = enemySniper[i].texture;
		if (enemySniper[i].isFlash) {
			texture.color = kFlashColor;
		}
		else {
			texture.color = kNormalColor;
		}

		texture.color = (texture.color & 0xFFFFFF00) | enemySniper[i].alpha;

		DrawTextureRotateObj(texture, enemySniper[i].pos, size,enemySniper[i].theta);
	}
}
#pragma endregion

#pragma region Enemy全滅処理

void DeadAllEnemy(void) {
	// Shooter
	for (int i = 0; i < kEnemyShooterLimit; i++) {
		if (enemyShooter[i].isAlive) {
			DeadEnemyShooter(i);
		}
	}

	// Guardian
	for (int i = 0; i < kEnemyGuardianLimit; i++) {
		if (enemyGuardian[i].isAlive) {
			DeadEnemyGuardian(i);
		}
	}

	// Basic
	for (int i = 0; i < kEnemyBasicLimit; i++) {
		if (enemyBasic[i].isAlive) {
			DeadEnemyBasic(i);
		}
	}

	// M
	for (int i = 0; i < kEnemyMLimit; i++) {
		if (enemyM[i].isAlive) {
			DeadEnemyM(i);
		}
	}

	// Small
	for (int i = 0; i < kEnemySmallLimit; i++) {
		if (enemySmall[i].isAlive) {
			DeadEnemySmall(i);
		}
	}

	// Bomb
	for (int i = 0; i < kEnemyBombLimit; i++) {
		if (enemyBomb[i].isAlive) {
			DeadEnemyBomb(i);
		}
	}

	// Sniper
	for (int i = 0; i < kEnemySniperLimit; i++) {
		if (enemySniper[i].isAlive) {
			DeadEnemySniper(i);
		}
	}

	return;

}

#pragma endregion

void InitEnemy(void) {
	InitEnemyShooter();
	InitEnemyGuardian();
	InitEnemyBasic();
	InitEnemyM();
	InitEnemySmall();
	InitEnemyBomb();
	InitEnemySniper();

	SpawnEnemy({ 500, 500 }, EnemyType::EnemySniper);
}

void UpdateEnemy(void) {
	//enemyShooter
	CheckDeadEnemyShooter();
	MoveEnemyShooter();
	UpdateFlashTimerEnemyShooter();
	UpdateHitStopTimerEnemyShooter();
	UpdateSpawnEffectEnemyShooter();
	CountIgnoreShockEnemyShooter();
	ShootEnemyBulletShooter();

	//enemyGuardian
	CheckDeadEnemyGuardian();
	UpdateFlashTimerEnemyGuardian();
	CountIgnoreShockEnemyGuardian();
	UpdateSpawnEffectEnemyGuardian();
	ShootEnemyBulletguardian();

	//enemyBasic
	MoveEnemyBasic();
	CountIgnoreShockEnemyBasic();
	UpdateFlashTimerEnemyBasic();
	UpdateHitStopTimerEnemyBasic();
	UpdateSpawnEffectEnemyBasic();
	CheckDeadEnemyBasic();

	//enemyM
	CheckDeadEnemyM();
	UpdateFlashTimerEnemyM();
	CountIgnoreShockEnemyM();
	UpdateSpawnEffectEnemyM();
	ShootEnemySmall();

	//enemySmall
	MoveEnemySmall();
	CountIgnoreShockEnemySmall();
	UpdateFlashTimerEnemySmall();
	UpdateSpawnEffectEnemySmall();
	CheckDeadEnemySmall();

	//enemyBomb
	MoveEnemyBomb();
	BurstEnemyBomb();
	CountIgnoreShockEnemyBomb();
	UpdateFlashTimerEnemyBomb();
	UpdateSpawnEffectEnemyBomb();
	CheckDeadEnemyBomb();

	//enemySniper
	CheckDeadEnemySniper();
	UpdateFlashTimerEnemySniper();
	UpdateHitStopTimerEnemySniper();
	CountIgnoreShockEnemySniper();
	UpdateSpawnEffectEnemySniper();
	ShootEnemyBulletSniper();

	Attack2EnemyGuardianMove();
}

void DrawEnemy(void) {
	DrawEnemyShooter();
	DrawEnemyGuardian();
	DrawEnemyBasic();
	DrawEnemyM();
	DrawEnemySmall();
	DrawEnemyBomb();
	DrawEnemySniper();
}

#pragma region 関数: 外部参照関係

void SpawnEnemy(Vector2 pos, EnemyType type) {
	switch (type) {
	case EnemyType::EnemyShooter: {
		for (int i = 0; i < kEnemyShooterLimit; i++) {
			if (enemyShooter[i].isAlive) continue;
			enemyShooter[i].isAlive = true;
			enemyShooter[i].pos = pos;
			enemyShooter[i].hp = enemyShooterData.hpMax;

			enemyShooter[i].spawnEasingT = 0.0f;
			enemyShooter[i].scale = kScaleMax;
			enemyShooter[i].alpha = kAlphaMin;

			break;
		}

		break;
	}
	case EnemyType::EnemyBasic: {
		for (int i = 0; i < kEnemyBasicLimit; i++) {
			if (enemyBasic[i].isAlive) continue;
			enemyBasic[i].isAlive = true;
			enemyBasic[i].thetaUpDate = true;
			enemyBasic[i].pos = pos;
			enemyBasic[i].hp = enemyBasicData.hpMax;

			enemyBasic[i].spawnEasingT = 0.0f;
			enemyBasic[i].scale = kScaleMax;
			enemyBasic[i].alpha = kAlphaMin;

			//spawner.spawnTimer.time = spawner.kSpawnTime * nowenemyBasicIsAliveCount;
			break;
		}

		break;
	}
	case EnemyType::EnemySmall: {
		for (int i = 0; i < kEnemySmallLimit; i++) {
			if (enemySmall[i].isAlive) continue;

			enemySmall[i].isAlive = true;
			enemySmall[i].velocityUpDate = true;
			enemySmall[i].pos = pos;
			enemySmall[i].hp = enemySmallData.hpMax;

			enemySmall[i].spawnEasingT = 0.0f;
			enemySmall[i].scale = kScaleMax;
			enemySmall[i].alpha = kAlphaMin;

			enemySmall[i].nowSpeed = 0.0f;
			enemySmall[i].velocity = { 0,0 };

			break;
		}
		break;
	}
	case EnemyType::EnemySmallGroup: {
		for (int createNum = 0; createNum < 3; createNum++) {
			for (int i = 0; i < kEnemySmallLimit; i++) {
				if (enemySmall[i].isAlive) continue;

				enemySmall[i].isAlive = true;
				enemySmall[i].velocityUpDate = true;
				enemySmall[i].pos = CreateScatterPos(pos);
				enemySmall[i].hp = enemySmallData.hpMax;

				enemySmall[i].spawnEasingT = 0.0f;
				enemySmall[i].scale = kScaleMax;
				enemySmall[i].alpha = kAlphaMin;

				// 初期化
				enemySmall[i].nowSpeed = 0.0f;
				enemySmall[i].velocity = { 0,0 };

				break;
			}
		}
		break;
	}
	case EnemyType::EnemySmallFarmation: {
		for (int createNum = 0; createNum < 3; createNum++) {
			for (int i = 0; i < kEnemySmallLimit; i++) {
				if (enemySmall[i].isAlive) continue;

				enemySmall[i].isAlive = true;
				enemySmall[i].velocityUpDate = false;
				Vector2 targetPos = GetPlayerPos();
				Vector2 moveVec = { targetPos.x - pos.x,   targetPos.y - pos.y };

				float disLength = sqrtf(moveVec.x * moveVec.x + moveVec.y * moveVec.y);
				if (disLength != 0.0f) {
					moveVec.x /= disLength;
					moveVec.y /= disLength;
				}
				enemySmall[i].velocity.x = moveVec.x;
				enemySmall[i].velocity.y = moveVec.y;
				enemySmall[i].pos.x += enemySmall[i].velocity.x * enemySmallData.smallFormationRange * createNum;
				enemySmall[i].pos.y += enemySmall[i].velocity.y * enemySmallData.smallFormationRange * createNum;

				enemySmall[i].hp = enemySmallData.hpMax;

				enemySmall[i].spawnEasingT = 0.0f;
				enemySmall[i].scale = kScaleMax;
				enemySmall[i].alpha = kAlphaMin;

				// 初期化
				enemySmall[i].nowSpeed = 0.0f;
				enemySmall[i].velocity = { 0,0 };

				break;
			}
		}
		break;
	}
	case EnemyType::EnemyM: {
		for (int i = 0; i < kEnemyMLimit; i++) {
			if (enemyM[i].isAlive) continue;

			enemyM[i].isAlive = true;
			enemyM[i].pos = pos;
			enemyM[i].hp = enemyMData.hpMax;

			enemyM[i].spawnEasingT = 0.0f;
			enemyM[i].scale = kScaleMax;
			enemyM[i].alpha = kAlphaMin;

			// 初期化（必要なら）
			enemyM[i].shootTimer.count = 0;

			break;
		}
		break;
	}
	case EnemyType::EnemyBomb: {
		for (int i = 0;i < kEnemyBombLimit;i++) {
			if (enemyBomb[i].isAlive)continue;

			enemyBomb[i].isAlive = true;
			enemyBomb[i].pos = pos;

			enemyBomb[i].hp = enemyBombData.hpMax;

			enemyBomb[i].spawnEasingT = 0.0f;
			enemyBomb[i].scale = kScaleMax;
			enemyBomb[i].alpha = kAlphaMin;

			enemyBomb[i].nowSpeed = ToFloat(GetRand(100, 170)) / 10;
			enemyBomb[i].velocity = { ToFloat(GetRand(-10, 10)),ToFloat(GetRand(-10,10)) };
			if (enemyBomb[i].velocity.x != 0) {
				enemyBomb[i].velocity.x /= 10;
			}
			if (enemyBomb[i].velocity.y != 0) {
				enemyBomb[i].velocity.y /= 10;
			}
			enemyBomb[i].burst.time = GetRand(200, 600);
			enemyBomb[i].burst.count = 0;

			break;
		}
	break;
	}
	case EnemyType::EnemyFormation: {
		Vector2 playerPos = GetPlayerPos();
		float formationTheta = atan2f(playerPos.y - pos.y, playerPos.x - pos.x);
		int enemyFomationCount = 0;
		for (int i = 0; i < kEnemyBasicLimit; i++) {
			if (enemyBasic[i].isAlive) continue;

			enemyBasic[i].isAlive = true;
			enemyBasic[i].thetaUpDate = false;
			enemyBasic[i].hp = enemyBasicData.hpMax;

			enemyBasic[i].spawnEasingT = 0.0f;
			enemyBasic[i].scale = kScaleMax;
			enemyBasic[i].alpha = kAlphaMin;


			enemyBasic[i].moveTheta = formationTheta;
			enemyBasic[i].pos.x = pos.x + (enemyBasicData.formationRange * enemyFomationCount - (enemyBasicData.formationRange * 2)) * cosf(formationTheta);
			enemyBasic[i].pos.y = pos.y + (enemyBasicData.formationRange * enemyFomationCount - (enemyBasicData.formationRange * 2)) * sinf(formationTheta);
			enemyFomationCount++;

			//5回生成したら終了
			if (enemyFomationCount == 5) {
				break;
			}
		}

		break;
	}
	case EnemyType::EnemySniper: {
		if (pos.x < 0 || pos.x>2560 || pos.y < 0 || pos.y>1440)break;
		for (int i = 0; i < kEnemySniperLimit; i++) {
			if (enemySniper[i].isAlive) continue;
			enemySniper[i].isAlive = true;
			enemySniper[i].pos = pos;
			enemySniper[i].hp = enemySniperData.hpMax;

			enemySniper[i].spawnEasingT = 0.0f;
			enemySniper[i].scale = kScaleMax;
			enemySniper[i].alpha = kAlphaMin;

			break;
		}

		break;
	}
	}
}


EnemyShooter* GetEnemyShooterArray(void) {
	return enemyShooter;
}

int GetEnemyShooterLimit(void) {
	return kEnemyShooterLimit;
}

void DamageEnemyShooter(EnemyShooter& enemy, int pow, bool hitStopSwitch) {
	enemy.hp -= pow;

	//フラッシュさせる
	enemy.isFlash = true;
	enemy.flashTimer.count = 0;

	//ヒットストップの実装
	if (hitStopSwitch) {
		enemy.isHitStop = true;
		enemy.hitStopTimer.count = 0;
	}

	//ダメージエフェクト
	CreateEnemyDamageEffect(enemy.pos);
	PlaySe(SeType::DamageEnemy);
}

EnemyGuardian* GetEnemyGuardianArray(void) {
	return enemyGuardian;
}

int GetEnemyGuardianLimit(void) {
	return kEnemyGuardianLimit;
}

void DamageEnemyGuardian(EnemyGuardian& enemy, int pow) {
	enemy.hp -= pow;

	//フラッシュさせる
	enemy.isFlash = true;
	enemy.flashTimer.count = 0;

	//ダメージエフェクト
	CreateEnemyDamageEffect(enemy.pos);

	PlaySe(SeType::DamageEnemy);
}

EnemyBasic* GetEnemyBasicArray(void) {
	return enemyBasic;
}

int GetEnemyBasicLimit(void) {
	return kEnemyBasicLimit;
}

void DamageEnemyBasic(EnemyBasic& enemy, int pow, bool hitStopSwitch) {
	enemy.hp -= pow;

	//フラッシュさせる
	enemy.isFlash = true;
	enemy.flashTimer.count = 0;

	//ヒットストップの実装
	if (hitStopSwitch) {
		enemy.isHitStop = true;
		enemy.hitStopTimer.count = 0;
	}

	//ダメージエフェクト
	CreateEnemyDamageEffect(enemy.pos);

	PlaySe(SeType::DamageEnemy);
}

EnemyM* GetEnemyMArray(void) {
	return enemyM;
}

int GetEnemyMLimit(void) {
	return kEnemyMLimit;
}

void DamageEnemyM(EnemyM& enemy, int pow) {
	enemy.hp -= pow;

	//フラッシュさせる
	enemy.isFlash = true;
	enemy.flashTimer.count = 0;

	//ダメージエフェクト
	CreateEnemyDamageEffect(enemy.pos);

	PlaySe(SeType::DamageEnemy);
}

EnemySmall* GetEnemySmallArray(void) {
	return enemySmall;
}

int GetEnemySmallLimit(void) {
	return kEnemySmallLimit;
}

void DamageEnemySmall(EnemySmall& enemy, int pow) {
	enemy.hp -= pow;

	//フラッシュさせる
	enemy.isFlash = true;
	enemy.flashTimer.count = 0;

	//ダメージエフェクト
	CreateEnemyDamageEffect(enemy.pos);

	PlaySe(SeType::DamageEnemy);
}

EnemyBomb* GetEnemyBombArray(void) {
	return enemyBomb;
}

int GetEnemyBombLimit(void) {
	return kEnemyBombLimit;
}

void DamageEnemyBomb(EnemyBomb& enemy, int pow) {
	enemy.hp -= pow;

	//フラッシュさせる
	enemy.isFlash = true;
	enemy.flashTimer.count = 0;

	//ダメージエフェクト
	CreateEnemyDamageEffect(enemy.pos);
	PlaySe(SeType::DamageEnemy);
}

EnemySniper* GetEnemySniperArray(void) {
	return enemySniper;
}

int GetEnemySniperLimit(void) {
	return kEnemySniperLimit;
}

void DamageEnemySniper(EnemySniper& enemy, int pow, bool hitStopSwitch) {
	enemy.hp -= pow;

	//フラッシュさせる
	enemy.isFlash = true;
	enemy.flashTimer.count = 0;

	//ヒットストップの実装
	if (hitStopSwitch) {
		enemy.isHitStop = true;
		enemy.hitStopTimer.count = 0;
	}

	//ダメージエフェクト
	CreateEnemyDamageEffect(enemy.pos);

	PlaySe(SeType::DamageEnemy);
}

float GetEnemyHitRadius(EnemyType type) {
	float hitRadius = {};

	switch (type) {
	case EnemyType::EnemyShooter:
		hitRadius = enemyShooterData.hitRadius;
		break;
	case EnemyType::EnemyGuardian:
		hitRadius = enemyGuardianData.hitRadius;
		break;
	case EnemyType::EnemyBasic:
		hitRadius = enemyBasicData.hitRadius;
		break;
	case EnemyType::EnemyM:
		hitRadius = enemyMData.hitRadius;
		break;
	case EnemyType::EnemySmall:
		hitRadius = enemySmallData.hitRadius;
		break;
	case EnemyType::EnemyBomb:
		hitRadius = enemyBombData.hitRadius;
		break;
	case EnemyType::EnemySniper:
		hitRadius = enemySniperData.hitRadius;
		break;
	}

	return hitRadius;
}

float GetEnemyPow(EnemyType type) {
	float pow = 0;

	switch (type) {
	case EnemyType::EnemyBasic:
		pow = enemyBasicData.pow;
		break;
	case EnemyType::EnemySmall:
		pow = enemySmallData.pow;
		break;
	case EnemyType::EnemyBomb:
		pow = enemyBombData.pow;
		break;
	}

	return pow;
}

#pragma endregion