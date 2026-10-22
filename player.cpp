#include "player.h"
#include "player_laser.h"
#include "elec_bullet.h"
#include "stage.h"
#include "enemy.h"
#include "gem.h"
#include "effect.h"
#include "boss_enemy.h"
#include "play_ui.h"
#include "se.h"
#include "bgm.h"

#include "vector2.h"
#include "tool.h"
#include "input.h"
#include "texture.h"

#include <Novice.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <algorithm>


//ギリ避けスイッチ
constexpr bool kDodgeCloseSwitch = false;
//ジェムをロックオンの対象にするフラグ
constexpr bool kGemLockOnSwitch = true;
//一定範囲内に敵がいなければ、ジェムをロックオンするスイッチ
constexpr bool KCancelLockOnEnemySwitch = true;
//シールド制スイッチ
constexpr bool kElecShieldSwitch = true;

#pragma region データ: player本体
struct Player {
	Vector2 pos = { 1280, 720 };
	Vector2 size = { 32, 32 };

	//<生存関係>
	bool isAlive = true;
	//当たり判定の大きさ
	float hitRadius = 10.0f;
	int hp = 1;
	int hpMax = 1;

	//<ギリ避け関係>
	float dodgeCloseRadius = 50.0f;

	//<無敵関係>
	//無敵フラグ
	bool isInvincible = false;
	//無敵の持続時間
	Timer invincibleTimer = { 100, 0 };
	//ノックバック関係
	bool isKnockBack = false;

	//<移動関係>
	Vector2 velocity = { 0.0f, 0.0f };
	//加速度
	float accleretionSpeed = 0.14f;
	//スピード上限
	float moveSpeedLimit = 9.0f;
	//減速度
	float decelerationSpeed = 0.15f;

	//移動速度(等速)
	float moveSpeed[2] = { 6.0f, 3.0f };

	Vector2 inputVec = { 0, 0 };


	//<回転関係>
	float rotateTheta = 0.0f;
	//回転速度
	float rotateSpeed = 0.05f;


	//<ロックオン関係>
	//ロック状態フラグ
	bool isLock = false;
	//回転速度
	float lockOnRotateSpeed = 0.2f;


	////<プレイヤー状態>
	//PlayerState nowState = PlayerState::Normal;

	//<射撃関係>
	//射撃スパン
	Timer shootTimer = { 2, 0 };
	//エネルギー残量
	float remainEnergy = 0.0f;

	//<レベル関係>
	//レーザーのレベル
	PlayerLaserLevel laserLevel = PlayerLaserLevel::Level0;
	//レベルアップのライン
	float levelUpLine[ToInt(PlayerLaserLevel::Count)] = { 0.0f, 1.0f, 80.0f };

	Texture texture = {};
};
Player player;

//<無敵時の点滅関係>
//点滅のスパン
static bool isInvisible = false;
static Timer invisibleTimer = { 5, 0 };

#pragma endregion

#pragma region データ: 可動域
Vector2 movablePos[2] = { {0, 0}, {0, 0} };

#pragma endregion

#pragma region データ: ロックオン関係

enum class LockTargetType {
	None = -1,
	Boss = -2,
	Enemy1 = 0,
	Enemy2 = 1,
	EnemyBasic = 2,
	EnemySmall = 3,
	EnemyM = 4,
	EnemyBomb = 5,

	Gem,
};
LockTargetType nowTargetType = LockTargetType::None;
//ターゲットの番号
int targetIndex = -1;

//敵を除外する範囲
static constexpr float kCancelLockOnEnemyRange = 400;

struct LockOnSign {
	Vector2 pos = {};
	Vector2 size = { 64, 64 };

	Texture texture = {};
};
LockOnSign lockOnSign;

#pragma endregion

#pragma region データ: 射撃関係

static Vector2 shootPos = {};
//プレイヤーの中心からの発射位置の距離
constexpr float kShootDisLength = 15.0f;

//チャージタイマー
Timer chargeElecBulletTimer = { 40, 0 };
float consumptionShootNormal = 0.5f;
float consumptionShootCharge = 10.0f;
static bool isCharge = false;

float laserPowRate;

#pragma endregion

#pragma region データ: エネルギー関係

//エネルギー上限
constexpr float kEnergyMax = 100.0f;
//エネルギー下限
constexpr float kEnergyMin = 0.0f;
//エネルギーゲージの中間
constexpr float kEnergyCenter = (kEnergyMax - kEnergyMin) / 2;
constexpr float kFirstEnergy = kEnergyMax;

//エネルギー消費量(常時)
static constexpr float kConsumptionEnergy = 0.05f;

//シールドが消えるエネルギー量のライン
static constexpr float kBreakShieldLine = 5.0f;

////故障時のenergy回復量
//constexpr float kRecoveryEnergyDuringEmpty = 0.15f;

#pragma endregion

#pragma region データ: ギリ避け・ノックバック関係

//ギリ避け時のボーナススパン
static Timer dodgeCloseBonusTimer = { 8, 0 };

//ノックバックの初速度
static constexpr float kFirstKnockBackPow = 20.0f;
//ノックバックの減速度
static constexpr float kDecelerationKnockBackPow = 0.1f;

static float knockBackPow = kFirstKnockBackPow;
static Vector2 knockBackVec = {};
static Timer knockBackTimer = { 20, 0 };

#pragma endregion

#pragma region データ: 演出関係

//プレイヤーの色管理データ
struct ColorManager {
	static constexpr int colorValueMin = 32;
	static constexpr int colorValueMax = 255;
	//現在のプレイヤーの色
	int colorValue = 255;

	//イージング用
	float easingT = 0.0f;
	float changeSpeed = 0.05f;
	bool isGoToBlack = true;
};
ColorManager colorManager;

//チャージの破片表示を開始する時間
constexpr int kStartCreateChargeChipTime = 7;
Timer createChargeOrbTimer = { 10, 0 };
Timer createChargeChipTimer = { 10, 0 };


//オーバーヒート時の生成範囲(player中心で)
constexpr int kCreateSmokeRange = 45;
//オーバーヒート時に出す煙の数
constexpr int kCreateSmokeNum = 4;
//オーバーヒート時のケムリ生成スパン
Timer createSmokeTimer = { 15, 0 };

//警告演出が出てくるエネルギーの量
static constexpr float kWarnEnergyLine = 30.0f;

//開始演出処理
struct StartEffectManager {

	//開始演出の状態
	ModeStartEffect nowMode = ModeStartEffect::WaitPlayerSpawn;

	//イージング関係
	float easingT = 0.0f;
	float easingSpeed = 0.05f;
	EasingMode easingMode = EasingMode::EaseIn;

	//回転速度
	float rotateSpeed = ToFloat(M_PI) / 20.0f;

	//サイズ
	float sizeValue = 0;
	float sizeValueMax = 500;
	float sizeValueMin = player.size.x;

	//不透明度
	int alpha = 0;
	int alphaMin = 0;
	int alphaMax = 255;
};
StartEffectManager startEffectManager;

struct DeadEffectManager {
	int exploseCount = 6;
	int createExploseRange = 100;
};
DeadEffectManager deadEffectManager;

struct ElecShield {
	Vector2 size = {};

	//イージング関係
	float easingT = 0.0f;
	float easingSpeedMin = 0.01f;
	float easingSpeedMax = 0.05f;
	bool isScaleUp = true;
	EasingMode easingMode = EasingMode::EaseInOut;

	//サイズ関係
	Vector2 sizeValueMin = { 40.0f, 40.0f };
	Vector2 sizeValueMinDanger = { 10.0f, 10.0f };
	Vector2 sizeValueMax = { 60.0f, 60.0f };

	//透明度関係
	int alpha = 0;
	int alphaMin = 130;
	int alphaMinDanger = 0;
	int alphaMax = 170;

	unsigned int color = 0x04C1E8FF;

	Texture texture = {};
};
ElecShield elecShield;

#pragma endregion

/// <summary>
/// 入力検知
/// </summary>
static void CheckInput(void) {

#pragma region 移動関係
	//横
	if (CheckInputAction(InputAction::MoveRight)) {
		player.inputVec.x = 1;
	} else if (CheckInputAction(InputAction::MoveLeft)) {
		player.inputVec.x = -1;
	} else {
		player.inputVec.x = 0;
	}
	//縦
	if (CheckInputAction(InputAction::MoveUp)) {
		player.inputVec.y = 1;
	} else if (CheckInputAction(InputAction::MoveDown)) {
		player.inputVec.y = -1;
	} else {
		player.inputVec.y = 0;
	}

#pragma endregion

}

#pragma region 関数: 基本動作

static void MovePlayer(void) {
	if (!player.isAlive) {
		return;
	}

	Vector2 moveVec = player.inputVec;
	float moveVecLength = sqrtf(moveVec.x * moveVec.x + moveVec.y * moveVec.y);
	if (moveVecLength != 0) {
		moveVec.x /= moveVecLength;
		moveVec.y /= moveVecLength;
	}

	//energy量で移動速度を変更
	float moveSpeed = (player.remainEnergy > 0.0f) ? player.moveSpeed[0] : player.moveSpeed[1];

	player.pos.x += moveSpeed * moveVec.x;
	player.pos.y += moveSpeed * moveVec.y;

	//playerの移動エフェクト
	CreatePlayerMoveEffect(player.pos, moveVec);
}

static void ClampPlayerPos(void) {
	//横
	if (player.pos.x < movablePos[0].x) {
		player.pos.x = movablePos[0].x;
	} else if (player.pos.x > movablePos[1].x) {
		player.pos.x = movablePos[1].x;
	}

	//縦
	if (player.pos.y < movablePos[0].y) {
		player.pos.y = movablePos[0].y;
	} else if (player.pos.y > movablePos[1].y) {
		player.pos.y = movablePos[1].y;
	}
}

#pragma endregion

#pragma region 関数: 回転&ロックオン関係

/// <summary>
/// 通常時の回転処理
/// </summary>
/// <param name=""></param>
static void RotatePlayer(void) {
	if (player.isLock) return;

	float closestDis = 99999.0f;
	nowTargetType = LockTargetType::None;
	targetIndex = -1;

	EnemyShooter* enemy1 = GetEnemyShooterArray();
	int enemy1Limit = GetEnemyShooterLimit();

	EnemyGuardian* enemy2 = GetEnemyGuardianArray();
	int enemy2Limit = GetEnemyGuardianLimit();

	EnemyBasic* enemyBasic = GetEnemyBasicArray();
	int enemyBasicLimit = GetEnemyBasicLimit();

	EnemySmall* enemySmall = GetEnemySmallArray();
	int enemySmallLimit = GetEnemySmallLimit();

	EnemyM* enemyM = GetEnemyMArray();
	int enemyMLimit = GetEnemyMLimit();

	EnemyBomb* enemyBomb = GetEnemyBombArray();
	int enemyBombLimit = GetEnemyBombLimit();

	Gem* gem = GetGemArray();
	int gemLimit = GetGemLimit();

	// enemy1探索
	for (int i = 0; i < enemy1Limit; i++) {
		if (!enemy1[i].isAlive) continue;

		float dx = enemy1[i].pos.x - player.pos.x;
		float dy = enemy1[i].pos.y - player.pos.y;
		float dis = sqrtf(dx * dx + dy * dy);

		if (dis < closestDis) {
			closestDis = dis;
			targetIndex = i;
			nowTargetType = LockTargetType::Enemy1;
		}
	}

	for (int i = 0; i < enemy2Limit; i++) {
		if (!enemy2[i].isAlive) continue;

		float dx = enemy2[i].pos.x - player.pos.x;
		float dy = enemy2[i].pos.y - player.pos.y;
		float dis = sqrtf(dx * dx + dy * dy);

		if (dis < closestDis) {
			closestDis = dis;
			targetIndex = i;
			nowTargetType = LockTargetType::Enemy2;
		}
	}

	for (int i = 0; i < enemyBasicLimit; i++) {
		if (!enemyBasic[i].isAlive) continue;

		float dx = enemyBasic[i].pos.x - player.pos.x;
		float dy = enemyBasic[i].pos.y - player.pos.y;
		float dis = sqrtf(dx * dx + dy * dy);

		if (dis < closestDis) {
			closestDis = dis;
			targetIndex = i;
			nowTargetType = LockTargetType::EnemyBasic;
		}
	}

	for (int i = 0; i < enemySmallLimit; i++) {
		if (!enemySmall[i].isAlive) continue;

		float dx = enemySmall[i].pos.x - player.pos.x;
		float dy = enemySmall[i].pos.y - player.pos.y;
		float dis = sqrtf(dx * dx + dy * dy);

		if (dis < closestDis) {
			closestDis = dis;
			targetIndex = i;
			nowTargetType = LockTargetType::EnemySmall;
		}
	}

	for (int i = 0; i < enemyMLimit; i++) {
		if (!enemyM[i].isAlive) continue;

		float dx = enemyM[i].pos.x - player.pos.x;
		float dy = enemyM[i].pos.y - player.pos.y;
		float dis = sqrtf(dx * dx + dy * dy);

		if (dis < closestDis) {
			closestDis = dis;
			targetIndex = i;
			nowTargetType = LockTargetType::EnemyM;
		}
	}

	for (int i = 0; i < enemyBombLimit; i++) {
		if (!enemyBomb[i].isAlive) continue;

		float dx = enemyBomb[i].pos.x - player.pos.x;
		float dy = enemyBomb[i].pos.y - player.pos.y;
		float dis = sqrtf(dx * dx + dy * dy);

		if (dis < closestDis) {
			closestDis = dis;
			targetIndex = i;
			nowTargetType = LockTargetType::EnemyBomb;
		}
	}

	// boss探索
	BossEnemy* boss = GetBossEnemy();
	if (boss->isAlive) {
		float dx = boss->pos.x - player.pos.x;
		float dy = boss->pos.y - player.pos.y;
		float dis = sqrtf(dx * dx + dy * dy);

		if (dis < closestDis) {
			closestDis = dis;
			targetIndex = 0;
			nowTargetType = LockTargetType::Boss;
		}
	}

	//ジェム
	if (kGemLockOnSwitch && KCancelLockOnEnemySwitch && closestDis > kCancelLockOnEnemyRange) {
		for (int i = 0; i < gemLimit; i++) {
			if (!gem[i].isAlive) continue;

			float dx = gem[i].pos.x - player.pos.x;
			float dy = gem[i].pos.y - player.pos.y;
			float dis = sqrtf(dx * dx + dy * dy);

			if (dis < closestDis) {
				closestDis = dis;
				targetIndex = i;
				nowTargetType = LockTargetType::Gem;
			}
		}
	}

	// ターゲットが死んだらロック解除
	if (nowTargetType == LockTargetType::None) return;

	Vector2 targetPos = {};

	//ボス
	if (nowTargetType == LockTargetType::Boss) {
		if (!boss->isAlive) {
			nowTargetType = LockTargetType::None;
			return;
		}
		targetPos = boss->pos;
	} else if (nowTargetType == LockTargetType::Enemy1) {
		if (!enemy1[targetIndex].isAlive) {
			nowTargetType = LockTargetType::None;
			return;
		}
		targetPos = enemy1[targetIndex].pos;
	} else if (nowTargetType == LockTargetType::Enemy2) {
		if (!enemy2[targetIndex].isAlive) {
			nowTargetType = LockTargetType::None;
			return;
		}
		targetPos = enemy2[targetIndex].pos;
	} else if (nowTargetType == LockTargetType::EnemyBasic) {
		if (!enemyBasic[targetIndex].isAlive) {
			nowTargetType = LockTargetType::None;
			return;
		}
		targetPos = enemyBasic[targetIndex].pos;
	} else if (nowTargetType == LockTargetType::EnemySmall) {
		if (!enemySmall[targetIndex].isAlive) {
			nowTargetType = LockTargetType::None;
			return;
		}
		targetPos = enemySmall[targetIndex].pos;
	} else if (nowTargetType == LockTargetType::EnemyM) {
		if (!enemyM[targetIndex].isAlive) {
			nowTargetType = LockTargetType::None;
			return;
		}
		targetPos = enemyM[targetIndex].pos;
	} else if (nowTargetType == LockTargetType::EnemyBomb) {
		if (!enemyBomb[targetIndex].isAlive) {
			nowTargetType = LockTargetType::None;
			return;
		}
		targetPos = enemyBomb[targetIndex].pos;
	} else if (nowTargetType == LockTargetType::Gem) {
		if (!gem[targetIndex].isAlive) {
			nowTargetType = LockTargetType::None;
			return;
		}
		targetPos = gem[targetIndex].pos;
	}

#pragma region 回転処理
	float dx = targetPos.x - player.pos.x;
	float dy = targetPos.y - player.pos.y;

	float targetTheta = atan2f(dy, dx);
	float diff = targetTheta - player.rotateTheta;

	//差を-π~πに正規化
	while (diff > ToFloat(M_PI)) {
		diff -= ToFloat(M_PI) * 2;
	}
	while (diff < -ToFloat(M_PI)) {
		diff += ToFloat(M_PI) * 2;
	}

	player.rotateTheta += diff * player.lockOnRotateSpeed;

#pragma endregion



}


/// <summary>
/// ロックオンマークの初期化処理
/// </summary>
/// <param name=""></param>
static void InitLockSign(void) {
	lockOnSign = {};
	lockOnSign.texture = GetTexture(TextureType::LockOnSign);
}

/// <summary>
/// ロックオンサインの更新
/// </summary>
/// <param name=""></param>
static void UpdateLockOnSign(void) {
	if (nowTargetType == LockTargetType::None) return;

	//ボスの場合、
	if (nowTargetType == LockTargetType::Boss) {
		BossEnemy* boss = GetBossEnemy();

		lockOnSign.pos = boss->pos;
	}
	//enemy1
	else if (nowTargetType == LockTargetType::Enemy1) {
		EnemyShooter* enemy1 = GetEnemyShooterArray();
		lockOnSign.pos = enemy1[targetIndex].pos;
	}
	//enemy2
	else if (nowTargetType == LockTargetType::Enemy2) {
		EnemyGuardian* enemy2 = GetEnemyGuardianArray();
		lockOnSign.pos = enemy2[targetIndex].pos;
	}
	//enemyBasic
	else if (nowTargetType == LockTargetType::EnemyBasic) {
		EnemyBasic* enemyBasic = GetEnemyBasicArray();
		lockOnSign.pos = enemyBasic[targetIndex].pos;
	}
	//enemySmall
	else if (nowTargetType == LockTargetType::EnemySmall) {
		EnemySmall* enemySmall = GetEnemySmallArray();
		lockOnSign.pos = enemySmall[targetIndex].pos;
	}
	//enemyM
	else if (nowTargetType == LockTargetType::EnemyM) {
		EnemyM* enemyM = GetEnemyMArray();
		lockOnSign.pos = enemyM[targetIndex].pos;
	}
	//enemyM
	else if (nowTargetType == LockTargetType::Gem) {
		Gem* gem = GetGemArray();
		lockOnSign.pos = gem[targetIndex].pos;
	}

	UpdateAnimation(lockOnSign.texture);

}

static void DrawLockOnSign(void) {
	if (nowTargetType == LockTargetType::None) return;

	DrawTextureObj(lockOnSign.texture, lockOnSign.pos, lockOnSign.size);
}

#pragma endregion

#pragma region 関数: 射撃・エネルギー関係

static void UpdateShootPos(void) {
	if (!player.isAlive) return;

	//射撃位置設定
	shootPos.x = player.pos.x + kShootDisLength * cosf(player.rotateTheta);
	shootPos.y = player.pos.y + kShootDisLength * sinf(player.rotateTheta);
}


/// <summary>
/// デンゲキ弾発射処理
/// </summary>
/// <param name=""></param>
static void ShootElecBullet(void) {
	if (!player.isAlive) return;

	////異常発生時は発射しない
	//if (player.nowState != PlayerState::Normal) return;

	//押し
	if (CheckInputAction(InputAction::Charge)) {
		if (chargeElecBulletTimer.count >= chargeElecBulletTimer.time) return;

		if (chargeElecBulletTimer.count > chargeElecBulletTimer.time) return;

		chargeElecBulletTimer.count++;

		//チャージ完了したら、エネルギー消費
		if (chargeElecBulletTimer.count >= chargeElecBulletTimer.time) {

			//一定量たまっていないと打てない
			if (player.remainEnergy < consumptionShootCharge) return;

			player.remainEnergy -= consumptionShootCharge;
			isCharge = true;

			if (player.remainEnergy < 0.0f) {
				player.remainEnergy = 0.0f;
			}
		}
	}
	//離す
	else {
		if (chargeElecBulletTimer.count <= 0) return;

		//通常弾
		if (!isCharge) {
			ShootElecBullet(shootPos, player.rotateTheta, ElecBulletType::Normal);
			player.remainEnergy -= consumptionShootNormal;

			//SEの再生
			PlaySe(SeType::ShootElecBullet);
		}
		//チャージ弾
		else {
			ShootElecBullet(shootPos, player.rotateTheta, ElecBulletType::Charge);

			//SEの再生
			PlaySe(SeType::ShootElecBulletCharge);

			isCharge = false;
		}

		chargeElecBulletTimer.count = 0;

	}

}

/// <summary>
/// エネルギーの常時消費処理
/// </summary>
static void ConsumptionEnergy(void) {
	if (!player.isAlive) return;
	if (startEffectManager.nowMode != ModeStartEffect::Standby) return;

	player.remainEnergy -= kConsumptionEnergy;

	if (player.remainEnergy <= 0.0f) {
		player.remainEnergy = 0.0f;
	}

}

/// <summary>
/// ゲージ維持失敗の確認処理
/// </summary>
/// <param name=""></param>
static void CheckFailedMaintenance(void) {
	//if (!player.isAlive) return;
	//if (GetBossEnemyIsDead()) return;

	///*if (player.nowState != PlayerState::Normal) return;*/

	////デンリョク不足
	//if (player.remainEnergy <= kEnergyMin) {
	//	player.nowState = PlayerState::DeadEnergy;
	//	player.remainEnergy = 0.0f;
	//	chargeElecBulletTimer.count = 0;
	//}

}

///// <summary>
///// 故障中のエネルギー回復処理
///// </summary>
///// <param name=""></param>
//static void RecoveryPlayerState(void) {
//	if (!player.isAlive) return;
//
//	if (kLifeGaugeSwitch) return;
//
//	if (player.nowState == PlayerState::DeadEnergy) {
//		player.remainEnergy += kRecoveryEnergyDuringEmpty;
//		if (player.remainEnergy >= kEnergyCenter) {
//			player.nowState = PlayerState::Normal;
//		}
//	}
//}

#pragma endregion

#pragma region 関数: 演出関係

/// <summary>
/// playerの点滅処理など
/// </summary>
/// <param name=""></param>
static void UpdatePlayerEffect(void) {

}

/// <summary>
/// チャージ中の演出
/// </summary>
/// <param name=""></param>
static void CreateChargeOrbEffect(void) {

	if (chargeElecBulletTimer.count > kStartCreateChargeChipTime) {

		createChargeChipTimer.count++;
		if (createChargeChipTimer.count >= createChargeChipTimer.time) {
			createChargeChipTimer.count = 0;
			CreateChargeChipEffect();
		}

		//チャージSE
		PlaySeContinuous(SeType::Charge);

	} else {
		StopPlayingSe(SeType::Charge);
	}

	if (chargeElecBulletTimer.count >= chargeElecBulletTimer.time) {

		createChargeOrbTimer.count++;
		if (createChargeOrbTimer.count >= createChargeOrbTimer.time) {
			createChargeOrbTimer.count = 0;
			CreateChargeOrbEffect(shootPos);
		}
	}
}

/// <summary>
/// 開始演出時の処理
/// </summary>
/// <param name=""></param>
static void UpdateStartEffect(void) {
	startEffectManager.nowMode = GetNowModeStart();

	if (startEffectManager.nowMode != ModeStartEffect::WaitPlayerSpawn) return;

	if (startEffectManager.easingT >= 1.0f) return;

	startEffectManager.easingT += startEffectManager.easingSpeed;
	if (startEffectManager.easingT >= 1.0f) {
		startEffectManager.easingT = 1.0f;
	}

	player.rotateTheta += startEffectManager.rotateSpeed;

	//サイズ変更
	startEffectManager.sizeValue = UpdateValueByEasing(startEffectManager.sizeValueMax, startEffectManager.sizeValueMin, startEffectManager.easingT, startEffectManager.easingMode);

	//フェード処理
	startEffectManager.alpha = UpdateValueByEasing(startEffectManager.alphaMin, startEffectManager.alphaMax, startEffectManager.easingT, startEffectManager.easingMode);
}

static void InitElecShield(void) {
	elecShield = {};
	elecShield.texture = GetTexture(TextureType::EffectCircle);
}

static void UpdateElecShield(void) {
	//シールドが十分な量存在したら
	if (player.remainEnergy > kBreakShieldLine) {

		//イージング処理
		float energyRate = std::clamp(player.remainEnergy / kEnergyMax, 0.0f, 1.0f);

		if (energyRate >= 0.8f) {
			if (elecShield.easingT < 1.0f) {
				elecShield.easingT += elecShield.easingSpeedMax;

				if (elecShield.easingT > 1.0f) {
					elecShield.easingT = 1.0f;
					elecShield.isScaleUp = false;
				}
			}
			
		} 
		else {
			float t = std::clamp(energyRate / 0.8f, 0.0f, 1.0f);

			float easingSpeed = elecShield.easingSpeedMin * t + elecShield.easingSpeedMax * (1.0f - t);

			if (elecShield.isScaleUp) {
				elecShield.easingT += easingSpeed;
				if (elecShield.easingT >= 1.0f) {
					elecShield.easingT = 1.0f;
					elecShield.isScaleUp = false;
				}
			} else {
				elecShield.easingT -= easingSpeed;
				if (elecShield.easingT < 0.0f) {
					elecShield.easingT = 0.0f;
					elecShield.isScaleUp = true;
				}
			}
		}
	} else {

		elecShield.easingT -= elecShield.easingSpeedMax;
		if (elecShield.easingT < 0.0f) {
			elecShield.easingT = 0.0f;
		}

		elecShield.isScaleUp = true;
	}

	//サイズの変更
	if (player.remainEnergy > kWarnEnergyLine) {
		elecShield.size = MoveByEasing(elecShield.sizeValueMin, elecShield.sizeValueMax, elecShield.easingT, elecShield.easingMode);

		//透明度の変更
		elecShield.alpha = UpdateValueByEasing(elecShield.alphaMin, elecShield.alphaMax, elecShield.easingT, elecShield.easingMode);
	} else {
		elecShield.size = MoveByEasing(elecShield.sizeValueMinDanger, elecShield.sizeValueMax, elecShield.easingT, elecShield.easingMode);

		//透明度の変更
		elecShield.alpha = UpdateValueByEasing(elecShield.alphaMinDanger, elecShield.alphaMax, elecShield.easingT, elecShield.easingMode);
	}


}

static void DrawElecShield(void) {
	Texture texture = elecShield.texture;
	texture.color = (elecShield.color & 0xFFFFFF00) | elecShield.alpha;

	DrawTextureObj(texture, player.pos, elecShield.size);
}

#pragma endregion

#pragma region 関数: 死亡判定など

/// <summary>
/// ノックバック処理
/// </summary>
/// <param name=""></param>
static void KnockBack(void) {
	if (!player.isAlive) return;
	if (!player.isKnockBack) return;

	//ノックバック処理
	player.pos.x += knockBackVec.x * knockBackPow;
	player.pos.y += knockBackVec.y * knockBackPow;

	knockBackPow -= kDecelerationKnockBackPow;

	knockBackTimer.count++;
	if (knockBackTimer.count >= knockBackTimer.time) {
		knockBackTimer.count = 0;
		player.isKnockBack = false;
	}
}

/// <summary>
/// 無敵時の処理
/// </summary>
/// <param name=""></param>
static void UpdateInvinciblePlayer(void) {
	if (!player.isInvincible) return;

#pragma region 点滅処理

	invisibleTimer.count++;
	if (invisibleTimer.count >= invisibleTimer.time) {
		invisibleTimer.count = 0;
		if (!isInvisible) {
			isInvisible = true;
		} else {
			isInvisible = false;
		}
	}

#pragma endregion

	//制限時間のカウント処理
	player.invincibleTimer.count++;
	if (player.invincibleTimer.count >= player.invincibleTimer.time) {
		player.invincibleTimer.count = 0;

		//無敵フラグを折る
		player.isInvincible = false;

		//透明処理をリセット
		isInvisible = false;
		invisibleTimer.count = 0;
	}
}

/// <summary>
/// 死亡判定の処理
/// </summary>
static void CheckDeadPlayer(void) {
	if (!player.isAlive) return;
	if (GetBossEnemyIsDead()) return;

	if (player.hp <= 0) {
		player.remainEnergy = 0;
		player.hp = 0;

		player.isAlive = false;
		chargeElecBulletTimer.count = 0;

		//爆発エフェクト生成
		for (int i = 0; i < deadEffectManager.exploseCount; i++) {
			Vector2 createPos;
			createPos.x = player.pos.x + ToFloat(GetRand(-deadEffectManager.createExploseRange / 2, deadEffectManager.createExploseRange / 2));
			createPos.y = player.pos.y + ToFloat(GetRand(-deadEffectManager.createExploseRange / 2, deadEffectManager.createExploseRange / 2));

			CreateExplodeEffect(createPos, GetPlayerColor());
		}

		//SE
		PlaySe(SeType::DeadPlayer);

		ChangeModeBgmFade(ModeBgmFade::FadeOut);

	}
}

#pragma endregion


void InitPlayer(void) {
	player = {};
	player.texture = GetTexture(TextureType::Player);
	player.remainEnergy = kFirstEnergy;
	startEffectManager = {};

	//chargeOrb.texture = GetTexture(TextureType::ChargeOrb);

	GetMovablePos(movablePos);
	InitLockSign();
	InitElecShield();
}

void UpdatePlayer(void) {
	CheckInput();

	MovePlayer();
	ClampPlayerPos();

	RotatePlayer();
	UpdateLockOnSign();

	UpdateShootPos();
	ShootElecBullet();

	CheckFailedMaintenance();
	//RecoveryPlayerState();

	ConsumptionEnergy();

	UpdatePlayerEffect();
	CreateChargeOrbEffect();

	KnockBack();
	UpdateInvinciblePlayer();
	CheckDeadPlayer();

	UpdateStartEffect();
	UpdateElecShield();
}

void DrawPlayer(void) {
	if (!player.isAlive || isInvisible) return;

	//通常時
	if (startEffectManager.nowMode != ModeStartEffect::WaitPlayerSpawn) {
		DrawTextureRotateObj(player.texture, player.pos, player.size, player.rotateTheta);
	}
	//開始演出
	else {
		Vector2 size = { startEffectManager.sizeValue, startEffectManager.sizeValue };

		Texture texture = player.texture;
		texture.color = (texture.color & 0xFFFFFF00) | startEffectManager.alpha;

		DrawTextureRotateObj(texture, player.pos, size, player.rotateTheta);
	}

	DrawLockOnSign();
	DrawElecShield();

#ifdef _DEBUG
	//DrawHitEllipse(player.pos, player.dodgeCloseRadius, BLUE, false);

#endif // _DEBUG

}

#pragma region 関数: 外部参照関係

Vector2 GetPlayerPos(void) {
	return player.pos;
}

float GetPlayerHitRadius(void) {
	return player.hitRadius;
}

int GetPlayerHp(void) {
	return player.hp;
}

bool GetPlayerIsAlive(void) {
	return player.isAlive;
}

float GetPlayerRemainEnergy(void) {
	return player.remainEnergy;
}

float GetPlayerEnergyLimit(void) {
	return kEnergyMax;
}

float GetPlayerLaserPowRate(void) {
	return laserPowRate;
}

void RecoveryEnergy(float recoveryValue) {
	player.remainEnergy += recoveryValue;
	if (player.remainEnergy > kEnergyMax) {
		player.remainEnergy = kEnergyMax;
	}
}

Vector2 GetPlayerShootPos(void) {
	return shootPos;
}

float GetPlayerRotateTheta(void) {
	return player.rotateTheta;
}

PlayerLaserLevel GetPlayerNowLaserLevel(void) {
	return player.laserLevel;
}

void PlayerDamage(Vector2 hitPos, float damage) {
	if (!player.isAlive || player.isInvincible) return;

	//エネルギーが足りていない状況で接触したら
	if (player.remainEnergy < kBreakShieldLine) {
		player.hp--;
	}

	//エネルギーを減らす
	player.remainEnergy -= damage;

	//まだ生存していたら無敵にする
	if (player.hp > 0) {
		player.isInvincible = true;
		PlaySe(SeType::DamagePlayer);
	}

	//ダメージエフェクトを生成
	CreatePlayerDamageEffect(player.pos);

	TriggerGaugeShakeByDamage();

#pragma region ノックバック方向設定

	knockBackVec = { player.pos.x - hitPos.x, player.pos.y - hitPos.y };
	float length = sqrtf(knockBackVec.x * knockBackVec.x + knockBackVec.y * knockBackVec.y);
	if (length != 0.0f) {
		knockBackVec.x /= length;
		knockBackVec.y /= length;
	}

	knockBackPow = kFirstKnockBackPow;
	player.isKnockBack = true;

#pragma endregion

}

void PlayerKnockBack(Vector2 hitPos) {

	if (player.isKnockBack) return;

	knockBackVec = { player.pos.x - hitPos.x, player.pos.y - hitPos.y };
	float length = sqrtf(knockBackVec.x * knockBackVec.x + knockBackVec.y * knockBackVec.y);
	if (length != 0.0f) {
		knockBackVec.x /= length;
		knockBackVec.y /= length;
	}

	knockBackPow = kFirstKnockBackPow;
	player.isKnockBack = true;


}

bool GetDodgeCloseSwitch(void) {
	return kDodgeCloseSwitch;
}

float GetPlayerDodgeCloseRadius(void) {
	return player.dodgeCloseRadius;
}

void TriggerDodgeClose(Vector2 eBulletPos) {
	if (!player.isAlive || player.isInvincible) return;

	dodgeCloseBonusTimer.count++;
	if (dodgeCloseBonusTimer.count >= dodgeCloseBonusTimer.time) {
		dodgeCloseBonusTimer.count = 0;

		SpawnGem(eBulletPos, GemType::S);
	}
}

bool GetPlayerIsInvincible(void) {
	return player.isInvincible;
}

//PlayerState GetPlayerNowState(void) {
//	return player.nowState;
//}

#pragma endregion