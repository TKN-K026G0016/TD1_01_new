#include "gameover.h"
#include "input.h"
#include "scene_manager.h"
#include "score_data.h"
#include "stage.h"
#include "se.h"

#include "tool.h"
#include "texture.h"
#include "common.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <algorithm>

enum class ModeGameover {
	//coverPanelがフェードインする処理
	PanelFadeIn,
	//アイコンが下に下がってくる
	IconFall,
	//スコアカウント処理
	CountUpScore,
	//項目選択状態
	Select
};
ModeGameover nowMode = ModeGameover::PanelFadeIn;

#pragma region データ: カバーパネル

struct CoverPanel {
	Vector2 pos = kWindowCenter;
	Vector2 size = { 1280, 720 };

	//<イージング関係>
	float easingT = 0.0f;
	float easingSpeed = 0.015f;
	EasingMode easingMode = EasingMode::EaseIn;

	int alpha = 0;
	int alphaMin = 0;
	int alphaMax = 200;

	unsigned int color = 0x3E0B21FF;

	Texture texture = {};
};
CoverPanel gameoverCoverPanel = {};

#pragma endregion

#pragma region データ: アイコン

struct GameoverIcon {
	Vector2 pos = {};
	Vector2 size = { 600, 150 };

	//イージング関係
	float easingT = 0.0f;
	float easingSpeed = 0.1f;
	EasingMode easingMode = EasingMode::EaseIn;

	Vector2 standbyPos = { 350, 900 };
	Vector2 showPos = { 350, 600 };

	Texture texture = {};
};
GameoverIcon gameoverIcon;

#pragma endregion

#pragma region データ: スコア表示

static constexpr int kScoreNumberLimit = 9;
struct ScoreNumber {
	Vector2 size = { 32, 64 };

	Vector2 startPos = { 220, 300 };
	Vector2 interval = { 32, 0 };

	int animNum = 0;
};
ScoreNumber scoreNumbers[kScoreNumberLimit];
Texture scoreNumberTexture = {};

struct ScoreManager {
	Vector2 pos = { 30, 30 };

	//表示の数値
	int showScore = 0;
	//獲得数値
	int score = 0;

	//イージング関係
	float easingT = 0.0f;
	float easingSpeed = 0.008f;
	EasingMode easingMode = EasingMode::EaseIn;
};
ScoreManager scoreManager;

#pragma endregion

#pragma region データ: 進捗度ゲージ

struct GameoverProgressGauge {
	Vector2 pos = { 350, 180 };
	Vector2 size = { 504, 23 };

	//ボスラウンドまでの時間
	int bossRoundTime = 0;
	int nowProgressTime = 0;
	float gaugeRate = 0;

	Texture frameTexture = {};
};
GameoverProgressGauge progressGauge;

struct GameoverProgressGaugeIcon {
	Vector2 pos = { 0.0f, progressGauge.pos.y };
	Vector2 size = { 32, 32 };

	Vector2 startPos = { progressGauge.pos.x - progressGauge.size.x / 2, progressGauge.pos.y };
	Vector2 endPos = { progressGauge.pos.x + progressGauge.size.x / 2, progressGauge.pos.y };

	Vector2 targetPos = {};

	Texture texture = {};
};
GameoverProgressGaugeIcon progressGaugeIcon;

#pragma endregion

#pragma region データ: ボタン

struct Icon {
	Vector2 pos = { 0, 0 };
	Vector2 size = { 256, 64 };

	Texture texture = {};
};
static Icon retryIcon;
static Vector2 retryIconOffset = { 430, -210};

static Icon exitIcon;
static Vector2 exitIconOffset = { 500, -280 };

struct GameoverMenuCursor {
	Vector2 pos = {};
	Vector2 size = { 64, 64 };

	Vector2 offset = { -160, 0 };

	float rotateTheta = 0.0f;
	float rotateSpeed = ToFloat(M_PI) / 30.0f;

	Texture texture = {};
};
GameoverMenuCursor gameoverMenuCursor;

#pragma endregion

#pragma region データ: MenuBackground

struct MenuBackground {
	Vector2 pos = {};
	Vector2 size = { 1280, 720 };

	//イージング関係
	float easingT = 0.0f;
	float easingSpeed = 0.1f;
	EasingMode easingModeFrameIn = EasingMode::EaseOut;
	EasingMode easingModeFrameOut = EasingMode::EaseOut;

	//座標
	Vector2 standbyPos = { 1300, kWindowCenter.y };
	Vector2 showPos = kWindowCenter;

	//透明度
	int alpha = 200;

	Texture texture = {};
};
MenuBackground gameoverMenuBackground;

#pragma endregion

#pragma region データ: 選択関係

enum class ModeGameoverSelect {
	Retry = 0,
	Exit = 1,

	Count
};
static ModeGameoverSelect nowSelect = ModeGameoverSelect::Retry;
//既に決定してるかのフラグ
static bool onceSelect = false;

#pragma endregion


#pragma region 関数: カバーパネル

static void InitCoverPanel(void) {
	gameoverCoverPanel = {};

	gameoverCoverPanel.texture = GetTexture(TextureType::White1x1);
}

static void UpdateCoverPanel(void) {
	if (gameoverCoverPanel.easingT < 1.0f) {
		gameoverCoverPanel.easingT += gameoverCoverPanel.easingSpeed;
		if (gameoverCoverPanel.easingT >= 1.0f) {
			gameoverCoverPanel.easingT = 1.0f;

			nowMode = ModeGameover::IconFall;
		}
	}

	gameoverCoverPanel.alpha = UpdateValueByEasing(gameoverCoverPanel.alphaMin, gameoverCoverPanel.alphaMax, gameoverCoverPanel.easingT, gameoverCoverPanel.easingMode);
}

static void DrawCoverPanel(void) {
	Texture texture = gameoverCoverPanel.texture;
	texture.color = (gameoverCoverPanel.color & 0xFFFFFF00) | gameoverCoverPanel.alpha;

	DrawTextureUI(texture, gameoverCoverPanel.pos, gameoverCoverPanel.size);
}

#pragma endregion

#pragma region 関数: アイコン落下

static void InitIcon(void) {
	gameoverIcon = {};
	gameoverIcon.pos = gameoverIcon.standbyPos;
	gameoverIcon.texture = GetTexture(TextureType::GameoverIcon);
}

static void UpdateIcon(void) {
	gameoverIcon.easingT += gameoverIcon.easingSpeed;
	if (gameoverIcon.easingT >= 1.0f) {
		gameoverIcon.easingT = 1.0f;
		nowMode = ModeGameover::CountUpScore;
	}

	gameoverIcon.pos = MoveByEasing(gameoverIcon.standbyPos, gameoverIcon.showPos, gameoverIcon.easingT, gameoverIcon.easingMode);
}

static void DrawIcon(void) {
	DrawTextureUI(gameoverIcon.texture, gameoverIcon.pos, gameoverIcon.size);
}

#pragma endregion

#pragma region 関数: スコアのカウント処理

static void InitScoreCount(void) {
	for (int i = 0; i < kScoreNumberLimit; i++) {
		scoreNumbers[i] = {};
	}

	scoreManager = {};

	scoreManager.score = GetNowScore();

	scoreNumberTexture = GetTexture(TextureType::ScoreNumbers);
}

static void UpdateScoreCount(void) {

	if (CheckInputAction(InputAction::Confirm)) {
		scoreManager.easingT = 1.0f;
	}

	//イージング処理
	scoreManager.easingT += scoreManager.easingSpeed;

	PlaySe(SeType::CountUpScore);

	if (scoreManager.easingT >= 1.0f) {
		scoreManager.easingT = 1.0f;
		nowMode = ModeGameover::Select;
	}

	//数値の更新処理
	scoreManager.showScore = UpdateValueByEasing(0, scoreManager.score, scoreManager.easingT, scoreManager.easingMode);


	for (int i = 0; i < kScoreNumberLimit; i++) {
		scoreNumbers[i].animNum = GetValueAtDigit(scoreManager.showScore, i);
	}
}

static void DrawScoreCount(void) {
	if (ToInt(nowMode) < ToInt(ModeGameover::CountUpScore)) return;

	for (int i = 0; i < kScoreNumberLimit; i++) {
		Vector2 pos = { scoreNumbers[i].startPos.x + scoreNumbers[i].interval.x * (kScoreNumberLimit - i - 1), scoreNumbers[i].startPos.y };

		Texture texture = scoreNumberTexture;
		texture.animNum = scoreNumbers[i].animNum;

		DrawTextureUI(texture, pos, scoreNumbers[i].size);
	}
}

#pragma endregion

#pragma region 関数: 進捗度ゲージ

static void InitProgressGauge(void) {

	progressGauge.frameTexture = GetTexture(TextureType::ProgressGauge);
	progressGaugeIcon.texture = GetTexture(TextureType::ProgressGaugeIcon);

	progressGauge.bossRoundTime = ConvertTimeDataToInt(GetBossRoundTimeData());
	progressGauge.nowProgressTime = ConvertTimeDataToInt(GetGameTimeData());


	//進行具合を表示
	progressGauge.gaugeRate = ToFloat(progressGauge.nowProgressTime) / ToFloat(progressGauge.bossRoundTime);
	progressGauge.gaugeRate = std::clamp(progressGauge.gaugeRate, 0.0f, 1.0f);
	//iconの目的地を設定
	progressGaugeIcon.targetPos.x = progressGaugeIcon.startPos.x + progressGauge.gaugeRate * (progressGaugeIcon.endPos.x - progressGaugeIcon.startPos.x);
}

static void UpdateProgressGauge(void) {
	//アイコンの移動
	progressGaugeIcon.pos.x = UpdateValueByEasing(progressGaugeIcon.startPos.x, progressGaugeIcon.targetPos.x, scoreManager.easingT, scoreManager.easingMode);
}

static void DrawProgressGauge(void) {
	if (ToInt(nowMode) < ToInt(ModeGameover::CountUpScore)) return;

	DrawTextureUI(progressGauge.frameTexture, progressGauge.pos, progressGauge.size);
	DrawTextureUI(progressGaugeIcon.texture, progressGaugeIcon.pos, progressGaugeIcon.size);
}

#pragma endregion

#pragma region 関数: MenuBackground

static void InitMenuBackground(void) {
	gameoverMenuBackground = {};
	gameoverMenuBackground.pos = gameoverMenuBackground.standbyPos;
	gameoverMenuBackground.texture = GetTexture(TextureType::MenuBackground);
}

static void UpdateMenuBackground(void) {
	//Pause入ってからの処理
	if (!onceSelect) {
		if (gameoverMenuBackground.easingT < 1.0f) {
			gameoverMenuBackground.easingT += gameoverMenuBackground.easingSpeed;
			if (gameoverMenuBackground.easingT > 1.0f) {
				gameoverMenuBackground.easingT = 1.0f;
			}
		}

		gameoverMenuBackground.pos = MoveByEasing(gameoverMenuBackground.standbyPos, gameoverMenuBackground.showPos, gameoverMenuBackground.easingT, gameoverMenuBackground.easingModeFrameIn);
	}
	//選択後の処理
	else {
		if (gameoverMenuBackground.easingT > 0.0f) {
			gameoverMenuBackground.easingT -= gameoverMenuBackground.easingSpeed;
			if (gameoverMenuBackground.easingT < 0.0f) {
				gameoverMenuBackground.easingT = 0.0f;
			}
		}

		gameoverMenuBackground.pos = MoveByEasing(gameoverMenuBackground.standbyPos, gameoverMenuBackground.showPos, gameoverMenuBackground.easingT, gameoverMenuBackground.easingModeFrameOut);
	}
}

static void DrawMenuBackground(void) {
	Texture texture = gameoverMenuBackground.texture;
	texture.color = (texture.color & 0xFFFFFF00) | gameoverMenuBackground.alpha;

	DrawTextureUI(texture, gameoverMenuBackground.pos, gameoverMenuBackground.size);
}

#pragma endregion

#pragma region 関数: 選択関係

static void InitSelect(void) {
	nowSelect = ModeGameoverSelect::Retry;
	onceSelect = false;
}


/// <summary>
/// カーソルの移動処理
/// </summary>
static void ChangeCursor(void) {
	if (onceSelect) return;

	int selectNum = ToInt(nowSelect);

	if (CheckInputAction(InputAction::SelectUp)) {
		selectNum--;

		//一番上まで来ていたら、一番下へ
		if (selectNum < 0) {
			selectNum = ToInt(ModeGameoverSelect::Count) - 1;
		}

		PlaySe(SeType::MoveCursor);

	} else if (CheckInputAction(InputAction::SelectDown)) {
		selectNum++;

		//一番下まで来ていたら、一番上へ
		if (selectNum >= ToInt(ModeGameoverSelect::Count)) {
			selectNum = 0;
		}

		PlaySe(SeType::MoveCursor);
	}

	//変更を適用
	nowSelect = static_cast<ModeGameoverSelect>(selectNum);
}

/// <summary>
/// 決定処理
/// </summary>
static void Confirm(void) {
	if (onceSelect) return;

	if (CheckInputAction(InputAction::Confirm)) {
		onceSelect = true;

		PlaySe(SeType::Confirm);
	}
}

static void ChangeScene(void) {
	if (onceSelect && gameoverMenuBackground.easingT <= 0.0f) {
		switch (nowSelect) {
		case ModeGameoverSelect::Retry: {
			ChangeModeFade(ModeFade::FadeOut);
			ChangeFadeTarget(FadeTarget::Play);

			break;
		}
		case ModeGameoverSelect::Exit: {
			ChangeModeFade(ModeFade::FadeOut);
			ChangeFadeTarget(FadeTarget::Title);

			break;
		}
		}
	}
}

#pragma endregion

#pragma region 関数: アイコン関係

static void InitButton(void) {
	if (retryIcon.texture.tHandle != -1) return;

	retryIcon.texture = GetTexture(TextureType::RetryIcon);

	exitIcon.texture = GetTexture(TextureType::ExitIcon);

	gameoverMenuCursor.texture = GetTexture(TextureType::MenuCursor);
}

static void UpdateCursor(void) {

	Vector2 targetPos = {};
	switch (nowSelect) {

	case ModeGameoverSelect::Retry:
		targetPos = { retryIcon.pos.x + gameoverMenuCursor.offset.x,retryIcon.pos.y + gameoverMenuCursor.offset.y };
		break;

	case ModeGameoverSelect::Exit:
		targetPos = { exitIcon.pos.x + gameoverMenuCursor.offset.x,exitIcon.pos.y + gameoverMenuCursor.offset.y };
		break;
	}

	gameoverMenuCursor.pos = targetPos;

	//回転処理
	gameoverMenuCursor.rotateTheta -= gameoverMenuCursor.rotateSpeed;

}

static void UpdateButton(void) {

	retryIcon.pos = { gameoverMenuBackground.pos.x + retryIconOffset.x, gameoverMenuBackground.pos.y + retryIconOffset.y };
	exitIcon.pos = { gameoverMenuBackground.pos.x + exitIconOffset.x, gameoverMenuBackground.pos.y + exitIconOffset.y };

#pragma region 点灯処理
	//RetryIcon
	if (nowSelect == ModeGameoverSelect::Retry) {
		retryIcon.texture.animNum = 1;
	} else {
		retryIcon.texture.animNum = 0;
	}

	//exitIcon
	if (nowSelect == ModeGameoverSelect::Exit) {
		exitIcon.texture.animNum = 1;
	} else {
		exitIcon.texture.animNum = 0;
	}
#pragma endregion
}

static void DrawButton(void) {
	DrawTextureUI(retryIcon.texture, retryIcon.pos, retryIcon.size);

	DrawTextureUI(exitIcon.texture, exitIcon.pos, exitIcon.size);

	DrawTextureRotateUI(gameoverMenuCursor.texture, gameoverMenuCursor.pos, gameoverMenuCursor.size, gameoverMenuCursor.rotateTheta);
}

#pragma endregion



void InitGameover(void) {
	nowMode = ModeGameover::PanelFadeIn;

	PlaySe(SeType::GameoverJingle);

	InitButton();
	InitCoverPanel();
	InitIcon();
	InitScoreCount();
	InitProgressGauge();
	InitMenuBackground();
	InitSelect();
}

void UpdateGameover(void) {

	switch (nowMode) {
	case ModeGameover::PanelFadeIn:
		UpdateCoverPanel();
		break;
	case ModeGameover::IconFall:
		UpdateIcon();
		break;
	case ModeGameover::CountUpScore:
		UpdateScoreCount();
		UpdateProgressGauge();
		break;
	case ModeGameover::Select:
		UpdateMenuBackground();

		ChangeCursor();
		Confirm();
		ChangeScene();


		break;
	}

	UpdateButton();
	UpdateCursor();
}

void DrawGameover(void) {
	DrawCoverPanel();

	DrawMenuBackground();

	DrawScoreCount();
	DrawProgressGauge();

	DrawIcon();
	DrawButton();
}