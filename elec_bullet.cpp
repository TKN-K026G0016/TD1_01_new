#include "elec_bullet.h"
#include "effect.h"
#include "play_ui.h"
#include "enemy.h"
#include "boss_enemy.h"
#include "gem.h"
#include "score_data.h"
#include "scene_manager.h"
#include "se.h"
#include "stage.h"
#include "bgm.h"
#include "input.h"

#include "vector2.h"
#include "timer.h"
#include "tool.h"
#include "texture.h"

#include <math.h>
#include <algorithm>

static Scene nowScene = Scene::Play;

#pragma region データ: ElecBullet
//レベルごとの性能
struct ElecBulletSpec {
	Vector2 size;
	float hitRadius;

	int pow;

	Texture texture = {};
};

ElecBulletSpec elecBulletSpec[ToInt(ElecBulletType::Count)] = {
	//Normal弾
	{ {32, 32}, 20, 1, {}},
	//Charge弾
	{ {100, 100}, 32, 4, {} },
};

constexpr static int kElecBulletLimit = 5;
ElecBullet elecBullet[kElecBulletLimit] = {};

#pragma region 残像関係

//残像のalphaの差
static constexpr int kAfterImageAlphaOffset = 40;

#pragma endregion

#pragma endregion

#pragma region データ: ElecShock
//レベルごとの性能
struct ElecShockSpec {
	float hitRadius;

	int pow;
};

ElecShockSpec elecShockSpec[ToInt(ElecBulletType::Count)] = {
	//Normal弾
	{ 120, 1 },
	//Charge弾
	{ 200, 3 },
};

constexpr static int kElecShockLimit = 30;
ElecShock elecShock[kElecShockLimit] = {};

#pragma endregion

#pragma region データ: ShockCombo関係

//現在のコンボ数
static int nowCombo = 0;

//今回のゲームの最大コンボ数
static int maxCombo = 0;
//コンボ持続時間
Timer comboTimer = { 100, 0 };

//通常弾のコンボ増加数
static constexpr int kAddCombo = 1;
//チャージ弾のコンボ増加数
static constexpr int kAddComboCharged = 5;
//通常弾でのコンボ回復量
constexpr int kRecoveryComboTime = 1;
//チャージ弾でのコンボ回復量
constexpr int kRecoveryComboTimeCharged = 30;

//コンボ中の累計撃破スコア
int totalDefeatScoreInCombo = 0;
//コンボ中に得たスコアの合計
int scoreInCombo = 0;
//スコア数による獲得スコアの倍率
constexpr float kScoreMugByCombo = 0.005f;

//コンボサウンドの間隔
static constexpr int comboSoundSpan = 100;

static constexpr int jingle1Combo = 100;
static constexpr int jingle2Combo = 500;
static constexpr int jingle3Combo = 1000;

//スコアジングル再生中のbgmVolume
static constexpr  float bgmVolumePlayingJingle = 0.5f;

#pragma endregion


#pragma region 関数: デンゲキ弾関係

static void InitBullet(void) {
	for (int i = 0; i < kElecBulletLimit; i++) {
		elecBullet[i] = {};
		for (int j = 0; j < kElecBulletAfterimageNum; j++) {
			elecBullet[i].afterImageAlpha[j] = 255 - (kAfterImageAlphaOffset * j);
			elecBullet[i].afterImageAlpha[j] = std::clamp(elecBullet[i].afterImageAlpha[j], 0, 255);
		}

	}

	elecBulletSpec[ToInt(ElecBulletType::Normal)].texture = GetTexture(TextureType::ElecBullet);
	elecBulletSpec[ToInt(ElecBulletType::Charge)].texture = GetTexture(TextureType::ElecBulletCharged);
}

/// <summary>
/// デンゲキ弾の移動処理
/// </summary>
static void MoveBullet(void) {
	for (int i = 0; i < kElecBulletLimit; i++) {
		if (!elecBullet[i].isShoot) continue;

#pragma region 残像の位置更新

		for (int j = kElecBulletAfterimageNum - 1; j > 0; j--) {
			elecBullet[i].prePos[j] = elecBullet[i].prePos[j - 1];
		}

		elecBullet[i].prePos[0] = elecBullet[i].pos;

#pragma endregion

		elecBullet[i].pos.x += cosf(elecBullet[i].moveTheta) * elecBullet[i].moveSpeed;
		elecBullet[i].pos.y += sinf(elecBullet[i].moveTheta) * elecBullet[i].moveSpeed;
	}
}

/// <summary>
/// デンゲキ弾の自壊タイマー進める処理
/// </summary>
static void BreakBulletItSelf(void) {
	for (int i = 0; i < kElecBulletLimit; i++) {
		if (!elecBullet[i].isShoot) continue;

		elecBullet[i].breakTimer.count++;
		if (elecBullet[i].breakTimer.count >= elecBullet[i].breakTimer.time) {
			elecBullet[i].breakTimer.count = 0;
			BreakElecBullet(i);
		}
	}
}

/// <summary>
/// デンゲキ弾の描画処理
/// </summary>
static void DrawBullet(void) {
	for (int i = 0; i < kElecBulletLimit; i++) {
		if (!elecBullet[i].isShoot) continue;

		if (nowScene == Scene::Play) {
#pragma region 残像の描画
			for (int j = kElecBulletAfterimageNum - 1; j >= 0; j--) {
				Texture texture = elecBullet[i].texture;

				texture.color =
					(texture.color & 0xFFFFFF00)
					| elecBullet[i].afterImageAlpha[j];

				DrawTextureRotateObj(
					texture,
					elecBullet[i].prePos[j],
					elecBullet[i].size,
					elecBullet[i].moveTheta
				);
			}

#pragma endregion
			//本体の描画
			DrawTextureRotateObj(elecBullet[i].texture, elecBullet[i].pos, elecBullet[i].size, elecBullet[i].moveTheta);
		} 
		else {
#pragma region 残像の描画
			for (int j = kElecBulletAfterimageNum - 1; j >= 0; j--) {
				Texture texture = elecBullet[i].texture;

				texture.color =
					(texture.color & 0xFFFFFF00)
					| elecBullet[i].afterImageAlpha[j];

				DrawTextureRotateUI(
					texture,
					elecBullet[i].prePos[j],
					elecBullet[i].size,
					elecBullet[i].moveTheta
				);
			}

#pragma endregion

			//本体の描画
			DrawTextureRotateUI(elecBullet[i].texture, elecBullet[i].pos, elecBullet[i].size, elecBullet[i].moveTheta);
		}
	}
}

#pragma endregion

#pragma region 関数: デンゲキショック関係

static void InitElecShock(void) {
	for (int i = 0; i < kElecShockLimit; i++) {
		elecShock[i] = {};
	}
}

static void BreakElecShockItSelf(void) {
	for (int i = 0; i < kElecShockLimit; i++) {
		if (!elecShock[i].isShoot) continue;

		elecShock[i].breakTimer.count++;
		if (elecShock[i].breakTimer.count >= elecShock[i].breakTimer.time) {
			elecShock[i].breakTimer.count = 0;
			BreakElecShock(i);
		}
	}
}

#pragma endregion

#pragma region 関数: ショックコンボ

static void InitShockCombo(void) {
	nowCombo = 0;
	maxCombo = 0;
	comboTimer.count = 0;
}

static void UpdateShockCombo(void) {
	//猶予時間がある際
	if (nowCombo > 0) {
		comboTimer.count++;

		//猶予時間が切れたら
		if (comboTimer.count >= comboTimer.time) {
			comboTimer.count = 0;

			//コンボの終了をUIに知らせる
			TriggerEndCombo();

			//コンボ中の得点を加算する
			AddScore(scoreInCombo);
			scoreInCombo = 0;

			//最大コンボ数更新処理
			if (nowCombo > maxCombo) {
				maxCombo = nowCombo;
			}

			//コンボ終了ジングル
			if (nowCombo > jingle3Combo) {
				PlaySe(SeType::ComboJingle1000);

				//フラッシュ演出
				TriggerBackgroundFlash(ModeBackground::Combo1000);
			}
			else if (nowCombo > jingle2Combo) {
				PlaySe(SeType::ComboJingle500);
				TriggerBackgroundFlash(ModeBackground::Combo500);
			}
			else if (nowCombo > jingle1Combo) {
				PlaySe(SeType::ComboJingle100);
				TriggerBackgroundFlash(ModeBackground::Combo100);
			}
			
			totalDefeatScoreInCombo = 0;
			nowCombo = 0;
		}
	}

	//BGMの音量調整
	if (GetNowScene() != Scene::Play || GetIsWarningBoss() || GetBossEnemyIsDead()) return;
	if (CheckIsPlayingSe(SeType::ComboJingle1000) || CheckIsPlayingSe(SeType::ComboJingle500) || CheckIsPlayingSe(SeType::ComboJingle100)) {
		ChangeBgmVolume(bgmVolumePlayingJingle);
	} else {
		ResetBgmVolume();
	}

}


#pragma endregion

void InitElecBullet(void) {
	nowScene = GetNowScene();

	InitBullet();
	InitElecShock();

	InitShockCombo();
}

void UpdateElecBullet(void) {
	MoveBullet();

	BreakBulletItSelf();
	BreakElecShockItSelf();

	UpdateShockCombo();
}

void DrawElecBullet(void) {
	DrawBullet();
}

#pragma region 関数: 外部参照関係

void ShootElecBullet(Vector2 pos, float moveTheta, ElecBulletType type) {
	for (int i = 0; i < kElecBulletLimit; i++) {
		if (elecBullet[i].isShoot) continue;

		elecBullet[i].isShoot = true;

		elecBullet[i].pos = pos;
		elecBullet[i].moveTheta = moveTheta;
		for (int j = 0; j < kElecBulletAfterimageNum; j++) {
			elecBullet[i].prePos[j] = pos;
		}

		elecBullet[i].type = type;
		elecBullet[i].size = elecBulletSpec[ToInt(type)].size;
		elecBullet[i].hitRadius = elecBulletSpec[ToInt(type)].hitRadius;
		elecBullet[i].pow = elecBulletSpec[ToInt(type)].pow;

		elecBullet[i].texture = elecBulletSpec[ToInt(type)].texture;

		break;
	}
}

void BreakElecBullet(int index) {
	elecBullet[index].isShoot = false;
	elecBullet[index].breakTimer.count = 0;
}

void BreakAllElecBullet(void) {
	for (int i = 0; i < kElecBulletLimit; i++) {
		BreakElecBullet(i);
		if (elecBullet[i].type == ElecBulletType::Normal) {
			CreateElecShockEffect(elecBullet[i].pos);
		} else {
			CreateElecShockChargedEffect(elecBullet[i].pos);
		}
	}
}

ElecBullet* GetElecBulletArray(void) {
	return elecBullet;
}

int GetElecBulletLimit(void) {
	return kElecBulletLimit;
}

void SpawnElecShock(Vector2 pos, ElecBulletType type, ElecShockOriginType originType, int originIndex, ElecShockOriginType originType2, int originIndex2) {
	for (int i = 0; i < kElecShockLimit; i++) {
		if (elecShock[i].isShoot) continue;

		elecShock[i].isShoot = true;
		elecShock[i].pos = pos;

		elecShock[i].type = type;
		elecShock[i].hitRadius = elecShockSpec[ToInt(type)].hitRadius;
		elecShock[i].pow = elecShockSpec[ToInt(type)].pow;

		//発生元を保存
		elecShock[i].originType = originType;
		elecShock[i].originIndex = originIndex;

		//エフェクトの生成
		if (type == ElecBulletType::Normal) {
			CreateElecShockEffect(pos);
			//Se
			if (!CheckIsPlayingSe(SeType::SpawnElecShock)) {
				PlaySe(SeType::SpawnElecShock);
			}

		} else if(type == ElecBulletType::Charge){
			CreateElecShockChargedEffect(pos);
			//Se
			if (!CheckIsPlayingSe(SeType::SpawnElecShockCharge)) {
				PlaySe(SeType::SpawnElecShockCharge);
			}
		}

		//ラインエフェクトの生成
		if (originType2 != ElecShockOriginType::None && originIndex2 >= 0) {

			Vector2 originPos = {};

			//自分を発生させたElecShockのoriginObjの座標を取得
			
			switch (originType2) {
			case ElecShockOriginType::Gem: {

				Gem* Gem = GetGemArray();

				originPos = Gem[originIndex2].pos;

				break;
			}
			case ElecShockOriginType::EnemyShooter: {
				
				EnemyShooter* enemyShooter = GetEnemyShooterArray();

				originPos = enemyShooter[originIndex2].pos;

				break;
			}
			case ElecShockOriginType::EnemyGuardian: {

				EnemyGuardian* enemyGuardian = GetEnemyGuardianArray();

				originPos = enemyGuardian[originIndex2].pos;

				break;
			}
			case ElecShockOriginType::EnemyBasic: {

				EnemyBasic* enemyBasic = GetEnemyBasicArray();

				originPos = enemyBasic[originIndex2].pos;

				break;
			}
			case ElecShockOriginType::EnemySmall: {

				EnemySmall* enemySmall = GetEnemySmallArray();

				originPos = enemySmall[originIndex2].pos;

				break;
			}
			case ElecShockOriginType::EnemyM: {

				EnemyM* enemyM = GetEnemyMArray();

				originPos = enemyM[originIndex2].pos;

				break;
			}
			case ElecShockOriginType::EnemyBomb: {

				EnemyBomb* enemyBomb = GetEnemyBombArray();

				originPos = enemyBomb[originIndex2].pos;

				break;
			}
			case ElecShockOriginType::EnemySniper: {

				EnemySniper* enemySniper = GetEnemySniperArray();

				originPos = enemySniper[originIndex2].pos;

				break;
			}
			case ElecShockOriginType::BossEnemy: {

				BossEnemy* BossEnemy = GetBossEnemy();

				originPos = BossEnemy->pos;

				break;
			}
			}

			//2点の座標をつかってLine描写
			if (type != ElecBulletType::Charge) {
				CreateElecLine(originPos, elecShock[i].pos, false);
			} else {
				CreateElecLine(originPos, elecShock[i].pos, true);
			}

		}

		break;
	}
}

void BreakElecShock(int index) {
	elecShock[index].isShoot = false;
	elecShock[index].breakTimer.count = 0;
}

ElecShock* GetElecShockArray(void) {
	return elecShock;
}

int GetElecShockLimit(void) {
	return kElecShockLimit;
}

void AddShockCombo(ElecBulletType type) {
	//コンボ増加
	int addCombo = (type == ElecBulletType::Normal) ? kAddCombo : kAddComboCharged;

	int beforeCombo = nowCombo;
	nowCombo += addCombo;
	int afterCombo = nowCombo;

	int recoveryComboTime = (type == ElecBulletType::Normal) ? kRecoveryComboTime : kRecoveryComboTimeCharged;
	comboTimer.count -= recoveryComboTime;
	if (comboTimer.count < 0) {
		comboTimer.count = 0;
	}

#pragma region サウンド追加

	// 10 の倍数を跨いだ瞬間に SE を鳴らす
	int beforeTier = beforeCombo / comboSoundSpan;
	int afterTier = afterCombo / comboSoundSpan;

	if (beforeTier != afterTier) {
		// SE の種類を段階的に変える
		// combo_1.wav ～ combo_10.wav を使う例
		int seIndex = afterTier;

		// 上限を決める（例：10段階まで）
		if (seIndex > 10) seIndex = 10;

		PlaySe((SeType)(ToInt(SeType::Combo1) + (seIndex - 1)));
	}

#pragma endregion

}

int GetShockCombo(void) {
	return nowCombo;
}

int GetMaxCombo(void) {
	return maxCombo;
}

Timer GetComboTimer(void) {
	return comboTimer;
}

void AddScoreByDefeatEnemy(int defeatPoint) {
	int point = (defeatPoint + totalDefeatScoreInCombo) * ToInt((1.0f + ToFloat(nowCombo) * kScoreMugByCombo));
	scoreInCombo += point;

	totalDefeatScoreInCombo += defeatPoint;
}



#pragma endregion