#include "play_ui.h"
#include "player.h"
#include "elec_bullet.h"
#include "boss_enemy.h"
#include "stage.h"
#include "effect.h"
#include "score_data.h"
#include "se.h"

#include "vector2.h"
#include "timer.h"
#include "tool.h"
#include "common.h"
#include "texture.h"

#include <Novice.h>

#pragma region データ: HPアイコン
struct LifeIcon {
	Vector2 startPos = { 30, 690 };
	Vector2 size = { 40, 40 };

	float intervalX = 10;

	Texture texture = {};
};
LifeIcon lifeIcon;

#pragma endregion

#pragma region データ: エネルギーゲージ

static constexpr bool kGaugeShakeSwitch = true;
Timer gaugeShakeDamageTimer = { 10, 10 };

struct EnergyGaugeFrame {
	Vector2 pos = { 640, 80 };
	Vector2 viewPos = { 640, 80 };

	Vector2 size = { 512, 100 };

	//デンリョク不足警告表示のタイミング
	float warnDeadEnergy = 30.0f;
	float dangerEnergy = 15.0f;

	////過ジュウデン警告表示のタイミング
	//float warnOverHeat = 75.0f;

	//ケムリの生成範囲(frameのposを中心に)
	Vector2i spawnSmokeRange = { 250, 60 };
	//警告中の煙エフェクト生成スパン
	Timer createSmokeWarningTimer = { 20, 0 };
	//オーバーヒート中の煙エフェクト生成スパン
	Timer createSmokeOverHeatTimer = { 7, 0 };

	//ゲージエフェクトの生成スパン
	Timer createGaugeEffectTimer = { 3, 0 };

	////警告音SEのスパン
	//Timer alarmSeTimer = { 60, 0 };

	Texture texture = {};
};
EnergyGaugeFrame energyGaugeFrame;

struct EnergyGaugeMeter {
	float gaugeRate = 0.0f;

	Texture texture = {};
};
EnergyGaugeMeter energyGaugeMeter;

struct EnergyGaugeDelayMeter {
	float gaugeRate = 0.0f;

	//追従速度
	float followSpeed = 0.007f;

	//イージングを使うかどうかの判定値
	float threshold = 0.01f;

	//イージング用
	float easingT = 0.0f;
	float startRate = 0.0f;
	float endRate = 0.0f;

	Texture texture = {};
};
EnergyGaugeDelayMeter energyGaugeDelayMeter;

struct MiniEnergyGauge {
	Vector2 pos = {};
	Vector2 size = { 64, 16 };
	Vector2 offset = { 0, 40 };

	Texture frameTexture = {};
	Texture meterTexture = {};
	Texture delayMeterTexture = {};
};
MiniEnergyGauge miniEnergyGauge;

#pragma endregion

#pragma region データ: ボスHPゲージ

struct BossHpGaugeFrame {
	Vector2 pos = { 640, 680 };
	Vector2 size = { 720, 32 };

	Texture texture = {};
};
BossHpGaugeFrame bossHpGaugeFrame;

struct BossHpGaugeMeter {
	float gaugeRate = 1.0f;

	Texture texture = {};
};
BossHpGaugeMeter bossHpGaugeMeter;

struct BossHpGaugeDelayMeter {
	float gaugeRate = 1.0f;

	//追従速度
	float followSpeed = 0.007f;

	//イージングを使うかどうかの判定値
	float threshold = 0.01f;

	//イージング用
	float easingT = 0.0f;
	float startRate = 0.0f;
	float endRate = 0.0f;

	Texture texture = {};
};
BossHpGaugeDelayMeter bossHpGaugeDelayMeter;

#pragma endregion

#pragma region データ: 持続ボーナス

struct LaserPowRate {
	Vector2i startPos = { 1000, 700 };

	float powRate = 1.0f;
};
LaserPowRate laserPowRate;



#pragma endregion

#pragma region データ: 進捗度ゲージ

static constexpr bool kShowProgressGaugeSwitch = true;
struct ProgressGauge {
	Vector2 pos = { 640, 680 };
	Vector2 size = { 720, 32 };

	//ボスラウンドまでの時間
	int bossRoundTime = 0;
	float gaugeRate = 0;

	Texture frameTexture = {};
};
ProgressGauge progressGauge;

struct ProgressGaugeIcon {
	Vector2 pos = { 0.0f, progressGauge.pos.y };
	Vector2 size = { 32, 32 };

	Vector2 startPos = { progressGauge.pos.x - progressGauge.size.x / 2, progressGauge.pos.y };
	Vector2 endPos = { progressGauge.pos.x + progressGauge.size.x / 2, progressGauge.pos.y };

	Texture texture = {};
};
ProgressGaugeIcon progressGaugeIcon;

#pragma endregion

#pragma region データ: ショックコンボ

constexpr int kComboNumbersLimit = 4;
struct ShockComboNumbers {
	Vector2 size = { 68, 128 };

	Vector2 startPos = { 1200, 630 };
	Vector2 interval = { -80.0f, 0.0f };

	float offsetY = 85.0f;

	int animNum = 0;
};
ShockComboNumbers comboNumbers[kComboNumbersLimit];
Texture comboNumbersTexture = {};
Texture comboNumbersMeterTexture = {};


struct ShockComboIcon {
	//コンボ数
	int nowCombo = 0;
	float gaugeRate = 0.0f;

	//アイコン座標
	Vector2 pos = { 1120, 580 };
	Vector2 size = { 160, 80 };


	float easingT = 0.0f;
	float easingSpeed = 0.05f;

	Vector2 startPos = { 1160, 550 };
	Vector2 endPos = { 1160, 800 };

	//前のコンボを見せている状態
	bool isShowPreCombo = false;
	Timer showPreComboTimer = { 60, 0 };
	//透明の時間
	bool isInvisible = false;
	Timer invisibleTimer = { 3, 0 };

	Texture texture = {};
};
ShockComboIcon comboIcon;

constexpr int kStartViewCombo = 100;
constexpr int kStartDisplayEffectCombo = 50;

#pragma endregion

#pragma region データ: スコア

static constexpr int kScoreNumbersLimit = 9;
struct ScoreNumbers {
	Vector2 size = { 28, 56 };

	Vector2 startPos = { 30, 680 };
	Vector2 interval = { 26, 0 };

	int animNum = 0;
};
ScoreNumbers scoreNumbers[kScoreNumbersLimit];

struct ScoreIcon {
	Vector2 pos = { 30, 30 };

	//表示の数値
	int showScore = 0;

	//イージング関係
	float easingT = 0.0f;
	float easingSpeed = 0.008f;

	//イージングを使うかどうかの判定値
	int threshold = 1000;

	int startScore = 0;
	int endScore = 0;

};
ScoreIcon scoreIcon;
Texture scoreNumbersTexture = {};

#pragma endregion

#pragma region データ: 画面エフェクト

struct DisplayEffect {
	Vector2 pos = kWindowCenter;

	bool isShoot = false;

	//イージング
	float easingT = 0.0f;
	float easingSpeed = 0.02f;

	//サイズ関係
	Vector2 size = {};
	Vector2 minSize = { 1280, 720 };
	Vector2 maxSize = { 1440, 810 };

	//透明度関係
	int alpha = 50;
	int alphaMax = 50;
	int alphaMin = 10;

	//縮小中フラグ
	bool isScaleDown = true;

	unsigned int color = GetPlayerColor();
};
DisplayEffect displayEffect = {};

struct DangerDisplayEffect {
	Vector2 pos = kWindowCenter;

	bool isShoot = false;

	//イージング
	float easingT = 0.0f;
	float easingSpeed = 0.02f;

	//サイズ関係
	Vector2 size = {};
	Vector2 minSize = { 1280, 720 };
	Vector2 maxSize = { 1440, 810 };

	//透明度関係
	int alpha = 50;
	int alphaMax = 50;
	int alphaMin = 10;

	//縮小中フラグ
	bool isScaleDown = true;

	unsigned int color = GetEnemyColor();
};
DangerDisplayEffect dangerDisplayEffect = {};

Texture displayEffectTexture = {};

#pragma endregion

#pragma region データ: 開始演出

struct StartTelop {
	Vector2 pos = { 640, 550 };
	Vector2 size = { 512, 128 };
};
StartTelop startTelop;
Texture readyTexture = {};
Texture goTexture = {};

#pragma endregion

#pragma region データ: ボス警告演出

struct WarningBossTelop {
	Vector2 pos = { 640, 590 };
	Vector2 size = { 512, 128 };

	//振動関係
	Timer shakeTimer = { 10, 0 };
	int shakeDuration = 3;

	//イージング関係
	float easingT = 0.0f;
	float easingSpeed = 0.03f;
	bool isFadeIn = true;
	EasingMode easingMode = EasingMode::EaseInOut;

	//透明度関係
	int alpha = 0;
	int alphaMin = 0;
	int alphaMax = 190;

	Texture texture = {};
};
WarningBossTelop bossTelop;

#pragma endregion


#pragma region 関数: ライフアイコン

static void InitLifeIcon(void) {
	lifeIcon.texture = GetTexture(TextureType::LifeIcon);
}

static void DrawLifeIcon(void) {
	if (GetLifeGaugeSwitch()) return;

	int remainLife = GetPlayerHp();

	for (int i = 0; i < remainLife; i++) {
		Vector2 posS = { lifeIcon.startPos.x + (lifeIcon.size.x + lifeIcon.intervalX) * i, lifeIcon.startPos.y };

		DrawTextureUI(lifeIcon.texture, posS, lifeIcon.size);
	}
}

#pragma endregion

#pragma region 関数: エネルギーゲージ

static void InitEnergyGauge(void) {
	energyGaugeFrame.texture = GetTexture(TextureType::EnergyGaugeFrame);
	energyGaugeMeter.texture = GetTexture(TextureType::EnergyGaugeMeter);
	energyGaugeDelayMeter.texture = GetTexture(TextureType::EnergyGaugeDelayMeter);

	miniEnergyGauge.frameTexture = GetTexture(TextureType::MiniEnergyGaugeFrame);
	miniEnergyGauge.meterTexture = GetTexture(TextureType::MiniEnergyGaugeMeter);
	miniEnergyGauge.delayMeterTexture = GetTexture(TextureType::MiniEnergyGaugeDelayMeter);
}

static void UpdateEnergyGauge(void) {
	float remainEnergy = GetPlayerRemainEnergy();
	float energyLimit = GetPlayerEnergyLimit();

#pragma region 残量の変化処理

	energyGaugeMeter.gaugeRate = remainEnergy / energyLimit;

#pragma endregion

#pragma region DelayMeterの変化処理

	float nowRate = energyGaugeMeter.gaugeRate;
	float diff = energyGaugeDelayMeter.gaugeRate - nowRate;

	//delayゲージが本ゲージより大きい(減少した際)
	if (energyGaugeDelayMeter.gaugeRate > nowRate) {

		//エージングでの更新
		if (diff > energyGaugeDelayMeter.threshold) {
			//始点と終点を更新
			energyGaugeDelayMeter.startRate = energyGaugeDelayMeter.gaugeRate;
			energyGaugeDelayMeter.endRate = nowRate;

			//easingTを更新
			energyGaugeDelayMeter.easingT += energyGaugeDelayMeter.followSpeed;
			if (energyGaugeDelayMeter.easingT > 1.0f) {
				energyGaugeDelayMeter.easingT = 1.0f;
			}

			//更新
			energyGaugeDelayMeter.gaugeRate =
				UpdateValueByEasing(
					energyGaugeDelayMeter.startRate,
					energyGaugeDelayMeter.endRate,
					energyGaugeDelayMeter.easingT,
					EasingMode::EaseIn
				);
		} else {
			energyGaugeDelayMeter.gaugeRate = nowRate;
			energyGaugeDelayMeter.easingT = 0.0f;
		}

	} else {
		energyGaugeDelayMeter.gaugeRate = nowRate;
		energyGaugeDelayMeter.easingT = 0.0f;
	}

#pragma endregion

#pragma region GaugeEffectの生成

	if (remainEnergy <= energyGaugeFrame.warnDeadEnergy) {
		energyGaugeFrame.createGaugeEffectTimer.count++;
		if (energyGaugeFrame.createGaugeEffectTimer.count >= energyGaugeFrame.createGaugeEffectTimer.time) {
			energyGaugeFrame.createGaugeEffectTimer.count = 0;

			CreateGageEffect(GetEnergyGaugeEndPos(), GetPlayerColor());
		}
	}

#pragma endregion

#pragma region 点滅処理&振動

	//警告表示のラインを下回るor故障状態時
	if (remainEnergy <= energyGaugeFrame.warnDeadEnergy) {
		UpdateAnimation(energyGaugeFrame.texture);
		UpdateAnimation(miniEnergyGauge.frameTexture);

		//振動処理
		if (kGaugeShakeSwitch) {
			energyGaugeFrame.viewPos.x = energyGaugeFrame.pos.x + GetRand(-2, 2);
			energyGaugeFrame.viewPos.y = energyGaugeFrame.pos.y + GetRand(-2, 2);
		}

		////alramSE
		//energyGaugeFrame.alarmSeTimer.count++;
		//if (energyGaugeFrame.alarmSeTimer.count >= energyGaugeFrame.alarmSeTimer.time) {
		//	energyGaugeFrame.alarmSeTimer.count = 0;
		//	PlaySe(SeType::GaugeAlarm);
		//}

		//displayEFfect
		TriggerDangerDisplayEffect();

	} else {
		energyGaugeFrame.texture.animNum = 0;
		energyGaugeFrame.texture.animChangeTimer.count = 0;

		miniEnergyGauge.frameTexture.animNum = 0;
		miniEnergyGauge.frameTexture.animChangeTimer.count = 0;

		energyGaugeFrame.viewPos = energyGaugeFrame.pos;

		//energyGaugeFrame.alarmSeTimer.count = 0;

		if (!GetIsWarningBoss()) {
			CancelDangerDisplayEffect();
		}
	}

	//ダメージによる振動処理
	if (gaugeShakeDamageTimer.count <= gaugeShakeDamageTimer.time) {
		gaugeShakeDamageTimer.count++;

		if (kGaugeShakeSwitch) {
			energyGaugeFrame.viewPos.x = energyGaugeFrame.pos.x + GetRand(-4, 4);
			energyGaugeFrame.viewPos.y = energyGaugeFrame.pos.y + GetRand(-4, 4);
		}

	}

#pragma endregion

#pragma region ケムリ生成処理

	if (remainEnergy <= energyGaugeFrame.warnDeadEnergy) {
		energyGaugeFrame.createSmokeWarningTimer.count++;
	} else if (remainEnergy <= energyGaugeFrame.dangerEnergy) {
		energyGaugeFrame.createSmokeOverHeatTimer.count++;
	}

	if (energyGaugeFrame.createSmokeWarningTimer.count >= energyGaugeFrame.createSmokeWarningTimer.time
		|| energyGaugeFrame.createSmokeOverHeatTimer.count >= energyGaugeFrame.createSmokeOverHeatTimer.time) {
		energyGaugeFrame.createSmokeWarningTimer.count = 0;
		energyGaugeFrame.createSmokeOverHeatTimer.count = 0;

		//生成座標を設定
		float posX = energyGaugeFrame.pos.x + ToFloat(GetRand(-energyGaugeFrame.spawnSmokeRange.x, energyGaugeFrame.spawnSmokeRange.x));
		float posY = energyGaugeFrame.pos.y + ToFloat(GetRand(0, energyGaugeFrame.spawnSmokeRange.y));

		CreateSmokeEffect({ posX, posY }, TypeDrawEffect::UI);
	}

#pragma endregion

#pragma region ミニゲージの座標更新
	//miniGaugeの座標を更新
	Vector2 playerPos = GetPlayerPos();
	miniEnergyGauge.pos = { playerPos.x + miniEnergyGauge.offset.x, playerPos.y + miniEnergyGauge.offset.y };

#pragma endregion

}

static void DrawEnergyGauge(void) {
	//通常ゲージ
	//フレーム
	DrawTextureUI(energyGaugeFrame.texture, energyGaugeFrame.viewPos, energyGaugeFrame.size);
	//delayメーター
	DrawGaugeAsUI(energyGaugeDelayMeter.texture, energyGaugeFrame.viewPos, energyGaugeFrame.size, energyGaugeDelayMeter.gaugeRate);
	//メーター
	DrawGaugeAsUI(energyGaugeMeter.texture, energyGaugeFrame.viewPos, energyGaugeFrame.size, energyGaugeMeter.gaugeRate);

	//ミニゲージ
	//フレーム
	DrawTextureObj(miniEnergyGauge.frameTexture, miniEnergyGauge.pos, miniEnergyGauge.size);
	//delayメーター
	DrawGaugeAsObj(miniEnergyGauge.delayMeterTexture, miniEnergyGauge.pos, miniEnergyGauge.size, energyGaugeDelayMeter.gaugeRate);
	//メーター
	DrawGaugeAsObj(miniEnergyGauge.meterTexture, miniEnergyGauge.pos, miniEnergyGauge.size, energyGaugeMeter.gaugeRate);
}

#pragma endregion

#pragma region 関数: ボスHPゲージ

static void InitBossHpGauge(void) {
	bossHpGaugeFrame.texture = GetTexture(TextureType::BossHpGaugeFrame);
	bossHpGaugeMeter.texture = GetTexture(TextureType::BossHpGaugeMeter);
	bossHpGaugeDelayMeter.texture = GetTexture(TextureType::BossHpGaugeDelayMeter);
}

static void UpdateBossHpGauge(void) {
	if (!GetBossEnemyIsAlive()) return;

	bossHpGaugeMeter.gaugeRate = ToFloat(GetBossEnemyRemainLife()) / ToFloat(GetBossEnemyRemainLifeMax());

#pragma region delayMeterの変化処理


	float nowRate = bossHpGaugeMeter.gaugeRate;
	float diff = bossHpGaugeDelayMeter.gaugeRate - nowRate;

	//delayゲージが本ゲージより大きい(減少した際)
	if (bossHpGaugeDelayMeter.gaugeRate > nowRate) {

		//エージングでの更新
		if (diff > bossHpGaugeDelayMeter.threshold) {
			//始点と終点を更新
			bossHpGaugeDelayMeter.startRate = bossHpGaugeDelayMeter.gaugeRate;
			bossHpGaugeDelayMeter.endRate = nowRate;

			//easingTを更新
			bossHpGaugeDelayMeter.easingT += bossHpGaugeDelayMeter.followSpeed;
			if (bossHpGaugeDelayMeter.easingT > 1.0f) {
				bossHpGaugeDelayMeter.easingT = 1.0f;
			}

			//更新
			bossHpGaugeDelayMeter.gaugeRate =
				UpdateValueByEasing(
					bossHpGaugeDelayMeter.startRate,
					bossHpGaugeDelayMeter.endRate,
					bossHpGaugeDelayMeter.easingT,
					EasingMode::EaseIn
				);
		} else {
			bossHpGaugeDelayMeter.gaugeRate = nowRate;
			bossHpGaugeDelayMeter.easingT = 0.0f;
		}

	} else {
		bossHpGaugeDelayMeter.gaugeRate = nowRate;
		bossHpGaugeDelayMeter.easingT = 0.0f;
	}

#pragma endregion

}

static void DrawBossHpGauge(void) {
	if (!GetBossEnemyIsAlive()) return;

	//フレーム
	DrawTextureUI(bossHpGaugeFrame.texture, bossHpGaugeFrame.pos, bossHpGaugeFrame.size);
	//delayMeter
	DrawGaugeAsUI(bossHpGaugeDelayMeter.texture, bossHpGaugeFrame.pos, bossHpGaugeFrame.size, bossHpGaugeDelayMeter.gaugeRate);
	//メーター
	DrawGaugeAsUI(bossHpGaugeMeter.texture, bossHpGaugeFrame.pos, bossHpGaugeFrame.size, bossHpGaugeMeter.gaugeRate);

#ifdef _DEBUG

	//DrawTextureUI(lifeIcon.texture, GetEnergyGaugeEndPos(), lifeIcon.size);

#endif // _DEBUG


}

#pragma endregion

#pragma region 関数: 持続ボーナス

static void UpdatePowRate(void) {
	if (GetPlayerNowLaserLevel() == PlayerLaserLevel::Level2) {
		laserPowRate.powRate = GetPlayerLaserPowRate();
	}
}

static void DrawPowRate(void) {
	if (GetPlayerNowLaserLevel() != PlayerLaserLevel::Level2) return;

	if (!GetPlayerIsAlive()) return;

	/*Vector2i posS = laserPowRate.startPos;*/
	Vector2 playerPos = GetPlayerPos();
	Vector2i posS = ConvertPosWToS({ playerPos.x, playerPos.y + 40 });

	Novice::ScreenPrintf(posS.x, posS.y, "DAMAGE RATE: %.3f!!!", laserPowRate.powRate);
}

#pragma endregion

#pragma region 関数: 進捗度ゲージ

static void InitProgressGauge(void) {
	if (!kShowProgressGaugeSwitch) return;

	progressGauge.frameTexture = GetTexture(TextureType::ProgressGauge);
	progressGaugeIcon.texture = GetTexture(TextureType::ProgressGaugeIcon);

	progressGauge.bossRoundTime = ConvertTimeDataToInt(GetBossRoundTimeData());
}

static void UpdateProgressGauge(void) {
	if (!kShowProgressGaugeSwitch) return;
	//ボスが出たら出現しない
	if (GetBossEnemyIsAlive()) return;

	progressGauge.gaugeRate = ToFloat(ConvertTimeDataToInt(GetGameTimeData())) / ToFloat(progressGauge.bossRoundTime);

	//アイコンの移動
	progressGaugeIcon.pos.x = UpdateValueByEasing(progressGaugeIcon.startPos.x, progressGaugeIcon.endPos.x, progressGauge.gaugeRate, EasingMode::Linear);
}

static void DrawProgressGauge(void) {
	if (!kShowProgressGaugeSwitch) return;
	//ボスが出たら出現しない
	if (GetBossEnemyIsAlive()) return;

	DrawTextureUI(progressGauge.frameTexture, progressGauge.pos, progressGauge.size);
	DrawTextureUI(progressGaugeIcon.texture, progressGaugeIcon.pos, progressGaugeIcon.size);
}

#pragma endregion

#pragma region 関数: コンボ表示

static void InitShockCombo(void) {
	for (int i = 0; i < kComboNumbersLimit; i++) {
		comboNumbers[i] = {};
	}
	comboNumbersTexture = GetTexture(TextureType::ComboNumbers);
	comboNumbersMeterTexture = GetTexture(TextureType::ComboNumbersMeter);

	comboIcon = {};
	comboIcon.texture = GetTexture(TextureType::ComboIcon);
}

static void UpdateShockCombo(void) {

	//gaugeRateの更新
	Timer comboTimer = GetComboTimer();
	comboIcon.gaugeRate = 1.0f - ToFloat(comboTimer.count) / ToFloat(comboTimer.time);

	//コンボ持続中
	if (!comboIcon.isShowPreCombo) {
		comboIcon.nowCombo = GetShockCombo();

		for (int i = 0; i < kComboNumbersLimit; i++) {
			comboNumbers[i].animNum = GetValueAtDigit(comboIcon.nowCombo, i);
		}
	}
	//コンボ終了後
	else {

#pragma region 点滅処理
		comboIcon.invisibleTimer.count++;
		if (comboIcon.invisibleTimer.count >= comboIcon.invisibleTimer.time) {
			comboIcon.invisibleTimer.count = 0;

			if (!comboIcon.isInvisible) {
				comboIcon.isInvisible = true;
			} else {
				comboIcon.isInvisible = false;
			}
		}
#pragma endregion

		//表示後、上に移動する処理
		comboIcon.showPreComboTimer.count++;
		if (comboIcon.showPreComboTimer.count >= comboIcon.showPreComboTimer.time) {

			//一定時間表示したら上へ向かう
			comboIcon.easingT += comboIcon.easingSpeed;
			if (comboIcon.easingT >= 1.0f) {
				comboIcon.easingT = 0.0f;

				comboIcon.showPreComboTimer.count = 0;
				comboIcon.isShowPreCombo = false;

				comboIcon.isInvisible = false;
				comboIcon.invisibleTimer.count = 0;
				comboIcon.nowCombo = 0;
			}
		}
	}

	comboIcon.pos = MoveByEasing(comboIcon.startPos, comboIcon.endPos, comboIcon.easingT, EasingMode::EaseIn);

	if (comboIcon.nowCombo >= kStartDisplayEffectCombo) {
		TriggerDisplayEffect();
	} else {
		displayEffect.isShoot = false;
	}

}

static void DrawShockCombo(void) {

	if (comboIcon.isInvisible) return;

	int digits = 0;
	//表示しない
	if (comboIcon.nowCombo < kStartViewCombo) {
		return;
	} else if (comboIcon.nowCombo < 1000) {
		digits = 3;
	} else {
		digits = 4;
	}

	//scoreNumbers
	for (int i = 0; i < digits; i++) {
		Vector2 pos = { comboNumbers[i].startPos.x + comboNumbers[i].interval.x * i, comboIcon.pos.y + comboNumbers[i].offsetY };

		Texture texture = comboNumbersTexture;
		texture.animNum = comboNumbers[i].animNum;

		Texture meterTexture = comboNumbersMeterTexture;
		meterTexture.animNum = comboNumbers[i].animNum;

		DrawTextureUI(texture, pos, comboNumbers[i].size);
		DrawGaugeYAsUI(meterTexture, pos, comboNumbers[i].size, comboIcon.gaugeRate);
	}
	//ComboIconの表示
	DrawTextureUI(comboIcon.texture, comboIcon.pos, comboIcon.size);

}

#pragma endregion

#pragma region 関数: スコア

static void InitScore(void) {
	for (int i = 0; i < kScoreNumbersLimit; i++) {
		scoreNumbers[i] = {};
	}
	scoreNumbersTexture = GetTexture(TextureType::ScoreNumbers);
}

static void UpdateScore(void) {

	int nowScore = GetNowScore();
	int diff = nowScore - scoreIcon.showScore;

	//一定値以上得点を得ていたら、
	if (diff >= scoreIcon.threshold) {
		//イージング開始時のみstartScoreを更新
		if (scoreIcon.easingT == 0.0f) {
			scoreIcon.startScore = scoreIcon.showScore;
			scoreIcon.endScore = nowScore;
		}

		scoreIcon.easingT += scoreIcon.easingSpeed;
		if (scoreIcon.easingT > 1.0f) {
 			scoreIcon.easingT = 1.0f;


		}

 		scoreIcon.showScore = UpdateValueByEasing(scoreIcon.startScore, scoreIcon.endScore, scoreIcon.easingT, EasingMode::Linear);

		PlaySe(SeType::CountUpScore);

	} else {
		scoreIcon.showScore = nowScore;
		scoreIcon.easingT = 0.0f;

	}

	for (int i = 0; i < kScoreNumbersLimit; i++) {
		scoreNumbers[i].animNum = GetValueAtDigit(scoreIcon.showScore, i);
	}
}

static void DrawScore(void) {
	for (int i = 0; i < kScoreNumbersLimit; i++) {
		Vector2 pos = { scoreNumbers[i].startPos.x + scoreNumbers[i].interval.x * (kScoreNumbersLimit - i - 1), scoreNumbers[i].startPos.y };

		Texture texture = scoreNumbersTexture;
		texture.animNum = scoreNumbers[i].animNum;

		DrawTextureUI(texture, pos, scoreNumbers[i].size);

		//scoreIconの表示
		//DrawTextureUI(scoreIcon.texture, scoreIcon.pos, scoreIcon.size);
	}

	//Novice::ScreenPrintf(30, 30, "Score: %07d", GetNowScore());
}

#pragma endregion

#pragma region 関数: DisplayEffect

static void InitDisplayEffect(void) {
	displayEffect = {};
	displayEffectTexture = GetTexture(TextureType::DisplayEffect);
}

static void UpdateDisplayEffect(void) {

	//イージング処理
	if (displayEffect.isScaleDown && displayEffect.isShoot) {
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

	//拡大処理
	displayEffect.size = MoveByEasing(displayEffect.maxSize, displayEffect.minSize, displayEffect.easingT, EasingMode::EaseIn);
	//透明度の処理
	displayEffect.alpha = UpdateValueByEasing(displayEffect.alphaMin, displayEffect.alphaMax, displayEffect.easingT, EasingMode::EaseIn);
}

static void DrawDisplayEffect(void) {
	if (!displayEffect.isShoot && displayEffect.alpha <= displayEffect.alphaMin) return;

	Texture texture = displayEffectTexture;
	texture.color = (displayEffect.color & 0xFFFFFF00) | displayEffect.alpha;

	DrawTextureUI(texture, displayEffect.pos, displayEffect.size);
}

void TriggerDisplayEffect(void) {
	if (displayEffect.isShoot) return;

	displayEffect.isShoot = true;
	displayEffect.easingT = 0.0f;
	displayEffect.isScaleDown = true;

}

static void InitDangerDisplayEffect(void) {
	dangerDisplayEffect = {};
	displayEffectTexture = GetTexture(TextureType::DisplayEffect);
}

static void UpdateDangerDisplayEffect(void) {

	//イージング処理
	if (dangerDisplayEffect.isScaleDown && dangerDisplayEffect.isShoot) {
		dangerDisplayEffect.easingT += dangerDisplayEffect.easingSpeed;
		if (dangerDisplayEffect.easingT >= 1.0f) {
			dangerDisplayEffect.easingT = 1.0f;
			dangerDisplayEffect.isScaleDown = false;

			if (!GetIsWarningBoss()) {
				//ゲージ警告SE
				PlaySe(SeType::GaugeAlarm);
			} else {
				//ボス警告演出
				PlaySe(SeType::WarningBossAlarm);
			}
		}
	} else {
		dangerDisplayEffect.easingT -= dangerDisplayEffect.easingSpeed;
		if (dangerDisplayEffect.easingT <= 0.0f) {
			dangerDisplayEffect.easingT = 0.0f;
			dangerDisplayEffect.isScaleDown = true;
		}
	}

	//拡大処理
	dangerDisplayEffect.size = MoveByEasing(dangerDisplayEffect.maxSize, dangerDisplayEffect.minSize, dangerDisplayEffect.easingT, EasingMode::EaseIn);
	//透明度の処理
	dangerDisplayEffect.alpha = UpdateValueByEasing(dangerDisplayEffect.alphaMin, dangerDisplayEffect.alphaMax, dangerDisplayEffect.easingT, EasingMode::EaseIn);
}

static void DrawDangerDisplayEffect(void) {
	if (!dangerDisplayEffect.isShoot && dangerDisplayEffect.alpha <= dangerDisplayEffect.alphaMin) return;

	Texture texture = displayEffectTexture;
	texture.color = (dangerDisplayEffect.color & 0xFFFFFF00) | dangerDisplayEffect.alpha;

	DrawTextureUI(texture, dangerDisplayEffect.pos, dangerDisplayEffect.size);
}

void TriggerDangerDisplayEffect(void) {
	if (dangerDisplayEffect.isShoot) return;

	dangerDisplayEffect.isShoot = true;
	dangerDisplayEffect.easingT = 0.0f;
	dangerDisplayEffect.isScaleDown = true;

}

void CancelDangerDisplayEffect(void) {
	if (!dangerDisplayEffect.isShoot) return;

	dangerDisplayEffect.isShoot = false;
}

#pragma endregion

#pragma region 関数: 開始演出

static void InitStartTelop(void) {
	startTelop = {};
	readyTexture = GetTexture(TextureType::ReadyTelop);
	goTexture = GetTexture(TextureType::GoTelop);
}

static void UpdateStartTelop(void) {

}

static void DrawStartTelop(void) {
	ModeStartEffect nowMode = GetNowModeStart();

	if (nowMode == ModeStartEffect::WaitPlayerSpawn || nowMode == ModeStartEffect::Standby) return;

	Texture texture;
	texture = (nowMode == ModeStartEffect::Ready) ? readyTexture : goTexture;

	DrawTextureUI(texture, startTelop.pos, startTelop.size);
}

#pragma endregion

#pragma region 関数: ボス警告演出

static void InitBossTelop(void) {
	bossTelop = {};
	bossTelop.texture = GetTexture(TextureType::WarnBossTelop);
}

static void UpdateBossTelop(void) {

	//媒介変数の更新
	if (bossTelop.isFadeIn && GetIsWarningBoss()) {
		TriggerDangerDisplayEffect();

		bossTelop.easingT += bossTelop.easingSpeed;
		if (bossTelop.easingT >= 1.0f) {
			bossTelop.easingT = 1.0f;
			bossTelop.isFadeIn = false;

			/*PlaySe(SeType::WarningBossAlarm);*/

		}

	} else {
		bossTelop.easingT -= bossTelop.easingSpeed;
		if (bossTelop.easingT <= 0.0f) {
			bossTelop.easingT = 0.0f;
			bossTelop.isFadeIn = true;


		}
	}

	//フェード処理
	bossTelop.alpha = UpdateValueByEasing(bossTelop.alphaMin, bossTelop.alphaMax, bossTelop.easingT, bossTelop.easingMode);
}

static void DrawBossTelop(void) {
	if (!GetIsWarningBoss() && bossTelop.alpha <= 0) return;

	//振動処理
	Vector2 showPos;
	showPos.x = bossTelop.pos.x + ToFloat(GetRand(-bossTelop.shakeDuration, bossTelop.shakeDuration));
	showPos.y = bossTelop.pos.y + ToFloat(GetRand(-bossTelop.shakeDuration, bossTelop.shakeDuration));

	Texture texture = bossTelop.texture;
	texture.color = (texture.color & 0xFFFFFF00) | bossTelop.alpha;

	DrawTextureUI(texture, showPos, bossTelop.size);
}

#pragma endregion

void InitPlayUI(void) {
	InitLifeIcon();
	InitEnergyGauge();
	InitBossHpGauge();
	InitProgressGauge();
	InitShockCombo();
	InitScore();
	InitDisplayEffect();
	InitDangerDisplayEffect();
	InitStartTelop();
	InitBossTelop();
}

void UpdatePlayUI(void) {
	UpdateEnergyGauge();
	UpdateBossHpGauge();
	UpdatePowRate();
	UpdateProgressGauge();
	UpdateShockCombo();
	UpdateScore();
	UpdateDisplayEffect();
	UpdateDangerDisplayEffect();
	UpdateStartTelop();
	UpdateBossTelop();
}

void DrawPlayUI(void) {
	DrawDisplayEffect();
	DrawDangerDisplayEffect();

	DrawLifeIcon();
	DrawEnergyGauge();
	DrawBossHpGauge();
	DrawPowRate();
	DrawProgressGauge();
	DrawShockCombo();
	DrawStartTelop();
	DrawBossTelop();

	DrawScore();
}

#pragma region 関数: 外部参照関係

Vector2 GetEnergyGaugeEndPos(void) {
	float posX = energyGaugeFrame.pos.x - energyGaugeFrame.size.x / 2 + energyGaugeFrame.size.x * energyGaugeMeter.gaugeRate;
	float posY = energyGaugeFrame.pos.y - energyGaugeFrame.size.y * 1 / 4;

	return { posX, posY };
}

Vector2 GetBossHpGaugeEndPos(void) {
	float posX = bossHpGaugeFrame.pos.x - bossHpGaugeFrame.size.x / 2 + bossHpGaugeFrame.size.x * bossHpGaugeMeter.gaugeRate;
	float posY = bossHpGaugeFrame.pos.y - bossHpGaugeFrame.size.y / 2;

	return { posX, posY };
}

void TriggerEndCombo(void) {
	comboIcon.isShowPreCombo = true;
	comboIcon.showPreComboTimer.count = 0;
}

void TriggerGaugeShackByDamage(void) {
	gaugeShakeDamageTimer.count = 0;
}

#pragma endregion