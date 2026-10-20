#include "stage.h"
#include "player.h"
#include "enemy.h"
#include "boss_enemy.h"
#include "scene_manager.h"
#include "score_data.h"
#include "effect.h"
#include "play_ui.h"
#include "se.h"
#include "bgm.h"

#include "texture.h"
#include "vector2.h"
#include "timer.h"
#include "common.h"
#include "tool.h"

#include <Novice.h>
#include <math.h>
#include <algorithm>

#pragma region データ: 可動域

constexpr Vector2 movablePos[2] = { {0, 0}, {2560, 1440} };
constexpr Vector2 cameraMovablePos[2] = { {movablePos[0].x + 640, movablePos[0].y + 360}, {movablePos[1].x - 640, movablePos[1].y - 360} };
constexpr float cameraMovablePosOffset = 64;

#pragma endregion

#pragma region データ: カメラ関係
struct Camera {
	Vector2 pos = { 0, 0 };

	float zoom = 1.0f;
	float zoomMax = 1.0f;
	float zoomMin = 0.5f;

	//<移動処理関係>
	float moveSpeed = 6.5f;
	float moveSpeedDece = 1.0f;
	//減速するまでの距離
	float deceleratonDis = 60.0f;
	//補間速度
	float chaseLerpRate = 0.10f;
	//デッドゾーン
	const float deadZone = 5.0f;

	Vector2 moveVec = { 0, 0 };
};
Camera camera;

#pragma endregion

#pragma region データ: ラウンド関係

enum class Round {
	Round1,
	Round2,
	Round3,
	Round4,
	Round5,

	RoundBoss,

	Count
};
//現在のラウンド
Round nowRound = Round::Round1;

//経過時間
TimeData gameTime = { 0, 0, 0 };
//ラウンドの変更時間
TimeData changeRoundTime[ToInt(Round::Count)] = {
	{ 0,0,0 },
	{ 0,20,0 },
	{ 1,0,0 },
	{ 1,30,0 },
	{ 2,0,0 },

	//ボス
	{ 2,30,0 },
};

#pragma endregion

#pragma region データ: Enemy生成関係

constexpr int kMaxSpawner = 5;

struct SpawnerData {
	//生成する種類
	EnemyType type = EnemyType::None;
	Timer timer = { 0, 0 };
};


SpawnerData spawners[ToInt(Round::Count)][kMaxSpawner] = {
	//Round1
	{
		{EnemyType::EnemyBasic, {230, 120}},
	},
	//Round2
	{
		{ EnemyType::EnemyBasic, {200, 0} },
		{ EnemyType::EnemySmallGroup, {180, 0} },
		{ EnemyType::EnemyFormation, {300, 0} },
	},
	//Round3
	{
		{ EnemyType::EnemyBasic, {150, 0} },
		{ EnemyType::EnemySmallGroup, {160, 0} },
		{ EnemyType::EnemyFormation, {280, 0} },
		{ EnemyType::EnemyShooter, {300, 0} },
	},
	//Round4
	{
		{ EnemyType::EnemyBasic, {180, 0} },
		{ EnemyType::EnemySmallGroup, {200, 0} },
		//{ EnemyType::EnemyFormation, {280, 0} },
		//{ EnemyType::EnemySniper, {300, 0} },
		{EnemyType::EnemyBomb, {300, 300}},
		{EnemyType::EnemyM,{180,0}},
	},
	//Round5
	{
		{ EnemyType::EnemyBasic, {120, 0} },
		{ EnemyType::EnemySmallGroup, {60, 50} },
		{ EnemyType::EnemyFormation, {250, 0} },
		{ EnemyType::EnemyShooter, {150, 0} },
		{EnemyType::EnemyBomb, {300, 0}},
	},

	//BossRound
		{
		{ EnemyType::EnemyBasic, {150, 0} },
		{ EnemyType::EnemySmallGroup, {200, 50} },
		{ EnemyType::EnemyFormation, {250, 0} },
		{ EnemyType::EnemyShooter, {250, 0} },
		//{EnemyType::EnemyBomb, {500, 0}},
	},
};


//カメラ端からの生成位置のずれ
float spawnPosOffset = 100;

#pragma endregion

#pragma region データ: 背景
struct BackGround {
	Vector2 pos = { 1280, 720 };
	Vector2 size = { 2560, 1440 };

	float scrollRate = 1.0f;

	Texture texture = {};
};
BackGround backGround;

#pragma endregion

#pragma region 演出関係

//現在の開始演出処理
ModeStartEffect nowModeStart = ModeStartEffect::WaitPlayerSpawn;

//開始演出
Timer waitPlayerTimer = { 60, 0 };
//Readyの表示される時間
Timer readyTelopTimer = { 100, 0 };
//Goの表示される時間
Timer goTelopTimer = { 30, 0 };

//プレイヤー死亡後、ゲームオーバーへ向かうまでの時間
Timer goToGameoverTimer = { 150, 0 };

//boss死亡後、クリアへ向かうまでの時間
Timer goToClearTimer = { 180, 0 };

#pragma region データ: ボス警告

static bool isWarningBoss = false;
Timer warnBossTimer = { 300, 0 };

#pragma endregion

#pragma endregion



#pragma region 関数: カメラ関係

static void InitCamera(void) {
	camera.pos = GetPlayerPos();
}

static void MoveCameraPos(void) {
	//追跡処理
	Vector2 targetPos = GetPlayerPos();
	//方向計算
	Vector2 moveVec;
	moveVec.x = targetPos.x - camera.pos.x;
	moveVec.y = targetPos.y - camera.pos.y;
	float moveVecLength = sqrtf(moveVec.x * moveVec.x + moveVec.y * moveVec.y);

	//デッドゾーン判定
	if (moveVecLength <= camera.deadZone) {
		return;
	}

	if (moveVecLength != 0) {
		moveVec.x /= moveVecLength;
		moveVec.y /= moveVecLength;
	}
	camera.moveVec.x = camera.moveVec.x * (1 - camera.chaseLerpRate) + moveVec.x * camera.chaseLerpRate;
	camera.moveVec.y = camera.moveVec.y * (1 - camera.chaseLerpRate) + moveVec.y * camera.chaseLerpRate;

	//移動速度
	float moveSpeed = (moveVecLength > camera.deceleratonDis) ? camera.moveSpeed : camera.moveSpeedDece;

	camera.pos.x += camera.moveVec.x * moveSpeed;
	camera.pos.y += camera.moveVec.y * moveSpeed;

}

static void ClampCameraPos(void) {

	Vector2 minPos = {
		 cameraMovablePos[0].x - cameraMovablePosOffset,
		 cameraMovablePos[0].y - cameraMovablePosOffset
	};

	Vector2 maxPos = {
		cameraMovablePos[1].x + cameraMovablePosOffset,
		cameraMovablePos[1].y + cameraMovablePosOffset
	};

	camera.pos.x = std::clamp(camera.pos.x, minPos.x, maxPos.x);
	camera.pos.y = std::clamp(camera.pos.y, minPos.y, maxPos.y);

	//if (camera.pos.x < minPos.x) {
	//	camera.pos.x = minPos.x;
	//} else if (camera.pos.x > maxPos.x) {
	//	camera.pos.x = maxPos.x;
	//}
	//if (camera.pos.y < minPos.y) {
	//	camera.pos.y = minPos.y;
	//} else if (camera.pos.y > maxPos.y) {
	//	camera.pos.y = maxPos.y;
	//}



}

#pragma endregion

#pragma region 関数: 背景

static void InitBackGround(void) {
	backGround = {};
	backGround.texture = GetTexture(TextureType::BackGround);
}

static void DrawBackGround(void) {
	DrawTextureBG(backGround.texture, backGround.pos, backGround.size, backGround.scrollRate);
}

#pragma endregion

#pragma region 関数: ラウンド関係

static void InitRound(void) {
	ResetTimeData(gameTime);
	nowRound = Round::Round1;
}

/// <summary>
/// ラウンドの変更処理
/// </summary>
static void ChangeRound(void) {
	//開始演出中はカウントしない
	if (nowModeStart != ModeStartEffect::Standby) return;

	//経過時間の更新
	CountTimeData(gameTime);

#pragma region ラウンド変更処理

	int nextRound = ToInt(nowRound) + 1;
	if (nextRound == ToInt(Round::Count)) return;

	//現在の時間の合計
	int totalGameTime = ConvertTimeDataToInt(gameTime);
	int totalNextRoundTime = ConvertTimeDataToInt(changeRoundTime[nextRound]);

	//変更時間を変えたら
	if (totalGameTime >= totalNextRoundTime) {
		nowRound = static_cast<Round>(nextRound);

		//ボスラウンドに入ったら
		if (nowRound == Round::RoundBoss) {
			//ボスの警告演出
			isWarningBoss = true;
			TriggerDangerDisplayEffect();
		}
	}

#pragma endregion

}

//カメラの外側周りのランダム位置を作成
Vector2 CreatePosOutCameraZone(void) {
	float camLeft = camera.pos.x - kWindowCenter.x;
	float camRight = camera.pos.x + kWindowCenter.x;
	float camTop = camera.pos.y + kWindowCenter.y;
	float camBottom = camera.pos.y - kWindowCenter.y;

	int camLeftI = ToInt(camLeft);
	int camRightI = ToInt(camRight);
	int camTopI = ToInt(camTop);
	int camBottomI = ToInt(camBottom);

	Vector2 resultPos = {};

	//どの外周ゾーンに出すか
	int zone = GetRand(0, 3);

	switch (zone) {
		//左端
	case 0:
		resultPos.x = camLeft - spawnPosOffset;
		resultPos.y = ToFloat(GetRand(camBottomI, camTopI));

		break;

		//右端
	case 1:
		resultPos.x = camRight + spawnPosOffset;
		resultPos.y = ToFloat(GetRand(camBottomI, camTopI));

		break;

		//上端
	case 2:
		resultPos.x = ToFloat(GetRand(camLeftI, camRightI));
		resultPos.y = camTop + spawnPosOffset;

		break;

		//下端
	case 3:
		resultPos.x = ToFloat(GetRand(camLeftI, camRightI));
		resultPos.y = camBottom - spawnPosOffset;

		break;
	}

	return resultPos;
}

/// <summary>
/// 敵の生成処理
/// </summary>
/// <param name=""></param>
static void SpawnEnemyByRound(void) {
	if (nowModeStart != ModeStartEffect::Standby) return;
	if (GetBossEnemyIsDead()) return;

	int round = ToInt(nowRound);
	for (int i = 0; i < kMaxSpawner; i++) {
		SpawnerData& data = spawners[round][i];

		if (data.type == EnemyType::None) break;

		data.timer.count++;
		if (data.timer.count >= data.timer.time) {
			data.timer.count = 0;

			Vector2 spawnPos;
			
			spawnPos = CreatePosOutCameraZone();
			SpawnEnemy(spawnPos, data.type);
		}
	}
}

#pragma endregion

#pragma region 関数: 演出関係

static void InitStartEffect(void) {
	nowModeStart = ModeStartEffect::WaitPlayerSpawn;
	waitPlayerTimer.count = 0;
}

static void UpdateStartEffect(void) {
	switch (nowModeStart) {
	case ModeStartEffect::WaitPlayerSpawn: {
		waitPlayerTimer.count++;
		if (waitPlayerTimer.count >= waitPlayerTimer.time) {
			waitPlayerTimer.count = 0;
			nowModeStart = ModeStartEffect::Ready;

			PlaySe(SeType::Ready);
		}

		break;
	}
	case ModeStartEffect::Ready: {
		readyTelopTimer.count++;
		if (readyTelopTimer.count >= readyTelopTimer.time) {
			readyTelopTimer.count = 0;
			nowModeStart = ModeStartEffect::Go;

			PlaySe(SeType::Go);
		}

		break;
	}
	case ModeStartEffect::Go: {
		goTelopTimer.count++;
		if (goTelopTimer.count >= goTelopTimer.time) {
			goTelopTimer.count = 0;
			nowModeStart = ModeStartEffect::Standby;

			//BGM再生
			//PlayBgm(BgmType::Stage);
		}

		break;
	}
	}

}

static void InitGoSceneTimer(void) {
	goToGameoverTimer.count = 0;
	goToClearTimer.count = 0;
}

static void GoGameoverScene(void) {
	//生存していたら、スキップ
	if (GetPlayerIsAlive()) return;
	////フェードアウトし始めたらスキップ
	//if (GetNowModeFade() != ModeFade::Standby) return;

	goToGameoverTimer.count++;
	if (goToGameoverTimer.count >= goToGameoverTimer.time) {
		goToGameoverTimer.count = 0;
		ChangeScene(Scene::Gameover);
	}
}

static void InitWarningBoss(void) {
	warnBossTimer.count = 0;
	isWarningBoss = false;
}

static void UpdateWarningBoss(void) {
	if (!isWarningBoss) return;

	warnBossTimer.count++;
	if (warnBossTimer.count >= warnBossTimer.time) {
		warnBossTimer.count = 0;
		isWarningBoss = false;

		//ボスの生成処理
		SpawnBoss({ 1200, 720 });
		CancelDangerDisplayEffect();
		CreateFlashPanel();
	}

}

static void GoClearScene(void) {
	/*if (nowRound != Round::RoundBoss) return;*/

	//生存していたらスキップ
	if (GetBossEnemyIsAlive() || !GetBossEnemyIsDead()) return;

	//フェードアウトし始めたらスキップ
	if (GetNowModeFade() != ModeFade::Standby) return;

	goToClearTimer.count++;
	if (goToClearTimer.count >= goToClearTimer.time) {
		ChangeModeFade(ModeFade::FadeOut);
		ChangeFadeTarget(FadeTarget::Clear);
	}
}

#pragma endregion

void InitStage(void) {
	ResetNowScore();

	InitCamera();
	InitBackGround();
	InitGoSceneTimer();

	InitRound();

	InitStartEffect();
	InitWarningBoss();
}

void UpdateStage(void) {
	//カメラ関係
	MoveCameraPos();
	ClampCameraPos();

	//ラウンド関係
	ChangeRound();
	SpawnEnemyByRound();

	//演出関係
	UpdateStartEffect();
	GoGameoverScene();
	GoClearScene();
	UpdateWarningBoss();
}

void DrawStage(void) {

	DrawBackGround();

#ifdef _DEBUG
	//Vector2 playerPos = GetPlayerPos();
	//Novice::ScreenPrintf(1000, 20, "PlayerPos:(%.2f, %.2f)", playerPos.x, playerPos.y);

	//Novice::ScreenPrintf(1000, 40, "CameraPos:(%.2f, %.2f)", camera.pos.x, camera.pos.y);

#endif // _DEBUG

}

#pragma region 関数: 外部参照関係

void GetMovablePos(Vector2 pos[2]) {
	pos[0] = movablePos[0];
	pos[1] = movablePos[1];
}

Vector2 GetCameraPos(void) {
	return camera.pos;
}

float GetCameraZoom(void) {
	return camera.zoom;
}

TimeData GetGameTimeData(void) {
	return gameTime;
}

TimeData GetBossRoundTimeData(void) {
	return changeRoundTime[ToInt(Round::Count) - 1];
}

ModeStartEffect GetNowModeStart(void) {
	return nowModeStart;
}

bool GetIsWarningBoss(void) {
	return isWarningBoss;
}

#pragma endregion