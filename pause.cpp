#include "pause.h"
#include "input.h"
#include "scene_manager.h"
#include "se.h"
#include "bgm.h"

#include "texture.h"
#include "tool.h"
#include "common.h"

#define _USE_MATH_DEFINES
#include <math.h>

#pragma region データ: 選択関係

enum class ModePauseSelect {
	Resume = 0,
	Retry = 1,
	Exit = 2,

	Count
};
static ModePauseSelect nowSelect = ModePauseSelect::Resume;
//既に決定してるかのフラグ
static bool onceSelect = false;

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
MenuBackground menuBackground;

#pragma endregion

#pragma region データ: アイコン関係

struct Icon {
	Vector2 pos = { 0, 0 };
	Vector2 size = { 0, 0 };

	Texture texture = {};
};
//ポーズアイコン
Icon pauseIcon;
static Vector2 pauseIconSize = { 600, 150 };
Vector2 iconStandbyPos = { -200, 600 };
Vector2 iconViewPos = { 300, 600 };

//セレクトアイコンのサイズ
static Vector2 selectIconSize = { 256, 64 };

static Icon resumeIcon;
static Vector2 resumeIconOffset = { 390, -140 };
static Icon retryIcon;
static Vector2 retryIconOffset = { 430, -210 };
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
MenuCursor menuCursor;

#pragma endregion

#pragma region データ: カバーパネル

struct CoverPanel {
	Vector2 pos = kWindowCenter;
	Vector2 size = { 1280, 720 };

	unsigned int color = 0x00000088;

	Texture texture = {};
};
CoverPanel pauseCoverPanel = {};

#pragma endregion


#pragma region 関数: カバーパネル

static void InitCoverPanel(void) {
	//テクスチャデータの取得
	if (pauseCoverPanel.texture.tHandle != -1) return;

	pauseCoverPanel.texture = GetTexture(TextureType::White1x1);
	pauseCoverPanel.texture.color = pauseCoverPanel.color;
}

static void DrawCoverPanel(void) {
	DrawTextureUI(pauseCoverPanel.texture, pauseCoverPanel.pos, pauseCoverPanel.size);
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

#pragma region 関数: 選択処理

static void InitSelect(void) {
	nowSelect = ModePauseSelect::Resume;
	onceSelect = false;
}

/// <summary>
/// ボタンによるポーズ解除処理
/// </summary>
/// <param name=""></param>
static void InputResume(void) {
	if (onceSelect) return;

	if (CheckInputAction(InputAction::Pause)) {
		onceSelect = true;
		nowSelect = ModePauseSelect::Resume;

		PlaySe(SeType::Pause);
	}
}

/// <summary>
/// カーソル移動処理
/// </summary>
static void ChangeCursor(void) {
	if (onceSelect) return;

	int selectNum = ToInt(nowSelect);

	if (CheckInputAction(InputAction::SelectUp)) {
		selectNum--;

		//一番上まで来ていたら、一番下へ
		if (selectNum < 0) {
			selectNum = ToInt(ModePauseSelect::Count) - 1;
		}

		PlaySe(SeType::MoveCursor);


	} else if (CheckInputAction(InputAction::SelectDown)) {
		selectNum++;

		//一番下まで来ていたら、一番上へ
		if (selectNum >= ToInt(ModePauseSelect::Count)) {
			selectNum = 0;
		}

		PlaySe(SeType::MoveCursor);
	}

	//変更を適用
	nowSelect = static_cast<ModePauseSelect>(selectNum);
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

/// <summary>
/// シーンの変更処理
/// </summary>
static void ChangeScene(void) {
	if (GetNowModeFade() != ModeFade::Standby) return;

	if (onceSelect && menuBackground.easingT <= 0.0f) {
		switch (nowSelect) {
		case ModePauseSelect::Resume: {
			ChangeSceneImmediate(Scene::Play);
			ResumeBgm();

			break;
		}
		case ModePauseSelect::Retry: {
			ChangeModeFade(ModeFade::FadeOut);
			ChangeFadeTarget(FadeTarget::Play);

			break;
		}
		case ModePauseSelect::Exit: {
			ChangeModeFade(ModeFade::FadeOut);
			ChangeFadeTarget(FadeTarget::Title);

			break;
		}
		}
	}
}

#pragma endregion

#pragma region 関数: アイコン

/// <summary>
/// アイコンの初期化処理
/// </summary>
static void InitIcon(void) {
	if (pauseIcon.texture.tHandle != -1) return;

	pauseIcon.size = pauseIconSize;
	pauseIcon.texture = GetTexture(TextureType::PauseIcon);

	resumeIcon.size = selectIconSize;
	resumeIcon.texture = GetTexture(TextureType::ResumeIcon);

	retryIcon.size = selectIconSize;
	retryIcon.texture = GetTexture(TextureType::RetryIcon);

	exitIcon.size = selectIconSize;
	exitIcon.texture = GetTexture(TextureType::ExitIcon);

	menuCursor.texture = GetTexture(TextureType::MenuCursor);
}

static void UpdateIcon(void) {

	//PauseIconの処理
	pauseIcon.pos = MoveByEasing(iconStandbyPos, iconViewPos, menuBackground.easingT, EasingMode::EaseOut);

	//resumeIconの処理
	resumeIcon.pos = { menuBackground.pos.x + resumeIconOffset.x, menuBackground.pos.y + resumeIconOffset.y };
	//retryIconの処理
	retryIcon.pos = { menuBackground.pos.x + retryIconOffset.x, menuBackground.pos.y + retryIconOffset.y };
	//exitIconの処理
	exitIcon.pos = { menuBackground.pos.x + exitIconOffset.x, menuBackground.pos.y + exitIconOffset.y };

#pragma region 点灯処理
	//resumeIcon
	if (nowSelect == ModePauseSelect::Resume) {
		resumeIcon.texture.animNum = 1;
	} else {
		resumeIcon.texture.animNum = 0;
	}

	//retry
	if (nowSelect == ModePauseSelect::Retry) {
		retryIcon.texture.animNum = 1;
	} else {
		retryIcon.texture.animNum = 0;
	}

	//exit
	if (nowSelect == ModePauseSelect::Exit) {
		exitIcon.texture.animNum = 1;
	} else {
		exitIcon.texture.animNum = 0;
	}

#pragma endregion

}

static void UpdateCursor(void) {

	Vector2 targetPos = {};
	switch (nowSelect) {
	case ModePauseSelect::Resume:
		targetPos = { resumeIcon.pos.x + menuCursor.offset.x,resumeIcon.pos.y + menuCursor.offset.y };
		break;

	case ModePauseSelect::Retry:
		targetPos = { retryIcon.pos.x + menuCursor.offset.x,retryIcon.pos.y + menuCursor.offset.y };
		break;

	case ModePauseSelect::Exit:
		targetPos = { exitIcon.pos.x + menuCursor.offset.x,exitIcon.pos.y + menuCursor.offset.y };
		break;
	}

	menuCursor.pos = targetPos;

	//回転処理
	menuCursor.rotateTheta -= menuCursor.rotateSpeed;

}

static void DrawIcon(void) {
	//PauseIcon
	DrawTextureUI(pauseIcon.texture, pauseIcon.pos, pauseIcon.size);

	//resumeIcon
	DrawTextureUI(resumeIcon.texture, resumeIcon.pos, resumeIcon.size);

	//retryIcon
	DrawTextureUI(retryIcon.texture, retryIcon.pos, retryIcon.size);

	//exitIcon
	DrawTextureUI(exitIcon.texture, exitIcon.pos, exitIcon.size);
}

static void DrawCursor(void) {
	DrawTextureRotateUI(menuCursor.texture, menuCursor.pos, menuCursor.size, menuCursor.rotateTheta);
}

#pragma endregion

void InitPause(void) {
	PlaySe(SeType::Pause);
	PauseBgm();

	InitSelect();
	InitIcon();
	InitCoverPanel();
	InitMenuBackground();
}

void UpdatePause(void) {
	InputResume();

	ChangeCursor();
	Confirm();

	UpdateMenuBackground();
	UpdateIcon();
	UpdateCursor();

	ChangeScene();
}

void DrawPause(void) {
	DrawCoverPanel();
	DrawMenuBackground();

	DrawIcon();
	DrawCursor();
}