#include "enemy_bullet.h"
#include "boss_enemy.h"

#include "vector2.h"
#include "timer.h"
#include "tool.h"
#include "texture.h"

#include <math.h>

static constexpr int kEnemyBulletLimit = 20;
static EnemyBullet enemyBullet[kEnemyBulletLimit];

static constexpr int kEnemyLaserLimit = 100;
static EnemyLaser enemyLaser[kEnemyLaserLimit];

#pragma region 関数: enemyBullet

static void MoveEnemyBullet(void) {
	for (int i = 0; i < kEnemyBulletLimit; i++) {
		if (!enemyBullet[i].isShoot) continue;

		enemyBullet[i].pos.x += cosf(enemyBullet[i].moveTheta) * enemyBullet[i].moveSpeed;
		enemyBullet[i].pos.y += sinf(enemyBullet[i].moveTheta) * enemyBullet[i].moveSpeed;
	}
}

/// <summary>
/// 自壊タイマーのカウント処理
/// </summary>
/// <param name=""></param>
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

#pragma endregion

#pragma region 関数: enemyLaser

/// <summary>
/// 移動処理
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

static void UpdateEnemyLaserHitVertex(void) {
	for (int i = 0; i < kEnemyLaserLimit; i++) {
		if (!enemyLaser[i].isShoot) continue;

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
	for (int i = 0; i < kEnemyBulletLimit; i++) {
		if (!enemyBullet[i].isShoot) continue;

		UpdateAnimation(enemyBullet[i].texture);
		DrawTextureObj(enemyBullet[i].texture, enemyBullet[i].pos, enemyBullet[i].size);
	}

	for (int i = 0; i < kEnemyLaserLimit; i++) {
		if (!enemyLaser[i].isShoot) continue;

		DrawTextureRotateObj(enemyLaser[i].texture, enemyLaser[i].pos, enemyLaser[i].size, enemyLaser[i].rotateTheta);
		
	}
}

#pragma region 関数: 外部参照関係

void ShootEnemyBullet(Vector2 shootPos, float moveTheta) {
	for (int i = 0; i < kEnemyBulletLimit; i++) {
		if (enemyBullet[i].isShoot) continue;

		enemyBullet[i].isShoot = true;
		enemyBullet[i].pos = shootPos;
		enemyBullet[i].moveTheta = moveTheta;

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

void ShootEnemyLaser(Vector2 pos, float moveTheta, float firstDisLength) {;
	for (int i = 0; i < kEnemyLaserLimit; i++) {
		if (enemyLaser[i].isShoot) continue;

		enemyLaser[i].isShoot = true;

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