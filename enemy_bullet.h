#pragma once
#include "vector2.h"
#include "timer.h"
#include "texture.h"
#include "tool.h"

struct EnemyBullet {
	Vector2 pos = {};
	Vector2 size = { 32, 32 };

	float hitRadius = 10;

	float pow = 30.0f;

	//<移動関係>
	float moveSpeed = 4.0f;
	float moveTheta = 0.0f;

	bool isShoot = false;
	Timer breakTimer = { 200, 0 };

	Texture texture = {};
};

struct EnemyLaser {
	Vector2 pos = { 0, 0 };
	Vector2 size = { 60, 90 };

	//<移動関係>
	float moveSpeed = 20.0f;
	float rotateTheta = 0.0f;
	//playerとの距離
	float disToBoss = 0.0f;

	float pow = 10.0f;

	//射撃フラグ
	bool isShoot = false;
	//自壊タイマー
	Timer breakTimer = { 120, 0 };

	//当たり判定の頂点座標
	Vector2 hitBoxVertex[kVertexNum] = {};

	Texture texture = {};
};

void InitEnemyBullet(void);
void UpdateEnemyBullet(void);
void DrawEnemyBullet(void);

/// <summary>
/// enemyBulletの射撃処理
/// </summary>
/// <param name="shootPos">生成位置</param>
/// <param name="moveTheta">射撃方向</param>
void ShootEnemyBullet(Vector2 shootPos, float moveTheta);

void BreakEnemyBullet(int index);

EnemyBullet* GetEnemyBulletArray(void);

int GetEnemyBulletLimit(void);

void ShootEnemyLaser(Vector2 pos, float moveTheta, float firstDisLength);

EnemyLaser* GetEnemyLaserArray(void);

int GetEnemyLaserLimit(void);

void BreakEnemyLaser(int index);