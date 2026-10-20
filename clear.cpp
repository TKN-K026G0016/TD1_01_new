#include "clear.h"
#include "input.h"
#include "scene_manager.h"
#include "score_data.h"
#include "elec_bullet.h"
#include "title_background.h"
#include "effect.h"
#include "se.h"

#include "tool.h"
#include "texture.h"
#include "common.h"

#define _USE_MATH_DEFINES
#include <math.h>

enum class ModeClear {
	IconFrameIn,
	CountUpScore,
	CountUpCombo,
	CheckBreakScore,
	Select,
};
ModeClear nowMode = ModeClear::IconFrameIn;

#pragma region データ: ステージクリアアイコン

struct ClearIcon {
	Vector2 pos = {};
	Vector2 size = { 600, 150 };

	//イージング関係
	float easingT = 0.0f;
	float easingSpeed = 0.1f;
	EasingMode easingMode = EasingMode::EaseOut;

	Vector2 standbyPos = { 350, 900 };
	Vector2 showPos = { 350, 600 };

	Texture texture = {};
};
ClearIcon clearIcon;

#pragma endregion

#pragma region データ: スコアなど

static constexpr int kScoreNumberLimit = 9;
static constexpr int kComboNumberLimit = 4;
struct ScoreNumber {
	Vector2 size = { 32, 64 };

	Vector2 startPos = {};
	Vector2 interval = { 32, 0 };

	float offsetY = -5;

	int animNum = 0;
};
//現在のスコア
ScoreNumber nowScoreNumbers[kScoreNumberLimit];
float nowScoreStartPosOffset = 320;

//Vector2 nowScoreStartPos = { 330, 300 };
//最大コンボ
ScoreNumber maxComboNumbers[kComboNumberLimit];
float maxComboStartPosOffset = 210;
//Vector2 maxComboStartPos = { 330, 240 };
//ハイスコア
ScoreNumber bestScoreNumbers[kScoreNumberLimit];
float bestScoreStartPosOffset = 250;
//Vector2 bestScoreStartPos = { 330, 120 };

Texture clearScoreNumbersTexture = {};

struct ClearScoreManager {
	Vector2 pos = { 30, 30 };

	//表示の数値
	int showScore = 0;
	//獲得数値
	int nowScore = 0;

	//表示するコンボ数
	int showMaxCombo = 0;
	int maxCombo = 0;

	//ベストスコア
	int bestScore = 0;

	//イージング関係
	float easingTScore = 0.0f;
	float easingTCombo = 0.0f;
	float easingSpeed = 0.01f;
	EasingMode easingMode = EasingMode::EaseIn;

	//スコア更新フラグ
	bool breakScore = false;
	//スコア更新時の待機時間
	Timer waitBreakScoreTimer = { 200, 0 };
};
ClearScoreManager scoreManager;

struct ScoreValueIcon {
	Vector2 pos = {};
	Vector2 size = { 512, 64 };

	float standbyPosX = -900;
	float showPosX =  250 ;

	Texture texture = {};
};
ScoreValueIcon scoreIcon;
float scoreIconShowPosX = 110;
float scoreIconPosY = 300;

ScoreValueIcon maxComboIcon;
float maxComboIconShowPosX = 220;
float maxComboIconPosY = 220;

ScoreValueIcon bestScoreIcon;
float bestScoreIconShowPosX = 180;
float bestScoreIconPosY = 110;

#pragma endregion

#pragma region データ: 選択関係

enum class ModeClearSelect {
	Retry = 0,
	Exit = 1,

	Count
};
static ModeClearSelect nowSelect = ModeClearSelect::Retry;
//既に決定してるかのフラグ
static bool onceSelect = false;

#pragma endregion

#pragma region データ: MenuBackground

struct ClearMenuBackground {
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
ClearMenuBackground menuBackground;

#pragma endregion

#pragma region データ: ボタン

struct Icon {
	Vector2 pos = { 0, 0 };
	Vector2 size = { 256, 64 };

	Texture texture = {};
};
static Icon retryIcon;
static Vector2 retryIconOffset = { 430, -210 };

static Icon exitIcon;
static Vector2 exitIconOffset = { 500, -280 };

struct ClearMenuCursor {
	Vector2 pos = {};
	Vector2 size = { 64, 64 };

	Vector2 offset = { -160, 0 };

	float rotateTheta = 0.0f;
	float rotateSpeed = ToFloat(M_PI) / 30.0f;

	Texture texture = {};
};
ClearMenuCursor clearMenuCursor;

#pragma endregion


#pragma region 関数: アイコン落下

static void InitIcon(void) {
	clearIcon = {};
	clearIcon.pos = clearIcon.standbyPos;
	clearIcon.texture = GetTexture(TextureType::ClearIcon);
}

static void UpdateIcon(void) {
	if (CheckInputAction(InputAction::Confirm)) {
		clearIcon.easingT = 1.0f;
	}

	clearIcon.easingT += clearIcon.easingSpeed;
	if (clearIcon.easingT >= 1.0f) {
		clearIcon.easingT = 1.0f;
		nowMode = ModeClear::CountUpScore;
	}

	clearIcon.pos = MoveByEasing(clearIcon.standbyPos, clearIcon.showPos, clearIcon.easingT, clearIcon.easingMode);
}

static void DrawIcon(void) {
	DrawTextureUI(clearIcon.texture, clearIcon.pos, clearIcon.size);
}

#pragma endregion

#pragma region 関数: スコア関係

static void InitScore(void) {

	//データ初期化
	for (int i = 0; i < kScoreNumberLimit; i++) {
		nowScoreNumbers[i] = {};
		//nowScoreNumbers[i].startPos = nowScoreStartPos;

		bestScoreNumbers[i] = {};
		//bestScoreNumbers[i].startPos = bestScoreStartPos;
	}
	for (int i = 0; i < kComboNumberLimit; i++) {
		maxComboNumbers[i] = {};
		//maxComboNumbers[i].startPos = maxComboStartPos;
	}
	clearScoreNumbersTexture = GetTexture(TextureType::ScoreNumbers);

	//アイコンのテクスチャ取得
	scoreIcon.texture = GetTexture(TextureType::ClearScoreIcon);
	maxComboIcon.texture = GetTexture(TextureType::ClearMaxComboIcon);
	bestScoreIcon.texture = GetTexture(TextureType::ClearBestScoreIcon);

	//アイコンの座標を決定
	scoreIcon.pos.y = scoreIconPosY;
	scoreIcon.showPosX = scoreIconShowPosX;
	maxComboIcon.pos.y = maxComboIconPosY;
	maxComboIcon.showPosX = maxComboIconShowPosX;
	bestScoreIcon.pos.y = bestScoreIconPosY;
	bestScoreIcon.showPosX = bestScoreIconShowPosX;

	//必要なデータ取得
	scoreManager.nowScore = GetNowScore();
	scoreManager.maxCombo = GetMaxCombo();
	scoreManager.bestScore = GetHighScore();

}

static void MoveScoreIcon(void) {


	//アイコンの移動処理
	scoreIcon.pos.x = UpdateValueByEasing(scoreIcon.standbyPosX, scoreIcon.showPosX, clearIcon.easingT, clearIcon.easingMode);
	maxComboIcon.pos.x = UpdateValueByEasing(maxComboIcon.standbyPosX, maxComboIcon.showPosX, clearIcon.easingT, clearIcon.easingMode);
	bestScoreIcon.pos.x = UpdateValueByEasing(bestScoreIcon.standbyPosX, bestScoreIcon.showPosX, clearIcon.easingT, clearIcon.easingMode);

	//numberのstartPos変更
	for (int i = 0; i < kScoreNumberLimit; i++) {
		nowScoreNumbers[i].startPos = { scoreIcon.pos.x + nowScoreStartPosOffset,  scoreIcon.pos.y + nowScoreNumbers[i].offsetY };
		bestScoreNumbers[i].startPos = { bestScoreIcon.pos.x + bestScoreStartPosOffset,  bestScoreIcon.pos.y + bestScoreNumbers[i].offsetY };
	}
	for (int i = 0; i < kComboNumberLimit; i++) {
		maxComboNumbers[i].startPos = { maxComboIcon.pos.x + maxComboStartPosOffset,  maxComboIcon.pos.y + maxComboNumbers[i].offsetY };
	}
}

static void UpdateScore(void) {
	if (CheckInputAction(InputAction::Confirm)) {
		scoreManager.easingTScore = 1.0f;
	}

	//<イージングの更新>
	scoreManager.easingTScore += scoreManager.easingSpeed;
	if (scoreManager.easingTScore >= 1.0f) {
		scoreManager.easingTScore = 1.0f;
		nowMode = ModeClear::CountUpCombo;
	}

	//スコアのカウントアップ演出
	scoreManager.showScore = UpdateValueByEasing(0, scoreManager.nowScore, scoreManager.easingTScore, scoreManager.easingMode);
}

static void UpdateCombo(void) {
	if (CheckInputAction(InputAction::Confirm)) {
		scoreManager.easingTCombo = 1.0f;
	}

	scoreManager.easingTCombo += scoreManager.easingSpeed;
	if (scoreManager.easingTCombo >= 1.0f) {
		scoreManager.easingTCombo = 1.0f;
		nowMode = ModeClear::CheckBreakScore;
	}

	//コンボのカウント処理
	scoreManager.showMaxCombo = UpdateValueByEasing(0, scoreManager.maxCombo, scoreManager.easingTCombo, scoreManager.easingMode);
}

static void DrawScore(void) {
	//アイコンの描画
	DrawTextureUI(scoreIcon.texture, scoreIcon.pos, scoreIcon.size);
	DrawTextureUI(maxComboIcon.texture, maxComboIcon.pos, maxComboIcon.size);
	DrawTextureUI(bestScoreIcon.texture, bestScoreIcon.pos, bestScoreIcon.size);


	//スコア
	for (int i = 0; i < kScoreNumberLimit; i++) {
		nowScoreNumbers[i].animNum = GetValueAtDigit(scoreManager.showScore, i);

		Vector2 pos = { nowScoreNumbers[i].startPos.x + nowScoreNumbers[i].interval.x * (kScoreNumberLimit - i - 1), nowScoreNumbers[i].startPos.y };

		Texture texture = clearScoreNumbersTexture;
		texture.animNum = nowScoreNumbers[i].animNum;

		DrawTextureUI(texture, pos, nowScoreNumbers[i].size);
	}

	//最大コンボ
	for (int i = 0; i < kComboNumberLimit; i++) {
		maxComboNumbers[i].animNum = GetValueAtDigit(scoreManager.showMaxCombo, i);

		Vector2 pos = { maxComboNumbers[i].startPos.x + maxComboNumbers[i].interval.x * (kScoreNumberLimit - i - 1), maxComboNumbers[i].startPos.y };

		Texture texture = clearScoreNumbersTexture;
		texture.animNum = maxComboNumbers[i].animNum;

		DrawTextureUI(texture, pos, maxComboNumbers[i].size);
	}

	//ハイスコア
	for (int i = 0; i < kScoreNumberLimit; i++) {
		bestScoreNumbers[i].animNum = GetValueAtDigit(scoreManager.bestScore, i);

		Vector2 pos = { bestScoreNumbers[i].startPos.x + bestScoreNumbers[i].interval.x * (kScoreNumberLimit - i - 1), bestScoreNumbers[i].startPos.y };

		Texture texture = clearScoreNumbersTexture;
		texture.animNum = bestScoreNumbers[i].animNum;

		DrawTextureUI(texture, pos, bestScoreNumbers[i].size);
	}
}

#pragma endregion

#pragma region 関数: ハイスコア更新処理

static void CheckBreakScore(void) {
	if (scoreManager.nowScore > scoreManager.bestScore && !scoreManager.breakScore) {
		UpdateHighScore();

		//演出実装
		CreateFlashPanel();

		PlaySe(SeType::BreakScoreJingle);

		scoreManager.breakScore = true;
	}

	//スコア更新時
	if (scoreManager.breakScore) {
		scoreManager.waitBreakScoreTimer.count++;
		if (scoreManager.waitBreakScoreTimer.count >= scoreManager.waitBreakScoreTimer.time) {
			scoreManager.waitBreakScoreTimer.count = 0;

			nowMode = ModeClear::Select;

			scoreManager.bestScore = GetHighScore();
		}

	}
	//更新出来なかった場合
	else {
		nowMode = ModeClear::Select;
	}
	
}

#pragma endregion

#pragma region 関数: MenuBackground

static void InitMenuBackground(void) {
	menuBackground = {};
	menuBackground.pos = menuBackground.standbyPos;
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
	else {
		if (menuBackground.easingT > 0.0f) {
			menuBackground.easingT -= menuBackground.easingSpeed;
			if (menuBackground.easingT < 0.0f) {
				menuBackground.easingT = 0.0f;
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

#pragma region 関数: 選択関係

static void InitSelect(void) {
	nowSelect = ModeClearSelect::Retry;
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
			selectNum = ToInt(ModeClearSelect::Count) - 1;
		}

		PlaySe(SeType::MoveCursor);

	} else if (CheckInputAction(InputAction::SelectDown)) {
		selectNum++;

		//一番下まで来ていたら、一番上へ
		if (selectNum >= ToInt(ModeClearSelect::Count)) {
			selectNum = 0;
		}

		PlaySe(SeType::MoveCursor);
	}

	//変更を適用
	nowSelect = static_cast<ModeClearSelect>(selectNum);
}

/// <summary>
/// 決定処理
/// </summary>
static void Confirm(void) {
	if (onceSelect) return;

	if (CheckInputAction(InputAction::Confirm)) {
		switch (nowSelect) {
		case ModeClearSelect::Retry: {
			ChangeModeFade(ModeFade::FadeOut);
			ChangeFadeTarget(FadeTarget::Play);

			break;
		}
		case ModeClearSelect::Exit: {
			ChangeModeFade(ModeFade::FadeOut);
			ChangeFadeTarget(FadeTarget::Title);

			break;
		}
		}

		PlaySe(SeType::Confirm);
	}
}

#pragma endregion

#pragma region 関数: ボタン関係

static void InitButton(void) {
	if (retryIcon.texture.tHandle != -1) return;

	retryIcon.texture = GetTexture(TextureType::RetryIcon);

	exitIcon.texture = GetTexture(TextureType::ExitIcon);

	clearMenuCursor.texture = GetTexture(TextureType::MenuCursor);
}

static void UpdateButton(void) {

	//retryIconの処理
	retryIcon.pos = { menuBackground.pos.x + retryIconOffset.x, menuBackground.pos.y + retryIconOffset.y };
	//exitIconの処理
	exitIcon.pos = { menuBackground.pos.x + exitIconOffset.x, menuBackground.pos.y + exitIconOffset.y };

#pragma region 点灯処理
	//RetryIcon
	if (nowSelect == ModeClearSelect::Retry) {
		retryIcon.texture.animNum = 1;
	} else {
		retryIcon.texture.animNum = 0;
	}

	//exitIcon
	if (nowSelect == ModeClearSelect::Exit) {
		exitIcon.texture.animNum = 1;
	} else {
		exitIcon.texture.animNum = 0;
	}
#pragma endregion
}

static void UpdateCursor(void) {

	Vector2 targetPos = {};
	switch (nowSelect) {

	case ModeClearSelect::Retry:
		targetPos = { retryIcon.pos.x + clearMenuCursor.offset.x,retryIcon.pos.y + clearMenuCursor.offset.y };
		break;

	case ModeClearSelect::Exit:
		targetPos = { exitIcon.pos.x + clearMenuCursor.offset.x,exitIcon.pos.y + clearMenuCursor.offset.y };
		break;
	}

	clearMenuCursor.pos = targetPos;

	//回転処理
	clearMenuCursor.rotateTheta -= clearMenuCursor.rotateSpeed;

}

static void DrawButton(void) {
	DrawTextureUI(retryIcon.texture, retryIcon.pos, retryIcon.size);

	DrawTextureUI(exitIcon.texture, exitIcon.pos, exitIcon.size);

	DrawTextureRotateUI(clearMenuCursor.texture, clearMenuCursor.pos, clearMenuCursor.size, clearMenuCursor.rotateTheta);
}

#pragma endregion

void InitClear(void) {
	nowMode = ModeClear::IconFrameIn;

	PlaySe(SeType::StageClearJingle);

	InitIcon();
	InitScore();
	InitMenuBackground();
	InitButton();
	InitSelect();

	InitTitleBackground();
}

void UpdateClear(void) {
	switch (nowMode) {
		case ModeClear::IconFrameIn:
			UpdateIcon();

			break;
		case ModeClear::CountUpScore:
			UpdateScore();
			break;
		case ModeClear::CountUpCombo:
			UpdateCombo();

			break;
		case ModeClear::CheckBreakScore:
			CheckBreakScore();

			break;
		case ModeClear::Select:
			UpdateMenuBackground();

			ChangeCursor();
			Confirm();

			break;
	}

	MoveScoreIcon();
	UpdateButton();
	UpdateCursor();

	UpdateTitleBackground();
}

void DrawClear(void) {
	DrawTitleBackground();

	DrawMenuBackground();

	DrawIcon();
	DrawScore();
	DrawButton();

	DrawEffect();
}