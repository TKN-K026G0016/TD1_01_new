#include "reset_score.h"
#include "scene_manager.h"
#include "input.h"
#include "se.h"
#include "score_data.h"
#include "effect.h"

#include "common.h"
#include "texture.h"
#include "tool.h"

#define _USE_MATH_DEFINES
#include <math.h>

static bool onceSelect = false;

//現在の状態
enum class ModeResetScore {
	Select,
	AfterReset,
};
ModeResetScore nowMode = ModeResetScore::Select;

//現在の選択状態
enum class ModeResetScoreSelect {
	Confirm,
	Cancel,

	Count
};
ModeResetScoreSelect nowSelect = ModeResetScoreSelect::Cancel;

#pragma region データ: カバーパネル

struct ResetScoreCoverPanel {
	Vector2 pos = kWindowCenter;
	Vector2 size = { 1280, 720 };

	unsigned int color = 0x00000DD;

	Texture texture = {};
};
ResetScoreCoverPanel coverPanel = {};

#pragma endregion

#pragma region データ: MenuBackground

struct ResetScoreMenuBackground {
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
ResetScoreMenuBackground menuBackground;

#pragma endregion

#pragma region データ: アイコン関係

struct Icon {
	Vector2 pos = { 0, 0 };
	Vector2 size = { 0, 0 };

	Texture texture = {};
};
//ポーズアイコン
Icon resetIcon;
static Vector2 resetIconSize = { 600, 256 };
static Vector2 iconStandbyPos = { -200, 560 };
static Vector2 iconViewPos = { 400, 560 };

//セレクトアイコンのサイズ
static Vector2 selectIconSize = { 256, 64 };

static Icon confirmIcon;
static Vector2 confirmIconOffset = { 430, -210 };
static Icon exitIcon;
static Vector2 exitIconOffset = { 500, -280 };

struct MenuCursor {
	Vector2 pos = {};
	Vector2 size = { 64, 64 };

	Vector2 offset = { -160, 0 };

	float rotateTheta = 0.0f;
	float rotateSpeed = ToFloat(M_PI) / 30.0f;

	Texture texture = {};
};
static MenuCursor menuCursor;

#pragma endregion

#pragma region データ: スコア

static constexpr int kScoreNumberLimit = 9;
struct ScoreNumber {
	Vector2 size = { 40, 80 };

	Vector2 startPosOffset = { 0, 0 };
	Vector2 interval = { 40, 0 };

	int animNum = 0;
};
static ScoreNumber scoreNumbers[kScoreNumberLimit];
static Texture scoreNumberTexture = {};

struct ScoreManager {
	Vector2 pos = { 30, 30 };

	Vector2 standbyPos = { -500, 270 };
	Vector2 showPos = { 300, 270 };

	int showScore = 0;

	int exploseLimit = 8;
	Vector2 exploseRange = { 400, 30 };

};
static ScoreManager scoreManager;

#pragma endregion

#pragma region 関数: 選択関係

static void InitSelect(void) {
	nowMode = ModeResetScore::Select;
	nowSelect = ModeResetScoreSelect::Cancel;

	onceSelect = false;
}

static void MoveCursor(void) {
	//項目選択状態時のみ
	if (nowMode != ModeResetScore::Select) return;
	if (onceSelect) return;


	int selectNum = ToInt(nowSelect);

	if (CheckInputAction(InputAction::SelectUp)) {
		selectNum--;

		//一番上まで来ていたら、一番下へ
		if (selectNum < 0) {
			selectNum = ToInt(ModeResetScoreSelect::Count) - 1;
		}

		PlaySe(SeType::MoveCursor);


	} else if (CheckInputAction(InputAction::SelectDown)) {
		selectNum++;

		//一番下まで来ていたら、一番上へ
		if (selectNum >= ToInt(ModeResetScoreSelect::Count)) {
			selectNum = 0;
		}

		PlaySe(SeType::MoveCursor);
	}

	//変更を適用
	nowSelect = static_cast<ModeResetScoreSelect>(selectNum);

}

static void Confirm(void) {
	if (!CheckInputAction(InputAction::Confirm)) return;

	if (nowMode == ModeResetScore::Select) {
		if (nowSelect == ModeResetScoreSelect::Confirm) {
			nowMode = ModeResetScore::AfterReset;

			//初期化処理
			ResetHighScore();

			//爆発演出
			for (int i = 0; i < scoreManager.exploseLimit; i++) {
				Vector2 createPos;
				createPos.x = scoreManager.pos.x + ToFloat(GetRand(0, ToInt(scoreManager.exploseRange.x)));
				createPos.y = scoreManager.pos.y + ToFloat(GetRand(-ToInt(scoreManager.exploseRange.y), ToInt(scoreManager.exploseRange.y)));

				CreateExplodeEffect(createPos, WHITE);
			}


		} else {
			onceSelect = true;
		}
	} 
	else {
		onceSelect = true;
	}

	PlaySe(SeType::Confirm);

}

#pragma endregion

#pragma region 関数: カバーパネル

static void InitCoverPanel(void) {
	//テクスチャデータの取得
	if (coverPanel.texture.tHandle != -1) return;

	coverPanel.texture = GetTexture(TextureType::White1x1);
	coverPanel.texture.color = coverPanel.color;
}

static void DrawCoverPanel(void) {
	DrawTextureUI(coverPanel.texture, coverPanel.pos, coverPanel.size);
}

#pragma endregion

#pragma region 関数: MenuBackground

static void InitMenuBackground(void) {

	menuBackground = {};
	menuBackground.texture = GetTexture(TextureType::MenuBackground);
}

static void UpdateMenuBackground(void) {
	//Pause入ってからの処理
	if (!onceSelect) {
		if (menuBackground.easingT < 1.0f) {
			menuBackground.easingT += menuBackground.easingSpeed;
			if (menuBackground.easingT > 1.0f) {
				menuBackground.easingT = 1.0f;
			}
		}

		menuBackground.pos = MoveByEasing(menuBackground.standbyPos, menuBackground.showPos, menuBackground.easingT, menuBackground.easingModeFrameIn);
	}
	//選択後の処理
	else if(onceSelect) {
		if (menuBackground.easingT > 0.0f) {
			menuBackground.easingT -= menuBackground.easingSpeed;
			if (menuBackground.easingT < 0.0f) {
				menuBackground.easingT = 0.0f;

				ChangeSceneImmediate(Scene::Title);
			}
		}

		menuBackground.pos = MoveByEasing(menuBackground.standbyPos, menuBackground.showPos, menuBackground.easingT, menuBackground.easingModeFrameOut);
	}
}

static void DrawMenuBackground(void) {
	Texture texture = menuBackground.texture;
	texture.color = (texture.color & 0xFFFFFF00) | menuBackground.alpha;

	DrawTextureUI(texture, menuBackground.pos, menuBackground.size);
}

#pragma endregion

#pragma region 関数: アイコン

/// <summary>
/// アイコンの初期化処理
/// </summary>
static void InitIcon(void) {
	if (resetIcon.texture.tHandle != -1) return;

	resetIcon.size = resetIconSize;
	resetIcon.texture = GetTexture(TextureType::ResetIcon);


	confirmIcon.size = selectIconSize;
	confirmIcon.texture = GetTexture(TextureType::ResetConfirmIcon);

	exitIcon.size = selectIconSize;
	exitIcon.texture = GetTexture(TextureType::CreditsExitIcon);

	menuCursor.texture = GetTexture(TextureType::MenuCursor);
}

static void UpdateIcon(void) {

	//PauseIconの処理
	resetIcon.pos = MoveByEasing(iconStandbyPos, iconViewPos, menuBackground.easingT, EasingMode::EaseOut);

	//confirmIconの処理
	confirmIcon.pos = { menuBackground.pos.x + confirmIconOffset.x, menuBackground.pos.y + confirmIconOffset.y };
	//exitIconの処理
	exitIcon.pos = { menuBackground.pos.x + exitIconOffset.x, menuBackground.pos.y + exitIconOffset.y };

#pragma region 点灯処理

	//confirm
	if (nowSelect == ModeResetScoreSelect::Confirm) {
		confirmIcon.texture.animNum = 1;
	} else {
		confirmIcon.texture.animNum = 0;
	}

	//exit
	if (nowSelect == ModeResetScoreSelect::Cancel || nowMode == ModeResetScore::AfterReset) {
		exitIcon.texture.animNum = 1;
	} else {
		exitIcon.texture.animNum = 0;
	}

#pragma endregion

	if (nowMode == ModeResetScore::Select) {
		resetIcon.texture.animNum = 0;
	} else {
		resetIcon.texture.animNum = 1;
	}

}

static void UpdateCursor(void) {

	//リセットしたら、モドルアイコンに移る
	if (nowMode == ModeResetScore::AfterReset) {
		menuCursor.pos = { exitIcon.pos.x + menuCursor.offset.x,exitIcon.pos.y + menuCursor.offset.y };
	} else {

		Vector2 targetPos = {};
		switch (nowSelect) {

		case ModeResetScoreSelect::Confirm:
			targetPos = { confirmIcon.pos.x + menuCursor.offset.x,confirmIcon.pos.y + menuCursor.offset.y };
			break;

		case ModeResetScoreSelect::Cancel:
			targetPos = { exitIcon.pos.x + menuCursor.offset.x,exitIcon.pos.y + menuCursor.offset.y };
			break;
		}

		menuCursor.pos = targetPos;
	}

	//回転処理
	menuCursor.rotateTheta -= menuCursor.rotateSpeed;

}

static void DrawIcon(void) {
	Texture texture = resetIcon.texture;

	//PauseIcon
	DrawTextureUI(texture, resetIcon.pos, resetIcon.size);

	if (nowMode != ModeResetScore::AfterReset) {
		//confirmIcon
		DrawTextureUI(confirmIcon.texture, confirmIcon.pos, confirmIcon.size);
	}

	//exitIcon
	DrawTextureUI(exitIcon.texture, exitIcon.pos, exitIcon.size);
}

static void DrawCursor(void) {
	DrawTextureRotateUI(menuCursor.texture, menuCursor.pos, menuCursor.size, menuCursor.rotateTheta);
}

#pragma endregion

#pragma region 関数: スコア

static void InitScoreCount(void) {
	for (int i = 0; i < kScoreNumberLimit; i++) {
		scoreNumbers[i] = {};
	}

	scoreManager = {};

	scoreNumberTexture = GetTexture(TextureType::ScoreNumbers);
}

static void UpdateScoreCount(void) {

#pragma region アイコンの移動

	scoreManager.pos = MoveByEasing(scoreManager.standbyPos, scoreManager.showPos, menuBackground.easingT, EasingMode::EaseOut);

#pragma endregion

	scoreManager.showScore = GetHighScore();

	for (int i = 0; i < kScoreNumberLimit; i++) {
		scoreNumbers[i].animNum = GetValueAtDigit(scoreManager.showScore, i);
	}
}

static void DrawScoreCount(void) {

	for (int i = 0; i < kScoreNumberLimit; i++) {
		Vector2 pos = {  scoreManager.pos.x + scoreNumbers[i].startPosOffset.x + scoreNumbers[i].interval.x * (kScoreNumberLimit - i - 1), scoreManager.pos.y + scoreNumbers[i].startPosOffset.y };

		Texture texture = scoreNumberTexture;
		texture.animNum = scoreNumbers[i].animNum;

		DrawTextureUI(texture, pos, scoreNumbers[i].size);
	}
}

#pragma endregion

void InitResetScore(void) {
	InitSelect();

	InitCoverPanel();
	InitMenuBackground();

	InitIcon();
	InitScoreCount();
	InitEffect();
}

void UpdateResetScore(void) {
	MoveCursor();
	Confirm();

	UpdateMenuBackground();
	UpdateIcon();
	UpdateCursor();
	UpdateScoreCount();
	UpdateEffect();
}

void DrawResetScore(void) {
	DrawCoverPanel();
	DrawMenuBackground();

	DrawIcon();
	DrawCursor();
	DrawScoreCount();
	DrawEffect();
}