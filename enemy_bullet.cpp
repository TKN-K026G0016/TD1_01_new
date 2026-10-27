#include "enemy_bullet.h"
#include "boss_enemy.h"

#include "vector2.h"
#include "timer.h"
#include "tool.h"
#include "texture.h"

#include <math.h>

#pragma region データ: enemyBullet

struct EnemyBulletSpec {
	Vector2 size = {};

	float hitRadius = 10;

	float pow =40.0f;

	float moveSpeed = 4.0f;
};
static EnemyBulletSpec bulletSpec[ToInt(TypeEnemyBullet::Count)] = {
	{{32, 32}, 10.0f, 30.0f, 4.0f},
	{{40, 40}, 20.0f, 30.0f, 5.0f},
};

static constexpr int kEnemyBulletLimit = 50;
static EnemyBullet enemyBullet[kEnemyBulletLimit];

#pragma endregion

#pragma region データ: enemyLaser
static constexpr int kEnemyLaserLimit = 100;
static EnemyLaser enemyLaser[kEnemyLaserLimit];
//模擬弾レーザー不透明度
static constexpr int kDummyLaserAlpha = 15;

#pragma endregion

#pragma region 関数: enemyBullet

static void MoveEnemyBullet(void) {
	for (int i = 0; i < kEnemyBulletLimit; i++) {
		if (!enemyBullet[i].isShoot) continue;

		enemyBullet[i].pos.x += cosf(enemyBullet[i].moveTheta) * enemyBullet[i].moveSpeed;
		enemyBullet[i].pos.y += sinf(enemyBullet[i].moveTheta) * enemyBullet[i].moveSpeed;

		//アニメーション処理
		UpdateAnimation(enemyBullet[i].texture);
	}
}

/// <summary>
/// 自壊タイマーのカウント処理
/// </summary>
static void BreakEnemyBulletItSelf(void) {
	for (int i = 0; i < kEnemyBulletLimit; i++) {
		if (!enemyBullet[i].isShoot) {
			continue;
		}

		enemyBullet[i].breakTimer.count++;
		if (enemyBullet[i].breakTimer.count >= enemyBullet[i].breakTimer.time) {
			BreakEnemyBullet(i);
		}
	}
}

/// <summary>
/// enemyBulletの描画処理
/// </summary>
static void DrawBullet(void) {
	for (int i = 0; i < kEnemyBulletLimit; i++) {
		if (!enemyBullet[i].isShoot) continue;

		DrawTextureObj(enemyBullet[i].texture, enemyBullet[i].pos, enemyBullet[i].size);
	}
}

#pragma endregion

#pragma region 関数: enemyLaser

/// <summary>
/// Laserの移動処理
/// </summary>
static void MoveEnemyLaser(void) {
	Vector2 bossPos = GetBossEnemyPos();
	float theta = GetBossEnemyRotateTheta();

	for (int i = 0; i < kEnemyLaserLimit; i++) {
		if (!enemyLaser[i].isShoot) {
			continue;
		}

		enemyLaser[i].disToBoss += enemyLaser[i].moveSpeed;

		enemyLaser[i].rotateTheta = theta;

		enemyLaser[i].pos.x = bossPos.x + enemyLaser[i].disToBoss * cosf(enemyLaser[i].rotateTheta);
		enemyLaser[i].pos.y = bossPos.y + enemyLaser[i].disToBoss * sinf(enemyLaser[i].rotateTheta);
	}
}

/// <summary>
/// Laserの当たり判定更新処理
/// </summary>
/// <param name=""></param>
static void UpdateEnemyLaserHitVertex(void) {
	for (int i = 0; i < kEnemyLaserLimit; i++) {
		if (!enemyLaser[i].isShoot) continue;
		//模擬弾の場合はスキップ
		if (enemyLaser[i].type == TypeEnemyLaser::Dummy) continue;

		SetVertexRotate(enemyLaser[i].pos, enemyLaser[i].size, enemyLaser[i].hitBoxVertex, enemyLaser[i].rotateTheta);
	}
}

/// <summary>
/// 自壊タイマーのカウント処理
/// </summary>
/// <param name=""></param>
static void BreakLaserItSelf(void) {
	for (int i = 0; i < kEnemyLaserLimit; i++) {
		if (!enemyLaser[i].isShoot) {
			continue;
		}

		enemyLaser[i].breakTimer.count++;
		if (enemyLaser[i].breakTimer.count >= enemyLaser[i].breakTimer.time) {
			BreakEnemyLaser(i);
		}
	}
}

/// <summary>
/// enemyLaserの描画処理
/// </summary>
static void DrawLaser(void) {
	for (int i = 0; i < kEnemyLaserLimit; i++) {
		if (!enemyLaser[i].isShoot) continue;

		Texture texture = enemyLaser[i].texture;
		if (enemyLaser[i].type == TypeEnemyLaser::Dummy) {
			texture.color = (texture.color & 0xFFFFFF00) | kDummyLaserAlpha;
		}

		DrawTextureRotateObj(texture, enemyLaser[i].pos, enemyLaser[i].size, enemyLaser[i].rotateTheta);

	}
}

#pragma endregion

void InitEnemyBullet(void) {
	for (int i = 0; i < kEnemyBulletLimit; i++) {
		enemyBullet[i] = {};

		enemyBullet[i].texture = GetTexture(TextureType::EnemyBullet);
	}

	for (int i = 0; i < kEnemyLaserLimit; i++) {
		enemyLaser[i] = {};

		enemyLaser[i].texture = GetTexture(TextureType::EnemyLaser);
	}
}

void UpdateEnemyBullet(void) {
	MoveEnemyBullet();
	BreakEnemyBulletItSelf();

	MoveEnemyLaser();
	UpdateEnemyLaserHitVertex();
	BreakLaserItSelf();
}

void DrawEnemyBullet(void) {
	DrawBullet();
	DrawLaser();
}

#pragma region 関数: 外部参照関係

void ShootEnemyBullet(Vector2 shootPos, float moveTheta, TypeEnemyBullet type) {
	for (int i = 0; i < kEnemyBulletLimit; i++) {
		if (enemyBullet[i].isShoot) continue;

		enemyBullet[i].isShoot = true;
		enemyBullet[i].pos = shootPos;
		enemyBullet[i].moveTheta = moveTheta;

		//データの設定
		enemyBullet[i].size = bulletSpec[ToInt(type)].size;
		enemyBullet[i].hitRadius = bulletSpec[ToInt(type)].hitRadius;
		enemyBullet[i].moveSpeed = bulletSpec[ToInt(type)].moveSpeed;
		enemyBullet[i].pow = bulletSpec[ToInt(type)].pow;

		break;
	}
}

EnemyBullet* GetEnemyBulletArray(void) {
	return enemyBullet;
}

int GetEnemyBulletLimit(void) {
	return kEnemyBulletLimit;
}

void BreakEnemyBullet(int index) {
	enemyBullet[index].isShoot = false;
	enemyBullet[index].breakTimer.count = 0;
}

void ShootEnemyLaser(Vector2 pos, float moveTheta, float firstDisLength, TypeEnemyLaser type) {;
	for (int i = 0; i < kEnemyLaserLimit; i++) {
		if (enemyLaser[i].isShoot) continue;

		enemyLaser[i].isShoot = true;
		enemyLaser[i].type = type;

		enemyLaser[i].pos = pos;
		enemyLaser[i].rotateTheta = moveTheta;

		enemyLaser[i].disToBoss = firstDisLength;

		break;
	}
}

EnemyLaser* GetEnemyLaserArray(void) {
	return enemyLaser;
}

int GetEnemyLaserLimit(void) {
	return kEnemyLaserLimit;
}

/// <summary>
/// 消滅処理
/// </summary>
/// <param name="index">番号</param>
void BreakEnemyLaser(int index) {
	enemyLaser[index].isShoot = false;
	enemyLaser[index].breakTimer.count = 0;
}

#pragma endregion