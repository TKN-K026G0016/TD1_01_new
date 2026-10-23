#include "gem.h"
#include "player.h"
#include "score_data.h"
#include "se.h"
#include "effect.h"
#include "stage.h"

#include "vector2.h"
#include "timer.h"
#include "tool.h"
#include "texture.h"
#include "effect.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <algorithm>

constexpr int kGetGemScore = 50;

constexpr int kGemLimit = 100;
Gem gem[kGemLimit];

#pragma region 共通データ
//可動域
static Vector2 movablePos[2] = {};

//吸引の範囲(距離)
static constexpr float kStartVacumeRange = 75.0f;
//吸引速度
static constexpr float kVacumeSpeed = 6.5f;
//吸引の追従割合
static constexpr float kVacumeLerpRate = 0.3f;

//散らばる範囲(最大)
constexpr int kScatterLengthMax = 200;
//散らばる範囲(最小)
constexpr int kScatterLengthMin = 0;


//通電できる最大回数
static constexpr int kHpMax = 8;

#pragma endregion

struct GemSpec {
	GemType type;
	Vector2 size;
	float hitRadius;
	float recoveryEnergy;

	Texture texture = {};
	Texture lightTexture = {};
};

GemSpec spec[ToInt(GemType::Count)] = {
	//type //size //hitRadius // //回復量
	{GemType::S, {32,32}, 20.0f, 8.0f, {}},
	{GemType::M, {32,32}, 20.0f, 15.0f, {}},
	{GemType::L, {40,40}, 20.0f, 40.0f, {}},
};



#pragma region 関数: 本体

static void MoveGem(void) {
	for (int i = 0; i < kGemLimit; i++) {
		if (!gem[i].isAlive) continue;

		//playerとの距離を測る
		Vector2 playerPos = GetPlayerPos();
		float disX = playerPos.x - gem[i].pos.x;
		float disY = playerPos.y - gem[i].pos.y;
		float disLength = sqrtf(disX * disX + disY * disY);
		if (disLength != 0) {
			disX /= disLength;
			disY /= disLength;
		}

		//吸い込みフラグON
		if (disLength <= kStartVacumeRange && !GetPlayerIsInvincible()) {
			gem[i].isVacume = true;

		}

		//吸い込みでの移動処理
		if (gem[i].isVacume) {
			gem[i].moveVec.x = gem[i].moveVec.x * (1 - kVacumeLerpRate) + disX * kVacumeLerpRate;
			gem[i].moveVec.y = gem[i].moveVec.y * (1 - kVacumeLerpRate) + disY * kVacumeLerpRate;

			float length = sqrtf(gem[i].moveVec.x * gem[i].moveVec.x + gem[i].moveVec.y * gem[i].moveVec.y);

			if (length != 0.0f) {
				gem[i].moveVec.x /= length;
				gem[i].moveVec.y /= length;
			}

			gem[i].pos.x += gem[i].moveVec.x * kVacumeSpeed;
			gem[i].pos.y += gem[i].moveVec.y * kVacumeSpeed;
		}


		//ステージから出ないように補正を掛ける
		gem[i].pos.x = std::clamp(gem[i].pos.x, movablePos[0].x, movablePos[1].x);
		gem[i].pos.y = std::clamp(gem[i].pos.y, movablePos[0].y, movablePos[1].y);
	}
}

static void CountIgnoreElecShock(void) {
	for (int i = 0; i < kGemLimit; i++) {
		if (!gem[i].isAlive) continue;

		UpdateAnimation(gem[i].texture);
		UpdateAnimation(gem[i].lightTexture);

		if (!gem[i].isIgnoreElecShock) continue;

		gem[i].ignoreElecShockTimer.count++;
		if (gem[i].ignoreElecShockTimer.count >= gem[i].ignoreElecShockTimer.time) {
			gem[i].ignoreElecShockTimer.count = 0;
			gem[i].isIgnoreElecShock = false;
		}
	}
}

/// <summary>
/// プレイヤーとの当たり判定処理
/// </summary>
/// <param name=""></param>
static void CheckHitVSPlayer(void) {
	Vector2 playerPos = GetPlayerPos();
	float playerHitRadius = GetPlayerHitRadius();

	for (int i = 0; i < kGemLimit; i++) {
		if (!gem[i].isAlive) continue;

		//接触したら、エネルギー回復させる
		if (CheckCollisionCircleVSCircle(playerPos, playerHitRadius, gem[i].pos, gem[i].hitRadius)) {
			
			BreakGem(i);

			AddScore(kGetGemScore);

			RecoveryEnergy(gem[i].recoveryEnergy);

			//SEの再生
			PlaySe(SeType::GetGem);
			//CreateJemEffect();
			TriggerCreateRecoveryEffect();

			//シールドの回復エフェクト
			ChangeModeElecShield(ModeElecShield::Recovery);
		}
	}

}

/// <summary>
/// ジェムの自壊タイマーカウント処理
/// </summary>
/// <param name=""></param>
static void CountBreakTimer(void) {
	for (int i = 0; i < kGemLimit; i++) {
		if (!gem[i].isAlive) continue;

		gem[i].breakTimer.count++;
		if (gem[i].breakTimer.count >= gem[i].breakTimer.time) {
			BreakGem(i);
		}
	}
}

#pragma endregion

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

void InitGem(void) {
	spec[ToInt(GemType::S)].texture = GetTexture(TextureType::GemSmall);
	spec[ToInt(GemType::S)].lightTexture = GetTexture(TextureType::GemSmallLight);
	spec[ToInt(GemType::M)].texture = GetTexture(TextureType::Gem);
	spec[ToInt(GemType::M)].lightTexture = GetTexture(TextureType::GemSmallLight);
	spec[ToInt(GemType::L)].texture = GetTexture(TextureType::Gem);
	spec[ToInt(GemType::L)].lightTexture = GetTexture(TextureType::GemSmallLight);

	//可動域を取得
	GetMovablePos(movablePos);

	for (int i = 0; i < kGemLimit; i++) {
		gem[i] = {};
	}
}

void UpdateGem(void) {
	MoveGem();
	CheckHitVSPlayer();
	CountIgnoreElecShock();
	CountBreakTimer();
}

void DrawGem(void) {
	for (int i = 0; i < kGemLimit; i++) {
		if (!gem[i].isAlive) continue;

		if (!gem[i].isIgnoreElecShock) {
			DrawTextureObj(gem[i].texture, gem[i].pos, gem[i].size);
		} else {
			DrawTextureObj(gem[i].lightTexture, gem[i].pos, gem[i].size);
		}

#ifdef _DEBUG
		//DrawHitEllipse(gem[i].pos, gem[i].hitRadius, BLUE, false);
#endif // _DEBUG

	}
}

#pragma region 関数: 外部参照関係

void SpawnGem(Vector2 pos, GemType type) {
	for (int i = 0; i < kGemLimit; i++) {
		if (gem[i].isAlive) continue;

		gem[i].isAlive = true;
		gem[i].pos = CreateScatterPos(pos);
		gem[i].hp = kHpMax;

		gem[i].isVacume = false;

		gem[i].type = type;
		gem[i].size = spec[ToInt(type)].size;
		gem[i].hitRadius = spec[ToInt(type)].hitRadius;
		gem[i].recoveryEnergy = spec[ToInt(type)].recoveryEnergy;

		gem[i].texture = spec[ToInt(type)].texture;
		gem[i].lightTexture = spec[ToInt(type)].lightTexture;

		break;
	}
}

void BreakGem(int index) {
	gem[index].isAlive = false;
	gem[index].breakTimer.count = 0;

	CreateBreakGemEffect(gem[index].pos);
}

Gem* GetGemArray(void) {
	return gem;
}

int GetGemLimit(void) {
	return kGemLimit;
}



#pragma endregion