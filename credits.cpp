#include "credits.h"
#include "scene_manager.h"
#include "input.h"
#include "se.h"

#include "common.h"
#include "texture.h"
#include "tool.h"

static bool onceSelect = false;

#pragma region データ: カバーパネル

struct CreditsCoverPanel {
	Vector2 pos = kWindowCenter;
	Vector2 size = { 1280, 720 };

	unsigned int color = 0x00000088;

	Texture texture = {};
};
CreditsCoverPanel coverPanel = {};

#pragma endregion

#pragma region データ: MenuBackground

struct CreditsMenuBackground {
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
CreditsMenuBackground menuBackground;

#pragma endregion

#pragma region データ: クレジット表記
//menubackgroundのイージングにあわせて位置変更

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
	else {
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

#pragma region 関数: 入力関係

static void Confirm(void) {
	if (onceSelect) return;

	if (CheckInputAction(InputAction::Confirm)) {
		onceSelect = true;

		PlaySe(SeType::Confirm);
	}
}

#pragma endregion

void InitCredits(void) {
	onceSelect = false;

	InitCoverPanel();
	InitMenuBackground();
}


void UpdateCredits(void) {
	Confirm();

	UpdateMenuBackground();
}

void DrawCredits(void) {
	DrawCoverPanel();
	DrawMenuBackground();
}