#include "player_laser.h"
#include "player.h"

#include "vector2.h"
#include "timer.h"
#include "tool.h"
#include "texture.h"

#include <math.h>

static constexpr int kLaserLevel = ToInt(PlayerLaserLevel::Count);
static constexpr int kPlayerLaserLimit = 60;

//レベルごとの性能
struct LaserSpec {
	LaserLevel Level;

	Vector2 size;

	int pow;

	Texture texture = {};
};

LaserSpec laserSpec[kLaserLevel] = {
	//level0
	{ LaserLevel::Level0, {50, 20}, 1, { }},
	//level1
	{ LaserLevel::Level1, {50, 60}, 3, {} },
	//level2
	{ LaserLevel::Level2, {50, 100}, 5, {} },
};

#pragma region データ

static PlayerLaser laser[kPlayerLaserLimit];

#pragma endregion

#pragma region 関数

/// <summary>
/// 移動処理
/// </summary>
static void MoveLaser(void) {
	Vector2 playerPos = GetPlayerPos();
	float theta = GetPlayerRotateTheta();

	for (int i = 0; i < kPlayerLaserLimit; i++) {
		if (!laser[i].isShoot) {
			continue;
		}

		laser[i].disToPlayer += laser[i].moveSpeed;

		laser[i].rotateTheta = theta;

		laser[i].pos.x = playerPos.x + laser[i].disToPlayer * cosf(laser[i].rotateTheta);
		laser[i].pos.y = playerPos.y + laser[i].disToPlayer * sinf(laser[i].rotateTheta);
	}
}

///// <summary>
///// 回転処理
///// </summary>
//static void RotateLaser(void) {
//	float theta = GetPlayerRotateTheta();
//	for (int i = 0; i < kPlayerLaserLimit; i++) {
//		if (!laser[i].isShoot) {
//			continue;
//		}
//
//		laser[i].rotateTheta = theta;
//	}
//}

static void UpdateHitVertex(void) {
	for (int i = 0; i < kPlayerLaserLimit; i++) {
		if (!laser[i].isShoot) continue;

		SetVertexRotate(laser[i].pos, laser[i].size, laser[i].hitBoxVertex, laser[i].rotateTheta);

	}
}

/// <summary>
/// 自壊タイマーのカウント処理
/// </summary>
/// <param name=""></param>
static void BreakLaserItSelf(void) {
	for (int i = 0; i < kPlayerLaserLimit; i++) {
		if (!laser[i].isShoot) {
			continue;
		}

		laser[i].breakTimer.count++;
		if (laser[i].breakTimer.count >= laser[i].breakTimer.time) {
			BreakLaser(i);
		}
	}
}

#pragma endregion

void InitPlayerLaser(void) {
	for (int i = 0; i < kPlayerLaserLimit; i++) {
		laser[i] = {};
	}

	//テクスチャの取得
	if (laserSpec[0].texture.tHandle == -1) {
		laserSpec[0].texture = GetTexture(TextureType::PlayerLaser0);
		laserSpec[1].texture = GetTexture(TextureType::PlayerLaser1);
		laserSpec[2].texture = GetTexture(TextureType::PlayerLaser2);
	}
}

void UpdatePlayerLaser(void) {
	MoveLaser();
	UpdateHitVertex();
	BreakLaserItSelf();
}

void DrawPlayerLaser(void) {
	for (int i = 0; i < kPlayerLaserLimit; i++) {
		if (!laser[i].isShoot) continue;

		//Vector2 drawSize = { laser[i].length, laser[i].size.y };
		//DrawTextureRotateObj(laser[i].texture, laser[i].pos, drawSize, laser[i].moveTheta);

		DrawTextureRotateObj(laser[i].texture, laser[i].pos, laser[i].size, laser[i].rotateTheta);
	}
}

#pragma region 関数: 外部参照関係

void ShootPlayerLaser(Vector2 pos, float moveTheta, float firstDisLength) {
	int level = ToInt(GetPlayerNowLaserLevel());
	LaserSpec& spec = laserSpec[level];

	for (int i = 0; i < kPlayerLaserLimit; i++) {
		if (laser[i].isShoot) continue;

		laser[i].isShoot = true;

		laser[i].pos = pos;
		laser[i].rotateTheta = moveTheta;

		laser[i].disToPlayer = firstDisLength;

		//レベルごとの性能を適用
		laser[i].level = spec.Level;
		laser[i].size = spec.size;
		laser[i].pow = spec.pow;
		laser[i].texture = spec.texture;

		break;
	}
}

PlayerLaser* GetPlayerLaserArray(void) {
	return laser;
}

int GetPlayerLaserLimit(void) {
	return kPlayerLaserLimit;
}

int GetLaserPow(int index) {
	int pow;

	if (laser[index].level != LaserLevel::Level2) {
		pow = laser[index].pow;
	} else {
		pow = ToInt(laser[index].pow * GetPlayerLaserPowRate());
	}
	return pow;
}

/// <summary>
/// 消滅処理
/// </summary>
/// <param name="index">番号</param>
void BreakLaser(int index) {
	laser[index].isShoot = false;
	laser[index].breakTimer.count = 0;
}

#pragma endregion