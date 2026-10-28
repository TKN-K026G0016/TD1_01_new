
#include "effect.h"
#include "player.h"
#include "title_player.h"
#include "vector2.h"
#include "texture.h"
#include "play_ui.h"
#include "timer.h"
#include "tool.h"
#include "scene_manager.h"
#include "common.h"
#include "se.h"

#include <Novice.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <time.h>

#pragma region データ: 共通

static Scene nowScene = {};

Texture whiteBoxTexture = {};
Texture triangleTexture = {};

constexpr int kAlphaMax = 255;
constexpr int kAlphaMin = 0;

unsigned int kPlayerColor = 0x04C1E8FF;
unsigned int kPlayerChargedColor = 0x5004E8FF;
unsigned int kEnemyColor = 0xE01068FF;

#pragma endregion

#pragma region データ: ジェム取得エフェクト

const int KJemEffectNumber = 10;
struct JemEffect {
	Vector2 pos = { };
	Vector2 newPos = { };
	float theta = { };
	Vector2  size = { };
	unsigned int color = { 0xffffffff };
	bool isShot = { false };
	Texture texture = {};
};
JemEffect jemEffect[KJemEffectNumber];

#pragma endregion

#pragma region データ: ゲージエフェクト

const int KGageEffectNumber = 20;
struct GageEffect {
	Vector2 pos = { };

	Vector2 acceleration = { };
	Vector2 size = { 8,8 };

	//<回転関係>
	float rotateTheta = 0.0f;
	float rotateSpeed = ToFloat(M_PI) / 14.0f;

	bool appear = false;

	//<イージング関係>
	float easingT = 0.0f;
	float easingSpeed = 0.08f;

	//<不透明度>
	int alpha = kAlphaMax;

	unsigned int color = { };
};
GageEffect gageEffect[KGageEffectNumber];

#pragma endregion

#pragma region データ: ShootLaserEffect

static constexpr int kShootLaserEffectLimit = 30;

//生成時のブレ度合い
static constexpr int kShootLaserEffectSpreadTheta = 15;
//初速度の範囲
static constexpr int kShootLaserEffectFirstSpeedRange[2] = { 25, 40 };

//初速度
static constexpr float kShootLaserEffectFistSpped = 20.0f;
//shootLaserEffectの重力の強さ
static constexpr float kShootLaserEffectGravity = 0.03f;

struct ShootLaserEffect {
	Vector2 pos = { };
	//Vector2 size = { 12,12 };

	bool isShoot = false;

	//移動関係
	//Vector2 velocity = {};
	float moveSpeed = 0.0f;
	float moveTheta = 0.0f;

	//<回転関係>
	float rotateTheta = 0.0f;
	float rotateSpeed = ToFloat(M_PI) / 14.0f;


	//<イージング関係>
	float easingT = 0.0f;
	float easingSpeed = 0.08f;
	EasingMode easingMode = EasingMode::EaseIn;

	//<サイズ関係>
	float sizeValue = 0.0f;
	float sizeValueMin = 8.0f;
	float sizeValueMax = 20.0f;

	//<不透明度>
	int alpha = kAlphaMax;

	unsigned int color = kEnemyColor;
};
ShootLaserEffect shootLaserEffect[kShootLaserEffectLimit];

#pragma endregion

#pragma region データ: 煙エフェクト
//煙エフェクトの数
static constexpr int kSmokeEffectLimit = 10;
//残像の数
static constexpr int kSmokeAfterimageLimit = 2;

struct SmokeEffect {
	Vector2 pos = {};

	TypeDrawEffect drawType = TypeDrawEffect::Obj;

	bool isShoot = false;

	//移動速度
	float moveSpeed = 5.0f;

	//<サイズ関係>
	float firstSizeValue = 0;
	float sizeValue = firstSizeValue;
	float changeSizeSpeed = 0;

	//<回転関係>
	float rotateTheta = 0.0f;
	float rotateSpeed = ToFloat(M_PI) / 20.0f;

	//透明度関係
	int alpha = 220;
	int alphaMax = 220;
	int alphaMin = 0;
	int changeAlphaSpeed = 12;

	//<残像関係>
	float preSizeValue[kSmokeAfterimageLimit] = {};
	float preRotateTheta[kSmokeAfterimageLimit] = {};
	//本体との座標のずれ
	float posOffset = 10;
	//本体との透明度のズレ
	int diffAlpha = 30;

	Texture texture = {};
	unsigned int color = WHITE;
};
SmokeEffect smokeEffect[kSmokeEffectLimit];

struct PlayerSmoke {
	float moveSpeed = 5.0f;
	float firstSizeValue = 8;
	float changeSizeSpeed = 4;
	int changeAlphaSpeed = 12;

	float posOffset = 10;

	unsigned int color = 0x555555FF;
};
PlayerSmoke playerSmoke;

struct EnemyBombSmoke {
	float moveSpeed = 7.0f;
	float firstSizeValue = 10;
	float changeSizeSpeed = 4;
	int changeAlphaSpeed = 8;

	float posOffset = 20;

	unsigned int color = WHITE;
};
EnemyBombSmoke enemyBombSmoke;


#pragma endregion

#pragma region データ: Shockエフェクト

struct ElecShockEffect {
	int createTriangleLimit = 3;
	float triangleSizeValue = 20.0f;
	float triangleMoveSpeed = 30.0f;

	float circleSize = 150.0f;
};
ElecShockEffect elecShockEffect;

struct ElecShockChargedEffect {
	int createTriangleLimit = 6;
	float triangleSizeValue = 30.0f;
	float triangleMoveSpeed = 50.0f;

	float circleSize = 250.0f;
};
ElecShockChargedEffect elecShockChargedEffect;

constexpr int kElecLineEffectLimit = 100;
struct ElecLineEffect {

	bool charged = false;

	bool isShoot = false;

	Vector2 startPos = {};
	Vector2 endPos = {};

	Vector2 centerPos = {};
	float rotateTheta = 0.0f;

	//ラインの長さ
	float length = 0.0f;
	//ラインの太さ
	float thickness = 16.0f;
	float thicknessCharged = 23.0f;

	int alpha = 90;

	Timer breakTimer = { 10, 0 };

	Texture texture = {};
};
ElecLineEffect elecLineEffect[kElecLineEffectLimit];


#pragma endregion

#pragma region データ: ChargeOrbエフェクト

//ChargeOrbエフェクト
static constexpr int kChargeOrbEffectLimit = 8;

struct ChargeOrbEffect {
	Vector2 pos = {};

	bool isShoot = false;

	//<イージング用>
	float easingT = 0.0f;
	float easingSpeed = 0.05f;
	float easingTFadeIn = 0.5f;

	//<サイズ関係>
	float startSizeValue = 50.0f;
	float endSizeValue = 0.1f;
	float sizeValue = 0.0f;

	//<透明度関係>
	int alphaMin = 0;
	int alphaMax = 255;
	int alpha = 0;

	Texture texture = {};
	unsigned int color = kPlayerChargedColor;
};
ChargeOrbEffect chargeOrbEffect[kChargeOrbEffectLimit];

#pragma endregion

#pragma region データ: ChargeChipEffect

static constexpr int kChargeChipEffecLimit = 4;

struct ChargeChipEffect {
	Vector2 pos = {};

	bool isShoot = false;

	//イージング
	float easingT = 0.0f;
	float easingSpeed = 0.03f;
	float easingTFadeIn = 0.25f;

	//サイズ
	float startSizeValue = 15.0f;
	float endSizeValue = 0.1f;
	float sizeValue = 0.0f;

	//<回転関係>
	float rotateTheta = 0.0f;
	float rotateSpeed = ToFloat(M_PI) / 10.0f;

	//<公転関係>
	float revolutionTheta = 0.0f;
	float revolutionSpeed = ToFloat(M_PI) / 12.0f;

	float startTargetVecLength = 60.0f;
	float endTargetVecLength = 1.0f;
	float targetVecLength = 0.0f;

	//<透明度関係>
	int alphaMin = 0;
	int alphaMax = 255;
	int alpha = 0;

	Texture texture = {};
};
ChargeChipEffect chargeChipEffect[kChargeChipEffecLimit];

#pragma endregion

#pragma region データ: 爆発エフェクト

constexpr int kExplodeEffectLimit = 30;
struct ExplodeEffect {
	Vector2 pos = {};
	Vector2 size = { 128, 128 };

	bool isShoot = false;

	unsigned int color = WHITE;

	Texture texture = {};
};
ExplodeEffect explodeEffect[kExplodeEffectLimit];

#pragma endregion

#pragma region データ: triangleEffect

const int kTriangleEffectNumber = 80;
struct TriangleEffect {
	Vector2 pos = { };
	Vector2 size = { 26,22 };

	//表示フラグ
	bool appear = false;

	//加速度
	Vector2 acceleration = { };
	//回転量
	float rotateTheta = 0.0f;
	float rotateSpeed = ToFloat(M_PI) / 20.0f;

	float easingT = 0.0f;
	float easingSpeed = 0.4f;
	//透明化が始まるタイミング
	float startFadeEasingT = 0.0f;

	//不透明度
	int alpha = kAlphaMax;

	//色
	unsigned int color = { };

	//Texture texture = {};

};
TriangleEffect triangleEffect[kTriangleEffectNumber];


#pragma endregion

#pragma region データ: triangleEffect2

//等速直線運動
struct TriangleEffect2 {
	Vector2 pos = { };

	//表示フラグ
	bool appear = false;
	//表示タイプ
	TypeDrawEffect drawType = TypeDrawEffect::Obj;

	float sizeValue = 0.0f;

	//<移動関係>
	float moveTheta = 0.0f;
	float moveSpeed = 0.0f;

	//<回転関係>
	float rotateTheta = 0.0f;
	float rotateSpeed = ToFloat(M_PI) / 12.0f;

	//<easing関係>
	float easingT = 0.0f;
	float easingSpeed = 0.09f;
	//透明化が始まるタイミング
	float startFadeEasingT = 0.1f;

	//不透明度
	int alpha = kAlphaMax;

	//色
	unsigned int color = { };

};
TriangleEffect2 triangleEffect2[kTriangleEffectNumber];


#pragma endregion

#pragma region データ: circleEffect

const int kCircleEffectNumber = 80;
struct CircleEffect {
	Vector2 pos = { };
	//Vector2 size = { 256,256 };

	bool appear = false;

	//イージング関係
	float easingT = 0.0f;
	float easingSpeed = 0.1f;
	float easingSpeedDefault = 0.1f;
	//透明化が始まるタイミング
	float startFadeEasingT = 0.1f;

	//サイズ関係
	float sizeValue = 0.0f;
	float startSizeValue = 0.0f;
	float endSizeValue = 0.0f;

	//透明度関係
	int alpha = kAlphaMax;

	unsigned int color = WHITE;
	Texture texture = {};
};
CircleEffect circleEffect[kCircleEffectNumber];


#pragma endregion

#pragma region データ; PlayerDamageEffect
struct PlayerDamageEffect {
	int createTriangleLimit = 4;
	float triangleSizeValue = 25.0f;
	float triangleMoveSpeed = 20.0f;
};
PlayerDamageEffect playerDamageEffect;

#pragma endregion

#pragma region データ: EnemyDamageEffect

struct EnemyDamageEffect {
	int createTriangleLimit = 1;
	float triangleSizeValue = 30.0f;
	float triangleMoveSpeed = 30.0f;

};
EnemyDamageEffect enemyDamageEffect;

#pragma endregion

#pragma region データ: PlayerMoveEffect

static constexpr int kPlayerMoveEffectLimit = 7;
static Timer createPlayerMoveEffectTimer = { 7, 0 };
//ブレる量
int playerMoveEffectShakeAmp = 10;

struct PlayerMoveEffect {
	Vector2 pos = {};

	bool isShoot = false;

	//移動関係
	float moveTheta = 0.0f;

	//回転関係
	float rotateTheta = 0.0f;
	float rotateSpeed = ToFloat(M_PI) / 20.0f;

	//<イージング関係>
	float easingT = 0;
	float easingSpeed = 0.03f;
	EasingMode easingMode = EasingMode::EaseOut;

	//サイズ関係
	Vector2 size = {};
	Vector2 sizeMax = { 15, 15 };
	Vector2 sizeMin = { 2,2 };

	//移動速度関係
	float moveSpeed = 0;
	float moveSpeedMax = 8.0f;
	float moveSpeedMin = 0.1f;

	//透明度関係
	int alpha = 0;
	int alphaMax = 80;
};
static PlayerMoveEffect playerMoveEffect[kPlayerMoveEffectLimit];


#pragma endregion

#pragma region データ: RecoveryEnergyEffect

static constexpr int kRecoveryEnergyEffectLimit = 20;
//残像の数
static constexpr int kRecoveryEffectAfterimageLimit = 2;

//何個生成するか
static constexpr int kCreateRecoveryEffectLimit = 3;
static int createRecoveryEffectCount = 0;
//生成中フラグ
static bool isCreateRecoveryEffect = false;
static Timer createRecoveryEffectTimer = { 5, 0 };

//recoveryEffectを生成する範囲
static constexpr Vector2i kCreateRecoveryEffectPosRange = { 30, 20 };


struct RecoveryEnergyEffect {
	Vector2 pos = {};
	

	bool isShoot = false;

	//移動速度
	float moveSpeed = 8.0f;

	//<回転関係>
	float rotateTheta = 0.0f;
	float rotateSpeed = ToFloat(M_PI) / 20.0f;

	//<イージング関係>
	float easingT = 0.0f;
	float easingSpeed = 0.04f;
	EasingMode easingMode = EasingMode::EaseOut;

	//サイズ関係
	Vector2 size = { 16, 16 };
	Vector2 sizeMax = { 40, 40 };
	Vector2 sizeMin = { 16,16 };

	//透明度関係
	int alpha = 0;
	int alphaMax = 180;
	int alphaMin = 0;

	//<残像関係>
	float preRotateTheta[kRecoveryEffectAfterimageLimit] = {};
	//本体との座標のずれ
	float posOffset = 10;
	//本体との透明度のズレ
	int diffAlpha = 30;
};
static RecoveryEnergyEffect recoveryEffect[kRecoveryEnergyEffectLimit];

#pragma endregion

#pragma region データ: BreakGemEffect

struct BreakGemEffect {
	float endSizeValue = 100;

	float easingSpeed = 0.05f;

	unsigned int color = WHITE;
};
BreakGemEffect breakGemEffect;

#pragma endregion

#pragma region データ: SpawnGemEffect

struct SpawnGemEffect {
	float endSizeValue = 90;

	float easingSpeed = 0.05f;

	unsigned int color = kPlayerColor;
};
SpawnGemEffect spawnGemEffect;

#pragma endregion

#pragma region データ: BombExplosionEffect
//爆発本体
const int kBombExplosionEffectNumber = 4;
struct BombExplosionEffect {
	Vector2 pos;
	bool appear = false;

	//イージング関係
	float easingT = 0.0f;
	float easingSpeed = 0.1f;
	//透明化が始まるタイミング
	float startFadeEasingT = 0.1f;

	//サイズ関係
	float sizeValue = 0.0f;
	float startSizeValue = 0.0f;
	float endSizeValue = 0.0f;

	//透明度関係
	int alpha = kAlphaMax;

	unsigned int color = WHITE;
	Texture texture = {};
};
BombExplosionEffect bombExplosionEffect[kBombExplosionEffectNumber] = {};
//爆風
const int kBombBlastEffectNumber = 4;
struct BombBlastEffect {
	Vector2 pos;
	bool appear = false;

	//イージング関係
	float easingT = 0.0f;
	float easingSpeed = 0.1f;
	//透明化が始まるタイミング
	float startFadeEasingT = 0.1f;

	//サイズ関係
	float sizeValue = 0.0f;
	float startSizeValue = 0.0f;
	float endSizeValue = 0.0f;

	//透明度関係
	int alpha = kAlphaMax;

	unsigned int color = WHITE;
	Texture texture = {};
};
BombBlastEffect bombBlastEffect[kBombExplosionEffectNumber][kBombBlastEffectNumber] = {};
#pragma endregion

#pragma region データ: フラッシュパネル

struct FlashPanel {
	Vector2 pos = kWindowCenter;
	Vector2 size = { 1280, 720 };

	bool isShoot = false;

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
static FlashPanel flashPanel;

#pragma endregion

#pragma region データ: barrierbreak

const int kBarrierBreakNumber = 6;
struct  BarrierBreakEffect {
	Vector2 pos = {};
	Vector2 size = { 30,30 };

	//表示フラグ
	bool appear = false;

	//加速度
	Vector2 acceleration = {};

	float gravityVelocity = 0.01f;

	//回転量
	float rotateTheta = 0.0f;
	float rotateSpeed = ToFloat(M_PI) / 20.0f;

	float easingT = 0.0f;
	float easingSpeed = 0.02f;
	//透明化が始まるタイミング
	float startFadeEasingT = 0.0f;

	//不透明度
	int alpha = kAlphaMax;

	//色
	unsigned int color = {};

	Texture texture = {};

};
BarrierBreakEffect barrierBreakEffect[kBarrierBreakNumber];


#pragma endregion


#pragma region 関数: ジェム取得effect

void CreateJemEffect() {
	for (int i = 0; i < KJemEffectNumber; i++) {
		if (jemEffect[i].isShot == false) {
			jemEffect[i].isShot = true;
			jemEffect[i].pos = GetPlayerPos();
			jemEffect[i].theta = static_cast<float>((M_PI) / 4);
			jemEffect[i].size.x = 0;
			jemEffect[i].size.y = 0;
			jemEffect[i].color = 0xffffffff;
			break;
		}
	}
}

static void InitGemEffect(void) {
	for (int i = 0; i < KJemEffectNumber; i++) {
		jemEffect[i].texture = GetTexture(TextureType::EffectSquare);
	}
}

static void UpdateGemEffect(void) {
	for (int i = 0; i < KJemEffectNumber; i++) {
		if (jemEffect[i].isShot == true) {
			if (jemEffect[i].size.x <= 128 && jemEffect[i].size.y <= 128) {
				jemEffect[i].size.y += 20;
				jemEffect[i].size.x += 20;
			}


			jemEffect[i].newPos.x = jemEffect[i].pos.x + (-(jemEffect[i].size.x / 2)) * cosf(jemEffect[i].theta) - (-(jemEffect[i].size.x / 2)) * sinf(jemEffect[i].theta);
			jemEffect[i].newPos.y = jemEffect[i].pos.y + (-(jemEffect[i].size.y / 2)) * cosf(jemEffect[i].theta) + (-(jemEffect[i].size.y / 2)) * sinf(jemEffect[i].theta);

			if (jemEffect[i].color >= 0xffffff10) {
				if (jemEffect[i].size.x < 128 && jemEffect[i].size.y < 128) {
					jemEffect[i].color -= 0x00000005;
				} else {
					jemEffect[i].color -= 0x00000015;
				}
				jemEffect[i].texture.color = jemEffect[i].color;
			} else {
				jemEffect[i].color = 0xffffffff;
				jemEffect[i].isShot = false;
			}
		}
	}
}

void DrawGetEffect() {
	for (int i = 0; i < KJemEffectNumber; i++) {
		if (jemEffect[i].isShot == true) {
			DrawTextureRotateObj(jemEffect[i].texture, jemEffect[i].pos, jemEffect[i].size, jemEffect[i].theta);
		}
	}
}

#pragma endregion

#pragma region 関数: ゲージeffect
void CreateGageEffect(Vector2 pos, unsigned int color) {
	for (int i = 0; i < KGageEffectNumber; i++) {
		if (gageEffect[i].appear) continue;

		gageEffect[i].appear = true;

		gageEffect[i].pos = pos;
		gageEffect[i].color = color;

		gageEffect[i].rotateTheta = 0.0f;
		gageEffect[i].easingT = 0.0f;

		float randomSpeedX = ToFloat(GetRand(0, 25));
		float randomSpeedY = ToFloat(GetRand(-10, 10));
		gageEffect[i].acceleration.x = randomSpeedX;
		gageEffect[i].acceleration.y = randomSpeedY;

		break;
	}

}


static void InitGageEffect(void) {
	for (int i = 0; i < KGageEffectNumber; i++) {
		gageEffect[i] = {};
	}
}

void UodateGageEffect() {
	for (int i = 0; i < KGageEffectNumber; i++) {
		if (!gageEffect[i].appear) continue;

		//移動処理
		gageEffect[i].pos.x += gageEffect[i].acceleration.x;
		gageEffect[i].pos.y += gageEffect[i].acceleration.y;
		gageEffect[i].acceleration.y -= 0.5f;

		//回転処理
		gageEffect[i].rotateTheta = gageEffect[i].rotateSpeed;

		//イージング
		gageEffect[i].easingT += gageEffect[i].easingSpeed;
		if (gageEffect[i].easingT >= 1.0f) {
			gageEffect[i].easingT = 1.0f;
			gageEffect[i].appear = false;
		}

		//透明化
		gageEffect[i].alpha = UpdateValueByEasing(kAlphaMax, kAlphaMin, gageEffect[i].easingT, EasingMode::EaseIn);

		////透明化(消滅)
		//if (gageEffect[i].color >= 0xffffff05) {
		//	gageEffect[i].color -= 0x00000015;
		//	gageEffect[i].texture.color = gageEffect[i].color;
		//} else {
		//	gageEffect[i].color = 0xffffffff;
		//	gageEffect[i].appear = false;
		//}
	}
}

void DrawGageEffect() {
	for (int i = 0; i < KGageEffectNumber; i++) {
		if (!gageEffect[i].appear) continue;

		Texture texture = whiteBoxTexture;
		texture.color = (gageEffect[i].color & 0xFFFFFF00) | gageEffect[i].alpha;

		DrawTextureRotateUI(texture, gageEffect[i].pos, gageEffect[i].size, gageEffect[i].rotateTheta);

	}
}
#pragma endregion

#pragma region 関数: ShootLaserEffect

static void InitShootLaserEffect(void) {
	for (int i = 0; i < kShootLaserEffectLimit; i++) {
		shootLaserEffect[i] = {};
	}
}

static void UpdateShootLaserEffect(void) {
	for (int i = 0; i < kShootLaserEffectLimit; i++) {
		if (!shootLaserEffect[i].isShoot) continue;
		//移動処理
		shootLaserEffect[i].moveSpeed -= kShootLaserEffectGravity;
		if (shootLaserEffect[i].moveSpeed < 0.0f) {
			shootLaserEffect[i].moveSpeed = 0.0f;
		}

		shootLaserEffect[i].pos.x += cosf(shootLaserEffect[i].moveTheta) * shootLaserEffect[i].moveSpeed;
		shootLaserEffect[i].pos.y += sinf(shootLaserEffect[i].moveTheta) * shootLaserEffect[i].moveSpeed;

		//回転処理
		shootLaserEffect[i].rotateTheta += shootLaserEffect[i].rotateSpeed;

		//イージング
		shootLaserEffect[i].easingT += shootLaserEffect[i].easingSpeed;
		if (shootLaserEffect[i].easingT >= 1.0f) {
			shootLaserEffect[i].easingT = 1.0f;
			shootLaserEffect[i].isShoot = false;
		}

		//サイズ変更
		shootLaserEffect[i].sizeValue = UpdateValueByEasing(shootLaserEffect[i].sizeValueMax, shootLaserEffect[i].sizeValueMin, shootLaserEffect[i].easingT, shootLaserEffect[i].easingMode);
		//透明度変更
		shootLaserEffect[i].alpha = UpdateValueByEasing(kAlphaMax, kAlphaMin, shootLaserEffect[i].easingT, shootLaserEffect[i].easingMode);
	}
}


static void DrawShootLaserEffect(void) {
	for (int i = 0; i < kShootLaserEffectLimit; i++) {
		if (!shootLaserEffect[i].isShoot) continue;

		Vector2 size = { shootLaserEffect[i].sizeValue, shootLaserEffect[i].sizeValue };

		Texture texture = whiteBoxTexture;
		texture.color = (shootLaserEffect[i].color & 0xFFFFFF00) | shootLaserEffect[i].alpha;
		DrawTextureRotateObj(texture, shootLaserEffect[i].pos, size, shootLaserEffect[i].rotateTheta);
	}
}

void CreateShootLaserEffect(Vector2 pos, float shootTheta) {
	for (int i = 0; i < kShootLaserEffectLimit; i++) {
		if (shootLaserEffect[i].isShoot) continue;

		shootLaserEffect[i].isShoot = true;
		shootLaserEffect[i].pos = pos;

		//角度設定
		float spreadTheta = ToFloat(GetRand(-kShootLaserEffectSpreadTheta, kShootLaserEffectSpreadTheta)) * ToFloat(M_PI) / 180.0f;
		shootLaserEffect[i].moveTheta = shootTheta + spreadTheta;

		//初速度設定
		float speed = ToFloat(GetRand(kShootLaserEffectFirstSpeedRange[0], kShootLaserEffectFirstSpeedRange[1]));
		shootLaserEffect[i].moveSpeed = speed;

		//初期化
		shootLaserEffect[i].easingT = 0.0f;

		break;
	}
}

#pragma endregion

#pragma region 関数: smokeEffect

static void InitSmokeEffect(void) {
	for (int i = 0; i < kSmokeEffectLimit; i++) {
		smokeEffect[i] = {};
		smokeEffect[i].texture = GetTexture(TextureType::White1x1);
		smokeEffect[i].texture.color = smokeEffect[i].color;
	}
}

static void UpdateSmokeEffect(void) {
	for (int i = 0; i < kSmokeEffectLimit; i++) {
		if (!smokeEffect[i].isShoot) continue;

#pragma region 残像の更新処理
		//prePos(残像位置の更新)
		for (int j = 0; j < kSmokeAfterimageLimit; j++) {
			if (j == 0) {
				smokeEffect[i].preSizeValue[j] = smokeEffect[i].sizeValue;
				smokeEffect[i].preRotateTheta[j] = smokeEffect[i].rotateTheta;
			} else {
				smokeEffect[i].preSizeValue[j] = smokeEffect[i].preSizeValue[j - 1];
				smokeEffect[i].preRotateTheta[j] = smokeEffect[i].preRotateTheta[j - 1];
			}
		}

#pragma endregion

		//本体の移動処理
		smokeEffect[i].pos.y += smokeEffect[i].moveSpeed;

		//拡大処理
		smokeEffect[i].sizeValue += smokeEffect[i].changeSizeSpeed;
		//回転処理
		smokeEffect[i].rotateTheta += smokeEffect[i].rotateSpeed;
		//alpha値の変更
		smokeEffect[i].alpha -= smokeEffect[i].changeAlphaSpeed;

		//消滅処理
		if (smokeEffect[i].alpha <= smokeEffect[i].alphaMin) {
			smokeEffect[i].alpha = smokeEffect[i].alphaMin;
			smokeEffect[i].isShoot = false;
		}

	}
}

static void DrawSmokeEffect(void) {
	for (int i = 0; i < kSmokeEffectLimit; i++) {
		if (!smokeEffect[i].isShoot) continue;
		if (smokeEffect[i].drawType != TypeDrawEffect::Obj) continue;

		//残像の処理
		for (int j = 0; j < kSmokeAfterimageLimit; j++) {
			Vector2 pos = { smokeEffect[i].pos.x, smokeEffect[i].pos.y - smokeEffect[i].posOffset * (j + 1) };
			Vector2 size = { smokeEffect[i].preSizeValue[j], smokeEffect[i].preSizeValue[j] };

			Texture afterFadeTexture = smokeEffect[i].texture;

			//透明度の変更
			int alpha = smokeEffect[i].alpha - smokeEffect[i].diffAlpha * (j + 1);
			if (alpha < 0) {
				alpha = 0;
			}
			unsigned int color = (smokeEffect[i].texture.color & 0xFFFFFF00) | alpha;
			afterFadeTexture.color = color;


			DrawTextureRotateObj(afterFadeTexture, pos, size, smokeEffect[i].preRotateTheta[j]);
		}

		//本体の描写
		Vector2 size = { smokeEffect[i].sizeValue, smokeEffect[i].sizeValue };
		Texture texture = smokeEffect[i].texture;
		//透明度の変更
		unsigned int color = (smokeEffect[i].texture.color & 0xFFFFFF00) | smokeEffect[i].alpha;
		texture.color = color;

		DrawTextureRotateObj(texture, smokeEffect[i].pos, size, smokeEffect[i].rotateTheta);
	}
}

static void DrawSmokeEffectAsUI(void) {
	for (int i = 0; i < kSmokeEffectLimit; i++) {
		if (!smokeEffect[i].isShoot) continue;
		if (smokeEffect[i].drawType != TypeDrawEffect::UI) continue;

		//残像の処理
		for (int j = 0; j < kSmokeAfterimageLimit; j++) {
			Vector2 pos = { smokeEffect[i].pos.x, smokeEffect[i].pos.y - smokeEffect[i].posOffset * (j + 1) };
			Vector2 size = { smokeEffect[i].preSizeValue[j], smokeEffect[i].preSizeValue[j] };

			Texture afterFadeTexture = smokeEffect[i].texture;

			//透明度の変更
			int alpha = smokeEffect[i].alpha - smokeEffect[i].diffAlpha * (j + 1);
			if (alpha < 0) {
				alpha = 0;
			}
			unsigned int color = (smokeEffect[i].texture.color & 0xFFFFFF00) | alpha;
			afterFadeTexture.color = color;


			DrawTextureRotateUI(afterFadeTexture, pos, size, smokeEffect[i].preRotateTheta[j]);
		}

		//本体の描写
		Vector2 size = { smokeEffect[i].sizeValue, smokeEffect[i].sizeValue };
		Texture texture = smokeEffect[i].texture;
		//透明度の変更
		unsigned int color = (smokeEffect[i].texture.color & 0xFFFFFF00) | smokeEffect[i].alpha;
		texture.color = color;

		DrawTextureRotateUI(texture, smokeEffect[i].pos, size, smokeEffect[i].rotateTheta);
	}
}

void CreateSmokeEffect(Vector2 pos, TypeDrawEffect drawType, TypeSmoke smokeType) {
	for (int i = 0; i < kSmokeEffectLimit; i++) {
		if (smokeEffect[i].isShoot) continue;

		smokeEffect[i].isShoot = true;
		smokeEffect[i].pos = pos;
		smokeEffect[i].drawType = drawType;

		//変数変更
		if (smokeType == TypeSmoke::EnergyGauge) {
			smokeEffect[i].moveSpeed = playerSmoke.moveSpeed;
			smokeEffect[i].changeSizeSpeed = playerSmoke.changeSizeSpeed;
			smokeEffect[i].firstSizeValue = playerSmoke.firstSizeValue;
			smokeEffect[i].changeAlphaSpeed = playerSmoke.changeAlphaSpeed;
			smokeEffect[i].posOffset = playerSmoke.posOffset;

			smokeEffect[i].color = playerSmoke.color;
		} else {
			smokeEffect[i].moveSpeed = enemyBombSmoke.moveSpeed;
			smokeEffect[i].changeSizeSpeed = enemyBombSmoke.changeSizeSpeed;
			smokeEffect[i].firstSizeValue = enemyBombSmoke.firstSizeValue;
			smokeEffect[i].changeAlphaSpeed = enemyBombSmoke.changeAlphaSpeed;
			smokeEffect[i].posOffset = enemyBombSmoke.posOffset;

			smokeEffect[i].color = enemyBombSmoke.color;
		}

		//初期化
		smokeEffect[i].sizeValue = smokeEffect[i].firstSizeValue;
		smokeEffect[i].rotateTheta = 0.0f;
		smokeEffect[i].alpha = smokeEffect[i].alphaMax;
		for (int j = 0; j < kSmokeAfterimageLimit; j++) {
			smokeEffect[i].preRotateTheta[j] = smokeEffect[i].rotateTheta;
		}

		break;
	}
}

#pragma endregion

#pragma region 関数: elecShockEffect

//static void InitElecShockEffect(void) {
//	for (int i = 0; i < kElecShockEffectLimit; i++) {
//		elecShockEffect[i] = {};
//
//		elecShockEffect[i].texture = GetTexture(TextureType::ElecShock);
//	}
//}
//
//static void UpdateElecShockEffect(void) {
//	for (int i = 0; i < kElecShockEffectLimit; i++) {
//		if (!elecShockEffect[i].isShoot) continue;
//
//		//媒介変数の更新
//		elecShockEffect[i].easingT += elecShockEffect[i].changeSpeed;
//		if (elecShockEffect[i].easingT > 1.0f) {
//			elecShockEffect[i].easingT = 1.0f;
//		}
//
//		//サイズ変更処理
//		elecShockEffect[i].sizeValue = UpdateValueByEasing(elecShockEffect[i].firstSizeValue, elecShockEffect[i].endSizeValue, elecShockEffect[i].easingT, EasingMode::EaseInOut);
//		//フェード処理
//#pragma region 0.6まではキープ
//		if (elecShockEffect[i].easingT < elecShockEffect[i].keepAlphaT) {
//			elecShockEffect[i].alpha = elecShockEffect[i].alphaMax;
//		} else {
//			float t = (elecShockEffect[i].easingT - 0.6f) / 0.4f;  // 0.6→1.0 を 0→1 に正規化
//			if (t > 1.0f) t = 1.0f;
//
//			elecShockEffect[i].alpha = UpdateValueByEasing(elecShockEffect[i].alphaMax, elecShockEffect[i].alphaMin, t, EasingMode::EaseOut);
//		}
//
//#pragma endregion
//
//		//消滅処理
//		if (elecShockEffect[i].easingT >= 1.0f) {
//			elecShockEffect[i].isShoot = false;
//		}
//	}
//}
//
//static void DrawElecShockEffect(void) {
//	for (int i = 0; i < kElecShockEffectLimit; i++) {
//		if (!elecShockEffect[i].isShoot) continue;
//
//		//size
//		Vector2 size = { elecShockEffect[i].sizeValue, elecShockEffect[i].sizeValue };
//
//		//color
//		Texture texture = elecShockEffect[i].texture;
//		texture.color = (texture.color & 0xFFFFFF00) | elecShockEffect[i].alpha;
//
//		DrawTextureObj(texture, elecShockEffect[i].pos, size);
//	}
//}

//void CreateElecShockEffect(Vector2 pos) {
//	for (int i = 0; i < kElecShockEffectLimit; i++) {
//		if (elecShockEffect[i].isShoot) continue;
//
//		elecShockEffect[i].isShoot = true;
//		elecShockEffect[i].pos = pos;
//
//		elecShockEffect[i].sizeValue = elecShockEffect[i].firstSizeValue;
//		elecShockEffect[i].alpha = elecShockEffect[i].alphaMax;
//		elecShockEffect[i].easingT = 0.0f;
//
//		break;
//	}
//}

//static void InitElecShockChargedEffect(void) {
//	for (int i = 0; i < kElecShockEffectLimit; i++) {
//		elecShockChargedEffect[i] = {};
//
//		elecShockChargedEffect[i].texture = GetTexture(TextureType::ElecShockCharged);
//	}
//}
//
//static void UpdateElecShockChargedEffect(void) {
//	for (int i = 0; i < kElecShockEffectLimit; i++) {
//		if (!elecShockChargedEffect[i].isShoot) continue;
//
//		//媒介変数の更新
//		elecShockChargedEffect[i].easingT += elecShockChargedEffect[i].changeSpeed;
//		if (elecShockChargedEffect[i].easingT > 1.0f) {
//			elecShockChargedEffect[i].easingT = 1.0f;
//		}
//
//		//サイズ変更処理
//		elecShockChargedEffect[i].sizeValue = UpdateValueByEasing(elecShockChargedEffect[i].firstSizeValue, elecShockChargedEffect[i].endSizeValue, elecShockChargedEffect[i].easingT, EasingMode::EaseInOut);
//		//フェード処理
//#pragma region 0.6まではキープ
//		if (elecShockChargedEffect[i].easingT < elecShockChargedEffect[i].keepAlphaT) {
//			elecShockChargedEffect[i].alpha = elecShockChargedEffect[i].alphaMax;
//		} else {
//			float t = (elecShockChargedEffect[i].easingT - 0.6f) / 0.4f;  // 0.6→1.0 を 0→1 に正規化
//			if (t > 1.0f) t = 1.0f;
//
//			elecShockChargedEffect[i].alpha = UpdateValueByEasing(elecShockChargedEffect[i].alphaMax, elecShockChargedEffect[i].alphaMin, t, EasingMode::EaseOut);
//		}
//
//#pragma endregion
//
//		//消滅処理
//		if (elecShockChargedEffect[i].easingT >= 1.0f) {
//			elecShockChargedEffect[i].isShoot = false;
//		}
//	}
//}
//
//static void DrawElecShockChargedEffect(void) {
//	for (int i = 0; i < kElecShockEffectLimit; i++) {
//		if (!elecShockChargedEffect[i].isShoot) continue;
//
//		//size
//		Vector2 size = { elecShockChargedEffect[i].sizeValue, elecShockChargedEffect[i].sizeValue };
//
//		//color
//		Texture texture = elecShockChargedEffect[i].texture;
//		texture.color = (texture.color & 0xFFFFFF00) | elecShockChargedEffect[i].alpha;
//
//		DrawTextureObj(texture, elecShockChargedEffect[i].pos, size);
//	}
//}
//
//void CreateElecShockChargedEffect(Vector2 pos) {
//	for (int i = 0; i < kElecShockEffectLimit; i++) {
//		if (elecShockChargedEffect[i].isShoot) continue;
//
//		elecShockChargedEffect[i].isShoot = true;
//		elecShockChargedEffect[i].pos = pos;
//
//		elecShockChargedEffect[i].sizeValue = elecShockChargedEffect[i].firstSizeValue;
//		elecShockChargedEffect[i].alpha = elecShockChargedEffect[i].alphaMax;
//		elecShockChargedEffect[i].easingT = 0.0f;
//
//		break;
//	}
//}

#pragma endregion

#pragma region 関数: chargeOrbEffect

static void InitChargeOrbEffect(void) {
	for (int i = 0; i < kChargeOrbEffectLimit; i++) {
		chargeOrbEffect[i] = {};

		chargeOrbEffect[i].texture = GetTexture(TextureType::EffectCircle);
	}
}

static void UpdateChargeOrbEffect(void) {

	Vector2 targetPos;
	if (nowScene == Scene::Play) {
		targetPos = GetPlayerShootPos();     // 本編プレイヤー
	} else {
		targetPos = GetTitlePlayerShootPos(); // タイトルプレイヤー
	}

	for (int i = 0; i < kChargeOrbEffectLimit; i++) {
		if (!chargeOrbEffect[i].isShoot) continue;

		chargeOrbEffect[i].pos = targetPos;

		//easingTの更新
		chargeOrbEffect[i].easingT += chargeOrbEffect[i].easingSpeed;
		if (chargeOrbEffect[i].easingT >= 1.0f) {
			chargeOrbEffect[i].easingT = 1.0f;
			chargeOrbEffect[i].isShoot = false;
		}

		//サイズの更新
		chargeOrbEffect[i].sizeValue = UpdateValueByEasing(chargeOrbEffect[i].startSizeValue, chargeOrbEffect[i].endSizeValue, chargeOrbEffect[i].easingT, EasingMode::EaseIn);

#pragma region 透明度の更新

		if (chargeOrbEffect[i].easingT < chargeOrbEffect[i].easingTFadeIn) {
			float t = chargeOrbEffect[i].easingT / 0.5f;  // 0.0→0.5 を 0→1 に正規化
			if (t > 1.0f) t = 1.0f;

			chargeOrbEffect[i].alpha = UpdateValueByEasing(chargeOrbEffect[i].alphaMin, chargeOrbEffect[i].alphaMax, t, EasingMode::EaseOut);
		} else {
			float t = (chargeOrbEffect[i].easingT - 0.5f) / 0.5f;  // 0.5→1.0 を 0→1 に正規化
			if (t > 1.0f) t = 1.0f;

			chargeOrbEffect[i].alpha = UpdateValueByEasing(chargeOrbEffect[i].alphaMax, chargeOrbEffect[i].alphaMin, t, EasingMode::EaseIn);
		}

#pragma endregion

	}
}

static void DrawChargeOrbEffect(void) {
	for (int i = 0; i < kChargeOrbEffectLimit; i++) {
		if (!chargeOrbEffect[i].isShoot) continue;

		Vector2 size = { chargeOrbEffect[i].sizeValue, chargeOrbEffect[i].sizeValue };

		Texture texture = chargeOrbEffect[i].texture;
		texture.color = (chargeOrbEffect[i].color & 0xFFFFFF00) | chargeOrbEffect[i].alpha;

		if (nowScene == Scene::Play) {
			DrawTextureObj(texture, chargeOrbEffect[i].pos, size);
		} else {
			DrawTextureUI(texture, chargeOrbEffect[i].pos, size);
		}
	}
}

void CreateChargeOrbEffect(Vector2 pos) {
	for (int i = 0; i < kChargeOrbEffectLimit; i++) {
		if (chargeOrbEffect[i].isShoot) continue;

		chargeOrbEffect[i].isShoot = true;
		chargeOrbEffect[i].pos = pos;

		chargeOrbEffect[i].sizeValue = chargeOrbEffect[i].startSizeValue;
		chargeOrbEffect[i].alpha = chargeOrbEffect[i].alphaMin;
		chargeOrbEffect[i].easingT = 0.0f;

		break;
	}
}

#pragma endregion

#pragma region 関数: chargeChipEffect

static void InitChargeChipEffect(void) {
	for (int i = 0; i < kChargeChipEffecLimit; i++) {
		chargeChipEffect[i] = {};

		chargeChipEffect[i].texture = GetTexture(TextureType::EffectTriangle);
	}
}

static void UpdateChargeChipEffect(void) {
	for (int i = 0; i < kChargeChipEffecLimit; i++) {
		if (!chargeChipEffect[i].isShoot) continue;

		//イージング
		chargeChipEffect[i].easingT += chargeChipEffect[i].easingSpeed;
		if (chargeChipEffect[i].easingT >= 1.0f) {
			chargeChipEffect[i].easingT = 1.0f;
			chargeChipEffect[i].isShoot = false;
		}

#pragma region サイズの変更

		chargeChipEffect[i].sizeValue = UpdateValueByEasing(chargeChipEffect[i].startSizeValue, chargeChipEffect[i].endSizeValue, chargeChipEffect[i].easingT, EasingMode::EaseIn);

#pragma endregion

#pragma region 自転処理

		chargeChipEffect[i].rotateTheta += chargeChipEffect[i].rotateSpeed;

#pragma endregion

#pragma region 公転処理

		Vector2 shootPos;
		if (nowScene == Scene::Play) {
			shootPos = GetPlayerShootPos();     // 本編プレイヤー
		} else {
			shootPos = GetTitlePlayerShootPos(); // タイトルプレイヤー
		}

		chargeChipEffect[i].revolutionTheta += chargeChipEffect[i].revolutionSpeed;

		chargeChipEffect[i].targetVecLength = UpdateValueByEasing(chargeChipEffect[i].startTargetVecLength, chargeChipEffect[i].endTargetVecLength, chargeChipEffect[i].easingT, EasingMode::EaseIn);

		chargeChipEffect[i].pos.x = shootPos.x + cosf(chargeChipEffect[i].revolutionTheta) * chargeChipEffect[i].targetVecLength;
		chargeChipEffect[i].pos.y = shootPos.y + sinf(chargeChipEffect[i].revolutionTheta) * chargeChipEffect[i].targetVecLength;

#pragma endregion

#pragma region 透明度の変更

		if (chargeChipEffect[i].easingT < chargeChipEffect[i].easingTFadeIn) {
			float t = chargeChipEffect[i].easingT / 0.5f;  // 0.0→0.5 を 0→1 に正規化
			if (t > 1.0f) t = 1.0f;

			chargeChipEffect[i].alpha = UpdateValueByEasing(chargeChipEffect[i].alphaMin, chargeChipEffect[i].alphaMax, t, EasingMode::EaseOut);
		} else {
			float t = (chargeChipEffect[i].easingT - 0.5f) / 0.5f;  // 0.5→1.0 を 0→1 に正規化
			if (t > 1.0f) t = 1.0f;

			chargeChipEffect[i].alpha = UpdateValueByEasing(chargeChipEffect[i].alphaMax, chargeChipEffect[i].alphaMin, t, EasingMode::EaseIn);
		}

#pragma endregion

	}
}

static void DrawChargeChipEffect(void) {
	for (int i = 0; i < kChargeChipEffecLimit; i++) {
		if (!chargeChipEffect[i].isShoot) continue;

		Vector2 size = { chargeChipEffect[i].sizeValue, chargeChipEffect[i].sizeValue };

		Texture texture = chargeChipEffect[i].texture;
		texture.color = (kPlayerChargedColor & 0xFFFFFF00) | chargeChipEffect[i].alpha;

		if (nowScene == Scene::Play) {
			DrawTextureRotateObj(texture, chargeChipEffect[i].pos, size, chargeChipEffect[i].rotateTheta);
		} else {
			DrawTextureRotateUI(texture, chargeChipEffect[i].pos, size, chargeChipEffect[i].rotateTheta);
		}
	}
}

void CreateChargeChipEffect() {
	for (int i = 0; i < kChargeChipEffecLimit; i++) {
		if (chargeChipEffect[i].isShoot) continue;

		chargeChipEffect[i].isShoot = true;

		chargeChipEffect[i].easingT = 0.0f;
		chargeChipEffect[i].rotateTheta = 0.0f;
		chargeChipEffect[i].revolutionTheta = 0.0f;

		break;
	}
}

#pragma endregion

#pragma region 関数: ExplodeEffect

static void InitExplodeEffect(void) {
	for (int i = 0; i < kExplodeEffectLimit; i++) {
		explodeEffect[i] = {};
		explodeEffect[i].texture = GetTexture(TextureType::ExplodeEffect);
	}
}

static void UpdateExplodeEffect(void) {
	for (int i = 0; i < kExplodeEffectLimit; i++) {
		if (!explodeEffect[i].isShoot) continue;

		explodeEffect[i].texture.animChangeTimer.count++;
		if (explodeEffect[i].texture.animChangeTimer.count >= explodeEffect[i].texture.animChangeTimer.time) {
			explodeEffect[i].texture.animChangeTimer.count = 0;

			if (explodeEffect[i].texture.animNum < explodeEffect[i].texture.animLimit) {
				explodeEffect[i].texture.animNum++;
			} else {
				explodeEffect[i].texture.animNum = 0;
				explodeEffect[i].isShoot = false;
			}
		}
	}
}

static void DrawExplodeEffect(void) {
	for (int i = 0; i < kExplodeEffectLimit; i++) {
		if (!explodeEffect[i].isShoot) continue;

		Texture texture = explodeEffect[i].texture;
		texture.color = explodeEffect[i].color;

		if (nowScene == Scene::Play) {
			DrawTextureObj(texture, explodeEffect[i].pos, explodeEffect[i].size);
		} else {
			DrawTextureUI(texture, explodeEffect[i].pos, explodeEffect[i].size);
		}
	}
}

void CreateExplodeEffect(Vector2 pos, unsigned int color) {
	for (int i = 0; i < kExplodeEffectLimit; i++) {
		if (explodeEffect[i].isShoot) continue;

		explodeEffect[i].isShoot = true;
		explodeEffect[i].pos = pos;
		explodeEffect[i].color = color;

		PlaySe(SeType::DeadEnemy);

		break;
	}
}

#pragma endregion

#pragma region 関数: TriangleEffect

void CreateTriangleEffect(Vector2 pos, unsigned int color) {
	for (int i = 0; i < kTriangleEffectNumber; i++) {
		if (triangleEffect[i].appear) continue;

		triangleEffect[i].appear = true;

		triangleEffect[i].pos = pos;
		triangleEffect[i].color = color;

		float randomSpeedX = ToFloat(GetRand(-10, 10));
		float randomSpeedY = ToFloat(GetRand(-10, 10));
		triangleEffect[i].rotateTheta = ToFloat(GetRand(1, 4) * (M_PI) / 4);
		triangleEffect[i].acceleration.x = randomSpeedX;
		triangleEffect[i].acceleration.y = randomSpeedY;

		//triangleEffect[i].rotateTheta = 0.0f;
		triangleEffect[i].easingT = 0.0f;
		triangleEffect[i].alpha = kAlphaMax;

		break;

	}
}

static void InitTriangleEffect(void) {
	for (int i = 0; i < kTriangleEffectNumber; i++) {
		triangleEffect[i] = {};
	}
}

void UpdateTriangleEffect() {
	for (int i = 0; i < kTriangleEffectNumber; i++) {
		if (!triangleEffect[i].appear) continue;

		//移動処理
		triangleEffect[i].pos.x += triangleEffect[i].acceleration.x;
		triangleEffect[i].pos.y += triangleEffect[i].acceleration.y;

		//回転処理
		triangleEffect[i].rotateTheta += triangleEffect[i].rotateSpeed;

		//イージングを増価させる処理
		triangleEffect[i].easingT += triangleEffect[i].easingSpeed;
		if (triangleEffect[i].easingT >= 1.0f) {
			triangleEffect[i].easingT = 1.0f;
			triangleEffect[i].appear = false;
		}

#pragma region 透明化処理

		if (triangleEffect[i].easingT >= triangleEffect[i].startFadeEasingT) {
			float t = (triangleEffect[i].easingT - triangleEffect[i].startFadeEasingT) / (1.0f - triangleEffect[i].startFadeEasingT);
			if (t > 1.0f) t = 1.0f;

			triangleEffect[i].alpha = UpdateValueByEasing(kAlphaMax, kAlphaMin, t, EasingMode::EaseIn);
		}

#pragma endregion

#pragma region 元データ
		////透明化(消滅)
		//if (triangleEffect[i].color >= 0xffffff05) {
		//	triangleEffect[i].color -= 0x00000015;
		//	triangleEffect[i].texture.color = triangleEffect[i].color;
		//}
		//else {
		//	triangleEffect[i].color = 0xffffffff;
		//	triangleEffect[i].appear = false;
		//}

#pragma endregion
	}
}

void DrawTriangleEffect() {
	for (int i = 0; i < kTriangleEffectNumber; i++) {
		if (!triangleEffect[i].appear) continue;

		Texture texture = triangleTexture;
		texture.color = (triangleEffect[i].color & 0xFFFFFF00) | triangleEffect[i].alpha;

		if (nowScene == Scene::Play) {
			DrawTextureRotateObj(texture, triangleEffect[i].pos, triangleEffect[i].size, triangleEffect[i].rotateTheta);
		} else {
			DrawTextureRotateUI(texture, triangleEffect[i].pos, triangleEffect[i].size, triangleEffect[i].rotateTheta);
		}

	}
}

#pragma endregion

#pragma region 関数: TriangleEffect2

void CreateTriangleEffect2(Vector2 pos, float sizeValue, float moveSpeed, unsigned int color, TypeDrawEffect drawType) {
	for (int i = 0; i < kTriangleEffectNumber; i++) {
		if (triangleEffect2[i].appear) continue;

		triangleEffect2[i].appear = true;

		triangleEffect2[i].pos = pos;
		triangleEffect2[i].color = color;
		triangleEffect2[i].sizeValue = sizeValue;
		triangleEffect2[i].moveSpeed = moveSpeed;
		triangleEffect2[i].drawType = drawType;

		//ランダムな方向に飛んでいく
		triangleEffect2[i].moveTheta = ToFloat(GetRand(0, 360)) * (ToFloat(M_PI) / 180.0f);

		triangleEffect2[i].rotateTheta = ToFloat(GetRand(1, 4) * (M_PI) / 4);

		triangleEffect2[i].easingT = 0.0f;
		triangleEffect2[i].alpha = kAlphaMax;

		break;

	}
}

static void InitTriangleEffect2(void) {
	for (int i = 0; i < kTriangleEffectNumber; i++) {
		triangleEffect2[i] = {};
	}
}

void UpdateTriangleEffect2() {
	for (int i = 0; i < kTriangleEffectNumber; i++) {
		if (!triangleEffect2[i].appear) continue;

		//移動処理
		triangleEffect2[i].pos.x += cosf(triangleEffect2[i].moveTheta) * triangleEffect2[i].moveSpeed;
		triangleEffect2[i].pos.y += sinf(triangleEffect2[i].moveTheta) * triangleEffect2[i].moveSpeed;

		//回転処理
		triangleEffect2[i].rotateTheta += triangleEffect2[i].rotateSpeed;

		//イージングを増価させる処理
		triangleEffect2[i].easingT += triangleEffect2[i].easingSpeed;
		if (triangleEffect2[i].easingT >= 1.0f) {
			triangleEffect2[i].easingT = 1.0f;
			triangleEffect2[i].appear = false;
		}

#pragma region 透明化処理

		if (triangleEffect2[i].easingT >= triangleEffect2[i].startFadeEasingT) {
			float t = (triangleEffect2[i].easingT - triangleEffect2[i].startFadeEasingT) / (1.0f - triangleEffect2[i].startFadeEasingT);
			if (t > 1.0f) t = 1.0f;

			triangleEffect2[i].alpha = UpdateValueByEasing(kAlphaMax, kAlphaMin, t, EasingMode::EaseOut);
		}

#pragma endregion
	}
}

void DrawTriangleEffect2() {
	for (int i = 0; i < kTriangleEffectNumber; i++) {
		if (!triangleEffect2[i].appear) continue;

		Vector2 size = { triangleEffect2[i].sizeValue, triangleEffect2[i].sizeValue };

		Texture texture = triangleTexture;
		texture.color = (triangleEffect2[i].color & 0xFFFFFF00) | triangleEffect2[i].alpha;

		if (nowScene == Scene::Play) {

			if (triangleEffect2[i].drawType == TypeDrawEffect::Obj) {
				DrawTextureRotateObj(texture, triangleEffect2[i].pos, size, triangleEffect2[i].rotateTheta);
			} else {
				DrawTextureRotateUI(texture, triangleEffect2[i].pos, size, triangleEffect2[i].rotateTheta);
			}
		} else {
			DrawTextureRotateUI(texture, triangleEffect2[i].pos, size, triangleEffect2[i].rotateTheta);
		}

	}
}
#pragma endregion

#pragma region 関数: circleEffect

void CreateCircleEffect(Vector2 pos, float endSizeValue, float easingSpeed, unsigned int color) {
	for (int i = 0; i < kCircleEffectNumber; i++) {
		if (circleEffect[i].appear) continue;

		circleEffect[i].appear = true;

		circleEffect[i].pos = pos;
		circleEffect[i].endSizeValue = endSizeValue;

		if (easingSpeed <= 0.0f || easingSpeed >= 1.0f) {
			circleEffect[i].easingSpeed = circleEffect[i].easingSpeedDefault;
		} else {
			circleEffect[i].easingSpeed = easingSpeed;
		}

		circleEffect[i].color = color;

		circleEffect[i].easingT = 0.0f;

		break;
	}
}

static void InitCircleEffect(void) {
	for (int i = 0; i < kCircleEffectNumber; i++) {
		circleEffect[i] = {};
		circleEffect[i].texture = GetTexture(TextureType::EffectCircle);
	}
}

void UpdateCircleEffect() {
	for (int i = 0; i < kCircleEffectNumber; i++) {
		if (!circleEffect[i].appear) continue;

		//イージング
		circleEffect[i].easingT += circleEffect[i].easingSpeed;
		if (circleEffect[i].easingT >= 1.0f) {
			circleEffect[i].appear = false;
			circleEffect[i].easingT = 1.0f;
		}

		//拡大処理
		circleEffect[i].sizeValue = UpdateValueByEasing(circleEffect[i].startSizeValue, circleEffect[i].endSizeValue, circleEffect[i].easingT, EasingMode::EaseOut);

		//フェード処理
		if (circleEffect[i].easingT >= circleEffect[i].startFadeEasingT) {
			float t = (circleEffect[i].easingT - circleEffect[i].startFadeEasingT) / (1.0f - circleEffect[i].startFadeEasingT);
			if (t > 1.0f) t = 1.0f;

			circleEffect[i].alpha = UpdateValueByEasing(kAlphaMax, kAlphaMin, t, EasingMode::EaseOut);
		}

#pragma region 前データ
		//	circleEffect[i].size.x += 10;
		//	circleEffect[i].size.y += 10;
		//	if (circleEffect[i].size.x >= 256) {
		//		circleEffect[i].size.x = 256;
		//		circleEffect[i].size.y = 256;
		//	}

		//	//透明化(消滅)
		//	if (circleEffect[i].size.x == 256) {
		//		if (circleEffect[i].color >= 0xffffff05) {
		//			circleEffect[i].color -= 0x00000015;
		//			circleEffect[i].texture.color = circleEffect[i].color;
		//		} else {
		//			circleEffect[i].color = 0xffffffff;
		//			circleEffect[i].appear = false;
		//		}
		//	}
#pragma endregion
	}
}

void DrawCircleEffect() {
	for (int i = 0; i < kCircleEffectNumber; i++) {
		if (!circleEffect[i].appear) continue;

		Vector2 size = { circleEffect[i].sizeValue, circleEffect[i].sizeValue };

		Texture texture = circleEffect[i].texture;
		texture.color = (circleEffect[i].color & 0xFFFFFF00) | circleEffect[i].alpha;

		if (nowScene == Scene::Play) {
			DrawTextureObj(texture, circleEffect[i].pos, size);
		} else {
			DrawTextureUI(texture, circleEffect[i].pos, size);
		}

	}
}


#pragma endregion

#pragma region 関数: ElecShockEffect

static void InitElecLineEffect(void) {
	for (int i = 0; i < kElecLineEffectLimit; i++) {
		elecLineEffect[i] = {};

		elecLineEffect[i].texture = GetTexture(TextureType::ElecLine);
	}
}

static void UpdateElecLineEffect(void) {
	for (int i = 0; i < kElecLineEffectLimit; i++) {
		if (!elecLineEffect[i].isShoot) continue;

		//アニメーション処理
		elecLineEffect[i].texture.animChangeTimer.count++;
		if (elecLineEffect[i].texture.animChangeTimer.count >= elecLineEffect[i].texture.animChangeTimer.time) {
			elecLineEffect[i].texture.animChangeTimer.count = 0;

			if (elecLineEffect[i].texture.animNum < elecLineEffect[i].texture.animLimit) {
				elecLineEffect[i].texture.animNum++;
			} else {
				elecLineEffect[i].texture.animNum = 0;
				elecLineEffect[i].isShoot = false;
			}
		}

	}
}

static void DrawElecLineEffect(void) {
	for (int i = 0; i < kElecLineEffectLimit; i++) {
		if (!elecLineEffect[i].isShoot) continue;

		//Vector2i startPos = ConvertPosWToS(elecLineEffect[i].startPos);
		//Vector2i endPos = ConvertPosWToS(elecLineEffect[i].endPos);

		//Novice::DrawLine(startPos.x, startPos.y, endPos.x, endPos.y, kPlayerColor);

		float thickness = (elecLineEffect[i].charged) ? elecLineEffect[i].thicknessCharged : elecLineEffect[i].thickness;

		Vector2 size = { elecLineEffect[i].length, thickness };

		Texture texture = elecLineEffect[i].texture;

		if (!elecLineEffect[i].charged) {
			texture.color = (kPlayerColor & 0xFFFFFF00) | elecLineEffect[i].alpha;
		} else {
			texture.color = (kPlayerChargedColor & 0xFFFFFF00) | elecLineEffect[i].alpha;
		}

		DrawTextureRotateObj(texture, elecLineEffect[i].centerPos, size, elecLineEffect[i].rotateTheta);
	}
}

void CreateElecLine(Vector2 startPos, Vector2 endPos, bool charged) {
	for (int i = 0; i < kElecLineEffectLimit; i++) {
		if (elecLineEffect[i].isShoot) continue;

		elecLineEffect[i].isShoot = true;
		elecLineEffect[i].breakTimer.count = 0;

		float disX = endPos.x - startPos.x;
		float disY = endPos.y - startPos.y;

		elecLineEffect[i].length = sqrtf(disX * disX + disY * disY);
		elecLineEffect[i].rotateTheta = atan2f(disY, disX);

		elecLineEffect[i].centerPos.x = (startPos.x + endPos.x) / 2.0f;
		elecLineEffect[i].centerPos.y = (startPos.y + endPos.y) / 2.0f;

		elecLineEffect[i].charged = charged;

		break;
	}
}


void CreateElecShockEffect(Vector2 pos) {
	//円を生成
	CreateCircleEffect(pos, elecShockEffect.circleSize, 0.0f, kPlayerColor);

	//三角を生成
	for (int i = 0; i < elecShockEffect.createTriangleLimit; i++) {
		CreateTriangleEffect2(pos, elecShockEffect.triangleSizeValue, elecShockEffect.triangleMoveSpeed, kPlayerColor, TypeDrawEffect::Obj);
	}

}

void CreateElecShockChargedEffect(Vector2 pos) {
	//円を生成
	CreateCircleEffect(pos, elecShockChargedEffect.circleSize, 0.0f, kPlayerChargedColor);

	//三角を生成
	for (int i = 0; i < elecShockChargedEffect.createTriangleLimit; i++) {
		CreateTriangleEffect2(pos, elecShockChargedEffect.triangleSizeValue, elecShockChargedEffect.triangleMoveSpeed, kPlayerChargedColor, TypeDrawEffect::Obj);
	}

}


#pragma endregion

#pragma region 関数: PlayerDamageEffect

void CreatePlayerDamageEffect(Vector2 pos) {
	for (int i = 0; i < playerDamageEffect.createTriangleLimit; i++) {
		CreateTriangleEffect2(pos, playerDamageEffect.triangleSizeValue, playerDamageEffect.triangleMoveSpeed, kPlayerColor, TypeDrawEffect::Obj);
	}
}

#pragma endregion

#pragma region 関数: EnemyDamageEffect

void CreateEnemyDamageEffect(Vector2 pos) {
	for (int i = 0; i < enemyDamageEffect.createTriangleLimit; i++) {
		CreateTriangleEffect2(pos, enemyDamageEffect.triangleSizeValue, enemyDamageEffect.triangleMoveSpeed, kEnemyColor, TypeDrawEffect::Obj);
	}
}

#pragma endregion

#pragma region 関数: PlayerMoveEffectEffect

void CreatePlayerMoveEffect(Vector2 pos, Vector2 moveVec) {
	if (moveVec.x == 0.0f && moveVec.y == 0.0f) return;

	createPlayerMoveEffectTimer.count++;
	if (createPlayerMoveEffectTimer.count < createPlayerMoveEffectTimer.time) return;

	for (int i = 0; i < kPlayerMoveEffectLimit; i++) {
		if (playerMoveEffect[i].isShoot) continue;

		playerMoveEffect[i].isShoot = true;
		playerMoveEffect[i].pos = pos;

		//移動方向設定
		Vector2 effectMoveVec = { -moveVec.x, -moveVec.y };
		playerMoveEffect[i].moveTheta = atan2f(effectMoveVec.y, effectMoveVec.x);

		//ブレさせる処理
		float shakeValue = ToFloat(GetRand(-playerMoveEffectShakeAmp, playerMoveEffectShakeAmp)) * (ToFloat(M_PI) / 180.0f);
		playerMoveEffect[i].moveTheta += shakeValue;

		//初期化処理
		playerMoveEffect[i].easingT = 0.0f;
		playerMoveEffect[i].moveSpeed = playerMoveEffect[i].moveSpeedMax;

		createPlayerMoveEffectTimer.count = 0;

		break;
	}
}
static void InitPlayerMoveEffect(void) {
	for (int i = 0; i < kPlayerMoveEffectLimit; i++) {
		playerMoveEffect[i] = {};
	}
}

void UpdatePlayerMoveEffect() {
	for (int i = 0; i < kPlayerMoveEffectLimit; i++) {
		if (!playerMoveEffect[i].isShoot) continue;

		//移動処理
		playerMoveEffect[i].pos.x += cosf(playerMoveEffect[i].moveTheta) * playerMoveEffect[i].moveSpeed;
		playerMoveEffect[i].pos.y += sinf(playerMoveEffect[i].moveTheta) * playerMoveEffect[i].moveSpeed;

		//回転処理
		playerMoveEffect[i].rotateTheta += playerMoveEffect[i].rotateSpeed;

#pragma region イージング関係

		playerMoveEffect[i].easingT += playerMoveEffect[i].easingSpeed;
		if (playerMoveEffect[i].easingT >= 1.0f) {
			playerMoveEffect[i].easingT = 1.0f;
			playerMoveEffect[i].isShoot = false;
		}

		//サイズ
		playerMoveEffect[i].size = MoveByEasing(playerMoveEffect[i].sizeMax, playerMoveEffect[i].sizeMin, playerMoveEffect[i].easingT, playerMoveEffect[i].easingMode);

		//移動速度
		playerMoveEffect[i].moveSpeed = UpdateValueByEasing(playerMoveEffect[i].moveSpeedMax, playerMoveEffect[i].moveSpeedMin, playerMoveEffect[i].easingT, playerMoveEffect[i].easingMode);

		//透明度
		playerMoveEffect[i].alpha = UpdateValueByEasing(playerMoveEffect[i].alphaMax, kAlphaMin, playerMoveEffect[i].easingT, playerMoveEffect[i].easingMode);

#pragma endregion

	}
}
void DrawPlayerMoveEffect() {
	for (int i = 0; i < kPlayerMoveEffectLimit; i++) {
		if (!playerMoveEffect[i].isShoot) continue;

		Texture texture = triangleTexture;
		texture.color = (kPlayerColor & 0xFFFFFF00) | playerMoveEffect[i].alpha;

		if (nowScene == Scene::Play) {
			DrawTextureRotateObj(texture, playerMoveEffect[i].pos, playerMoveEffect[i].size, playerMoveEffect[i].rotateTheta);
		} else {
			DrawTextureRotateUI(texture, playerMoveEffect[i].pos, playerMoveEffect[i].size, playerMoveEffect[i].rotateTheta);
		}
	}
}
#pragma endregion

#pragma region 関数: RecoveryEffect

void TriggerCreateRecoveryEffect(void) {
	isCreateRecoveryEffect = true;
	createRecoveryEffectCount = 0;
	createRecoveryEffectTimer.count = createRecoveryEffectTimer.time;
}

static void CreateRecoveryEffect(void) {
	if (!isCreateRecoveryEffect) return;

	createRecoveryEffectTimer.count++;
	if (createRecoveryEffectTimer.count >= createRecoveryEffectTimer.time) {
		createRecoveryEffectTimer.count = 0;

		//生成処理
		for (int j = 0; j < kRecoveryEnergyEffectLimit; j++) {
			if (recoveryEffect[j].isShoot) continue;

			Vector2 createPos = GetPlayerPos();
			createPos.x += ToFloat(GetRand(-kCreateRecoveryEffectPosRange.x, kCreateRecoveryEffectPosRange.x));
			createPos.y += ToFloat(GetRand(0, kCreateRecoveryEffectPosRange.y));

			recoveryEffect[j].isShoot = true;
			recoveryEffect[j].pos = createPos;

			//初期化処理
			recoveryEffect[j].easingT = 0.0f;

			//一定数生成したら
			createRecoveryEffectCount++;
			if (createRecoveryEffectCount >= kCreateRecoveryEffectLimit) {
				isCreateRecoveryEffect = false;
			}

			break;
		}
	}
}

static void InitRecoveryEffect(void) {
	for (int i = 0; i < kRecoveryEnergyEffectLimit; i++) {
		recoveryEffect[i] = {};
	}
}

static void UpdateRecoveryEffect(void) {
	for (int i = 0; i < kRecoveryEnergyEffectLimit; i++) {
		if (!recoveryEffect[i].isShoot) continue;

#pragma region 残像の更新処理
		//prePos(残像位置の更新)
		for (int j = 0; j < kRecoveryEffectAfterimageLimit; j++) {
			if (j == 0) {
				recoveryEffect[i].preRotateTheta[j] = recoveryEffect[i].rotateTheta;
			} else {
				recoveryEffect[i].preRotateTheta[j] = recoveryEffect[i].preRotateTheta[j - 1];
			}
		}

#pragma endregion

		recoveryEffect[i].pos.y += recoveryEffect[i].moveSpeed;

		recoveryEffect[i].rotateTheta += recoveryEffect[i].rotateSpeed;

#pragma region イージング処理

		recoveryEffect[i].easingT += recoveryEffect[i].easingSpeed;
		if (recoveryEffect[i].easingT >= 1.0f) {
			recoveryEffect[i].easingT = 1.0f;
			recoveryEffect[i].isShoot = false;
		}

		//サイズ変更
		recoveryEffect[i].size = MoveByEasing(recoveryEffect[i].sizeMax, recoveryEffect[i].sizeMin, recoveryEffect[i].easingT, recoveryEffect[i].easingMode);

		//透明度変更
		recoveryEffect[i].alpha = UpdateValueByEasing(recoveryEffect[i].alphaMax, recoveryEffect[i].alphaMin, recoveryEffect[i].easingT, recoveryEffect[i].easingMode);

#pragma endregion

	}
}

static void DrawRecoveryEffect(void) {
	for (int i = 0; i < kRecoveryEnergyEffectLimit; i++) {
		if (!recoveryEffect[i].isShoot) continue;

		//残像の描写
		for (int j = 0; j < kRecoveryEffectAfterimageLimit; j++) {
			Vector2 pos = { recoveryEffect[i].pos.x, recoveryEffect[i].pos.y - recoveryEffect[i].posOffset * (j + 1) };

			Texture afterFadeTexture = whiteBoxTexture;

			//透明度の変更
			int alpha = recoveryEffect[i].alpha - recoveryEffect[i].diffAlpha * (j + 1);
			if (alpha < 0) {
				alpha = 0;
			}
			unsigned int color = (kPlayerColor & 0xFFFFFF00) | alpha;
			afterFadeTexture.color = color;


			DrawTextureRotateObj(afterFadeTexture, pos, recoveryEffect[i].size, recoveryEffect[i].preRotateTheta[j]);
		}

		//本体の描写
		Texture texture = whiteBoxTexture;
		texture.color = (kPlayerColor & 0xFFFFFF00) | recoveryEffect[i].alpha;
		DrawTextureRotateObj(texture, recoveryEffect[i].pos, recoveryEffect[i].size, recoveryEffect[i].rotateTheta);
	}
}


#pragma endregion

#pragma region 関数: BreakGemEffect

void CreateBreakGemEffect(Vector2 pos) {
	CreateCircleEffect(pos, breakGemEffect.endSizeValue, breakGemEffect.easingSpeed, breakGemEffect.color);
}

#pragma endregion

#pragma region 関数: SpawnGemEffect

void CreateSpawnGemEffect(Vector2 pos) {
	CreateCircleEffect(pos, spawnGemEffect.endSizeValue, spawnGemEffect.easingSpeed, spawnGemEffect.color);
}

#pragma endregion

#pragma region 関数: BombExplosionEffect

void CreateBombExplosionEffect(Vector2 pos, float endSizeValue, unsigned int color) {
	for (int i = 0; i < kBombExplosionEffectNumber; i++) {
		if (bombExplosionEffect[i].appear) continue;
		bombExplosionEffect[i].appear = true;
		bombExplosionEffect[i].pos = pos;
		bombExplosionEffect[i].endSizeValue = endSizeValue;
		bombExplosionEffect[i].color = color;
		bombExplosionEffect[i].easingT = 0.0f;

		//爆風の初期化
		bombBlastEffect[i][0].pos.x = pos.x + static_cast<float>((rand() % 374) - 374);
		bombBlastEffect[i][0].pos.y = pos.y + static_cast<float>((rand() % 374) - 374);

		bombBlastEffect[i][1].pos.x = pos.x + static_cast<float>((rand() % 374));
		bombBlastEffect[i][1].pos.y = pos.y + static_cast<float>((rand() % 374));

		bombBlastEffect[i][2].pos.x = pos.x + static_cast<float>((rand() % 374));
		bombBlastEffect[i][2].pos.y = pos.y + static_cast<float>((rand() % 374) - 374);

		bombBlastEffect[i][3].pos.x = pos.x + static_cast<float>((rand() % 374) - 374);
		bombBlastEffect[i][3].pos.y = pos.y + static_cast<float>((rand() % 374));
		for (int j = 0; j < kBombBlastEffectNumber; j++) {
			bombBlastEffect[i][j].endSizeValue = endSizeValue;
			bombBlastEffect[i][j].color = color;
			bombBlastEffect[i][j].easingT = 0.0f;
		}
	}
}

static void InitBombExplosionEffect(void) {
	for (int i = 0; i < kBombExplosionEffectNumber; i++) {
		bombExplosionEffect[i] = {};
		bombExplosionEffect[i].texture = GetTexture(TextureType::EffectExplosion);
		for (int j = 0; j < kBombBlastEffectNumber; j++) {
			bombBlastEffect[i][j] = {};
			bombBlastEffect[i][j].texture = GetTexture(TextureType::EffectCircle);
		}
	}
}

void UpdateBombExplosionEffect() {
	for (int i = 0; i < kBombExplosionEffectNumber; i++) {
		if (!bombExplosionEffect[i].appear) continue;

		//イージング
		bombExplosionEffect[i].easingT += bombExplosionEffect[i].easingSpeed;
		if (bombExplosionEffect[i].easingT >= 1.0f) {
			bombExplosionEffect[i].appear = false;
			bombExplosionEffect[i].easingT = 1.0f;
		}

		//拡大処理
		bombExplosionEffect[i].sizeValue = UpdateValueByEasing(bombExplosionEffect[i].startSizeValue, bombExplosionEffect[i].endSizeValue, bombExplosionEffect[i].easingT, EasingMode::EaseOut);

		//フェード処理
		if (bombExplosionEffect[i].easingT >= bombExplosionEffect[i].startFadeEasingT) {
			float t = (bombExplosionEffect[i].easingT - bombExplosionEffect[i].startFadeEasingT) / (1.0f - bombExplosionEffect[i].startFadeEasingT);
			if (t > 1.0f) t = 1.0f;

			bombExplosionEffect[i].alpha = UpdateValueByEasing(kAlphaMax, kAlphaMin, t, EasingMode::EaseOut);
		}
		//if (bombExplosionEffect[i].easingT <= 0.5f) {
		//	for (int j = 0; j < kBombBlastEffectNumber; j++)
		//	{
		//		//イージング
		//		bombBlastEffect[i][j].easingT += bombBlastEffect[i][j].easingSpeed;
		//		if (bombBlastEffect[i][j].easingT >= 1.0f) {
		//			bombBlastEffect[i][j].appear = false;
		//			bombBlastEffect[i][j].easingT = 1.0f;
		//		}

		//		//拡大処理
		//		bombBlastEffect[i][j].sizeValue = UpdateValueByEasing(bombBlastEffect[i][j].startSizeValue, bombBlastEffect[i][j].endSizeValue, bombBlastEffect[i][j].easingT, EasingMode::EaseOut);

		//		//フェード処理
		//		if (bombBlastEffect[i][j].easingT >= bombBlastEffect[i][j].startFadeEasingT) {
		//			float t = (bombBlastEffect[i][j].easingT - bombBlastEffect[i][j].startFadeEasingT) / (1.0f - bombBlastEffect[i][j].startFadeEasingT);
		//			if (t > 1.0f) t = 1.0f;

		//			bombBlastEffect[i][j].alpha = UpdateValueByEasing(kAlphaMax, kAlphaMin, t, EasingMode::EaseOut);
		//		}
		//	}
		//}
	}
}

void DrawBombExplosionEffect() {
	for (int i = 0; i < kBombExplosionEffectNumber; i++) {
		if (!bombExplosionEffect[i].appear) continue;

		Vector2 size = { bombExplosionEffect[i].sizeValue,bombExplosionEffect[i].sizeValue };

		Texture texture = bombExplosionEffect[i].texture;
		texture.color = (bombExplosionEffect[i].color & 0xFFFFFF00) | bombExplosionEffect[i].alpha;

		DrawTextureObj(texture, bombExplosionEffect[i].pos, size);

		for (int j = 0; j < kBombBlastEffectNumber; j++) {
			Vector2 Blostsize = { bombBlastEffect[i][j].sizeValue,bombBlastEffect[i][j].sizeValue };

			Texture Blosttexture = bombBlastEffect[i][j].texture;
			Blosttexture.color = (bombBlastEffect[i][j].color & 0xFFFFFF00) | bombBlastEffect[i][j].alpha;

			DrawTextureObj(texture, bombBlastEffect[i][j].pos, Blostsize);
		}
	}
}
#pragma endregion

#pragma region 関数: フラッシュパネル

static void InitFlashPanel(void) {
	flashPanel = {};
	flashPanel.texture = GetTexture(TextureType::White1x1);
}

static void UpdateFlashPanel(void) {
	if (!flashPanel.isShoot) return;

	flashPanel.easingT += flashPanel.easingSpeed;
	if (flashPanel.easingT >= 1.0f) {
		flashPanel.easingT = 1.0f;
		flashPanel.isShoot = false;
	}

	if (flashPanel.easingT < flashPanel.startFadeOutEasingT) {
		float t = (flashPanel.easingT - flashPanel.startFadeOutEasingT) / (1.0f - flashPanel.startFadeOutEasingT);
		if (t > 1.0f) t = 1.0f;

		flashPanel.alpha = UpdateValueByEasing(flashPanel.alphaMin, flashPanel.alphaMax, t, flashPanel.easingMode);
	} else {
		float t = (flashPanel.easingT - flashPanel.startFadeOutEasingT) / (1.0f - flashPanel.startFadeOutEasingT);
		if (t > 1.0f) t = 1.0f;

		flashPanel.alpha = UpdateValueByEasing(flashPanel.alphaMax, flashPanel.alphaMin, t, flashPanel.easingMode);
	}

}

static void DrawFlashPanel(void) {
	if (!flashPanel.isShoot) return;

	Texture texture = flashPanel.texture;
	texture.color = (texture.color & 0xFFFFFF00) | flashPanel.alpha;

	DrawTextureUI(texture, flashPanel.pos, flashPanel.size);
}

void CreateFlashPanel(void) {
	flashPanel.isShoot = true;
	flashPanel.easingT = 0.0f;
}

#pragma endregion

#pragma region 関数: BarrierBreak
void CreateBarrierBreakEffect(Vector2 pos, unsigned int color) {
	for (int i = 0; i < kBarrierBreakNumber; i++) {
		if (barrierBreakEffect[i].appear) continue;

		barrierBreakEffect[i].appear = true;

		barrierBreakEffect[i].pos = pos;
		barrierBreakEffect[i].color = color;

		float randomSpeedX = ToFloat(GetRand(-10, 10));
		float randomSpeedY = ToFloat(GetRand(-10, 10));
		barrierBreakEffect[i].rotateTheta = ToFloat(GetRand(1, 4) * (M_PI) / 4);
		barrierBreakEffect[i].acceleration.x = randomSpeedX;
		barrierBreakEffect[i].acceleration.y = randomSpeedY;

		barrierBreakEffect[i].easingT = 0.0f;
		barrierBreakEffect[i].alpha = kAlphaMax;
	}
}

static void InitBarrierBreakEffect(void) {
	for (int i = 0; i < kBarrierBreakNumber; i++) {
		barrierBreakEffect[i].texture = GetTexture(TextureType::BarrierBreakEffect);
	}
}

void UpdateBarrierBreakEffect() {
	for (int i = 0; i < kBarrierBreakNumber; i++) {
		if (!barrierBreakEffect[i].appear) continue;

		//移動処理
		barrierBreakEffect[i].pos.x += barrierBreakEffect[i].acceleration.x;
		barrierBreakEffect[i].pos.y += barrierBreakEffect[i].acceleration.y;

		//重力による落下処理
		barrierBreakEffect[i].acceleration.y -= barrierBreakEffect[i].gravityVelocity;

		//回転処理
		barrierBreakEffect[i].rotateTheta += barrierBreakEffect[i].rotateSpeed;

		//イージングを増価させる処理
		barrierBreakEffect[i].easingT += barrierBreakEffect[i].easingSpeed;
		if (barrierBreakEffect[i].easingT >= 1.0f) {
			barrierBreakEffect[i].easingT = 1.0f;
			barrierBreakEffect[i].appear = false;
		}

#pragma region 透明化処理

		if (barrierBreakEffect[i].easingT >= barrierBreakEffect[i].startFadeEasingT) {
			float t = (barrierBreakEffect[i].easingT - barrierBreakEffect[i].startFadeEasingT) / (1.0f - barrierBreakEffect[i].startFadeEasingT);
			if (t > 1.0f) t = 1.0f;

			barrierBreakEffect[i].alpha = UpdateValueByEasing(kAlphaMax, kAlphaMin, t, EasingMode::EaseIn);
		}

#pragma endregion


	}
}

void DrawBarrierBreakEffect() {
	for (int i = 0; i < kBarrierBreakNumber; i++) {
		if (!barrierBreakEffect[i].appear) continue;

		Texture texture = barrierBreakEffect[i].texture;
		texture.color = (barrierBreakEffect[i].color & 0xFFFFFF00) | barrierBreakEffect[i].alpha;

		if (nowScene == Scene::Play) {
			DrawTextureRotateObj(texture, barrierBreakEffect[i].pos, barrierBreakEffect[i].size, barrierBreakEffect[i].rotateTheta);
		}
		else {
			DrawTextureRotateUI(texture,  barrierBreakEffect[i].pos, barrierBreakEffect[i].size, barrierBreakEffect[i].rotateTheta);
		}

	}
}
#pragma endregion

void InitEffect() {
	nowScene = GetNowScene();

	triangleTexture = GetTexture(TextureType::EffectTriangle);
	whiteBoxTexture = GetTexture(TextureType::White1x1);

	InitGemEffect();

	InitGageEffect();
	InitShootLaserEffect();

	InitSmokeEffect();

	InitChargeOrbEffect();
	InitChargeChipEffect();

	InitExplodeEffect();

	InitTriangleEffect();
	InitTriangleEffect2();
	InitCircleEffect();
	InitPlayerMoveEffect();
	InitRecoveryEffect();
	InitElecLineEffect();
	InitBombExplosionEffect();
	InitFlashPanel();
	InitBarrierBreakEffect();

	/*CreateBombExplosionEffect({100,100}, 768,WHITE);*/

#ifdef _DEBUG

	/*CreateJemEffect();*/

	//CreateGageEffect();

	//CreateChargeOrbEffect(GetPlayerPos());

	

#endif // _DEBUG

}
void UpdateEffect() {

	UpdateGemEffect();
	UodateGageEffect();
	UpdateShootLaserEffect();
	UpdateSmokeEffect();
	UpdateChargeOrbEffect();
	UpdateChargeChipEffect();
	UpdateExplodeEffect();
	UpdateTriangleEffect();
	UpdateTriangleEffect2();
	UpdateCircleEffect();
	UpdatePlayerMoveEffect();
	UpdateRecoveryEffect();
	CreateRecoveryEffect();
	UpdateElecLineEffect();
	UpdateBombExplosionEffect();
	UpdateFlashPanel();
	UpdateBarrierBreakEffect();

}
void DrawEffect() {
	DrawGetEffect();

	DrawSmokeEffect();
	DrawShootLaserEffect();

	DrawChargeOrbEffect();
	DrawChargeChipEffect();
	DrawExplodeEffect();
	DrawTriangleEffect();
	DrawTriangleEffect2();
	DrawCircleEffect();
	DrawElecLineEffect();
	DrawPlayerMoveEffect();
	DrawRecoveryEffect();
	DrawBombExplosionEffect();
	DrawBarrierBreakEffect();
}

void DrawEffectAsUI() {
	DrawSmokeEffectAsUI();
	DrawGageEffect();

	DrawFlashPanel();
}

unsigned int GetPlayerColor(void) {
	return kPlayerColor;
}

unsigned int GetEnemyColor(void) {
	return kEnemyColor;
}