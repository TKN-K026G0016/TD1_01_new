#include "credits.h"
#include "scene_manager.h"
#include "input.h"
#include "se.h"
#include "bgm.h"

#include "common.h"
#include "texture.h"
#include "tool.h"

#define _USE_MATH_DEFINES
#include <math.h>

static bool onceSelect = false;

#pragma region データ: カバーパネル

struct CreditsCoverPanel {
	Vector2 pos = kWindowCenter;
	Vector2 size = { 1280, 720 };

	unsigned int color = 0x000000DD;

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

#pragma region データ: ボタン

struct Icon {
	Vector2 pos = { 0, 0 };
	Vector2 size = { 0, 0 };

	Texture texture = {};
};
//ポーズアイコン
static Icon creditsIcon;
static Vector2 creditsIconSize = { 600, 150 };
static Vector2 iconStandbyPos = { -200, 600 };
static Vector2 iconViewPos = { 300, 600 };

//セレクトアイコンのサイズ
static Vector2 selectIconSize = { 256, 64 };

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

#pragma region データ: クレジット表記
//menubackgroundのイージングにあわせて位置変更

struct CreditsContent {
	Vector2 pos = {};
	Vector2 size = { 600, 400 };

	Vector2 standbyPos = { -200, 280 };
	Vector2 showPos = { 360, 280 };

	Texture texture = {};
};
static CreditsContent credits;

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
				ResetBgmVolume();
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

#pragma region 関数: ボタン関係

/// <summary>
/// アイコンの初期化処理
/// </summary>
static void InitIcon(void) {
	if (creditsIcon.texture.tHandle != -1) return;

	creditsIcon.size = creditsIconSize;
	creditsIcon.texture = GetTexture(TextureType::CreditsIcon);

	exitIcon.size = selectIconSize;
	exitIcon.texture = GetTexture(TextureType::CreditsExitIcon);
	exitIcon.texture.animNum = 1;

	menuCursor.texture = GetTexture(TextureType::MenuCursor);
}

static void UpdateIcon(void) {

	//PauseIconの処理
	creditsIcon.pos = MoveByEasing(iconStandbyPos, iconViewPos, menuBackground.easingT, EasingMode::EaseOut);

	//exitIconの処理
	exitIcon.pos = { menuBackground.pos.x + exitIconOffset.x, menuBackground.pos.y + exitIconOffset.y };
}

static void UpdateCursor(void) {

	menuCursor.pos = { exitIcon.pos.x + menuCursor.offset.x,exitIcon.pos.y + menuCursor.offset.y };

	//回転処理
	menuCursor.rotateTheta -= menuCursor.rotateSpeed;

}

static void DrawIcon(void) {
	//PauseIcon
	DrawTextureUI(creditsIcon.texture, creditsIcon.pos, creditsIcon.size);

	//exitIcon
	DrawTextureUI(exitIcon.texture, exitIcon.pos, exitIcon.size);
}

static void DrawCursor(void) {
	DrawTextureRotateUI(menuCursor.texture, menuCursor.pos, menuCursor.size, menuCursor.rotateTheta);
}

#pragma endregion

#pragma region 関数: クレジット表記

static void InitContent(void) {
	credits = {};
	credits.texture = GetTexture(TextureType::CreditsContent);
}

static void UpdateContent(void) {
	credits.pos = MoveByEasing(credits.standbyPos, credits.showPos, menuBackground.easingT, EasingMode::EaseOut);
}


static void DrawContent(void) {
	DrawTextureUI(credits.texture, credits.pos, credits.size);
}

#pragma endregion

void InitCredits(void) {
	onceSelect = false;

	InitCoverPanel();
	InitMenuBackground();
	InitIcon();
	InitContent();
}


void UpdateCredits(void) {
	Confirm();

	UpdateMenuBackground();
	UpdateIcon();
	UpdateCursor();
	UpdateContent();
}

void DrawCredits(void) {
	DrawCoverPanel();
	DrawMenuBackground();
	DrawIcon();
	DrawCursor();
	DrawContent();
}