#include "player.h"
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


#pragma region データ: player本体
struct Player {
	Vector2 pos = { 1280, 720 };
	Vector2 size = { 32, 32 };

	//<生存関係>
	bool isAlive = true;
	bool isEquipShield = true;

	//当たり判定の大きさ
	float hitRadius = 10.0f;
	int hp = 1;

	//<無敵関係>
	//無敵フラグ
	bool isInvincible = false;
	//無敵の持続時間
	Timer invincibleTimer = { 100, 0 };
	//ノックバック関係
	bool isKnockBack = false;

	//<移動関係>
	//移動速度(等速)
	float moveSpeed = 6.0f;
	Vector2 inputVec = { 0, 0 };


	//<回転関係>
	float rotateTheta = 0.0f;

	//<ロックオン関係>
	//ロック状態フラグ
	bool isLock = false;
	//回転速度
	float lockOnRotateSpeed = 0.2f;


	//<射撃関係>
	//エネルギー残量
	float remainEnergy = 0.0f;

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
	EnemyShooter = 0,
	EnemyGuardian = 1,
	EnemyBasic = 2,
	EnemySmall = 3,
	EnemyM = 4,
	EnemyBomb = 5,
	EnemySniper = 6,

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
float consumptionShootNormal = 0.3f;
float consumptionShootCharge = 10.0f;
static bool isCharge = false;

#pragma endregion

#pragma region データ: エネルギー関係

//エネルギー上限
constexpr float kEnergyMax = 100.0f;
//エネルギー下限
constexpr float kEnergyMin = 0.0f;
constexpr float kFirstEnergy = kEnergyMax;

//エネルギー消費量(常時)
static constexpr float kConsumptionEnergy = 0.05f;

//シールドが消えるエネルギー量のライン
static constexpr float kBreakShieldLine = 0.0f;

#pragma endregion

#pragma region データ: ElecShield

//シールドがなくなってから再展開するまでの時間
Timer reEquipShieldTimer = { 200, 0 };

struct ElecShield {
	Vector2 size = {};

	ModeElecShield nowMode = ModeElecShield::Standby;

	//<イージング関係>
	float easingT = 0.0f;

	//通常時のイージング速度
	float easingSpeedNormal = 0.01f;
	EasingMode easingModeNormal = EasingMode::EaseInOut;
	//拡大中フラグ
	bool isScaleUp = true;
	//回復状態時のイージング速度
	float easingSpeedRecovery = 0.03f;
	EasingMode easingModeRecovery = EasingMode::EaseOut;
	//復活処理時のイージング速度
	float easingSpeedReEquip = 0.03f;
	float easingTLineReEquipScaleDown = 0.7f;
	EasingMode easingModeReEquip = EasingMode::EaseOutBack;
	EasingMode easingModeReEquipAfter = EasingMode::EaseIn;

	
	//<サイズ関係>
	//通常時のサイズ最小値
	Vector2 sizeValueMin = { 50.0f, 50.0f };
	//通常時のサイズ最大値
	Vector2 sizeValueMax = { 60.0f, 60.0f };
	//回復状態時のサイズ最大値
	Vector2 sizeValueMaxRecovery = { 180.0f, 180.0f };
	//復活処理時のサイズ最小値
	Vector2 sizeValueMinReEquip = { 0.0f, 0.0f };
	//復活処理時のサイズ最大値
	Vector2 sizeValueMaxReEquip = { 180.0f, 180.0f };

	//<不透明度関係>
	int alpha = 0;
	//通常時の透明度最低値
	int alphaMin = 100;
	int alphaMax = 150;
	int alphaMinRecovery = 0;
	int alphaMinReEquip = 0;
	int alphaMaxReEquip = 200;

	//<色関係>
	unsigned int color = WHITE;

	//色の最小値
	unsigned int colorMinNormal = 0x04C1E8FF;
	unsigned int colorMinDanger = 0x96083fFF;
	//色の最大値
	unsigned int colorMax = 0x485793FF;
	//危険色に変わるゲージの割合
	float dangerColorLine = 0.5f;

	Texture texture = {};
};
static ElecShield elecShield;

#pragma endregion

#pragma region データ: ギリ避け・ノックバック関係

//ノックバックの初速度
static constexpr float kFirstKnockBackPow = 20.0f;
//ノックバックの減速度
static constexpr float kDecelerationKnockBackPow = 0.1f;

static float knockBackPow = kFirstKnockBackPow;
static Vector2 knockBackVec = {};
static Timer knockBackTimer = { 20, 0 };

#pragma endregion

#pragma region データ: 演出関係

//チャージの破片表示を開始する時間
constexpr int kStartCreateChargeChipTime = 7;
Timer createChargeOrbTimer = { 10, 0 };
Timer createChargeChipTimer = { 10, 0 };

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

constexpr int exploseCount = 6;
constexpr int createExploseRange = 100;

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

	player.pos.x += player.moveSpeed * moveVec.x;
	player.pos.y += player.moveSpeed * moveVec.y;

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

	EnemyShooter* enemyShooter = GetEnemyShooterArray();
	int enemyShooterLimit = GetEnemyShooterLimit();

	EnemyGuardian* enemyGuardian = GetEnemyGuardianArray();
	int enemyGuardianLimit = GetEnemyGuardianLimit();

	EnemyBasic* enemyBasic = GetEnemyBasicArray();
	int enemyBasicLimit = GetEnemyBasicLimit();

	EnemySmall* enemySmall = GetEnemySmallArray();
	int enemySmallLimit = GetEnemySmallLimit();

	EnemyM* enemyM = GetEnemyMArray();
	int enemyMLimit = GetEnemyMLimit();

	EnemyBomb* enemyBomb = GetEnemyBombArray();
	int enemyBombLimit = GetEnemyBombLimit();

	EnemySniper* enemySniper = GetEnemySniperArray();
	int enemySniperLimit = GetEnemySniperLimit();

	Gem* gem = GetGemArray();
	int gemLimit = GetGemLimit();

	// enemy1探索
	for (int i = 0; i < enemyShooterLimit; i++) {
		if (!enemyShooter[i].isAlive) continue;

		float dx = enemyShooter[i].pos.x - player.pos.x;
		float dy = enemyShooter[i].pos.y - player.pos.y;
		float dis = sqrtf(dx * dx + dy * dy);

		if (dis < closestDis) {
			closestDis = dis;
			targetIndex = i;
			nowTargetType = LockTargetType::EnemyShooter;
		}
	}

	for (int i = 0; i < enemyGuardianLimit; i++) {
		if (!enemyGuardian[i].isAlive) continue;

		float dx = enemyGuardian[i].pos.x - player.pos.x;
		float dy = enemyGuardian[i].pos.y - player.pos.y;
		float dis = sqrtf(dx * dx + dy * dy);

		if (dis < closestDis) {
			closestDis = dis;
			targetIndex = i;
			nowTargetType = LockTargetType::EnemyGuardian;
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

	for (int i = 0; i < enemySniperLimit; i++) {
		if (!enemySniper[i].isAlive) continue;

		float dx = enemySniper[i].pos.x - player.pos.x;
		float dy = enemySniper[i].pos.y - player.pos.y;
		float dis = sqrtf(dx * dx + dy * dy);

		if (dis < closestDis) {
			closestDis = dis;
			targetIndex = i;
			nowTargetType = LockTargetType::EnemySniper;
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
	if (closestDis > kCancelLockOnEnemyRange) {
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
	} else if (nowTargetType == LockTargetType::EnemyShooter) {
		if (!enemyShooter[targetIndex].isAlive) {
			nowTargetType = LockTargetType::None;
			return;
		}
		targetPos = enemyShooter[targetIndex].pos;
	} else if (nowTargetType == LockTargetType::EnemyGuardian) {
		if (!enemyGuardian[targetIndex].isAlive) {
			nowTargetType = LockTargetType::None;
			return;
		}
		targetPos = enemyGuardian[targetIndex].pos;
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
	} else if (nowTargetType == LockTargetType::EnemySniper) {
		if (!enemySniper[targetIndex].isAlive) {
			nowTargetType = LockTargetType::None;
			return;
		}
		targetPos = enemySniper[targetIndex].pos;
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
	else if (nowTargetType == LockTargetType::EnemyShooter) {
		EnemyShooter* enemy1 = GetEnemyShooterArray();
		lockOnSign.pos = enemy1[targetIndex].pos;
	}
	//enemy2
	else if (nowTargetType == LockTargetType::EnemyGuardian) {
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
	//enemyBomb
	else if (nowTargetType == LockTargetType::EnemyBomb) {
		EnemyBomb* enemyBomb = GetEnemyBombArray();
		lockOnSign.pos = enemyBomb[targetIndex].pos;
	}
	//enemySniper
	else if (nowTargetType == LockTargetType::EnemySniper) {
		EnemySniper* enemySniper = GetEnemySniperArray();
		lockOnSign.pos = enemySniper[targetIndex].pos;
	}
	//gem
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
			createChargeOrbTimer.count = 0;
		}

		chargeElecBulletTimer.count = 0;

	}

}

/// <summary>
/// エネルギーの常時消費処理
/// </summary>
static void ConsumptionEnergy(void) {
	if (startEffectManager.nowMode != ModeStartEffect::Standby) return;
	if (!player.isAlive) return;
	if (!player.isEquipShield) return;

	player.remainEnergy -= kConsumptionEnergy;

	if (player.remainEnergy <= 0.0f) {
		player.remainEnergy = 0.0f;
	}

}

#pragma endregion

#pragma region 関数: ElecShield関係

static void InitElecShield(void) {
	elecShield = {};
	elecShield.texture = GetTexture(TextureType::ElecShield);
}

/// <summary>
/// ElecShieldの更新処理
/// </summary>
/// <param name=""></param>
static void UpdateElecShield(void) {
	if (!player.isAlive) return;
	if (GetBossEnemyIsDead()) return;

	//シールドの破壊処理
	if (player.remainEnergy <= kBreakShieldLine) {
		if (player.isEquipShield) {
			reEquipShieldTimer.count = 0;
			ChangeModeElecShield(ModeElecShield::Break);
			CreateBarrierBreakEffect(player.pos, elecShield.colorMinDanger);
		}
		player.isEquipShield = false;
	}
	//シールドの回復処理
	else if (!player.isEquipShield) {
		reEquipShieldTimer.count++;

		//デンリョクが溜まっている状態でしばらく経ったら
		if (reEquipShieldTimer.count >= reEquipShieldTimer.time) {
			reEquipShieldTimer.count = 0;
			player.isEquipShield = true;
			ChangeModeElecShield(ModeElecShield::ReEquip);
		}
	}
}

/// <summary>
/// ElecShieldの演出処理
/// </summary>
/// <param name=""></param>
static void UpdateElecShieldEffect(void) {

	switch (elecShield.nowMode) {
	case ModeElecShield::Standby: {
		if (elecShield.isScaleUp) {
			elecShield.easingT += elecShield.easingSpeedNormal;
			if (elecShield.easingT >= 1.0f) {
				elecShield.easingT = 1.0f;
				elecShield.isScaleUp = false;
			}
		}
		else {
			elecShield.easingT -= elecShield.easingSpeedNormal;
			if (elecShield.easingT <= 0.0f) {
				elecShield.easingT = 0.0f;
				elecShield.isScaleUp = true;
			}
		}
		
		//サイズの変更
		elecShield.size = MoveByEasing(elecShield.sizeValueMin, elecShield.sizeValueMax, elecShield.easingT, elecShield.easingModeNormal);
		//不透明度の変更
		elecShield.alpha = UpdateValueByEasing(elecShield.alphaMin, elecShield.alphaMax, elecShield.easingT, elecShield.easingModeNormal);

		break;
	}
	case ModeElecShield::Recovery: {
		elecShield.easingT += elecShield.easingSpeedRecovery;
		if (elecShield.easingT >= 1.0f) {
			elecShield.easingT = 1.0f;
			elecShield.nowMode = ModeElecShield::Standby;
			elecShield.isScaleUp = false;
		}

		//サイズの変更
		elecShield.size = MoveByEasing(elecShield.sizeValueMaxRecovery, elecShield.sizeValueMax, elecShield.easingT, elecShield.easingModeRecovery);
		//不透明度の変更
		elecShield.alpha = UpdateValueByEasing(elecShield.alphaMinRecovery, elecShield.alphaMax, elecShield.easingT, elecShield.easingModeRecovery);

		break;
	}
	case ModeElecShield::Break: {

		break;
	}
	case ModeElecShield::ReEquip: {
  		elecShield.easingT += elecShield.easingSpeedReEquip;



		if (elecShield.easingT >= 1.0f) {
			elecShield.easingT = 1.0f;
			elecShield.nowMode = ModeElecShield::Standby;
			elecShield.isScaleUp = true;
		}

		// 第1段階
		if (elecShield.easingT < elecShield.easingTLineReEquipScaleDown) {

			float localT = elecShield.easingT / elecShield.easingTLineReEquipScaleDown;

			elecShield.size = MoveByEasing(elecShield.sizeValueMinReEquip, elecShield.sizeValueMaxReEquip, localT, elecShield.easingModeReEquip);
			elecShield.alpha = UpdateValueByEasing(elecShield.alphaMinReEquip, elecShield.alphaMaxReEquip, localT, elecShield.easingModeReEquip);
		}
		// 第2段階
		else {

			float localT = (elecShield.easingT - elecShield.easingTLineReEquipScaleDown) / (1.0f - elecShield.easingTLineReEquipScaleDown);

			elecShield.size = MoveByEasing(elecShield.sizeValueMaxReEquip, elecShield.sizeValueMax, localT, elecShield.easingModeReEquipAfter);
			elecShield.alpha = UpdateValueByEasing(elecShield.alphaMaxReEquip, elecShield.alphaMax, localT, elecShield.easingModeReEquipAfter);
		}

		break;
	}
	}

#pragma region 色の変更

	float gaugeRate = player.remainEnergy / kEnergyMax;
	gaugeRate = std::clamp(gaugeRate, 0.0f, 1.0f);

	if (gaugeRate >= elecShield.dangerColorLine) {
		float localRate = (gaugeRate - elecShield.dangerColorLine) / (1.0f - elecShield.dangerColorLine);

		elecShield.color = UpdateColorByEasing(elecShield.colorMinNormal, elecShield.colorMax, localRate);
	}
	//危険
	else {
		float localRate = gaugeRate / elecShield.dangerColorLine;
		float easedT = CreateEasedT(localRate, EasingMode::EaseIn);
		elecShield.color = UpdateColorByEasing(elecShield.colorMinDanger, elecShield.colorMinNormal, easedT);
	}
	

	

#pragma endregion

}

static void DrawElecShield(void) {
	Texture texture = elecShield.texture;
	texture.color = (elecShield.color & 0xFFFFFF00) | elecShield.alpha;

	DrawTextureObj(texture, player.pos, elecShield.size);
}


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

	if ((chargeElecBulletTimer.count > kStartCreateChargeChipTime && player.remainEnergy > consumptionShootCharge) || isCharge) {

		createChargeChipTimer.count++;
		if (createChargeChipTimer.count >= createChargeChipTimer.time) {
			createChargeChipTimer.count = 0;
			CreateChargeChipEffect();
		}

		//チャージSE
		PlaySeContinuous(SeType::Charge);
		TriggerViberationGamePad(ModeViberation::S);

	} else {
		StopPlayingSe(SeType::Charge);
	}

	if (isCharge) {
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
		for (int i = 0; i < exploseCount; i++) {
			Vector2 createPos;
			createPos.x = player.pos.x + ToFloat(GetRand(-createExploseRange / 2, createExploseRange / 2));
			createPos.y = player.pos.y + ToFloat(GetRand(-createExploseRange / 2, createExploseRange / 2));

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

	UpdateElecShield();
	UpdateElecShieldEffect();
	//RecoveryPlayerState();

	ConsumptionEnergy();

	UpdatePlayerEffect();
	CreateChargeOrbEffect();

	KnockBack();
	UpdateInvinciblePlayer();
	CheckDeadPlayer();

	UpdateStartEffect();
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

void PlayerDamage(Vector2 hitPos, float damage) {
	if (!player.isAlive || player.isInvincible) return;

	//シールドがない状態で被弾したら
	if (!player.isEquipShield) {
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

bool GetPlayerIsInvincible(void) {
	return player.isInvincible;
}

void ChangeModeElecShield(ModeElecShield mode) {

	if (elecShield.nowMode == ModeElecShield::Break && mode == ModeElecShield::Recovery) return;

	elecShield.nowMode = mode;

	switch (mode) {
	case ModeElecShield::Recovery:
		elecShield.easingT = 0.0f;

		break;
	case ModeElecShield::Break:
		elecShield.alpha = 0;
		PlaySe(SeType::BreakElecShield);

		break;
	case ModeElecShield::ReEquip:
		elecShield.easingT = 0.0f;
		PlaySe(SeType::ReEquipElecShield);

		break;
	}

}

#pragma endregion