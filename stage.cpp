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

	ModeCameraZoom nowModeZoom = ModeCameraZoom::Standby;

	float zoom = 0.5f;
	float zoomMax = 1.0f;
	float zoomMin = 0.6f;

	float zoomEasingT = 0.0f;
	float zoomOutEasingSpeed = 0.01f;
	float zoomInEasingSpeed = 0.02f;
	EasingMode zoomEasingMode = EasingMode::EaseOutBack;

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
		/*{EnemyType::EnemyBomb,{300,300}},*/
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

//背景本体
struct BackGround {
	Vector2 pos = { 1280, 720 };
	Vector2 size = { 2960, 1840 };

	float scrollRate = 1.0f;

	//現在の状態
	ModeBackground nowMode = ModeBackground::Standby;

	float easingT = 0.0f;
	EasingMode easingMode = EasingMode::EaseInOut;
	bool isFadeIn = true;

	float easingSpeed100 = 0.06f;
	float easingSpeed500 = 0.04f;
	float easingSpeed1000 = 0.02f;

	unsigned int nowColor = 0x240912FF;
	unsigned int normalColor = 0x240912FF;

	unsigned int combo100Color = 0x241548FF;
	unsigned int combo500Color = 0x485793FF;
	unsigned int combo1000Color = 0x70AFCBFF;

	Texture texture = {};
	Texture lineTexture = {};
};
BackGround backGround;


//走査線
static constexpr int kScanLineLimit = 21;

struct ScanLine {
	Vector2 pos = {};
	Vector2 size = { 1280, 2 };

	float moveSpeed = 0.5f;
	float posYMin = -0.2f;
	float posYMax = 722;

	float intervalY = 36.0f;

	int shakeDuration = 1;

	unsigned int color = 0x340314AA;

};
static ScanLine scanLine[kScanLineLimit];
static Texture scanLineTexture = {};

#pragma endregion

#pragma region 演出関係

//現在の開始演出処理
ModeStartEffect nowModeStart = ModeStartEffect::WaitPlayerSpawn;

//開始演出
Timer waitPlayerTimer = { 60, 0 };
//Readyの表示される時間
Timer readyTelopTimer = { 80, 0 };
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

	float halfWidth = kWindowCenter.x / camera.zoom;
	float halfHeight = kWindowCenter.y / camera.zoom;

	float minX = movablePos[0].x - cameraMovablePosOffset + halfWidth;
	float maxX = movablePos[1].x + cameraMovablePosOffset - halfWidth;

	float minY = movablePos[0].y - cameraMovablePosOffset + halfHeight;
	float maxY = movablePos[1].y + cameraMovablePosOffset - halfHeight;

	camera.pos.x = std::clamp(camera.pos.x, minX, maxX);
	camera.pos.y = std::clamp(camera.pos.y, minY, maxY);

}

static void MoveCameraZoom(void) {
	switch (camera.nowModeZoom) {
	case ModeCameraZoom::ZoomOut: {
		camera.zoomEasingT += camera.zoomOutEasingSpeed;
		if (camera.zoomEasingT >= 1.0f) {
			camera.zoomEasingT = 1.0f;
			camera.nowModeZoom = ModeCameraZoom::Standby;
		}

		break;
	}

	case ModeCameraZoom::ZoomIn: {
		camera.zoomEasingT -= camera.zoomInEasingSpeed;
		if (camera.zoomEasingT <= 0.0f) {
			camera.zoomEasingT = 0.0f;
			camera.nowModeZoom = ModeCameraZoom::Standby;
		}

		break;
	}

	}

	camera.zoom = UpdateValueByEasing(camera.zoomMax, camera.zoomMin, camera.zoomEasingT, camera.zoomEasingMode);
}

#pragma endregion

#pragma region 関数: 背景

static void InitBackGround(void) {
	backGround = {};
	backGround.texture = GetTexture(TextureType::BackGround);
	backGround.lineTexture = GetTexture(TextureType::StageLine);

	//走査線
	for (int i = 0; i < kScanLineLimit; i++) {
		scanLine[i] = {};
		scanLine[i].pos = { 640, scanLine[i].intervalY * i };
	}
	scanLineTexture = GetTexture(TextureType::TitleLine);
}

static void UpdateBackGround(void) {
	if (backGround.nowMode == ModeBackground::Standby) return;

	float easingSpeed;
	if (backGround.nowMode == ModeBackground::Combo100) {
		easingSpeed = backGround.easingSpeed100;
	} else if (backGround.nowMode == ModeBackground::Combo500) {
		easingSpeed = backGround.easingSpeed500;
	} else {
		easingSpeed = backGround.easingSpeed1000;
	}

	//イージング処理
	if (backGround.isFadeIn) {
		backGround.easingT += easingSpeed;
		if (backGround.easingT >= 1.0f) {
			backGround.easingT = 1.0f;
			backGround.isFadeIn = false;
		}
	} else {
		backGround.easingT -= easingSpeed;
		if (backGround.easingT <= 0.0f) {
			backGround.easingT = 0.0f;
			backGround.isFadeIn = true;

			backGround.nowMode = ModeBackground::Standby;
		}
	}

	switch (backGround.nowMode) {
	case ModeBackground::Combo100: {

		//色の変更
		float easedT = CreateEasedT(backGround.easingT, backGround.easingMode);

		backGround.nowColor = UpdateColorByEasing(backGround.normalColor, backGround.combo100Color, easedT);

		break;
	}
	case ModeBackground::Combo500: {

		//色の変更
		float easedT = CreateEasedT(backGround.easingT, backGround.easingMode);

		backGround.nowColor = UpdateColorByEasing(backGround.normalColor, backGround.combo500Color, easedT);

		break;
	}
	case ModeBackground::Combo1000: {

		//色の変更
		float easedT = CreateEasedT(backGround.easingT, backGround.easingMode);

		backGround.nowColor = UpdateColorByEasing(backGround.normalColor, backGround.combo1000Color, easedT);

		break;
	}
	}
}

static void UpdateScanLine(void) {
	for (int i = 0; i < kScanLineLimit; i++) {
		scanLine[i].pos.y += scanLine[i].moveSpeed;

		if (scanLine[i].pos.y >= scanLine[i].posYMax) {
			scanLine[i].pos.y = scanLine[i].posYMin;
		}
	}
}

static void DrawScanLine(void) {
	for (int i = 0; i < kScanLineLimit; i++) {
		Vector2 pos;
		pos.y = scanLine[i].pos.y + ToFloat(GetRand(-scanLine[i].shakeDuration, scanLine[i].shakeDuration));

		Texture texture = scanLineTexture;
		texture.color = scanLine[i].color;

		DrawTextureUI(texture, scanLine[i].pos, scanLine[i].size);
	}
}

static void DrawBackGround(void) {
	Texture texture = backGround.texture;
	texture.color = backGround.nowColor;

	DrawTextureBG(texture, backGround.pos, backGround.size, backGround.scrollRate);
	DrawTextureBG(backGround.lineTexture, backGround.pos, backGround.size, backGround.scrollRate);
	DrawScanLine();
}

void TriggerBackgroundFlash(ModeBackground mode) {
	backGround.nowMode = mode;
	backGround.isFadeIn = true;
	backGround.easingT = 0.0f;
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

			//Bgmの変更
			ChangeModeBgmFade(ModeBgmFade::FadeOut);
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
			if (data.type == EnemyType::EnemyM) {
				int getrand = GetRand(0, 3);;
				//左上
				if (getrand == 0) {
					spawnPos = { ToFloat(GetRand(50,150)),ToFloat(GetRand(50,150)) };
					SpawnEnemy(spawnPos, data.type);
				}
				//左下
				else if (getrand == 1) {
					spawnPos = { ToFloat(GetRand(50,150)),ToFloat(GetRand(1290,1390)) };
					SpawnEnemy(spawnPos, data.type);
				}
				//右上 
				else if (getrand == 2) {
					spawnPos = { ToFloat(GetRand(2410,2510)),ToFloat(GetRand(50,150)) };
					SpawnEnemy(spawnPos, data.type);
				}
				//右下
				else if (getrand == 3) {
					spawnPos = { ToFloat(GetRand(2410,2510)),ToFloat(GetRand(1290,1390)) };
				SpawnEnemy(spawnPos, data.type);
				}
			}
			else {
				spawnPos = CreatePosOutCameraZone();
				SpawnEnemy(spawnPos, data.type);
			}
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
			TriggerStartIconFrameIn();
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

			//Bgm再生
			PlayBgm(BgmType::Stage);
			TriggerStartIconFrameOut();

			//PauseGuideの出現
			TriggerPauseGuideFrameIn();
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

 		PlayBgm(BgmType::Boss);
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
	MoveCameraZoom();

	//背景関係
	UpdateBackGround();

	//ラウンド関係
	ChangeRound();
	SpawnEnemyByRound();

	//演出関係
	UpdateStartEffect();
	GoGameoverScene();
	GoClearScene();
	UpdateWarningBoss();

	UpdateScanLine();
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

void ChangeModeCameraZoom(ModeCameraZoom nextMode) {
	camera.nowModeZoom = nextMode;

	switch (nextMode) {
	case ModeCameraZoom::ZoomIn:
		camera.zoom = camera.zoomMax;
		camera.zoomEasingT = 1.0f;

		break;

	case ModeCameraZoom::ZoomOut:
		camera.zoom = camera.zoomMin;
		camera.zoomEasingT = 0.0f;

		break;
	}
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