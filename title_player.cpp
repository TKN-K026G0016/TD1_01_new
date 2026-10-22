#include "title_player.h"
#include "elec_bullet.h"
#include "effect.h"
#include "se.h"

#include "vector2.h"
#include "timer.h"
#include "texture.h"
#include "input.h"
#include "tool.h"

#define _USE_MATH_DEFINES
#include <math.h>

struct TitlePlayer {
	Vector2 pos = { 640, -200 };
	Vector2 size = { 32, 32 };

	//<移動関係>
	Vector2 velocity = { 0.0f, 0.0f };
	//移動速度(等速)
	float moveSpeed= 6.0f;
	Vector2 inputVec = { 0, 0 };


	//<回転関係>
	float rotateTheta = ToFloat(M_PI) / 2.0f;

	//最初の移動を開始するまでの時間
	Timer waitMoveStartTimer = { 10, 0 };

	//最初の移動演出
	float easingT = 0.0f;
	float easingSpeed = 0.01f;
	EasingMode easingMode = EasingMode::EaseOut;
	Vector2 startPos = { 640, -200 };
	Vector2 endPos = { 640, 100 };

	Texture texture = {};
};
TitlePlayer player;

#pragma region データ: 射撃関係

static Vector2 shootPos = {};
//プレイヤーの中心からの発射位置の距離
constexpr float kShootDisLength = 15.0f;

//チャージタイマー
static Timer chargeElecBulletTimer = { 40, 0 };

//チャージの破片表示を開始する時間
constexpr int kStartCreateChargeChipTime = 7;
  
static Timer createChargeOrbTimer = { 10, 0 };
static Timer createChargeChipTimer = { 10, 0 };

#pragma endregion

static void CheckInput(void) {
	//横
	if (CheckInputAction(InputAction::MoveRight)) {
		player.inputVec.x = 1;
	} else if (CheckInputAction(InputAction::MoveLeft)) {
		player.inputVec.x = -1;
	} else {
		player.inputVec.x = 0;
	}
	//縦
	if (CheckInputAction(InputAction::MoveUp)) {
		player.inputVec.y = 1;
	} else if (CheckInputAction(InputAction::MoveDown)) {
		player.inputVec.y = -1;
	} else {
		player.inputVec.y = 0;
	}
}

static void MovePlayer(void) {
	if (player.easingT < 1.0f) return;

	Vector2 moveVec = player.inputVec;
	float moveVecLength = sqrtf(moveVec.x * moveVec.x + moveVec.y * moveVec.y);
	if (moveVecLength != 0) {
		moveVec.x /= moveVecLength;
		moveVec.y /= moveVecLength;
	}

	player.pos.x += player.moveSpeed * moveVec.x;
	player.pos.y += player.moveSpeed * moveVec.y;

	//playerの移動エフェクト
	CreatePlayerMoveEffect(player.pos, moveVec);
}

static void ClampPlayerPos(void) {
	if (player.easingT < 1.0f) return;

	//横
	if (player.pos.x < 0.0f) {
		player.pos.x = 0.0f;
	} else if (player.pos.x > 1280) {
		player.pos.x = 1280;
	}

	//縦
	if (player.pos.y < 0) {
		player.pos.y = 0;
	} else if (player.pos.y > 720) {
		player.pos.y = 720;
	}
}

static void ShootElecBullet(void) {

#pragma region 射撃位置設定
	shootPos.x = player.pos.x + kShootDisLength * cosf(player.rotateTheta);
	shootPos.y = player.pos.y + kShootDisLength * sinf(player.rotateTheta);

#pragma endregion

	//押し
	if (CheckInputAction(InputAction::Charge)) {
		if (chargeElecBulletTimer.count >= chargeElecBulletTimer.time) return;

		if (chargeElecBulletTimer.count > chargeElecBulletTimer.time) return;

		chargeElecBulletTimer.count++;
	}
	//離す
	else {
		if (chargeElecBulletTimer.count <= 0) return;

		//通常弾
		if (chargeElecBulletTimer.count < chargeElecBulletTimer.time) {
			ShootElecBullet(shootPos, player.rotateTheta, ElecBulletType::Normal);

			PlaySe(SeType::ShootElecBullet);
		}
		//チャージ弾
		else {
			ShootElecBullet(shootPos, player.rotateTheta, ElecBulletType::Charge);
			
			PlaySe(SeType::ShootElecBulletCharge);
		}

		chargeElecBulletTimer.count = 0;

	}

}

static void CreateChargeOrbEffect(void) {

	if (chargeElecBulletTimer.count > kStartCreateChargeChipTime) {

		createChargeChipTimer.count++;
		if (createChargeChipTimer.count >= createChargeChipTimer.time) {
			createChargeChipTimer.count = 0;
			CreateChargeChipEffect();
		}

		PlaySeContinuous(SeType::Charge);
	}
	else {
		StopPlayingSe(SeType::Charge);
	}

	if (chargeElecBulletTimer.count >= chargeElecBulletTimer.time) {

		createChargeOrbTimer.count++;
		if (createChargeOrbTimer.count >= createChargeOrbTimer.time) {
			createChargeOrbTimer.count = 0;
			CreateChargeOrbEffect(shootPos);
		}
	}
}

static void MovePlayerFirst(void) {
	if (player.easingT >= 1.0f) return;

	player.waitMoveStartTimer.count++;
	if (player.waitMoveStartTimer.count < player.waitMoveStartTimer.time) return;
	
	player.easingT += player.easingSpeed;
	if (player.easingT >= 1.0f) {
		player.easingT = 1.0f;
	}
	player.pos = MoveByEasing(player.startPos, player.endPos, player.easingT, player.easingMode);
}

void InitTitlePlayer(void) {
	player = {};
	player.pos = player.startPos;
	player.texture = GetTexture(TextureType::Player);
}

void UpdateTitlePlayer(void) {
	CheckInput();

	MovePlayer();
	ClampPlayerPos();

	ShootElecBullet();
	CreateChargeOrbEffect();

	MovePlayerFirst();
}

void DrawTitlePlayer(void) {
	DrawTextureRotateUI(player.texture, player.pos, player.size, player.rotateTheta);
}

Vector2 GetTitlePlayerPos() {
	return player.pos;
}

Vector2 GetTitlePlayerShootPos() {
	return shootPos;
}

void NoticeTitlePlayerGameStart(void) {
	chargeElecBulletTimer.count = 0;
	StopPlayingSe(SeType::Charge);
}