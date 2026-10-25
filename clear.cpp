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
static ModeClear nowMode = ModeClear::IconFrameIn;

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

	Vector2 startPosOffset = {};
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

#pragma region データ: NewRecordアイコン

struct NewRecordIcon {
	Vector2 pos = {};
	Vector2 size = { 512,64 };

	bool isBreakScore = false;

	//<イージング関係>
	float easingTMove = 0.0f;
	float easingSpeedMove = 0.03f;
	EasingMode easingMode = EasingMode::EaseOut;

	Vector2 standbyPos = { -300, 400 };
	Vector2 showPos = { 350, 400 };

	//<イージング関係(点滅処理)>
	float easingTAlpha = 0.0f;
	float easingSpeedAlpha = 0.03f;
	bool isFadeIn = true;

	int alpha = 0;
	int alphaMin = 0;
	int alphaMax = 255;

	Texture texture = {};
};
NewRecordIcon newRecordIcon;

struct ShockSpawner {
	Vector2i createPosRange[2] = { {200, 100}, {1080, 620} };

	Timer createTimer = { 24, 0 };

	int createLimit = 8;
	int createCount = 0;

};
static ShockSpawner shockSpawner;

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

	scoreManager = {};

	//必要なデータ取得
	scoreManager.nowScore = GetNowScore();
	if (scoreManager.nowScore > kCountStopScore) {
		scoreManager.nowScore = kCountStopScore;
	}
	scoreManager.maxCombo = GetMaxCombo();
	if (scoreManager.maxCombo > 9999) {
		scoreManager.maxCombo = 9999;
	}

	scoreManager.bestScore = GetHighScore();
	if (scoreManager.bestScore > kCountStopScore) {
		scoreManager.bestScore = kCountStopScore;
	}

}

static void MoveScoreIcon(void) {
	//アイコンの移動処理
	scoreIcon.pos.x = UpdateValueByEasing(scoreIcon.standbyPosX, scoreIcon.showPosX, clearIcon.easingT, clearIcon.easingMode);
	maxComboIcon.pos.x = UpdateValueByEasing(maxComboIcon.standbyPosX, maxComboIcon.showPosX, clearIcon.easingT, clearIcon.easingMode);
	bestScoreIcon.pos.x = UpdateValueByEasing(bestScoreIcon.standbyPosX, bestScoreIcon.showPosX, clearIcon.easingT, clearIcon.easingMode);

	//numberのstartPos変更
	for (int i = 0; i < kScoreNumberLimit; i++) {
		nowScoreNumbers[i].startPosOffset = { scoreIcon.pos.x + nowScoreStartPosOffset,  scoreIcon.pos.y + nowScoreNumbers[i].offsetY };
		bestScoreNumbers[i].startPosOffset = { bestScoreIcon.pos.x + bestScoreStartPosOffset,  bestScoreIcon.pos.y + bestScoreNumbers[i].offsetY };
	}
	for (int i = 0; i < kComboNumberLimit; i++) {
		maxComboNumbers[i].startPosOffset = { maxComboIcon.pos.x + maxComboStartPosOffset,  maxComboIcon.pos.y + maxComboNumbers[i].offsetY };
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
	PlaySe(SeType::CountUpScore);
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
	PlaySe(SeType::CountUpScore);
}

static void DrawScore(void) {
	//アイコンの描画
	DrawTextureUI(scoreIcon.texture, scoreIcon.pos, scoreIcon.size);
	DrawTextureUI(maxComboIcon.texture, maxComboIcon.pos, maxComboIcon.size);
	DrawTextureUI(bestScoreIcon.texture, bestScoreIcon.pos, bestScoreIcon.size);


	//スコア
	for (int i = 0; i < kScoreNumberLimit; i++) {
		nowScoreNumbers[i].animNum = GetValueAtDigit(scoreManager.showScore, i);

		Vector2 pos = { nowScoreNumbers[i].startPosOffset.x + nowScoreNumbers[i].interval.x * (kScoreNumberLimit - i - 1), nowScoreNumbers[i].startPosOffset.y };

		Texture texture = clearScoreNumbersTexture;
		texture.animNum = nowScoreNumbers[i].animNum;

		DrawTextureUI(texture, pos, nowScoreNumbers[i].size);
	}

	//最大コンボ
	for (int i = 0; i < kComboNumberLimit; i++) {
		maxComboNumbers[i].animNum = GetValueAtDigit(scoreManager.showMaxCombo, i);

		Vector2 pos = { maxComboNumbers[i].startPosOffset.x + maxComboNumbers[i].interval.x * (kScoreNumberLimit - i - 1), maxComboNumbers[i].startPosOffset.y };

		Texture texture = clearScoreNumbersTexture;
		texture.animNum = maxComboNumbers[i].animNum;

		DrawTextureUI(texture, pos, maxComboNumbers[i].size);
	}

	//ハイスコア
	for (int i = 0; i < kScoreNumberLimit; i++) {
		bestScoreNumbers[i].animNum = GetValueAtDigit(scoreManager.bestScore, i);

		Vector2 pos = { bestScoreNumbers[i].startPosOffset.x + bestScoreNumbers[i].interval.x * (kScoreNumberLimit - i - 1), bestScoreNumbers[i].startPosOffset.y };

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

		if (CheckIsPlayingSe(SeType::StageClearJingle)) {
			StopPlayingSe(SeType::StageClearJingle);
		}

		PlaySe(SeType::BreakScoreJingle);

		TriggerViberationGamePad(ModeViberation::L);

		scoreManager.breakScore = true;
		newRecordIcon.isBreakScore = true;
	}

	//スコア更新時
	if (scoreManager.breakScore) {
		scoreManager.waitBreakScoreTimer.count++;
		if (scoreManager.waitBreakScoreTimer.count >= scoreManager.waitBreakScoreTimer.time) {
			scoreManager.waitBreakScoreTimer.count = 0;

			nowMode = ModeClear::Select;

			scoreManager.bestScore = GetHighScore();
			if (scoreManager.bestScore > kCountStopScore) {
				scoreManager.bestScore = kCountStopScore;
			}
		}

	}
	//更新出来なかった場合
	else {
		nowMode = ModeClear::Select;
	}
	
}

#pragma endregion

#pragma region 関数: ハイスコア更新演出

static void InitNewRecordIcon(void) {
	newRecordIcon = {};
	newRecordIcon.texture = GetTexture(TextureType::NewRecordIcon);
}

static void UpdateNewRecordIcon(void) {
	if (!newRecordIcon.isBreakScore) return;

	//移動処理
	if (!onceSelect) {
		newRecordIcon.easingTMove += newRecordIcon.easingSpeedMove;
		if (newRecordIcon.easingTMove >= 1.0f) {
			newRecordIcon.easingTMove = 1.0f;
		}

		newRecordIcon.pos = MoveByEasing(newRecordIcon.standbyPos, newRecordIcon.showPos, newRecordIcon.easingTMove, newRecordIcon.easingMode);
	} else {
		newRecordIcon.pos = MoveByEasing(newRecordIcon.standbyPos, newRecordIcon.showPos, menuBackground.easingT, newRecordIcon.easingMode);
	}

	//点滅処理
	if (newRecordIcon.isFadeIn) {
		newRecordIcon.easingTAlpha += newRecordIcon.easingSpeedAlpha;
		if (newRecordIcon.easingTAlpha >= 1.0f) {
			newRecordIcon.easingTAlpha = 1.0f;
			newRecordIcon.isFadeIn = false;
		}
	} else {
		newRecordIcon.easingTAlpha -= newRecordIcon.easingSpeedAlpha;
		if (newRecordIcon.easingTAlpha <= 0.0f) {
			newRecordIcon.easingTAlpha = 0.0f;
			newRecordIcon.isFadeIn = true;
		}
	}
	newRecordIcon.alpha = UpdateValueByEasing(newRecordIcon.alphaMin, newRecordIcon.alphaMax, newRecordIcon.easingTAlpha, newRecordIcon.easingMode);

}

static void DrawNewRecordIcon(void) {
	if (!newRecordIcon.isBreakScore) return;

	Texture texture = newRecordIcon.texture;
	texture.color = (texture.color & 0xFFFFFF00) | newRecordIcon.alpha;
	DrawTextureUI(texture, newRecordIcon.pos, newRecordIcon.size);
}

static void InitShockSpawner(void) {
	shockSpawner = {};
}

static void UpdateShockSpawner(void) {
	if (!newRecordIcon.isBreakScore) return;

	if (shockSpawner.createCount >= shockSpawner.createLimit) return;

	shockSpawner.createTimer.count++;
	if (shockSpawner.createTimer.count >= shockSpawner.createTimer.time) {
		shockSpawner.createTimer.count = 0;

		Vector2 createPos;
		createPos.x = ToFloat(GetRand(shockSpawner.createPosRange[0].x, shockSpawner.createPosRange[1].x));
		createPos.y = ToFloat(GetRand(shockSpawner.createPosRange[0].y, shockSpawner.createPosRange[1].y));

		CreateElecShockChargedEffect(createPos);
		TriggerViberationGamePad(ModeViberation::M);

		shockSpawner.createCount++;
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
		StopPlayingSe(SeType::StageClearJingle);
		StopPlayingSe(SeType::BreakScoreJingle);
		onceSelect = true;
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
#ifdef _DEBUG

	//AddScore(1000005);

#endif

	nowMode = ModeClear::IconFrameIn;

	PlaySe(SeType::StageClearJingle);

	InitIcon();
	InitScore();
	InitMenuBackground();
	InitButton();
	InitSelect();
	InitNewRecordIcon();
	InitShockSpawner();
	InitEffect();

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
	UpdateNewRecordIcon();
	UpdateShockSpawner();
	UpdateEffect();

	UpdateTitleBackground();
}

void DrawClear(void) {
	DrawTitleBackground();

	DrawEffectAsUI();

	DrawMenuBackground();

	DrawIcon();
	DrawScore();
	DrawButton();
	DrawNewRecordIcon();

	DrawEffect();
}