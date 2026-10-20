#include "se.h"
#include "tool.h"

#include <Novice.h>

struct SE {
	int aHandle = -1;
	float volume = 1.0f;
	//再生中かの状態
	int pHandle = -1;
};
static SE ses[ToInt(SeType::Count)] = {};

#pragma region データ: 音量
float seVolume = 1.0f;

#pragma endregion


void InitSe(void) {
	SE temp[ToInt(SeType::Count)] = {
		//Sample
		{Novice::LoadAudio("./NoviceResources/fanfare.wav"), 1.0f},

		//menu
		{Novice::LoadAudio("./resources/sound/se/menu/confirm.wav"), 1.0f},
		{Novice::LoadAudio("./resources/sound/se/menu/cancel.wav"), 1.0f},
		{Novice::LoadAudio("./resources/sound/se/menu/move_cursor.wav"), 1.0f},
		{Novice::LoadAudio("./resources/sound/se/menu/pause.wav"), 1.0f},

		{Novice::LoadAudio("./resources/sound/se/menu/reset_score.wav"), 1.0f},

		{Novice::LoadAudio("./resources/sound/se/menu/stage_clear_jingle.wav"), 1.0f},
		{Novice::LoadAudio("./resources/sound/se/menu/break_score_jingle.wav"), 1.0f},

		{Novice::LoadAudio("./resources/sound/se/menu/gameover_jingle.wav"), 1.0f},

		//player
		{Novice::LoadAudio("./resources/sound/se/player/shoot_bullet.wav"), 1.0f},
		{Novice::LoadAudio("./resources/sound/se/player/shoot_charge_bullet.wav"), 1.0f},
		{Novice::LoadAudio("./resources/sound/se/player/elec_shock.wav"), 0.5f},
		{Novice::LoadAudio("./resources/sound/se/player/elec_shock_charge.wav"), 0.5f},
		{Novice::LoadAudio("./resources/sound/se/player/charge.wav"), 0.5f},

		{Novice::LoadAudio("./resources/sound/se/player/damage_player.wav"), 1.0f},
		{Novice::LoadAudio("./resources/sound/se/player/dead_player.wav"), 1.0f},

		{Novice::LoadAudio("./resources/sound/se/player/get_gem.wav"), 1.0f},

		//enemy
		{Novice::LoadAudio("./resources/sound/se/enemy/shoot_enemy_laser.wav"), 0.4f},
		{Novice::LoadAudio("./resources/sound/se/enemy/explose_enemy_bomb.wav"), 1.0f},

		{Novice::LoadAudio("./resources/sound/se/enemy/damage_enemy.wav"), 0.1f},
		{Novice::LoadAudio("./resources/sound/se/enemy/dead_enemy.wav"), 1.0f},

		//ui
		{Novice::LoadAudio("./resources/sound/se/ui/gauge_alarm.wav"), 1.0f},
		{Novice::LoadAudio("./resources/sound/se/ui/boss_alarm.wav"), 1.0f},

		{Novice::LoadAudio("./resources/sound/se/ui/ready.wav"), 1.0f},
		{Novice::LoadAudio("./resources/sound/se/ui/go.wav"), 1.0f},

		{Novice::LoadAudio("./resources/sound/se/ui/combo_1.wav"), 1.0f},
		{Novice::LoadAudio("./resources/sound/se/ui/combo_2.wav"), 1.0f},
		{Novice::LoadAudio("./resources/sound/se/ui/combo_3.wav"), 1.0f},
		{Novice::LoadAudio("./resources/sound/se/ui/combo_4.wav"), 1.0f},
		{Novice::LoadAudio("./resources/sound/se/ui/combo_5.wav"), 1.0f},
		{Novice::LoadAudio("./resources/sound/se/ui/combo_6.wav"), 1.0f},
		{Novice::LoadAudio("./resources/sound/se/ui/combo_7.wav"), 1.0f},
		{Novice::LoadAudio("./resources/sound/se/ui/combo_8.wav"), 1.0f},
		{Novice::LoadAudio("./resources/sound/se/ui/combo_9.wav"), 1.0f},
		{Novice::LoadAudio("./resources/sound/se/ui/combo_10.wav"), 1.0f},

		{Novice::LoadAudio("./resources/sound/se/ui/combo_jingle_10.wav"), 1.0f},
		{Novice::LoadAudio("./resources/sound/se/ui/combo_jingle_100.wav"), 1.0f},
		{Novice::LoadAudio("./resources/sound/se/ui/combo_jingle_1000.wav"), 1.0f},

		{Novice::LoadAudio("./resources/sound/se/ui/countup_score.wav"), 1.0f},

	};

	memcpy(ses, temp, sizeof(ses));
}

void PlaySe(SeType type) {
	ses[ToInt(type)].pHandle = Novice::PlayAudio(ses[ToInt(type)].aHandle, false, ses[ToInt(type)].volume * seVolume);
}

/// <summary>
/// Seのループ再生処理
/// </summary>
/// <param name="type">Seの種類</param>
void PlaySeContinuous(SeType type) {
	if (Novice::IsPlayingAudio(ses[ToInt(type)].pHandle) != 1) {
		ses[ToInt(type)].pHandle = Novice::PlayAudio(ses[ToInt(type)].aHandle, true, ses[ToInt(type)].volume * seVolume);
	}
}

/// <summary>
/// 再生中のSEを停止
/// </summary>
/// <param name="type">Seの種類</param>
void StopPlayingSe(SeType type) {
	if (Novice::IsPlayingAudio(ses[ToInt(type)].pHandle) == 1) {
		Novice::StopAudio(ses[ToInt(type)].pHandle);
	}
}

/// <summary>
/// 指定SEが再生中か
/// </summary>
/// <param name="type"></param>
/// <returns></returns>
bool CheckIsPlayingSe(SeType type) {
	//再生中であれば
	if (Novice::IsPlayingAudio(ses[ToInt(type)].pHandle) == 1) {
		return true;
	} else {
		return false;
	}
}