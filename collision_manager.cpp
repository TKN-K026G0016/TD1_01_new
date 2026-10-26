#include "collision_manager.h"
#include "player_laser.h"
#include "elec_bullet.h"
#include "enemy.h"
#include"boss_enemy.h"
#include "player.h"
#include "enemy_bullet.h"
#include "gem.h"
#include "input.h"

#include "tool.h"
#include "vector2.h"

static  void TriggerHitViberation(ElecBulletType type) {
	if (type == ElecBulletType::Normal) {
		TriggerViberationGamePad(ModeViberation::S);
	} else {
		TriggerViberationGamePad(ModeViberation::M);
	}
}

#pragma region 関数: elecBulletVS敵の当たり判定

/// <summary>
/// ElecBulletとenemyの判定
/// </summary>
/// <param name=""></param>
static void CheckCollisionElecBulletVSEnemy(void) {
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

	EnemySniper* enemySniper = GetEnemySniperArray();
	int enemySniperLimit = GetEnemySniperLimit();

	ElecBullet* elecBullet = GetElecBulletArray();
	int elecBulletLimit = GetElecBulletLimit();

#pragma region VSEnemy1
	for (int i = 0; i < enemy1Limit; i++) {
		if (!enemy1[i].isAlive) continue;

		for (int j = 0; j < elecBulletLimit; j++) {
			if (!elecBullet[j].isShoot) continue;

			if (CheckCollisionCircleVSCircle(enemy1[i].pos, GetEnemyHitRadius(EnemyType::EnemyShooter), elecBullet[j].pos, elecBullet[j].hitRadius)) {
				BreakElecBullet(j);
				//ホウデンの生成
				SpawnElecShock(enemy1[i].pos, elecBullet[j].type, ElecShockOriginType::EnemyShooter, i, ElecShockOriginType::None, -1);

				bool hitStopSwitch = (elecBullet[j].type == ElecBulletType::Normal) ? false: true;

				DamageEnemyShooter(enemy1[i], elecBullet[j].pow, hitStopSwitch);
			}
		}
	}


#pragma endregion

#pragma region VSEnemy2
	for (int i = 0; i < enemy2Limit; i++) {
		if (!enemy2[i].isAlive) continue;

		for (int j = 0; j < elecBulletLimit; j++) {
			if (!elecBullet[j].isShoot) continue;

			if (CheckCollisionCircleVSCircle(enemy2[i].pos, GetEnemyHitRadius(EnemyType::EnemyGuardian), elecBullet[j].pos, elecBullet[j].hitRadius)) {
				BreakElecBullet(j);
				SpawnElecShock(enemy2[i].pos, elecBullet[j].type, ElecShockOriginType::EnemyGuardian, i, ElecShockOriginType::None, -1);

				DamageEnemyGuardian(enemy2[i], elecBullet[j].pow);
			}
		}
	}


#pragma endregion

#pragma region VSEnemyBasic
	for (int i = 0; i < enemyBasicLimit; i++) {
		if (!enemyBasic[i].isAlive) continue;

		for (int j = 0; j < elecBulletLimit; j++) {
			if (!elecBullet[j].isShoot) continue;

			if (CheckCollisionCircleVSCircle(enemyBasic[i].pos, GetEnemyHitRadius(EnemyType::EnemyBasic), elecBullet[j].pos, elecBullet[j].hitRadius)) {
				BreakElecBullet(j);
				SpawnElecShock(enemyBasic[i].pos, elecBullet[j].type, ElecShockOriginType::EnemyBasic, i, ElecShockOriginType::None, -1);

				bool hitStopSwitch = (elecBullet[j].type == ElecBulletType::Normal) ? false : true;

				DamageEnemyBasic(enemyBasic[i], elecBullet[j].pow, hitStopSwitch);
			}
		}
	}


#pragma endregion

#pragma region VSEnemySmall
	for (int i = 0; i < enemySmallLimit; i++) {
		if (!enemySmall[i].isAlive) continue;

		for (int j = 0; j < elecBulletLimit; j++) {
			if (!elecBullet[j].isShoot) continue;

			if (CheckCollisionCircleVSCircle(enemySmall[i].pos, enemySmall[i].hitRadius, elecBullet[j].pos, elecBullet[j].hitRadius)) {
				BreakElecBullet(j);
				//ホウデンの生成
				SpawnElecShock(enemySmall[i].pos, elecBullet[j].type, ElecShockOriginType::EnemySmall, i, ElecShockOriginType::None, -1);

				DamageEnemySmall(enemySmall[i], elecBullet[j].pow);
			}
		}
	}


#pragma endregion

#pragma region VSEnemyM
	for (int i = 0; i < enemyMLimit; i++) {
		if (!enemyM[i].isAlive) continue;

		for (int j = 0; j < elecBulletLimit; j++) {
			if (!elecBullet[j].isShoot) continue;

			if (CheckCollisionCircleVSCircle(enemyM[i].pos, enemyM[i].hitRadius, elecBullet[j].pos, elecBullet[j].hitRadius)) {
				BreakElecBullet(j);
				//ホウデンの生成
				SpawnElecShock(enemyM[i].pos, elecBullet[j].type, ElecShockOriginType::EnemyM, i, ElecShockOriginType::None, -1);

				DamageEnemyM(enemyM[i], elecBullet[j].pow);
			}
		}
	}


#pragma endregion

#pragma region VSenemyBomb
	for (int i = 0; i < enemyBombLimit; i++) {
		if (!enemyBomb[i].isAlive) continue;

		for (int j = 0; j < elecBulletLimit; j++) {
			if (!elecBullet[j].isShoot) continue;

			if (CheckCollisionCircleVSCircle(enemyBomb[i].pos, enemyBomb[i].hitRadius, elecBullet[j].pos, elecBullet[j].hitRadius)) {
				BreakElecBullet(j);
				//ホウデンの生成
				SpawnElecShock(enemyBomb[i].pos, elecBullet[j].type, ElecShockOriginType::EnemyBomb, i, ElecShockOriginType::None, -1);

				DamageEnemyBomb(enemyBomb[i], elecBullet[j].pow);
			}
		}
	}


#pragma endregion

#pragma region VSEnemySniper
	for (int i = 0; i < enemySniperLimit; i++) {
		if (!enemySniper[i].isAlive) continue;

		for (int j = 0; j < elecBulletLimit; j++) {
			if (!elecBullet[j].isShoot) continue;

			if (CheckCollisionCircleVSCircle(enemySniper[i].pos, enemySniper[i].hitRadius, elecBullet[j].pos, elecBullet[j].hitRadius)) {
				BreakElecBullet(j);
				//ホウデンの生成
				SpawnElecShock(enemySniper[i].pos, elecBullet[j].type, ElecShockOriginType::EnemySniper, i, ElecShockOriginType::None, -1);

				bool hitStopSwitch = (elecBullet[j].type == ElecBulletType::Normal) ? false : true;

				DamageEnemySniper(enemySniper[i], elecBullet[j].pow, hitStopSwitch);
			}
		}
	}


#pragma endregion


}

/// <summary>
/// ElecBulletとBossEnemyの判定
/// </summary> 
void CheckCollisionElecBulletVSBossEnemy(void) {
	ElecBullet* elecBullet = GetElecBulletArray();
	int elecBulletLimit = GetElecBulletLimit();
	BossEnemy* boss = GetBossEnemy();
	if (!boss->isAlive) return;
	for (int i = 0; i < elecBulletLimit; i++) {
		if (!elecBullet[i].isShoot) continue;
		if (CheckCollisionCircleVSCircle(boss->pos, boss->hitRadius, elecBullet[i].pos, elecBullet[i].hitRadius)) {
			BreakElecBullet(i);
			//ホウデンの生成
			SpawnElecShock(elecBullet[i].pos, elecBullet[i].type, ElecShockOriginType::BossEnemy, i, ElecShockOriginType::None, -1);

			int pow = elecBullet[i].pow;

			DamageBossEnemy(pow);
		}
	}
}

#pragma endregion

#pragma region 関数: elecShockVS敵の当たり判定

/// <summary>
/// ElecShockとenemyの判定
/// </summary>
/// <param name=""></param>
static void CheckCollisionElecShockVSEnemy(void) {
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

	EnemySniper* enemySniper = GetEnemySniperArray();
	int enemySniperLimit = GetEnemySniperLimit();

	ElecShock* elecShock = GetElecShockArray();
	int elecShockLimit = GetElecShockLimit();

#pragma region VSEnemy1
	for (int i = 0; i < enemy1Limit; i++) {
		if (!enemy1[i].isAlive) continue;

		for (int j = 0; j < elecShockLimit; j++) {
			if (!elecShock[j].isShoot) continue;

			if (CheckCollisionCircleVSCircle(enemy1[i].pos, GetEnemyHitRadius(EnemyType::EnemyShooter), elecShock[j].pos, elecShock[j].hitRadius)) {

				//ホウデンの生成
				if (!enemy1[i].isIgnoreShock) {
					SpawnElecShock(enemy1[i].pos, elecShock[j].type, ElecShockOriginType::EnemyShooter, i, elecShock[j].originType, elecShock[j].originIndex);
					enemy1[i].isIgnoreShock = true;
				}
				//ダメージ判定()
				if (elecShock[j].breakTimer.count == elecShock[j].triggerDamageFrame) {

					bool hitStopSwitch = (elecShock[j].type == ElecBulletType::Normal) ? false : true;

					DamageEnemyShooter(enemy1[i], elecShock[j].pow, hitStopSwitch);

					//コンボ
					AddShockCombo(elecShock[j].type);

					TriggerHitViberation(elecShock[j].type);
				}
			}

		}
	}


#pragma endregion

#pragma region VSEnemy2
	for (int i = 0; i < enemy2Limit; i++) {
		if (!enemy2[i].isAlive) continue;

		for (int j = 0; j < elecShockLimit; j++) {
			if (!elecShock[j].isShoot) continue;

			////無視状態時はダメージを受けない(仮)
			//if (enemy2[i].isIgnoreShock) continue;

			if (CheckCollisionCircleVSCircle(enemy2[i].pos, GetEnemyHitRadius(EnemyType::EnemyGuardian), elecShock[j].pos, elecShock[j].hitRadius)) {
				//ホウデンの生成
				if (!enemy2[i].isIgnoreShock) {
					SpawnElecShock(enemy2[i].pos, elecShock[j].type, ElecShockOriginType::EnemyGuardian, i, elecShock[j].originType, elecShock[j].originIndex);
					enemy2[i].isIgnoreShock = true;
				}
				//ダメージ判定()
				if (elecShock[j].breakTimer.count == elecShock[j].triggerDamageFrame) {
					DamageEnemyGuardian(enemy2[i], elecShock[j].pow);

					//コンボ
					AddShockCombo(elecShock[j].type);
					TriggerHitViberation(elecShock[j].type);
				}
			}
		}
	}


#pragma endregion

#pragma region VSEnemyBasic
	for (int i = 0; i < enemyBasicLimit; i++) {
		if (!enemyBasic[i].isAlive) continue;

		for (int j = 0; j < elecShockLimit; j++) {
			if (!elecShock[j].isShoot) continue;

			////無視状態時はダメージを受けない(仮)
			//if (enemyBasic[i].isIgnoreShock) continue;

			if (CheckCollisionCircleVSCircle(enemyBasic[i].pos, GetEnemyHitRadius(EnemyType::EnemyBasic), elecShock[j].pos, elecShock[j].hitRadius)) {
				//ホウデンの生成
				if (!enemyBasic[i].isIgnoreShock) {
					SpawnElecShock(enemyBasic[i].pos, elecShock[j].type, ElecShockOriginType::EnemyBasic, i, elecShock[j].originType, elecShock[j].originIndex);
					enemyBasic[i].isIgnoreShock = true;
				}
				//ダメージ判定()
				if (elecShock[j].breakTimer.count == elecShock[j].triggerDamageFrame) {
					bool hitStopSwitch = (elecShock[j].type == ElecBulletType::Normal) ? false : true;

					DamageEnemyBasic(enemyBasic[i], elecShock[j].pow, hitStopSwitch);
					//コンボ
					AddShockCombo(elecShock[j].type);
					TriggerHitViberation(elecShock[j].type);
				}
			}
		}
	}


#pragma endregion

#pragma region VSEnemySmall
	for (int i = 0; i < enemySmallLimit; i++) {
		if (!enemySmall[i].isAlive) continue;

		for (int j = 0; j < elecShockLimit; j++) {
			if (!elecShock[j].isShoot) continue;

			if (CheckCollisionCircleVSCircle(enemySmall[i].pos, enemySmall[i].hitRadius, elecShock[j].pos, elecShock[j].hitRadius)) {

				//ホウデンの生成
				if (!enemySmall[i].isIgnoreShock) {
					SpawnElecShock(enemySmall[i].pos, elecShock[j].type, ElecShockOriginType::EnemySmall, i, elecShock[j].originType, elecShock[j].originIndex);
					enemySmall[i].isIgnoreShock = true;
				}
				//ダメージ判定()
				if (elecShock[j].breakTimer.count == elecShock[j].triggerDamageFrame) {
					DamageEnemySmall(enemySmall[i], elecShock[j].pow);

					//コンボ
					AddShockCombo(elecShock[j].type);

					TriggerHitViberation(elecShock[j].type);
				}
			}

		}
	}


#pragma endregion

#pragma region VSEnemyM
	for (int i = 0; i < enemyMLimit; i++) {
		if (!enemyM[i].isAlive) continue;

		for (int j = 0; j < elecShockLimit; j++) {
			if (!elecShock[j].isShoot) continue;

			if (CheckCollisionCircleVSCircle(enemyM[i].pos, enemyM[i].hitRadius, elecShock[j].pos, elecShock[j].hitRadius)) {

				//ホウデンの生成
				if (!enemyM[i].isIgnoreShock) {
					SpawnElecShock(enemyM[i].pos, elecShock[j].type, ElecShockOriginType::EnemyM, i, elecShock[j].originType, elecShock[j].originIndex);
					enemyM[i].isIgnoreShock = true;
				}
				//ダメージ判定()
				if (elecShock[j].breakTimer.count == elecShock[j].triggerDamageFrame) {
					DamageEnemyM(enemyM[i], elecShock[j].pow);


					//コンボ
					AddShockCombo(elecShock[j].type);

					TriggerHitViberation(elecShock[j].type);
				}
			}

		}
	}


#pragma endregion

#pragma region VSEnemyBomb
	for (int i = 0; i < enemyBombLimit; i++) {
		if (!enemyBomb[i].isAlive) continue;

		for (int j = 0; j < elecShockLimit; j++) {
			if (!elecShock[j].isShoot) continue;

			if (CheckCollisionCircleVSCircle(enemyBomb[i].pos, enemyBomb[i].hitRadius, elecShock[j].pos, elecShock[j].hitRadius)) {

				//ホウデンの生成
				if (!enemyBomb[i].isIgnoreShock) {
					SpawnElecShock(enemyBomb[i].pos, elecShock[j].type, ElecShockOriginType::EnemyBomb, i, elecShock[j].originType, elecShock[j].originIndex);
					enemyBomb[i].isIgnoreShock = true;
				}
				//ダメージ判定()
				if (elecShock[j].breakTimer.count == elecShock[j].triggerDamageFrame) {
					DamageEnemyBomb(enemyBomb[i], elecShock[j].pow);


					//コンボ
					AddShockCombo(elecShock[j].type);

					TriggerHitViberation(elecShock[j].type);
				}
			}

		}
	}


#pragma endregion

#pragma region VSEnemySniper
	for (int i = 0; i < enemySniperLimit; i++) {
		if (!enemySniper[i].isAlive) continue;

		for (int j = 0; j < elecShockLimit; j++) {
			if (!elecShock[j].isShoot) continue;

			if (CheckCollisionCircleVSCircle(enemySniper[i].pos, enemySniper[i].hitRadius, elecShock[j].pos, elecShock[j].hitRadius)) {

				//ホウデンの生成
				if (!enemySniper[i].isIgnoreShock) {
					SpawnElecShock(enemySniper[i].pos, elecShock[j].type, ElecShockOriginType::EnemySniper, i, elecShock[j].originType, elecShock[j].originIndex);
					enemySniper[i].isIgnoreShock = true;
				}
				//ダメージ判定()
				if (elecShock[j].breakTimer.count == elecShock[j].triggerDamageFrame) {

					bool hitStopSwitch = (elecShock[j].type == ElecBulletType::Normal) ? false : true;

					DamageEnemySniper(enemySniper[i], elecShock[j].pow, hitStopSwitch);

					//コンボ
					AddShockCombo(elecShock[j].type);

					TriggerHitViberation(elecShock[j].type);
				}
			}

		}
	}


#pragma endregion
}

/// <summary>
/// ElecShockとBossEnemyの判定
/// </summary> 
void CheckCollisionElecShockVSBossEnemy(void) {
	ElecShock* elecShock = GetElecShockArray();
	int elecShockLimit = GetElecShockLimit();
	BossEnemy* boss = GetBossEnemy();
	if (!boss->isAlive) return;
	for (int i = 0; i < elecShockLimit; i++) {
		if (!elecShock[i].isShoot) continue;

		if (CheckCollisionCircleVSCircle(boss->pos, boss->hitRadius, elecShock[i].pos, elecShock[i].hitRadius)) {
			//ホウデンの生成
			if (!boss->isIgnoreElecShock) {
				SpawnElecShock(boss->pos, elecShock[i].type, ElecShockOriginType::BossEnemy, -1, elecShock[i].originType, elecShock[i].originIndex);
				boss->isIgnoreElecShock = true;
			}
			//ダメージ判定()
			if (elecShock[i].breakTimer.count == elecShock[i].triggerDamageFrame) {
				int pow = elecShock[i].pow;
				DamageBossEnemy(pow);

				//コンボ
				AddShockCombo(elecShock[i].type);

				TriggerHitViberation(elecShock[i].type);
			}
		}
	}
}

#pragma endregion

#pragma region 関数: elecBulletVSジェムの判定

static void CheckCollisionElecBulletVSGem(void) {
	Gem* gem = GetGemArray();
	int gemLimit = GetGemLimit();


	ElecBullet* elecBullet = GetElecBulletArray();
	int elecBulletLimit = GetElecBulletLimit();

	for (int i = 0; i < elecBulletLimit; i++) {
		if (!elecBullet[i].isShoot) continue;

		for (int j = 0; j < gemLimit; j++) {
			if (!gem[j].isAlive) continue;
			if (gem[j].isIgnoreElecShock) continue;

			//被弾判定
			if (CheckCollisionCircleVSCircle(elecBullet[i].pos, elecBullet[i].hitRadius, gem[j].pos, gem[j].hitRadius)) {
				SpawnElecShock(gem[j].pos, elecBullet[i].type, ElecShockOriginType::Gem, j, ElecShockOriginType::None, -1);
				gem[j].isIgnoreElecShock = true;

				//BreakElecBullet(i);
			}
		}

	}
}

static void CheckCollisionElecShockVSGem(void) {
	Gem* gem = GetGemArray();
	int gemLimit = GetGemLimit();


	ElecShock* elecShock = GetElecShockArray();
	int elecShockLimit = GetElecShockLimit();

	for (int i = 0; i < elecShockLimit; i++) {
		if (!elecShock[i].isShoot) continue;

		for (int j = 0; j < gemLimit; j++) {
			if (!gem[j].isAlive) continue;

			if (gem[j].isIgnoreElecShock) continue;

			//被弾判定
			if (CheckCollisionCircleVSCircle(elecShock[i].pos, elecShock[i].hitRadius, gem[j].pos, gem[j].hitRadius)) {
				SpawnElecShock(gem[j].pos, elecShock[i].type, ElecShockOriginType::Gem, j, elecShock[i].originType, elecShock[i].originIndex);
				gem[j].isIgnoreElecShock = true;

				//規定回数放電したら破壊
				int damage = (elecShock[i].type == ElecBulletType::Normal) ? 1 : 2;

				gem[j].hp -= damage;
				if (gem[j].hp <= 0) {
					BreakGem(j);
				}

				TriggerHitViberation(elecShock[i].type);
			}
		}

	}
}

#pragma endregion

#pragma region 関数: PlayerVS敵弾の判定
/// <summary>
/// PlayerとenemyBulletの判定
/// </summary>
static void CheckCollisionPlayerVSEnemyBullet(void) {
	if (!GetPlayerIsAlive()) return;
	Vector2 playerPos = GetPlayerPos();
	float playerHitRadius = GetPlayerHitRadius();
	//float playerDodgeCloseRadius = GetPlayerDodgeCloseRadius();

	EnemyBullet* eBullet = GetEnemyBulletArray();
	int eBulletLimit = GetEnemyBulletLimit();

	for (int i = 0; i < eBulletLimit; i++) {
		if (!eBullet[i].isShoot) continue;

		//被弾判定
		if (CheckCollisionCircleVSCircle(eBullet[i].pos, eBullet[i].hitRadius, playerPos, playerHitRadius)) {
			PlayerDamage(eBullet[i].pos, eBullet[i].pow);
		}

	}
}

/// <summary>
/// PlayerとenemyBulletの判定
/// </summary>
static void CheckCollisionPlayerVSEnemyLaser(void) {
	if (!GetPlayerIsAlive()) return;
	Vector2 playerPos = GetPlayerPos();
	float playerHitRadius = GetPlayerHitRadius();
	//float playerDodgeCloseRadius = GetPlayerDodgeCloseRadius();

	EnemyLaser* eLaser = GetEnemyLaserArray();
	int eLaserLimit = GetEnemyLaserLimit();

	for (int i = 0; i < eLaserLimit; i++) {
		if (!eLaser[i].isShoot) continue;

		////ギリ避け判定
		//if (GetDodgeCloseSwitch()) {
		//	if (CheckCollisionOBBvsCircle(playerPos, playerDodgeCloseRadius, eLaser[i].hitBoxVertex)) {
		//		TriggerDodgeClose(eLaser[i].pos);
		//	}
		//}

		//被弾判定
		if (CheckCollisionOBBvsCircle(playerPos, playerHitRadius, eLaser[i].hitBoxVertex)) {
			PlayerDamage(eLaser[i].pos, eLaser[i].pow);
		}

	}
}

#pragma endregion

#pragma region 関数: PlayerVS敵の判定

static void CheckCollisionPlayerVSEnemy(void) {
	if (!GetPlayerIsAlive()) return;

	EnemyBasic* enemyBasic = GetEnemyBasicArray();
	int enemyBasicLimit = GetEnemyBasicLimit();

	EnemySmall* enemySmall = GetEnemySmallArray();
	int enemySmallLimit = GetEnemySmallLimit();

	Vector2 playerPos = GetPlayerPos();
	float playerHitRadius = GetPlayerHitRadius();

#pragma region VSEnemy

	for (int i = 0; i < enemyBasicLimit; i++) {
		if (!enemyBasic[i].isAlive) continue;

		if (CheckCollisionCircleVSCircle(enemyBasic[i].pos, GetEnemyHitRadius(EnemyType::EnemyBasic), playerPos, playerHitRadius) ){

			PlayerDamage(enemyBasic[i].pos, GetEnemyPow(EnemyType::EnemyBasic));
		}
	}

#pragma endregion

#pragma region VSEnemySmall

	for (int i = 0; i < enemySmallLimit; i++) {
		if (!enemySmall[i].isAlive) continue;

		if (CheckCollisionCircleVSCircle(enemySmall[i].pos, enemySmall[i].hitRadius, playerPos, playerHitRadius)) {

			PlayerDamage(enemySmall[i].pos, enemySmall[i].pow);
		}
	}

#pragma endregion


}

static void CheckCollisionPlayerVSBossEnemy(void) {
	Vector2 playerPos = GetPlayerPos();
	float playerHitRadius = GetPlayerHitRadius();

	BossEnemy* boss = GetBossEnemy();

	if (!GetPlayerIsAlive()) return;
	if (!boss->isAlive) return;

	if (CheckCollisionCircleVSCircle(boss->pos, boss->hitRadius, playerPos, playerHitRadius)) {

		PlayerKnockBack(boss->pos);
	}
}

#pragma endregion

void InitCollision() {

}

void UpdateCollision() {
	CheckCollisionElecBulletVSEnemy();
	CheckCollisionElecBulletVSBossEnemy();

	CheckCollisionElecShockVSEnemy();
	CheckCollisionElecShockVSBossEnemy();

	CheckCollisionElecBulletVSGem();
	CheckCollisionElecShockVSGem();

	CheckCollisionPlayerVSEnemyBullet();
	CheckCollisionPlayerVSEnemyLaser();
	CheckCollisionPlayerVSEnemy();
	CheckCollisionPlayerVSBossEnemy();

}