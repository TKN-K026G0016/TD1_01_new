#include "title.h"
#include "input.h"
#include "scene_manager.h"
#include "title_background.h"
#include "title_player.h"
#include "elec_bullet.h"
#include "effect.h"
#include "bgm.h"

#include "texture.h"
#include "se.h"
#include "vector2.h"
#include "common.h"
#include "tool.h"

#include <Novice.h>
#define _USE_MATH_DEFINES
#include <math.h>

static ModeTitle nowMode = ModeTitle::MovePlayer;

#pragma region データ: ロゴ

struct Logo {
	Vector2 pos = { 640, 580 };
	Vector2 size = { 640, 256 };

	//ロゴの当たり判定の大きさ
	Vector2 hitSize = { 640, 200 };
	Vector2 hitVertex[4] = {};

	//振動関係
	int shakeDuration = 1;
	Timer shakeTimer = { 10, 0 };
	//<ヒット演出>
	int shakeDurationHit = 8;
	Timer hitTimer = { 20, 0 };

	unsigned int lightOutColor = 0x222222FF;

	Texture texture = {};
};
static Logo logo;
Vector2 spawnShockPos[5] = {
	{logo.pos},
	{340, 500},
	{700, 630},
	{450, 590},
	{700, 480},
};

struct TitleLogoAfterimage {
	Vector2 size = { };

	//残像の振動
	int shakeDuration = 2;
	Timer shakeTimer = { 10, 0 };

	unsigned int color = 0x04C1E8FF;

	//イージング関係
	float easingT = 0.0f;
	float easingSpeed = 0.01f;
	bool isFadeIn = true;
	EasingMode easingMode = EasingMode::EaseInOut;

	//サイズ関係
	Vector2 startSize = logo.size;
	Vector2 endSize = { logo.size.x + 40, logo.size.y + 40 };

	//透明度関係
	int alpha = 0;
	int alphaMin = 0;
	int alphaMax = 120;
};
TitleLogoAfterimage logoAfterimage;

struct TitleInst {
	Vector2 pos = { 640, 200 };
	Vector2 size = { 640, 128 };

	//<イージング>
	float easingT = 0.0f;
	float easingSpeedShot = 0.01f;
	float easingSpeedStart = 0.03f;
	EasingMode easingMode = EasingMode::EaseInOut;
	float isFadeIn = true;

	//透明度
	int alpha = 0;
	int alphaMin = 0;
	int alphaMax = 170;

	Texture texture = {};
};
TitleInst instShot;
TitleInst instStart;

#pragma endregion

#pragma region データ: その他ボタン

struct Button {
	Vector2 pos = { };
	Vector2 showPos = {};
	Vector2 size = { 256, 128 };

	float hitRadius = 64.0f;

	//振動関係
	int shakeDuration = 1;
	Timer shakeTimer = { 10, 0 };
	//<ヒット演出>
	int shakeDurationHit = 8;
	Timer hitTimer = { 20, 0 };

	unsigned int lightOutColor = 0x222222FF;

	Texture texture = {};

	//<残像関係>
	float easingT = 0.0f;
	float easingSpeed = 0.01f;
	bool isScaleUp = true;
	EasingMode easingMode = EasingMode::EaseInOut;

	Vector2 afterimageSize = {};
	Vector2 startSize = size;
	Vector2 endSize = { size.x + 20, size.y + 20 };

	//透明度
	int alpha = 0;
	int alphaMin = 0;
	int alphaMax = 170;
};
Button creditsButton;
Vector2 creditsPos = { 90, 630 };
Button resetButton;
Vector2 resetPos = { 1190, 630 };


#pragma endregion

#pragma region データ: 演出

struct BlackPanel {
	Vector2 pos = kWindowCenter;
	Vector2 size = { 1280, 720 };

	bool isShow = true;

	unsigned int color = 0x000000DD;

	Texture texture = {};
};
BlackPanel blackPanel;

struct FlashPanel {
	Vector2 pos = kWindowCenter;
	Vector2 size = { 1280, 720 };

	//<イージング>
	float easingT = 0.0f;
	float easingSpeed = 0.05f;
	float startFadeOutEasingT = 0.1f;
	EasingMode easingMode = EasingMode::EaseOut;

	//透明度
	int alpha = 0;
	int alphaMin = 0;
	int alphaMax = 255;

	Texture texture = {};
};
FlashPanel flashPanel;

#pragma endregion

#pragma region データ: 音量

//消灯時のBgm音量
static constexpr float lightOutVolume = 0.2f;

#pragma endregion

#pragma region データ: Recommend

struct RecommendIcon {
	Vector2 pos = {};
	Vector2 size = { 1024, 512 };

	bool oncePushed = false;

	//<イージング関係>
	float easingT = 0.0f;
	float easingSpeed = 0.02f;
	float easingSpeedFrameOut = 0.05f;
	EasingMode easingMode = EasingMode::EaseOut;

	//最初の座標
	Vector2 standbyPos = { 640, 1000 };
	Vector2 showPos = { 640, 400 };

	//透明度
	int alpha = 0;
	int alphaMin = 0;
	int alphaMax = 255;

	Texture texture = {};
};
RecommendIcon recommendIcon;

#pragma endregion


#pragma region 関数: 項目決定

static void ChangeMode(void) {
	//モード切替
	if (nowMode == ModeTitle::MovePlayer) {
		nowMode = ModeTitle::TriggerLightUp;

		PlaySe(SeType::DeadEnemy);

		ResetBgmVolume();
		TriggerViberationGamePad(ModeViberation::L);

	}
	//シーン切り替え
	else {
		if (GetNowModeFade() != ModeFade::Standby) return;
		ChangeModeFade(ModeFade::FadeOut);
		ChangeFadeTarget(FadeTarget::Guide);

		PlaySe(SeType::DeadEnemy);
		PlaySe(SeType::Confirm);

		NoticeTitlePlayerGameStart();

		//Bgmをフェード
		ChangeModeBgmFade(ModeBgmFade::FadeOut);
	}
}

#pragma endregion

#pragma region 関数: ロゴ・インスト関係

static void InitLogo(void) {
	logo = {};
	SetVertex(logo.pos, logo.hitSize, logo.hitVertex);
	logo.texture = GetTexture(TextureType::TitleLogo);
}

static void CheckCollisionVSElecBullet(void) {
	ElecBullet* elecBullet = GetElecBulletArray();
	int elecBulletLimit = GetElecBulletLimit();

	for (int j = 0; j < elecBulletLimit; j++) {
		if (!elecBullet[j].isShoot) continue;

		if (CheckCollisionBoxVSCircle(elecBullet[j].pos, elecBullet[j].hitRadius, logo.hitVertex[0], logo.hitVertex[3])) {
			BreakElecBullet(j);

			//ホウデンの生成
			for (int i = 0; i < 5; i++) {
				SpawnElecShock(spawnShockPos[i], elecBullet[j].type, ElecShockOriginType::None, -1, ElecShockOriginType::None, -1);

				Vector2 createPos = logo.pos;
				createPos.x += ToFloat(GetRand(ToInt(-logo.size.x), ToInt(logo.size.x)));
				createPos.y += ToFloat(GetRand(ToInt(-logo.size.y), ToInt(logo.size.y)));

				SpawnElecShock(createPos, elecBullet[j].type, ElecShockOriginType::None, -1, ElecShockOriginType::None, -1);
			}
			
			//hitTimerを起動
			logo.hitTimer.count = logo.hitTimer.time;

			//モード切替
			ChangeMode();
		}
	}
}

static void UpdateLogoAfterimage(void) {
	if (nowMode != ModeTitle::WaitInput) return;

	if (logoAfterimage.isFadeIn) {
		logoAfterimage.easingT += logoAfterimage.easingSpeed;
		if (logoAfterimage.easingT >= 1.0f) {
			logoAfterimage.easingT = 1.0f;
			logoAfterimage.isFadeIn = false;
		}
	} else {
		logoAfterimage.easingT -= logoAfterimage.easingSpeed;
		if (logoAfterimage.easingT <= 0.0f) {
			logoAfterimage.easingT = 0.0f;
			logoAfterimage.isFadeIn = true;
		}
	}

	//サイズ関係
	logoAfterimage.size = MoveByEasing(logoAfterimage.startSize, logoAfterimage.endSize, logoAfterimage.easingT, logoAfterimage.easingMode);

	//透明度の処理
	logoAfterimage.alpha = UpdateValueByEasing(logoAfterimage.alphaMin, logoAfterimage.alphaMax, logoAfterimage.easingT, logoAfterimage.easingMode);
}

static void UpdateLogo(void) {
	CheckCollisionVSElecBullet();

	if (logo.hitTimer.count > 0) {
		logo.hitTimer.count--;
	}

	UpdateLogoAfterimage();
}

static void DrawLogoAfterimage(void) {
	if (nowMode != ModeTitle::WaitInput) return;

	Vector2 showPos = logo.pos;
	logoAfterimage.shakeTimer.count++;
	if (logoAfterimage.shakeTimer.count >= logoAfterimage.shakeTimer.time) {
		logoAfterimage.shakeTimer.count = 0;

		showPos.x += GetRand(-logoAfterimage.shakeDuration, logoAfterimage.shakeDuration);
		showPos.y += GetRand(-logoAfterimage.shakeDuration, logoAfterimage.shakeDuration);
	}

	Texture texture = logo.texture;
	texture.color = (logoAfterimage.color & 0xFFFFFF00) | logoAfterimage.alpha;

	DrawTextureUI(texture, showPos, logoAfterimage.size);
}

static void DrawLogo(void) {
	Vector2 showPos = logo.pos;

	if (logo.hitTimer.count > 0) {
		showPos.x += GetRand(-logo.shakeDurationHit, logo.shakeDurationHit);
		showPos.y += GetRand(-logo.shakeDurationHit, logo.shakeDurationHit);
	}
	else if (nowMode == ModeTitle::Recommend || nowMode == ModeTitle::MovePlayer) {
		logo.shakeTimer.count++;
		if (logo.shakeTimer.count >= logo.shakeTimer.time) {
			logo.shakeTimer.count = 0;

			showPos.x += GetRand(-logo.shakeDuration, logo.shakeDuration);
			showPos.y += GetRand(-logo.shakeDuration, logo.shakeDuration);
		}
	}

	//colorの変更
	Texture texture = logo.texture;
	if (nowMode == ModeTitle::Recommend || nowMode == ModeTitle::MovePlayer) {
		texture.color = logo.lightOutColor;
	}

	DrawLogoAfterimage();

	DrawTextureUI(texture, showPos, logo.size);
}

static void InitInst(void) {
	instShot = {};
	instShot.texture = GetTexture(TextureType::TitleInstShot);

	instStart = {};
	instStart.texture = GetTexture(TextureType::TitleInstStart);
}

static void UpdateInst(void) {

	//TitlePlayerが到着していなければスキップ
	if (GetTitlePlayerEasingT() < 1.0f) return;

#pragma region instShotの処理
	if (nowMode == ModeTitle::MovePlayer) {

		if (instShot.isFadeIn) {
			instShot.easingT += instShot.easingSpeedShot;
			if (instShot.easingT >= 1.0f) {
				instShot.easingT = 1.0f;
				instShot.isFadeIn = false;
			}
		} else {
			instShot.easingT -= instShot.easingSpeedShot;
			if (instShot.easingT < 0.0f) {
				instShot.easingT = 0.0f;
				instShot.isFadeIn = true;
			}
		}

		instShot.alpha = UpdateValueByEasing(instShot.alphaMin, instShot.alphaMax, instShot.easingT, instShot.easingMode);
	}

#pragma endregion

#pragma region instStartの処理

	if (nowMode == ModeTitle::WaitInput) {
		if (instStart.isFadeIn) {
			instStart.easingT += instStart.easingSpeedStart;
			if (instStart.easingT >= 1.0f) {
				instStart.easingT = 1.0f;
				instStart.isFadeIn = false;
			}
		} else {
			instStart.easingT -= instStart.easingSpeedStart;
			if (instStart.easingT < 0.0f) {
				instStart.easingT = 0.0f;
				instStart.isFadeIn = true;
			}
		}

		instStart.alpha = UpdateValueByEasing(instStart.alphaMin, instStart.alphaMax, instStart.easingT, instStart.easingMode);
	}

#pragma endregion
}

static void DrawInst(void) {
	if (nowMode == ModeTitle::MovePlayer) {
		Texture texture = instShot.texture;
		texture.color = (texture.color & 0xFFFFFF00) | instShot.alpha;

		//コントローラの接続チェック
		if (GetIsConnectGamePad()) {
			texture.animNum = 0;
		} else {
			texture.animNum = 1;
		}
		
		DrawTextureUI(texture, instShot.pos, instShot.size);
	}

	if (nowMode == ModeTitle::WaitInput) {
		Texture texture = instStart.texture;
		texture.color = (texture.color & 0xFFFFFF00) | instStart.alpha;

		//コントローラの接続チェック
		if (GetIsConnectGamePad()) {
			texture.animNum = 0;
		} else {
			texture.animNum = 1;
		}

		DrawTextureUI(texture, instStart.pos, instStart.size);
	}
}

#pragma endregion

#pragma region 関数: ボタン関係

static void InitButton(void) {
	creditsButton = {};
	creditsButton.pos = creditsPos;
	creditsButton.texture = GetTexture(TextureType::CreditsButtonNew);

	resetButton = {};
	resetButton.pos = resetPos;
	resetButton.texture = GetTexture(TextureType::ResetScoreButtonNew);
}

static void ChangeModeByCredits(void) {
	//モード切替
	if (nowMode == ModeTitle::MovePlayer) {
		nowMode = ModeTitle::TriggerLightUp;

		PlaySe(SeType::DeadEnemy);

		ResetBgmVolume();
		TriggerViberationGamePad(ModeViberation::L);
	}
	//シーン切り替え
	else {
		if (GetNowModeFade() != ModeFade::Standby) return;
		ChangeScene(Scene::Credits);

		PlaySe(SeType::Confirm);
		PlaySe(SeType::DeadEnemy);

		ChangeBgmVolume(lightOutVolume);
		BreakAllElecBullet();
	}
}

static void ChangeModeByReset(void) {
	//モード切替
	if (nowMode == ModeTitle::MovePlayer) {
		nowMode = ModeTitle::TriggerLightUp;

		PlaySe(SeType::DeadEnemy);

		ResetBgmVolume();
		TriggerViberationGamePad(ModeViberation::L);
	}
	//シーン切り替え
	else {
		if (GetNowModeFade() != ModeFade::Standby) return;
		ChangeScene(Scene::ResetScore);

		PlaySe(SeType::DeadEnemy);
		PlaySe(SeType::Confirm);

		ChangeBgmVolume(lightOutVolume);

		BreakAllElecBullet();
	}
}

static void CheckCollisionButtonVSBullet(void) {
	ElecBullet* elecBullet = GetElecBulletArray();
	int elecBulletLimit = GetElecBulletLimit();

	for (int j = 0; j < elecBulletLimit; j++) {
		if (!elecBullet[j].isShoot) continue;

		//CreditsButton
		if (CheckCollisionCircleVSCircle(elecBullet[j].pos, elecBullet[j].hitRadius, creditsButton.pos, creditsButton.hitRadius)) {
			BreakElecBullet(j);

			//ホウデンの生成
			for (int i = 0; i < 5; i++) {
				SpawnElecShock(spawnShockPos[i], elecBullet[j].type, ElecShockOriginType::None, -1, ElecShockOriginType::None, -1);

				Vector2 createPos = creditsButton.pos;
				createPos.x += ToFloat(GetRand(ToInt(-logo.size.x), ToInt(logo.size.x)));
				createPos.y += ToFloat(GetRand(ToInt(-logo.size.y), ToInt(logo.size.y)));

				SpawnElecShock(createPos, elecBullet[j].type, ElecShockOriginType::None, -1, ElecShockOriginType::None, -1);
			}

			//hitTimerを起動
			creditsButton.hitTimer.count = creditsButton.hitTimer.time;

			//モード切替
			ChangeModeByCredits();
		}

		//CreditsButton
		if (CheckCollisionCircleVSCircle(elecBullet[j].pos, elecBullet[j].hitRadius, resetButton.pos, resetButton.hitRadius)) {
			BreakElecBullet(j);

			//ホウデンの生成
			for (int i = 0; i < 5; i++) {
				SpawnElecShock(spawnShockPos[i], elecBullet[j].type, ElecShockOriginType::None, -1, ElecShockOriginType::None, -1);

				Vector2 createPos = resetButton.pos;
				createPos.x += ToFloat(GetRand(ToInt(-logo.size.x), ToInt(logo.size.x)));
				createPos.y += ToFloat(GetRand(ToInt(-logo.size.y), ToInt(logo.size.y)));

				SpawnElecShock(createPos, elecBullet[j].type, ElecShockOriginType::None, -1, ElecShockOriginType::None, -1);
			}

			//hitTimerを起動
			resetButton.hitTimer.count = resetButton.hitTimer.time;

			//モード切替
			ChangeModeByReset();
		}
	}
}

static void UpdateButton(void) {
	CheckCollisionButtonVSBullet();

	if (creditsButton.hitTimer.count > 0) {
		creditsButton.hitTimer.count--;
	}
	if (resetButton.hitTimer.count > 0) {
		resetButton.hitTimer.count--;
	}

#pragma region 振動処理

	creditsButton.showPos = creditsButton.pos;

	if (creditsButton.hitTimer.count > 0) {
		creditsButton.showPos.x += GetRand(-creditsButton.shakeDurationHit, creditsButton.shakeDurationHit);
		creditsButton.showPos.y += GetRand(-creditsButton.shakeDurationHit, creditsButton.shakeDurationHit);
	} else if (nowMode != ModeTitle::MovePlayer) {
		creditsButton.shakeTimer.count++;
		if (creditsButton.shakeTimer.count >= creditsButton.shakeTimer.time) {
			creditsButton.shakeTimer.count = 0;

			creditsButton.showPos.x += GetRand(-creditsButton.shakeDuration, creditsButton.shakeDuration);
			creditsButton.showPos.y += GetRand(-creditsButton.shakeDuration, creditsButton.shakeDuration);
		}
	}

	resetButton.showPos = resetButton.pos;

	if (resetButton.hitTimer.count > 0) {
		resetButton.showPos.x += GetRand(-resetButton.shakeDurationHit, resetButton.shakeDurationHit);
		resetButton.showPos.y += GetRand(-resetButton.shakeDurationHit, resetButton.shakeDurationHit);
	} else if (nowMode != ModeTitle::MovePlayer) {
		resetButton.shakeTimer.count++;
		if (resetButton.shakeTimer.count >= resetButton.shakeTimer.time) {
			resetButton.shakeTimer.count = 0;

			resetButton.showPos.x += GetRand(-resetButton.shakeDuration, resetButton.shakeDuration);
			resetButton.showPos.y += GetRand(-resetButton.shakeDuration, resetButton.shakeDuration);
		}
	}

#pragma endregion

#pragma region 残像の処理
	if (nowMode == ModeTitle::Recommend || nowMode == ModeTitle::MovePlayer) return;

#pragma region CreditsButton
	if (creditsButton.isScaleUp) {
		creditsButton.easingT += creditsButton.easingSpeed;
		if (creditsButton.easingT >= 1.0f) {
			creditsButton.easingT = 1.0f;
			creditsButton.isScaleUp = false;
		}
	} else {
		creditsButton.easingT -= creditsButton.easingSpeed;
		if (creditsButton.easingT < 0.0f) {
			creditsButton.easingT = 0.0f;
			creditsButton.isScaleUp = true;
		}
	}

	creditsButton.afterimageSize = MoveByEasing(creditsButton.startSize, creditsButton.endSize, creditsButton.easingT, creditsButton.easingMode);
	creditsButton.alpha = UpdateValueByEasing(creditsButton.alphaMin, creditsButton.alphaMax, creditsButton.easingT, creditsButton.easingMode);

#pragma endregion

#pragma region resetButton
	if (resetButton.isScaleUp) {
		resetButton.easingT += resetButton.easingSpeed;
		if (resetButton.easingT >= 1.0f) {
			resetButton.easingT = 1.0f;
			resetButton.isScaleUp = false;
		}
	} else {
		resetButton.easingT -= resetButton.easingSpeed;
		if (resetButton.easingT < 0.0f) {
			resetButton.easingT = 0.0f;
			resetButton.isScaleUp = true;
		}
	}

	resetButton.afterimageSize = MoveByEasing(resetButton.startSize, resetButton.endSize, resetButton.easingT, resetButton.easingMode);
	resetButton.alpha = UpdateValueByEasing(resetButton.alphaMin, resetButton.alphaMax, resetButton.easingT, resetButton.easingMode);

#pragma endregion

#pragma endregion

}

static void DrawButton(void) {
	Texture texture;

#pragma region Credits afterimage

	texture = creditsButton.texture;
	texture.color = (texture.color & 0xFFFFFF00) | creditsButton.alpha;
	DrawTextureUI(texture, creditsButton.pos, creditsButton.afterimageSize);

#pragma endregion

#pragma region CreditsButton

	//colorの変更
	texture = creditsButton.texture;
	if (nowMode == ModeTitle::Recommend || nowMode == ModeTitle::MovePlayer) {
		texture.color = creditsButton.lightOutColor;
	}

	DrawTextureUI(texture, creditsButton.showPos, creditsButton.size);

#pragma endregion

#pragma region Credits afterimage

	texture = resetButton.texture;
	texture.color = (texture.color & 0xFFFFFF00) | resetButton.alpha;
	DrawTextureUI(texture, resetButton.pos, resetButton.afterimageSize);

#pragma endregion

#pragma region resetButton

	//colorの変更
	texture = resetButton.texture;
	if (nowMode == ModeTitle::Recommend || nowMode == ModeTitle::MovePlayer) {
		texture.color = resetButton.lightOutColor;
	}

	DrawTextureUI(texture, resetButton.showPos, resetButton.size);

#pragma endregion

}

#pragma endregion

#pragma region 関数: 暗転パネル

static void InitBlackPanel(void) {
	blackPanel = {};
	blackPanel.texture = GetTexture(TextureType::White1x1);
	blackPanel.texture.color = blackPanel.color;
}

static void DrawBlackPanel(void) {
	if (!blackPanel.isShow) return;

	DrawTextureUI(blackPanel.texture, blackPanel.pos, blackPanel.size);
}

#pragma endregion

#pragma region 関数: フラッシュ演出

static void InitFlashPanel(void) {
	flashPanel = {};
	flashPanel.texture = GetTexture(TextureType::White1x1);
}

static void UpdateFlashPanel(void) {
	if (nowMode != ModeTitle::TriggerLightUp) return;

	flashPanel.easingT += flashPanel.easingSpeed;
	if (flashPanel.easingT >= 1.0f) {
		flashPanel.easingT = 1.0f;
		nowMode = ModeTitle::WaitInput;
	}

	if (flashPanel.easingT < flashPanel.startFadeOutEasingT) {
		float t = (flashPanel.easingT - flashPanel.startFadeOutEasingT) / (1.0f - flashPanel.startFadeOutEasingT);
		if (t > 1.0f) t = 1.0f;

		flashPanel.alpha = UpdateValueByEasing(flashPanel.alphaMin, flashPanel.alphaMax, t, flashPanel.easingMode);
	} else {
		float t = (flashPanel.easingT - flashPanel.startFadeOutEasingT) / (1.0f - flashPanel.startFadeOutEasingT);
		if (t > 1.0f) t = 1.0f;

		flashPanel.alpha = UpdateValueByEasing(flashPanel.alphaMax, flashPanel.alphaMin, t, flashPanel.easingMode);

		//暗転パネルの削除
		blackPanel.isShow = false;
	}

	if (flashPanel.easingT == flashPanel.startFadeOutEasingT) {
		PlaySe(SeType::Flash);
	}

}

static void DrawFlashPanel(void) {
	Texture texture = flashPanel.texture;
	texture.color = (texture.color & 0xFFFFFF00) | flashPanel.alpha;

	DrawTextureUI(texture, flashPanel.pos, flashPanel.size);
}

#pragma endregion

#pragma region 関数: Recommend関係

static void InitRecommend(void) {
	recommendIcon = {};
	recommendIcon.texture = GetTexture(TextureType::RecommendIcon);
}

static void UpdateRecommend(void) {
	if (nowMode != ModeTitle::Recommend) return;

#pragma region ボタン入力

	if (!recommendIcon.oncePushed) {
		if (CheckInputAction(InputAction::Confirm)) {
			recommendIcon.oncePushed = true;
			PlaySe(SeType::Confirm);
		}
	}

#pragma endregion

#pragma region アイコンの移動処理
	if (!recommendIcon.oncePushed) {
		if (recommendIcon.easingT >= 1.0f) return;

		recommendIcon.easingT += recommendIcon.easingSpeed;
		if (recommendIcon.easingT >= 1.0f) {
			recommendIcon.easingT = 1.0f;
		}
	} 
	else {
		recommendIcon.easingT -= recommendIcon.easingSpeedFrameOut;
		if (recommendIcon.easingT <= 0.0f) {
			recommendIcon.easingT = 0.0f;
			nowMode = ModeTitle::MovePlayer;
		}
	}

	recommendIcon.pos = MoveByEasing(recommendIcon.standbyPos, recommendIcon.showPos, recommendIcon.easingT, recommendIcon.easingMode);
	recommendIcon.alpha = UpdateValueByEasing(recommendIcon.alphaMin, recommendIcon.alphaMax, recommendIcon.easingT, recommendIcon.easingMode);

#pragma endregion
}

static void DrawRecommend(void) {
	if (nowMode != ModeTitle::Recommend) return;

	Texture texture = recommendIcon.texture;
	texture.color = (texture.color & 0xFFFFFF00) | recommendIcon.alpha;
	DrawTextureUI(texture, recommendIcon.pos, recommendIcon.size);
}

#pragma endregion

#pragma region 関数: Bgm関係

static void InitTitleBgm(void) {
	ChangeBgmVolume(lightOutVolume);
	PlayBgm(BgmType::Title);
}

#pragma endregion

void InitTitle(void) {
	if (nowMode != ModeTitle::Recommend) {
		nowMode = ModeTitle::MovePlayer;
	}
	InitTitleBgm();

	InitTitleBackground();

	InitRecommend();

	InitTitlePlayer();
	InitElecBullet();
	InitEffect();

	InitLogo();
	InitInst();
	InitButton();

	InitFlashPanel();
	InitBlackPanel();
}

void UpdateTitle(void) {
	UpdateTitlePlayer();
	UpdateElecBullet();
	UpdateEffect();

	UpdateRecommend();

	UpdateLogo();
	UpdateInst();
	UpdateButton();
	UpdateFlashPanel();

	UpdateTitleBackground();
}

void DrawTitle(void) {
	DrawTitleBackground();
	DrawBlackPanel();

	DrawElecBullet();
	
	DrawTitlePlayer();

	DrawLogo();
	DrawInst();
	DrawButton();

	DrawRecommend();

	DrawFlashPanel();
	DrawEffect();
}

#pragma region 関数: 外部参照

void TriggerTitleRecommend(void) {
	nowMode = ModeTitle::Recommend;
}

ModeTitle GetNowModeTitle(void) {
	return nowMode;
}

#pragma endregion
