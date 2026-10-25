#include "guide.h"
#include "input.h"
#include "scene_manager.h"
#include "title_background.h"
#include "se.h"

#include "tool.h"
#include "texture.h"

#pragma region データ: ガイド

//イージング関係
float easingT = 0.0f;
float easingSpeed = 0.01f;
EasingMode easingMode = EasingMode::EaseInOut;

static bool oncePushed = false;

//透明度関係
constexpr int alphaMin = 0;
constexpr int alphaMax = 255;

struct ControllerGuide {
	Vector2 pos = {};
	Vector2 size = { 1024, 512 };

	//最初の座標
	Vector2 standbyPos = { 640, 1000 };
	Vector2 showPos = { 640, 450 };

	//透明度
	int alpha = alphaMin;

	Texture texture = {};
};
ControllerGuide controllerGuide;

struct GaugeGuide {
	Vector2 pos = {};
	Vector2 size = { 512, 64 };

	//最初の座標
	Vector2 standbyPos = { 640, 1000 };
	Vector2 showPos = { 640, 170 };

	//透明度
	int alpha = alphaMin;

	Texture texture = {};
};
GaugeGuide gaugeGuide;

struct EnergyGauge {
	Vector2 pos = {};
	Vector2 size = { 512, 100 };

	//最初の座標
	Vector2 standbyPos = { 640, 1000 };
	Vector2 showPos = { 640, 80 };

	//透明度
	int alpha = alphaMin;

	Texture frameTexture = {};
	Texture meterTexture = {};
};
EnergyGauge energyGauge;

#pragma endregion

#pragma region データ: ボタンガイド
struct ButtonGuide {
	Vector2 pos = { 1180, 100 };
	Vector2 size = { 128, 128 };

	//座標
	Vector2 standbyPos = { 1400, 70 };
	Vector2 showPos = { 1180, 70 };

	//透明度
	int alpha = 0;
	int alphaMin = 0;
	int alphaMax = 255;

	Texture textureKeyboard = {};
	Texture textureGamepad = {};
};
static ButtonGuide buttonGuide;


#pragma endregion

#pragma region 関数: ガイド

static void InitControllerGuide(void) {
	easingT = 0.0f;

	controllerGuide = {};
	controllerGuide.texture = GetTexture(TextureType::ControllerGuide);

	gaugeGuide = {};
	gaugeGuide.texture = GetTexture(TextureType::ShieldGuide);

	energyGauge = {};
	energyGauge.frameTexture = GetTexture(TextureType::EnergyGaugeFrame);
	energyGauge.meterTexture = GetTexture(TextureType::EnergyGaugeMeter);
}

static void UpdateControllerGuide(void) {
	//イージングの更新
	if (easingT < 1.0f) {
		easingT += easingSpeed;
		if (easingT >= 1.0f) {
			easingT = 1.0f;
		}
	}

#pragma region CotrollerGuideの処理

	//座標の変更
	controllerGuide.pos = MoveByEasing(controllerGuide.standbyPos, controllerGuide.showPos, easingT, easingMode);

	//透明度の変更
	controllerGuide.alpha = UpdateValueByEasing(alphaMin, alphaMax, easingT, easingMode);

	//表示変更
	if (GetIsConnectGamePad()) {
		controllerGuide.texture.animNum = 0;
	} else {
		controllerGuide.texture.animNum = 1;
	}

#pragma endregion

#pragma region GaugeGuideの処理

	//座標の変更
	gaugeGuide.pos = MoveByEasing(gaugeGuide.standbyPos, gaugeGuide.showPos, easingT, easingMode);

	//透明度の変更
	gaugeGuide.alpha = UpdateValueByEasing(alphaMin, alphaMax, easingT, easingMode);

#pragma endregion

#pragma region Gaugeの処理

	//座標の変更
	energyGauge.pos = MoveByEasing(energyGauge.standbyPos, energyGauge.showPos, easingT, easingMode);

	//透明度の変更
	energyGauge.alpha = UpdateValueByEasing(alphaMin, alphaMax, easingT, easingMode);

#pragma endregion

}

static void DrawControllerGuide(void) {

	Texture texture;

#pragma region ControllerGuide
	
	texture = controllerGuide.texture;
	texture.color = (texture.color & 0xFFFFFF00) | controllerGuide.alpha;
	DrawTextureUI(texture, controllerGuide.pos, controllerGuide.size);

#pragma endregion

#pragma region GaugeGuide

	texture = gaugeGuide.texture;
	texture.color = (texture.color & 0xFFFFFF00) | gaugeGuide.alpha;
	DrawTextureUI(texture, gaugeGuide.pos, gaugeGuide.size);

#pragma endregion

#pragma region energyGauge

	texture = energyGauge.frameTexture;
	texture.color = (texture.color & 0xFFFFFF00) | energyGauge.alpha;
	DrawTextureUI(texture, energyGauge.pos, energyGauge.size);

	texture = energyGauge.meterTexture;
	texture.color = (texture.color & 0xFFFFFF00) | energyGauge.alpha;
	DrawTextureUI(texture, energyGauge.pos, energyGauge.size);

#pragma endregion

}

#pragma endregion

#pragma region 関数: ボタンガイド

static void InitButtonIcon(void) {
	buttonGuide = {};
	buttonGuide.textureKeyboard = GetTexture(TextureType::ButtonGuideKeyboard);
	buttonGuide.textureGamepad = GetTexture(TextureType::ButtonGuideGamepad);
}

static void UpdateButtonIcon(void) {
	buttonGuide.pos = MoveByEasing(buttonGuide.standbyPos, buttonGuide.showPos, easingT, easingMode);
	buttonGuide.alpha = UpdateValueByEasing(buttonGuide.alphaMin, buttonGuide.alphaMax, easingT, easingMode);

	//アニメーション処理
	if (oncePushed) {
		UpdateAnimation(buttonGuide.textureKeyboard);
		UpdateAnimation(buttonGuide.textureGamepad);
	}
}

static void DrawButtonIcon(void) {
	Texture texture = (GetIsConnectGamePad()) ? buttonGuide.textureGamepad : buttonGuide.textureKeyboard;
	texture.color = (texture.color & 0xFFFFFF00) | buttonGuide.alpha;

	DrawTextureUI(texture, buttonGuide.pos, buttonGuide.size);
}

#pragma endregion

#pragma region 関数: 決定処理

static void Confirm(void) {
	if (GetNowModeFade() != ModeFade::Standby) return;

	if (!CheckInputAction(InputAction::Confirm)) return;

	if (easingT < 1.0f) {
		easingT = 1.0f;

		PlaySe(SeType::MoveCursor);
	}
	else {
		ChangeModeFade(ModeFade::FadeOut);
		ChangeFadeTarget(FadeTarget::Play);

		PlaySe(SeType::Confirm);
		oncePushed = true;
	}
}

#pragma endregion

void InitGuide(void) {
	oncePushed = false;

	InitTitleBackground();
	InitControllerGuide();
	InitButtonIcon();
}

void UpdateGuide(void) {
	Confirm();

	UpdateTitleBackground();
	UpdateControllerGuide();
	UpdateButtonIcon();
}

void DrawGuide(void) {
	DrawTitleBackground();
	DrawControllerGuide();
	DrawButtonIcon();
}