#include "title_background.h"

#include "texture.h"
#include "vector2.h"
#include "common.h"
#include "tool.h"

#include <Novice.h>
#define _USE_MATH_DEFINES
#include <math.h>

#pragma region データ: 背景

struct Background {
	Vector2 pos = kWindowCenter;
	Vector2 size = { 1280, 720 };

	unsigned int color = 0x1A020AFF;

	Texture texture = {};
};
Background background;

#pragma endregion

#pragma region データ: 画面エフェクト
struct TitleDisplayEffect {
	Vector2 pos = kWindowCenter;
	Vector2 size = {};

	float easingT = 0.0f;
	float easingSpeed = 0.02f;
	EasingMode easingMode = EasingMode::EaseInOut;
	bool isScaleDown = true;

	Vector2 endSize = { 1280, 720 };
	Vector2 startSize = { 1440, 810 };

	int alpha = 30;
	int alphaMin = 30;
	int alphaMax = 100;

	int color = BLACK;

	Texture texture = {};
};
TitleDisplayEffect displayEffect;

#pragma endregion

#pragma region データ: 走査線

static constexpr int kScanLineLimit = 21;

struct ScanLine {
	Vector2 pos = {};
	Vector2 size = { 1280, 2 };

	float moveSpeed = 1.0f;
	float posYMin = -0.2f;
	float posYMax = 722;

	float intervalY = 36.0f;

	int shakeDuration = 1;

	unsigned int color = 0x340314FF;

};
static ScanLine scanLine[kScanLineLimit];
static Texture scanLineTexture = {};

#pragma endregion

#pragma region データ: 図形

struct TitleShapeSpawner {
	float spawnPosX = 1350;
	int spawnPosYMin = 0;
	int spawnPosYMax = 720;

	Timer spawnTimer = { 120, 120 };
};
TitleShapeSpawner shapeSpawner;

static constexpr int kTitleShapeLimit = 20;
struct TitleShape {
	Vector2 pos = {};
	Vector2 size = { 128,128 };

	bool isShoot = false;

	float moveSpeed = 4.0f;
	float breakPosX = -70;

	int shakeDuration = 3;

	float rotateTheta = 0.0f;
	float rotateSpeed = ToFloat(M_PI) / 60.0f;

	int alpha = 20;

	//残像の位置
	Vector2 prePos[2] = {};
	float preRotateTheta[2] = {};
	int diffAlpha = 5;

	unsigned int color = RED;

	Texture texture = {};
};
TitleShape titleShape[kTitleShapeLimit];

Texture titleTriangleTexture = {};
Texture titleBoxTexture = {};
Texture titleCircleTexture = {};

unsigned int playerColor = 0x04C1E8FF;
unsigned int enemyColor = 0xE01068FF;

#pragma endregion

#pragma region 関数: 背景

static void InitBackground(void) {
	background = {};
	background.texture = GetTexture(TextureType::White1x1);

	for (int i = 0; i < kScanLineLimit; i++) {
		scanLine[i] = {};
		scanLine[i].pos = { 640, scanLine[i].intervalY * i };
	}
	scanLineTexture = GetTexture(TextureType::TitleLine);
}

static void DrawBackground(void) {
	//背景
	Texture texture = background.texture;
	texture.color = background.color;
	DrawTextureUI(texture, background.pos, background.size);
}

#pragma endregion

#pragma region 関数: 走査線


static void UpdateScanLine(void) {
	for (int i = 0; i < kScanLineLimit; i++) {
		scanLine[i].pos.y += scanLine[i].moveSpeed;

		if (scanLine[i].pos.y >= scanLine[i].posYMax) {
			scanLine[i].pos.y = scanLine[i].posYMin;
		}
	}
}

static void DrawScanLine(void) {
	for (int i = 0; i < kScanLineLimit; i++) {
		Vector2 pos;
		pos.y = scanLine[i].pos.y + ToFloat(GetRand(-scanLine[i].shakeDuration, scanLine[i].shakeDuration));

		Texture texture = scanLineTexture;
		texture.color = scanLine[i].color;

		DrawTextureUI(texture, scanLine[i].pos, scanLine[i].size);
	}
}

#pragma endregion

#pragma region 関数: 図形
static void InitTitleShape(void) {
	for (int i = 0; i < kTitleShapeLimit; i++) {
		titleShape[i] = {};
	}

	titleTriangleTexture = GetTexture(TextureType::TitleTriangle);
	titleBoxTexture = GetTexture(TextureType::TitleBox);
	titleCircleTexture = GetTexture(TextureType::TitleCircle);
}

static void SpawnTitleShape(void) {
	for (int i = 0; i < kTitleShapeLimit; i++) {
		if (titleShape[i].isShoot) continue;

		titleShape[i].isShoot = true;

		titleShape[i].pos.x = shapeSpawner.spawnPosX;
		titleShape[i].pos.y = ToFloat(GetRand(shapeSpawner.spawnPosYMin, shapeSpawner.spawnPosYMax));

		//ランダムで形を変換
		int randomNum = GetRand(0, 2);
		switch (randomNum) {
		case 0:
			titleShape[i].texture = titleTriangleTexture;
			break;
		case 1:
			titleShape[i].texture = titleBoxTexture;
			break;
		case 2:
			titleShape[i].texture = titleCircleTexture;
			break;
		}

		randomNum = GetRand(0, 1);
		switch (randomNum) {
		case 0:
			titleShape[i].color = playerColor;
			break;
		case 1:
			titleShape[i].color = enemyColor;
			break;
		}

		break;
	}
}

static void UpdateTitleShape(void) {
	for (int i = 0; i < kTitleShapeLimit; i++) {
		if (!titleShape[i].isShoot) continue;

		//残像のデータ更新
		titleShape[i].prePos[1] = titleShape[i].prePos[0];
		titleShape[i].prePos[0] = titleShape[i].pos;
		titleShape[i].preRotateTheta[1] = titleShape[i].preRotateTheta[0];
		titleShape[i].preRotateTheta[0] = titleShape[i].rotateTheta;

		titleShape[i].pos.x += -titleShape[i].moveSpeed;
		if (titleShape[i].pos.x <= titleShape[i].breakPosX) {
			titleShape[i].isShoot = false;
		}

		titleShape[i].rotateTheta += titleShape[i].rotateSpeed;
	}

	//生成処理
	shapeSpawner.spawnTimer.count++;
	if (shapeSpawner.spawnTimer.count >= shapeSpawner.spawnTimer.time) {
		shapeSpawner.spawnTimer.count = 0;

		SpawnTitleShape();
	}
}

static void DrawTitleShape(void) {
	for (int i = 0; i < kTitleShapeLimit; i++) {
		if (!titleShape[i].isShoot) continue;

		Vector2 pos;
		pos.x = titleShape[i].pos.x + GetRand(-titleShape[i].shakeDuration, titleShape[i].shakeDuration);
		pos.y = titleShape[i].pos.y + GetRand(-titleShape[i].shakeDuration, titleShape[i].shakeDuration);

		Texture texture = titleShape[i].texture;
		texture.color = (titleShape[i].color & 0xFFFFFF00) | titleShape[i].alpha;

		DrawTextureRotateUI(texture, titleShape[i].pos, titleShape[i].size, titleShape[i].rotateTheta);

#pragma region 残像
	
		for (int j = 0; j < 2; j++) {
			texture.color = (titleShape[i].color & 0xFFFFFF00) | (titleShape[i].alpha - titleShape[i].diffAlpha * j);

			DrawTextureRotateUI(texture, titleShape[i].prePos[j], titleShape[i].size, titleShape[i].preRotateTheta[j]);
		}

#pragma endregion

	}
}

#pragma endregion

#pragma region 関数: 画面エフェクト
static void InitDisplayEffect(void) {
	displayEffect = {};
	displayEffect.texture = GetTexture(TextureType::DisplayEffect);
}

static void UpdateDisplayEffect(void) {
	//イージング処理
	if (displayEffect.isScaleDown) {
		displayEffect.easingT += displayEffect.easingSpeed;
		if (displayEffect.easingT >= 1.0f) {
			displayEffect.easingT = 1.0f;
			displayEffect.isScaleDown = false;
		}
	} else {
		displayEffect.easingT -= displayEffect.easingSpeed;
		if (displayEffect.easingT <= 0.0f) {
			displayEffect.easingT = 0.0f;
			displayEffect.isScaleDown = true;
		}
	}

	//サイズ変更
	displayEffect.size = MoveByEasing(displayEffect.startSize, displayEffect.endSize, displayEffect.easingT, displayEffect.easingMode);

	//不透明度変更
	displayEffect.alpha = UpdateValueByEasing(displayEffect.alphaMin, displayEffect.alphaMax, displayEffect.easingT, displayEffect.easingMode);
}

static void DrawDisplayEffect(void) {
	Texture texture = displayEffect.texture;
	texture.color = (displayEffect.color & 0xFFFFFF00) | displayEffect.alpha;

	DrawTextureUI(texture, displayEffect.pos, displayEffect.size);
}

#pragma endregion

void InitTitleBackground(void) {
	InitBackground();
	InitTitleShape();
	InitDisplayEffect();
}

void UpdateTitleBackground(void) {
	UpdateScanLine();
	UpdateTitleShape();
	UpdateDisplayEffect();
}

void DrawTitleBackground(void) {
	DrawBackground();

	DrawTitleShape();

	DrawScanLine();
	DrawDisplayEffect();
}