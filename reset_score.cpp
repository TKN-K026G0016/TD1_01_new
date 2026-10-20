#include "reset_score.h"
#include "scene_manager.h"
#include "input.h"
#include "se.h"
#include "score_data.h"

#include "common.h"
#include "texture.h"
#include "tool.h"

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

	unsigned int color = 0x00000088;

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


		} else {
			onceSelect = true;
		}
	} 
	else {
		onceSelect = true;
	}

	PlaySe(SeType::Confirm);

}

static void DrawSelect(void) {

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

void InitResetScore(void) {
	InitSelect();

	InitCoverPanel();
	InitMenuBackground();
}

void UpdateResetScore(void) {
	MoveCursor();
	Confirm();

	UpdateMenuBackground();
}

void DrawResetScore(void) {
	DrawCoverPanel();
	DrawMenuBackground();


	DrawSelect();
}