#include "play.h"
#include "input.h"
#include "scene_manager.h"

#include "player.h"
#include "elec_bullet.h"
#include "gem.h"
#include "enemy.h"
#include "boss_enemy.h"
#include "enemy_bullet.h"
#include "collision_manager.h"
#include "stage.h"
#include "play_ui.h"
#include "effect.h"

#include <Novice.h>

static void Pause(void) {
	if (CheckInputAction(InputAction::Pause) && GetPlayerIsAlive()) {
		ChangeScene(Scene::Pause);
	}
}


void InitPlay(void) {
	InitPlayer();
	InitElecBullet();
	InitGem();

	InitEnemy();
	InitEnemyBullet();

	InitBossEnemy();

	InitCollision();

	InitStage();

	InitPlayUI();

	InitEffect();
}

void UpdatePlay(void) {
	UpdatePlayer();
	UpdateElecBullet();
	UpdateGem();

	UpdateEnemy();
	UpdateEnemyBullet();

	UpdateBossEnemy();

	UpdateCollision();

	UpdateStage();

	UpdatePlayUI();

	UpdateEffect();

	Pause();
}

void DrawPlay(void) {
	DrawStage();

	DrawGem();

	DrawEnemy();
	DrawEnemyBullet();

	DrawBossEnemy();

	DrawElecBullet();

	DrawPlayer();
	DrawEffect();

	DrawPlayUI();
	DrawEffectAsUI();

}