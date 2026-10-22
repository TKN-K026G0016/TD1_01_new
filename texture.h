#pragma once
#include "vector2.h"
#include "timer.h"
#include <Novice.h>

/// テクスチャの登録から描画までの手順
/// 
/// 0. 使いたいファイルで #include "texture.h" をする
/// 
/// 1. texture.hのTextureTypeにテクスチャの名前を追加(他と混ざらないように)
/// 
/// 2. texture.cppのTextureInit内に例に沿って中身を記述(テクスチャ同士の順番に気を付けて)
/// 
/// 3. 使いたいファイルで変数を宣言
///		static Texture sample1Texture = {};
/// 
/// 4. 使いたいファイルのInit内で初期化
///		sample1Texture = GetTexture(TextureType::Sample1);
/// 
/// 5. 使いたいファイルのDraw内で描画(用途によって関数を使い分ける)
///		DrawTextureObj(sample1Texture, pos, size);

//テクスチャの種類
enum class TextureType {
	//===============
	//Common
	//===============
	White1x1,

	//===============
	//Menu
	//===============
	MenuBackground,
	ResumeIcon,
	RetryIcon,
	ExitIcon,
	MenuCursor,

	//===============
	//Title
	//===============
	TitleLogo,
	TitleInstShot,
	TitleInstStart,

	CreditsButton,
	CreditsButtonNew,
	ResetScoreButton,
	ResetScoreButtonNew,

	CreditsIcon,
	CreditsExitIcon,
	CreditsContent,

	ResetIcon,
	ResetConfirmIcon,

	TitleLine,
	TitleEffect,
	TitleTriangle,
	TitleBox,
	TitleCircle,

	//===============
	//Play
	//===============
	//Player
	Player,

	PlayerLaser0,
	PlayerLaser1,
	PlayerLaser2,

	ElecBullet,
	ElecBulletCharged,
	ChargeOrb,

	ElecShock,
	ElecShockCharged,
	Elec,
	ElecLine,

	ElecShield,

	LockOnSign,

	//Item
	Gem,
	GemSmall,
	GemSmallLight,

	//Enemy
	EnemySniper,
	EnemyGuardian,
	EnemyBasic,
	EnemySmall,


	BossBomb,
	BossBomBurstRange,

	EnemyBullet,
	EnemyLaser,

	//BossEnemy
	BossEnemy,

	//MapChip

	//background
	BackGround,

	//UI
	LifeIcon,

	EnergyGaugeFrame,
	EnergyGaugeMeter,
	EnergyGaugeDelayMeter,

	MiniEnergyGaugeFrame,
	MiniEnergyGaugeMeter,
	MiniEnergyGaugeDelayMeter,

	BossHpGaugeFrame,
	BossHpGaugeMeter,
	BossHpGaugeDelayMeter,

	ProgressGauge,
	ProgressGaugeIcon,

	ProgressGaugeFrame,
	ProgressGaugeMeter,

	ComboIcon,
	ComboNumbers,
	ComboNumbersMeter,

	ScoreNumbers,
	ScoreIcon,

	DisplayEffect,

	ReadyTelop,
	GoTelop,
	
	WarnBossTelop,

	//effect
	EffectSquare,
	EffectSquareMini,
	EffectCircle,
	EffectTriangle,
	EffectExplosion,

	ExplodeEffect,
	barrierEffect,

	//===============
	//Pause
	//===============
	PauseIcon,

	//===============
	//Clear
	//===============
	ClearIcon,
	ClearScoreIcon,
	ClearMaxComboIcon,
	ClearBestScoreIcon,
	NewRecordIcon,

	//===============
	//Gameover
	//===============
	GameoverIcon,

	//===============
	//Guide
	//===============
	ControllerGuide,
	GaugeGuide,

	Count
};

//テクスチャデータ
struct Texture {
	int tHandle = -1;
	Vector2i refSize = { 0, 0 };
	//<animation>
	Timer animChangeTimer = { 0, 0 };
	//アニメーションのコマ数
	int animLimit = 0;
	//現在のコマ数
	int animNum = 0;
	//色関係
	unsigned int color = WHITE;
};

/// <summary>
/// テクスチャの読み込み処理
/// </summary>
void InitTexture(void);

/// <summary>
/// テクスチャデータの取得
/// </summary>
/// <param name="type">テクスチャの種類</param>
/// <returns>テクスチャデータ</returns>
Texture GetTexture(TextureType type);

/// <summary>
/// オブジェクトの描画関数
/// </summary>
/// <param name="texture">テクスチャデータ</param>
/// <param name="centerPos">中心座標</param>
/// <param name="size">サイズ</param>
void DrawTextureObj(Texture texture, Vector2 centerPos, Vector2 size);

/// <summary>
/// オブジェクト(反転)の描画関数
/// </summary>
/// <param name="texture">テクスチャデータ</param>
/// <param name="centerPos">中心座標</param>
/// <param name="size">サイズ</param>
void DrawTextureObjReverse(Texture texture, Vector2 centerPos, Vector2 size);

/// <summary>
/// 回転するオブジェクトの描画関数
/// </summary>
/// <param name="texture">テクスチャデータ</param>
/// <param name="centerPos">中心座標</param>
/// <param name="size">サイズ</param>
/// <param name="rotateTheta">回転量</param>
void DrawTextureRotateObj(Texture texture, Vector2 centerPos, Vector2 size, float rotateTheta);

/// <summary>
/// 背景の描画関数
/// </summary>
/// <param name="texture">テクスチャデータ</param>
/// <param name="centerPos">中心座標</param>
/// <param name="size">サイズ</param>
/// <param name="scrollRate">スクロール倍率</param>
void DrawTextureBG(Texture texture, Vector2 centerPos, Vector2 size, float scrollRate);

/// <summary>
/// UIの描画関数
/// </summary>
/// <param name="texture">テクスチャデータ</param>
/// <param name="centerPos">中心座標</param>
/// <param name="size">サイズ</param>
void DrawTextureUI(Texture texture, Vector2 centerPos, Vector2 size);

void DrawTextureRotateUI(Texture texture, Vector2 centerPos, Vector2 size, float rotateTheta);

/// <summary>
/// ゲージの描画処理(Obj)
/// </summary>
/// <param name="texture">テクスチャデータ</param>
/// <param name="centerPos">中心座標</param>
/// <param name="size">サイズ</param>
/// <param name="rate">ゲージの割合</param>
void DrawGaugeAsObj(Texture texture, Vector2 centerPos, Vector2 size, float rate);

/// <summary>
/// ゲージの描画処理(UI)
/// </summary>
/// <param name="texture">テクスチャデータ</param>
/// <param name="centerPos">中心座標</param>
/// <param name="size">サイズ</param>
/// <param name="rate">ゲージの割合</param>
void DrawGaugeAsUI(Texture texture, Vector2 centerPos, Vector2 size, float rate);

void DrawGaugeYAsUI(Texture texture, Vector2 centerPos, Vector2 size, float rate);

/// <summary>
/// アニメーション変更処理(通常)
/// </summary>
/// <param name="texture">テクスチャデータ</param>
void UpdateAnimation(Texture& texture);