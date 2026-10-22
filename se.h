#pragma once

/// Seの登録から再生までの手順
/// 
/// 0. 使いたいファイルで #include "se.h" をする
/// 
/// 1. se.hのSeTypeにSeの名前を追加(他と混ざらないように)
/// 
/// 2. se.cppのSeInit内に例に沿って中身を記述(Se同士の順番に気を付けて)
/// 
/// 3. PlaySe()で再生(用途によって関数を使い分ける)
///		PlaySe(SeType:: sample);

enum class SeType {
	Sample,
	//menu
	Confirm,
	Cancel,
	MoveCursor,
	Pause,

	Flash,
	
	ResetHighScore,

	StageClearJingle,
	BreakScoreJingle,

	GameoverJingle,

	//player
	ShootElecBullet,
	ShootElecBulletCharge,
	SpawnElecShock,
	SpawnElecShockCharge,
	Charge,

	DamagePlayer,
	DeadPlayer,

	GetGem,

	//enemy
	ShootEnemyLaser,
	ExploseEnemyBomb,

	DamageEnemy,
	DeadEnemy,

	//ui
	GaugeAlarm,
	WarningBossAlarm,

	Ready,
	Go,

	Combo1,
	Combo2,
	Combo3,
	Combo4,
	Combo5,
	Combo6,
	Combo7,
	Combo8,
	Combo9,
	Combo10,

	ComboJingle100,
	ComboJingle500,
	ComboJingle1000,

	CountUpScore,
	Count
};

void InitSe(void);

void PlaySe(SeType type);

void PlaySeContinuous(SeType type);

void StopPlayingSe(SeType type);

bool CheckIsPlayingSe(SeType type);