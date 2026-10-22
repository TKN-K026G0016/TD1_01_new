#include "boss_enemy.h"
#include "player.h"
#include "player_laser.h"
#include "stage.h"
#include "enemy_bullet.h"
#include "enemy.h"
#include "effect.h"
#include "se.h"

#include "vector2.h"
#include "tool.h"
#include "input.h"
#include "texture.h"

#include <Novice.h>
#define _USE_MATH_DEFINES
#include <math.h>

//カラー関係
static constexpr unsigned int kNormalColor = 0xE01068FF;
static constexpr unsigned int kFlashColor = WHITE;

BossEnemy bossEnemy;

#pragma region データ: Bomb攻撃

struct BossEnemyBomb {
	Vector2 pos = { 0, 0 };
	Vector2 size = { 64, 64 };
	//<生存関係>
	bool isAlive = false;
	//当たり判定の大きさ
	float hitRadius = 32.0f;

	//<爆発関係>
	//爆発までの時間
	Timer burst = { 0,0 };
	//爆発範囲
	float burstRadius = 128.0f;
	Vector2 bustSize = { 256,256 };
	//<移動関係>
	//速度
	Vector2 velocity = { 0.0f, 0.0f };
	//スピード
	float nowSpeed = 0.0f;
	//加速度
	float accleretionSpeed = 0.05f;
	//スピード上限
	float moveSpeedLimit = 6.0f;
	//減速度
	float decelerationSpeed = 0.1f;
	Texture texture = {};
	Texture burstTexture = {};
};
BossEnemyBomb bossBomb[6]{};

struct StatesTimer {
	Timer normal = { 300,0 };
	Timer attack1 = { 120,0 };
	Timer attack1ShootBom = { 30,0 };
	Timer attack2 = { 360,0 };
};
StatesTimer statesTimer;

#pragma endregion

#pragma region データ: レーザー攻撃

enum class ModeShootLaser {
	//テレポート状態
	Teleport,
	//射撃準備段階
	ReadyShoot,
	//射撃状態
	Shoot,
	ShootAndRotate,
};
//レーザー攻撃の形態
ModeShootLaser nowModeShootLaser = ModeShootLaser::Teleport;

struct DataShootLaser {

	ModeShootLaser nowMode = ModeShootLaser::Teleport;

	//テレポート準備にかかる時間
	Timer readyTeleportTimer = { 60, 0 };
	//テレポート後の座標
	Vector2 teleportTargetPos = {};

	//テレポート終了から射撃開始までの時間
	Timer readyShootTimer = { 100, 0 };
	//射撃準備中の回転速度
	float rotatateSpeedReadyShoot = 0.05f;
	//射撃準備中の狙いのずれ
	float targetThetaOffset = ToFloat(M_PI) / 10.0f;

	//射撃時間
	Timer shootTimer = { 300, 0 };
	//回転が始まるまでの時間
	int startRotateTime = 90;
	//レーザーの射撃スパン
	Timer shootLaserTimer = { 2, 0 };


	//現在の回転速度
	float nowRotateSpeed = 0.0f;
	//回転速度の加速度
	float rotateAccelerationSpeed = 0.00035f;
	//回転速度の上限
	float rotateSpeedMax = 0.05f;
	//現在の回転角
	float nowRotateTheta = 0.0f;
	//回転角の上限
	float rotateThetaMax = 1.0f * float(M_PI);
	//レーザーの回転方向
	float rotateDiff = 0.0f;
};
DataShootLaser dataShootLaser;

#pragma endregion

#pragma region データ: 撃破演出
Timer createExploseTimer = { 30, 0 };
int createExploseTimerMax = 30;
int createExploseTimerMin =4;

Vector2i createExploseRange = { 300, 300 };

#pragma endregion

Vector2 playerPos;

#pragma region 関数: 行動パターン関係
void StatesCount(void) {
	if (!bossEnemy.isAlive || bossEnemy.isDead) return;

	if (bossEnemy.nowsSates == BossStates::Normal) {
		statesTimer.normal.count++;
		if (statesTimer.normal.count == statesTimer.normal.time) {
			statesTimer.normal.count = 0;
			bossEnemy.nowsSates = ToInt(GetRand(1, 3));
		}
	} else if (bossEnemy.nowsSates == BossStates::Attack1) {
		statesTimer.attack1.count++;
		statesTimer.attack1ShootBom.count++;
		if (statesTimer.attack1.count == statesTimer.attack1.time) {
			statesTimer.attack1.count = 0;
			bossEnemy.nowsSates = BossStates::Normal;
		}
	} else if (bossEnemy.nowsSates == BossStates::Attack2) {
		statesTimer.attack2.count++;
		if (statesTimer.attack2.count == statesTimer.attack2.time) {
			statesTimer.attack2.count = 0;
			bossEnemy.nowsSates = BossStates::Normal;
		}
	}
}

void MoveNormal(void) {
	if (!bossEnemy.isAlive || bossEnemy.isDead) return;

	if (bossEnemy.nowsSates == BossStates::Normal) {
		playerPos = GetPlayerPos();
		float DiffX = playerPos.x - bossEnemy.pos.x;
		float DiffY = playerPos.y - bossEnemy.pos.y;
		float distance = sqrtf(DiffX * DiffX + DiffY * DiffY);
		bossEnemy.rotateTheta = atan2f(DiffY, DiffX);
		if (distance < bossEnemy.viewRange) {
			if (DiffX != 0) {
				bossEnemy.velocity.x = -1 * (DiffX / distance);
			}
			if (DiffY != 0) {
				bossEnemy.velocity.y = -1 * (DiffY / distance);
			}
			bossEnemy.nowSpeed += bossEnemy.accleretionSpeed;
			if (bossEnemy.nowSpeed > bossEnemy.moveSpeedLimit) {
				bossEnemy.nowSpeed = bossEnemy.moveSpeedLimit;
			}
		} else {
			if (DiffX != 0) {
				bossEnemy.velocity.x = (DiffX / distance);
			}
			if (DiffY != 0) {
				bossEnemy.velocity.y = (DiffY / distance);
			}
			bossEnemy.nowSpeed += bossEnemy.accleretionSpeed;
			if (bossEnemy.nowSpeed > bossEnemy.moveSpeedLimit) {
				bossEnemy.nowSpeed = bossEnemy.moveSpeedLimit;
			}
		}
		bossEnemy.pos.x += bossEnemy.velocity.x * bossEnemy.nowSpeed;
		bossEnemy.pos.y += bossEnemy.velocity.y * bossEnemy.nowSpeed;
	}
}

void MoveAttack1(void) {
	if (!bossEnemy.isAlive || bossEnemy.isDead) return;

	playerPos = GetPlayerPos();
	if (bossEnemy.nowsSates == BossStates::Attack1) {
		if (statesTimer.attack1ShootBom.count >= statesTimer.attack1ShootBom.time) {
			statesTimer.attack1ShootBom.count = 0;
			SpawnEnemy(bossEnemy.pos, EnemyType::EnemyBomb);
		}
	}
}

void MoveAttack2(void) {
	if (!bossEnemy.isAlive || bossEnemy.isDead) return;

	if (bossEnemy.nowsSates == BossStates::Attack2) {
		playerPos = GetPlayerPos();
		float DiffX = playerPos.x - bossEnemy.pos.x;
		float DiffY = playerPos.y - bossEnemy.pos.y;
		float distance = sqrtf(DiffX * DiffX + DiffY * DiffY);
		bossEnemy.rotateTheta = atan2f(DiffY, DiffX);
		if (distance < bossEnemy.viewRange) {
			if (DiffX != 0) {
				bossEnemy.velocity.x = -1 * (DiffX / distance);
			}
			if (DiffY != 0) {
				bossEnemy.velocity.y = -1 * (DiffY / distance);
			}
			bossEnemy.nowSpeed += bossEnemy.accleretionSpeed;
			if (bossEnemy.nowSpeed > bossEnemy.moveSpeedLimit) {
				bossEnemy.nowSpeed = bossEnemy.moveSpeedLimit;
			}
		} else {
			if (DiffX != 0) {
				bossEnemy.velocity.x = (DiffX / distance);
			}
			if (DiffY != 0) {
				bossEnemy.velocity.y = (DiffY / distance);
			}
			bossEnemy.nowSpeed += bossEnemy.accleretionSpeed;
			if (bossEnemy.nowSpeed > bossEnemy.moveSpeedLimit) {
				bossEnemy.nowSpeed = bossEnemy.moveSpeedLimit;
			}
		}
		bossEnemy.pos.x += bossEnemy.velocity.x * bossEnemy.nowSpeed;
		bossEnemy.pos.y += bossEnemy.velocity.y * bossEnemy.nowSpeed;

	}
}

void MoveAttack3(void) {
	if (!bossEnemy.isAlive || bossEnemy.isDead) return;

	if (bossEnemy.nowsSates == BossStates::Attack3) {

		DataShootLaser& data = dataShootLaser;

		switch (data.nowMode) {
		case ModeShootLaser::Teleport: {
			data.readyTeleportTimer.count++;
			if (data.readyTeleportTimer.count >= data.readyTeleportTimer.time) {
				data.readyTeleportTimer.count = 0;

				//ワープする
				bossEnemy.pos = data.teleportTargetPos;
				data.nowMode = ModeShootLaser::ReadyShoot;
			}

			break;
		}
		case ModeShootLaser::ReadyShoot: {
			data.readyShootTimer.count++;

			if (data.readyShootTimer.count >= data.readyShootTimer.time) {
				data.readyShootTimer.count = 0;

				data.nowMode = ModeShootLaser::Shoot;
				data.nowRotateSpeed = 0.0f;
			}

#pragma region 回転処理
			//少しplayerからズレた場所を向く
			playerPos = GetPlayerPos();

			//プレイヤーを追跡するように回転
			float disX = playerPos.x - bossEnemy.pos.x;
			float disY = playerPos.y - bossEnemy.pos.y;
			float targetTheta = atan2f(disY, disX);

			//現在の方向との差を求める
			float diff = targetTheta + data.targetThetaOffset - bossEnemy.rotateTheta;

			//差を-π~πに正規化
			while (diff > ToFloat(M_PI)) {
				diff -= ToFloat(M_PI) * 2;
			}
			while (diff < -ToFloat(M_PI)) {
				diff += ToFloat(M_PI) * 2;
			}
			if (diff > 0) {
				data.rotateDiff = 1;
			} else if (diff <= 0) {
				data.rotateDiff = -1;
			}

			bossEnemy.rotateTheta += diff * data.rotatateSpeedReadyShoot;

#pragma endregion

			break;
		}
		case ModeShootLaser::Shoot:
		case ModeShootLaser::ShootAndRotate: {


			//射撃処理
			data.shootLaserTimer.count++;
			if (data.shootLaserTimer.count >= data.shootLaserTimer.time && data.shootTimer.count < data.shootTimer.time - 60) {
				data.shootLaserTimer.count = 0;
				ShootEnemyLaser(bossEnemy.pos, bossEnemy.rotateTheta, 0.0f);
				PlaySe(SeType::ShootEnemyLaser);
			}


			//回転処理
			if (data.nowMode == ModeShootLaser::ShootAndRotate) {
				playerPos = GetPlayerPos();

				/*if(data.shootLaserTimer.count==data.startRotateTime){
					プレイヤーを追跡するように回転
					float disX = playerPos.x - bossEnemy.pos.x;
					float disY = playerPos.y - bossEnemy.pos.y;
					float targetTheta = atan2f(disY, disX);

					現在の方向との差を求める
					data.rotateDiff = targetTheta - bossEnemy.rotateTheta;

					差を-π~πに正規化
					while (data.rotateDiff > ToFloat(M_PI)) {
						data.rotateDiff -= ToFloat(M_PI) * 2;
					}
					while (data.rotateDiff < -ToFloat(M_PI)) {
						data.rotateDiff += ToFloat(M_PI) * 2;
					}
				}*/
				if (data.nowRotateTheta < data.rotateThetaMax && data.nowRotateTheta>-1 * data.rotateThetaMax) {
					bossEnemy.rotateTheta += data.rotateDiff * data.nowRotateSpeed;
					data.nowRotateTheta += data.rotateDiff * data.nowRotateSpeed;
				}

				//回転の加速処理
				data.nowRotateSpeed += data.rotateAccelerationSpeed;
				if (data.nowRotateSpeed > data.rotateSpeedMax) {
					data.nowRotateSpeed = data.rotateSpeedMax;
				}

			}


			//射撃時間
			data.shootTimer.count++;
			//回転の開始
			if (data.nowMode != ModeShootLaser::ShootAndRotate && data.shootTimer.count >= data.startRotateTime) {
				data.nowMode = ModeShootLaser::ShootAndRotate;
			}
			//攻撃の終了
			if (data.shootTimer.count >= data.shootTimer.time) {
				data.nowRotateTheta = 0;
				data.shootTimer.count = 0;
				data.nowMode = ModeShootLaser::Teleport;
				bossEnemy.nowsSates = BossStates::Normal;
			}

			break;
		}
		}
	}
}

#pragma endregion

#pragma region 関数: 死亡判定関係

/// <summary>
/// デンゲキショックを無視する状態カウント
/// </summary>
/// <param name=""></param>
static void CountIgnoreElecShock(void) {
	if (!bossEnemy.isAlive) return;
	if (!bossEnemy.isIgnoreElecShock) return;

	bossEnemy.ignoreElecShockTimer.count++;
	if (bossEnemy.ignoreElecShockTimer.count >= bossEnemy.ignoreElecShockTimer.time) {
		bossEnemy.ignoreElecShockTimer.count = 0;
		bossEnemy.isIgnoreElecShock = false;
	}

}

static void UpdateFlashTimer(void) {
	if (!bossEnemy.isAlive) return;
	if (!bossEnemy.isFlash) return;

	bossEnemy.flashTimer.count++;
	if (bossEnemy.flashTimer.count >= bossEnemy.flashTimer.time) {
		bossEnemy.flashTimer.count = 0;
		bossEnemy.isFlash = false;
	}

}

void DeathBossEnemy(void) {
	if (!bossEnemy.isAlive || bossEnemy.isDead) return;

	if (bossEnemy.remainLife <= 0) {
		bossEnemy.isDead = true;
	}

}

#pragma endregion

#pragma region 関数: 死亡後の演出関係


static void MoveAfterDead(void) {
	if (!bossEnemy.isAlive || !bossEnemy.isDead) return;

	bossEnemy.deadTimer.count++;
	if (bossEnemy.deadTimer.count >= bossEnemy.deadTimer.time) {
		bossEnemy.deadTimer.count = 0;
		bossEnemy.isAlive = false;

		//最後に5発爆発
		for (int i = 0; i < 8; i++) {
			Vector2 createPos;
			createPos.x = bossEnemy.pos.x + ToFloat(GetRand(-createExploseRange.x / 2, createExploseRange.x / 2));
			createPos.y = bossEnemy.pos.y + ToFloat(GetRand(-createExploseRange.y / 2, createExploseRange.y / 2));
			CreateExplodeEffect(createPos, WHITE);
			CreateFlashPanel();
		}
	}

	//爆発演出
	createExploseTimer.count++;
	if (createExploseTimer.count >= createExploseTimer.time) {
		createExploseTimer.count = 0;
		if (createExploseTimer.time > createExploseTimerMin) {
			createExploseTimer.time-= 5;
			if (createExploseTimer.time < createExploseTimerMin) {
				createExploseTimer.time = createExploseTimerMin;
			}
		}

		Vector2 createPos;
		createPos.x = bossEnemy.pos.x + ToFloat(GetRand(-createExploseRange.x / 2, createExploseRange.x / 2));
		createPos.y = bossEnemy.pos.y + ToFloat(GetRand(-createExploseRange.y / 2, createExploseRange.y / 2));
		CreateExplodeEffect(createPos, WHITE);
	}

}

#pragma endregion

void InitBossEnemy(void) {
	bossEnemy = {};
	createExploseTimer.time = createExploseTimerMax;

	//ステージの中心座標を取得(shootLaser時のため)
	Vector2 movablePos[2];
	GetMovablePos(movablePos);
	dataShootLaser.teleportTargetPos = { (movablePos[1].x - movablePos[0].x) / 2, (movablePos[1].y - movablePos[0].y) / 2 };

	bossEnemy.texture = GetTexture(TextureType::BossEnemy);
	for (int i = 0;i < 6;i++) {
		bossBomb[i] = {};
		bossBomb[i].texture = GetTexture(TextureType::BossBomb);
		bossBomb[i].burstTexture = GetTexture(TextureType::BossBomBurstRange);
	}

	//SpawnBoss({ 500, 500 });

}

void UpdateBossEnemy(void) {
	DeathBossEnemy();

	StatesCount();

	MoveNormal();
	MoveAttack1();
	MoveAttack2();
	MoveAttack3();

	CountIgnoreElecShock();
	UpdateFlashTimer();



	MoveAfterDead();
};
void DrawBossEnemy(void) {
	if (bossEnemy.isAlive == true) {
		Texture texture = bossEnemy.texture;
		if (bossEnemy.isFlash) {
			texture.color = kFlashColor;
		} else {
			texture.color = kNormalColor;
		}

		DrawTextureRotateObj(texture, bossEnemy.pos, bossEnemy.size, bossEnemy.rotateTheta);
	}
}

#pragma region 関数: 外部参照関係

void SpawnBoss(Vector2 pos) {
	bossEnemy.isAlive = true;
	bossEnemy.pos = pos;
}

bool GetBossEnemyIsAlive(void) {
	return bossEnemy.isAlive;
}

bool GetBossEnemyIsDead(void) {
	return bossEnemy.isDead;
}

Vector2 GetBossEnemyPos(void) {
	return bossEnemy.pos;
}
float GetBossEnemyRotateTheta(void) {
	return bossEnemy.rotateTheta;
}
int GetBossEnemyRemainLife(void) {
	return bossEnemy.remainLife;
}
int GetBossEnemyRemainLifeMax(void) {
	return bossEnemy.remainLifeMax;
}
int GetBossEnemyNowStates(void) {
	return bossEnemy.nowsSates;
}
Timer GetAttack2Timer(void) {
	return statesTimer.attack2;
}

BossEnemy* GetBossEnemy(void) {
	return &bossEnemy;
}

void DamageBossEnemy(int pow) {
	bossEnemy.remainLife -= pow;
	if (bossEnemy.remainLife <= 0) {
		bossEnemy.remainLife = 0;
	}

	bossEnemy.isFlash = true;
	bossEnemy.flashTimer.count = 0;

	//ダメージエフェクト
	CreateEnemyDamageEffect(bossEnemy.pos);
}

#pragma endregion
