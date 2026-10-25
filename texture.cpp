#include "texture.h"
#include "timer.h"
#include "tool.h"

#include <Novice.h>
#include <algorithm>

Texture textures[ToInt(TextureType::Count)] = {};

void InitTexture(void) {
	Texture temp[ToInt(TextureType::Count)] = {

		//サンプル
		//テクスチャデータ, 参照サイズ, animChangeTimer(time, count), animLimit(4枚アニメなら3), animNum, color
		//{Novice::LoadTexture("./resources/player/player_body.png"), { 128, 128 }, {0, 0}, 0, 0, WHITE },

		//===============
		//Common
		//===============
		{ Novice::LoadTexture("./NoviceResources/white1x1.png"), { 1, 1 }, {0, 0}, 0, 0, WHITE },

		//===============
		//Menu
		//===============
		{ Novice::LoadTexture("./resources/texture/ui/menu/menu_background.png"), { 1280, 720 }, {0, 0}, 0, 0, WHITE },
		{ Novice::LoadTexture("./resources/texture/ui/menu/resume_button2.png"), { 256, 64 }, {0, 0}, 0, 0, WHITE },
		{ Novice::LoadTexture("./resources/texture/ui/menu/retry_button2.png"), { 256, 64 }, {0, 0}, 0, 0, WHITE },
		{ Novice::LoadTexture("./resources/texture/ui/menu/exit_button2.png"), { 256, 64 }, {0, 0}, 0, 0, WHITE },
		{ Novice::LoadTexture("./resources/texture/ui/menu/menu_cursor.png"), { 64, 64 }, {0, 0}, 0, 0, WHITE },

		//===============
		//Title
		//===============
		{ Novice::LoadTexture("./resources/texture/ui/title/title_logo.png"), { 640, 256 }, {0, 0}, 0, 0, WHITE },
		{ Novice::LoadTexture("./resources/texture/ui/title/inst_shot.png"), { 640, 128 }, {0, 0}, 0, 0, WHITE },
		{ Novice::LoadTexture("./resources/texture/ui/title/inst_start.png"), { 640, 128 }, {0, 0}, 0, 0, WHITE },

		{ Novice::LoadTexture("./resources/texture/ui/title/credits_button.png"), { 80, 80 }, {0, 0}, 0, 0, WHITE },
		{ Novice::LoadTexture("./resources/texture/ui/title/credits_button_new.png"), { 256, 128 }, {0, 0}, 0, 0, WHITE },
		{ Novice::LoadTexture("./resources/texture/ui/title/reset_score_button.png"), { 80, 80 }, {0, 0}, 0, 0, WHITE },
		{ Novice::LoadTexture("./resources/texture/ui/title/reset_score_button_new.png"), { 256, 128 }, {0, 0}, 0, 0, WHITE },

		{ Novice::LoadTexture("./resources/texture/ui/title/recommend_icon.png"), { 1024, 512 }, {0, 0}, 0, 0, WHITE },

		{ Novice::LoadTexture("./resources/texture/ui/title/credits_icon.png"), { 600, 150 }, {0, 0}, 0, 0, WHITE },
		{ Novice::LoadTexture("./resources/texture/ui/title/credits_exit_button.png"), { 256, 64 }, {0, 0}, 0, 0, WHITE },
		{ Novice::LoadTexture("./resources/texture/ui/title/credits_content.png"), { 600, 400 }, {0, 0}, 0, 0, WHITE },

		{ Novice::LoadTexture("./resources/texture/ui/title/reset_icon.png"), { 600, 256 }, {0, 0}, 0, 0, WHITE },
		{ Novice::LoadTexture("./resources/texture/ui/title/reset_button.png"), { 256, 64 }, {0, 0}, 0, 0, WHITE },

		{ Novice::LoadTexture("./resources/texture/ui/title/title_line.png"), { 1280, 2 }, {0, 0}, 0, 0, WHITE },
		{ Novice::LoadTexture("./resources/texture/ui/title/title_effect.png"), { 1280, 720 }, {0, 0}, 0, 0, WHITE },
		{ Novice::LoadTexture("./resources/texture/ui/title/title_triangle.png"), { 128, 128 }, {0, 0}, 0, 0, WHITE },
		{ Novice::LoadTexture("./resources/texture/ui/title/title_box.png"), { 128, 128 }, {0, 0}, 0, 0, WHITE },
		{ Novice::LoadTexture("./resources/texture/ui/title/title_circle.png"), { 128, 128 }, {0, 0}, 0, 0, WHITE },

		//===============
		//Play
		//===============
		//Player
		{ Novice::LoadTexture("./resources/texture/player/player.png"), { 128, 128 }, {0, 0}, 0, 0, WHITE },

		{ Novice::LoadTexture("./resources/texture/player/laser0.png"), { 64, 64 }, {0, 0}, 0, 0, WHITE },
		{ Novice::LoadTexture("./resources/texture/player/laser1.png"), { 64, 64 }, {0, 0}, 0, 0, WHITE },
		{ Novice::LoadTexture("./resources/texture/player/laser2.png"), { 64, 64 }, {0, 0}, 0, 0, WHITE },

		{ Novice::LoadTexture("./resources/texture/player/elec_bullet.png"), { 64, 64 }, {0, 0}, 0, 0, WHITE },
		{ Novice::LoadTexture("./resources/texture/player/elec_bullet_charged.png"), { 128, 128 }, {0, 0}, 0, 0, WHITE },
		{ Novice::LoadTexture("./resources/texture/player/charge_effect.png"), { 64, 64 }, {0, 0}, 0, 0, WHITE },

		{ Novice::LoadTexture("./resources/texture/player/elec_shock.png"), { 256, 256 }, {0, 0}, 0, 0, WHITE },
		{ Novice::LoadTexture("./resources/texture/player/erec_shock_charged.png"), { 256, 256 }, {0, 0}, 0, 0, WHITE },
		{ Novice::LoadTexture("./resources/texture/player/elec.png"), { 64, 64 }, {2, 0}, 2, 0, WHITE },
		{ Novice::LoadTexture("./resources/texture/player/elec_line2.png"), { 32, 32 }, {3, 0}, 4, 0, WHITE },

		{ Novice::LoadTexture("./resources/texture/effect/barrier.png"), { 256, 256 }, {0, 0}, 0, 0, WHITE },

		{ Novice::LoadTexture("./resources/texture/player/lock_on.png"), { 64, 64 }, {7, 0}, 2, 0, WHITE },

		//Item
		{ Novice::LoadTexture("./resources/texture/item/gem.png"), { 32, 32 }, {7, 0}, 3, 0, WHITE },
		{ Novice::LoadTexture("./resources/texture/item/gem.png"), { 32, 32 }, {7, 0}, 3, 0, WHITE },
		{ Novice::LoadTexture("./resources/texture/item/gem_light.png"), { 32, 32 }, {7, 0}, 3, 0, WHITE },

		//Enemy

		{ Novice::LoadTexture("./resources/texture/enemy/enemy_sniper.png"), { 128, 128 }, {0, 0}, 0, 0, WHITE },
		{ Novice::LoadTexture("./resources/texture/enemy/enemy_guardian.png"), { 128, 128 }, {0, 0}, 0, 0, WHITE },
		{ Novice::LoadTexture("./resources/texture/enemy/enemy_basic.png"), { 128, 128 }, {0, 0}, 0, 0, WHITE },
		{ Novice::LoadTexture("./resources/texture/enemy/enemy_small.png"), { 32, 32 }, {0, 0}, 0, 0, WHITE },

		{Novice::LoadTexture("./resources/texture/enemy/enemy_bomb.png"), { 128, 128 }, {0, 0}, 0, 0, WHITE},
		{Novice::LoadTexture("./resources/texture/enemy/bossBomBurstRange.png"), { 512, 512 }, {0, 0}, 0, 0, WHITE},

		{ Novice::LoadTexture("./resources/texture/enemy/enemy_bullet2.png"), { 32, 32 }, {6, 0}, 3, 0, WHITE },
		{ Novice::LoadTexture("./resources/texture/enemy/enemy_laser.png"), { 32, 32 }, {0, 0}, 0, 0, WHITE },

		//BossEnemy
		{ Novice::LoadTexture("./resources/texture/enemy/boss_enemy.png"), { 512, 512 }, {0, 0}, 0, 0, WHITE },

		//MapChip

		//background
		{ Novice::LoadTexture("./resources/texture/stage/background_white.png"), { 2960, 1840 }, {0, 0}, 0, 0, WHITE },
		{ Novice::LoadTexture("./resources/texture/stage/stage_line.png"), { 2960, 1840 }, {0, 0}, 0, 0, WHITE },

		//UI
		{ Novice::LoadTexture("./resources/texture/ui/play/life_icon.png"), { 64, 64 }, {0, 0}, 0, 0, WHITE },

		{ Novice::LoadTexture("./resources/texture/ui/play/energy_gauge_frame.png"), { 512, 128 }, {7, 0}, 5, 0, WHITE },
		{ Novice::LoadTexture("./resources/texture/ui/play/energy_gauge_meter.png"), { 512, 128 }, {0, 0}, 0, 0, WHITE },
		{ Novice::LoadTexture("./resources/texture/ui/play/energy_gauge_delay_meter.png"), { 512, 128 }, {0, 0}, 0, 0, WHITE },

		{ Novice::LoadTexture("./resources/texture/ui/play/mini_energy_gauge_frame.png"), { 128, 32 }, {7, 0}, 5, 0, WHITE },
		{ Novice::LoadTexture("./resources/texture/ui/play/mini_energy_gauge_meter.png"), { 128, 32 }, {0, 0}, 0, 0, WHITE },
		{ Novice::LoadTexture("./resources/texture/ui/play/mini_energy_gauge_delay_meter.png"), { 128, 32 }, {0, 0}, 0, 0, WHITE },

		{ Novice::LoadTexture("./resources/texture/ui/play/boss_hp_gauge_frame.png"), { 768, 32 }, {0, 0}, 0, 0, WHITE },
		{ Novice::LoadTexture("./resources/texture/ui/play/boss_hp_gauge_meter.png"), { 768, 32 }, {0, 0}, 0, 0, WHITE },
		{ Novice::LoadTexture("./resources/texture/ui/play/boss_hp_gauge_delay_meter.png"), { 768, 32 }, {0, 0}, 0, 0, WHITE },

		{ Novice::LoadTexture("./resources/texture/ui/play/progress_gauge.png"), { 768, 32 }, {0, 0}, 0, 0, WHITE },
		{ Novice::LoadTexture("./resources/texture/ui/play/progress_gauge_icon.png"), { 32, 32 }, {0, 0}, 0, 0, WHITE },

		{ Novice::LoadTexture("./resources/texture/ui/play/progress_gauge_frame.png"), { 768, 32 }, {0, 0}, 0, 0, WHITE },
		{ Novice::LoadTexture("./resources/texture/ui/play/progress_gauge_meter.png"), { 768, 32 }, {0, 0}, 0, 0, WHITE },

		{ Novice::LoadTexture("./resources/texture/ui/play/combo_icon.png"), { 128, 64 }, {0, 0}, 0, 0, WHITE },
		{ Novice::LoadTexture("./resources/texture/ui/play/combo_numbers.png"), { 68, 128 }, {0, 0}, 0, 0, WHITE },
		{ Novice::LoadTexture("./resources/texture/ui/play/combo_numbers_meter_reverse.png"), { 68, 128 }, {0, 0}, 0, 0, WHITE },

		{ Novice::LoadTexture("./resources/texture/ui/play/score_numbers.png"), { 64, 128 }, {0, 0}, 9, 0, WHITE },
		{ Novice::LoadTexture("./resources/texture/ui/play/score_numbers.png"), { 64, 128 }, {0, 0}, 9, 0, WHITE },

		{ Novice::LoadTexture("./resources/texture/ui/play/display_effect.png"), { 1280, 720 }, {0, 0}, 0, 0, WHITE },

		{ Novice::LoadTexture("./resources/texture/ui/play/ready.png"), { 512, 128 }, {0, 0}, 0, 0, WHITE },
		{ Novice::LoadTexture("./resources/texture/ui/play/go.png"), { 512, 128 }, {0, 0}, 0, 0, WHITE },

		{ Novice::LoadTexture("./resources/texture/ui/play/warn_boss.png"), { 512, 128 }, {0, 0}, 0, 0, WHITE },

		{ Novice::LoadTexture("./resources/texture/ui/play/pause_guide.png"), { 256, 64 }, {0, 0}, 1, 0, WHITE },

		//effect
		{ Novice::LoadTexture("./resources/texture/effect/effect.square.png"), {128 , 128 }, {0, 0}, 0, 0, WHITE },
		{ Novice::LoadTexture("./resources/texture/effect/effect.square.mini.png"), {32 , 32 }, {0, 0}, 0, 0, WHITE },
		{ Novice::LoadTexture("./resources/texture/effect/effect.circle.png"), {256 , 256 }, {0, 0}, 0, 0, WHITE },
		{ Novice::LoadTexture("./resources/texture/effect/effect.triangle.png"), {26 , 22 }, {0, 0}, 0, 0, WHITE },
		{ Novice::LoadTexture("./resources/texture/effect/effect.explosion.png"),{768 , 768}, {0 ,0}, 0, 0, WHITE },
		{ Novice::LoadTexture("./resources/texture/effect/explode_effect.png"), {128 , 128 }, {2, 0}, 6, 0, WHITE },
		{ Novice::LoadTexture("./resources/texture/effect/barrier.png"),{ 256,256 },{ 0,0 },0,0,WHITE },
		{ Novice::LoadTexture("./resources/texture/effect/barrierpiece.png"),{ 60,60 },{ 0,0 },0,0,WHITE },
		//===============
		//Pause
		//===============
		{ Novice::LoadTexture("./resources/texture/ui/pause/pause_icon.png"), { 600, 150 }, {0, 0}, 0, 0, WHITE },
		{ Novice::LoadTexture("./resources/texture/ui/pause/pause_guide.png"), { 600, 150 }, {0, 0}, 0, 0, WHITE },

		//===============
		//Clear
		//===============
		{ Novice::LoadTexture("./resources/texture/ui/clear/clear_icon.png"), { 680, 150 }, {0, 0}, 0, 0, WHITE },
		{ Novice::LoadTexture("./resources/texture/ui/clear/score_icon.png"), { 512, 64 }, {0, 0}, 0, 0, WHITE },
		{ Novice::LoadTexture("./resources/texture/ui/clear/max_combo_icon.png"), { 512, 64 }, {0, 0}, 0, 0, WHITE },
		{ Novice::LoadTexture("./resources/texture/ui/clear/best_score_icon.png"), { 512, 64 }, {0, 0}, 0, 0, WHITE },

		{ Novice::LoadTexture("./resources/texture/ui/clear/break_score_telop.png"), { 512, 64 }, {0, 0}, 0, 0, WHITE },

		//===============
		//Gameover
		//===============
		{ Novice::LoadTexture("./resources/texture/ui/gameover/gameover_icon.png"), { 600, 150 }, {0, 0}, 0, 0, WHITE },

		//===============
		//Guide
		//===============
		{ Novice::LoadTexture("./resources/texture/ui/guide/controller_guide.png"), { 1024, 512 }, {0, 0}, 1, 0, WHITE },
		{ Novice::LoadTexture("./resources/texture/ui/guide/gauge_guide.png"), { 1024, 256 }, {0, 0}, 0, 0, WHITE },
		{ Novice::LoadTexture("./resources/texture/ui/guide/shield_guide.png"), { 512, 64 }, {0, 0}, 0, 0, WHITE },


	};

	memcpy(textures, temp, sizeof(textures));

}


Texture GetTexture(TextureType type) {
	return textures[ToInt(type)];
}

/// <summary>
/// オブジェクトの描画関数
/// </summary>
/// <param name="texture">テクスチャデータ</param>
/// <param name="centerPos">中心座標</param>
/// <param name="size">サイズ</param>
void DrawTextureObj(Texture texture, Vector2 centerPos, Vector2 size) {

	Vector2 vertexW[kVertexNum] = { 0, 0 };
	Vector2i vertexS[kVertexNum] = { 0, 0 };

	SetVertex(centerPos, size, vertexW);

	for (int i = 0; i < kVertexNum; i++) {
		vertexS[i] = ConvertPosWToS(vertexW[i]);
	}

	Novice::DrawQuad(
		vertexS[0].x, vertexS[0].y,
		vertexS[1].x, vertexS[1].y,
		vertexS[2].x, vertexS[2].y,
		vertexS[3].x, vertexS[3].y,
		texture.refSize.x * texture.animNum, 0, texture.refSize.x, texture.refSize.y,
		texture.tHandle, texture.color
	);
}

/// <summary>
/// オブジェクト(反転)の描画関数
/// </summary>
/// <param name="texture">テクスチャデータ</param>
/// <param name="centerPos">中心座標</param>
/// <param name="size">サイズ</param>
void DrawTextureObjReverse(Texture texture, Vector2 centerPos, Vector2 size) {

	Vector2 vertexW[kVertexNum] = { 0, 0 };
	Vector2i vertexS[kVertexNum] = { 0, 0 };

	SetVertex(centerPos, size, vertexW);

	for (int i = 0; i < kVertexNum; i++) {
		vertexS[i] = ConvertPosWToS(vertexW[i]);
	}

	Novice::DrawQuad(
		vertexS[0].x, vertexS[0].y,
		vertexS[1].x, vertexS[1].y,
		vertexS[2].x, vertexS[2].y,
		vertexS[3].x, vertexS[3].y,
		texture.refSize.x * (texture.animNum + 1), 0, -texture.refSize.x, texture.refSize.y,
		texture.tHandle, texture.color
	);
}

/// <summary>
/// 回転するオブジェクトの描画関数
/// </summary>
/// <param name="texture">テクスチャデータ</param>
/// <param name="centerPos">中心座標</param>
/// <param name="size">サイズ</param>
/// <param name="rotateTheta">回転量</param>
void DrawTextureRotateObj(Texture texture, Vector2 centerPos, Vector2 size, float rotateTheta) {

	Vector2 vertexW[kVertexNum] = { 0, 0 };
	Vector2i vertexS[kVertexNum] = { 0, 0 };
	//頂点座標を更新
	SetVertexRotate(centerPos, size, vertexW, rotateTheta);
	//ワールド座標からスクリーン座標に変換
	for (int i = 0; i < kVertexNum; i++) {
		vertexS[i] = ConvertPosWToS(vertexW[i]);
	}

	Novice::DrawQuad(
		vertexS[0].x, vertexS[0].y,
		vertexS[1].x, vertexS[1].y,
		vertexS[2].x, vertexS[2].y,
		vertexS[3].x, vertexS[3].y,
		texture.refSize.x * texture.animNum, 0, texture.refSize.x, texture.refSize.y,
		texture.tHandle, texture.color
	);
}

/// <summary>
/// 背景の描画関数
/// </summary>
/// <param name="texture">テクスチャデータ</param>
/// <param name="centerPos">中心座標</param>
/// <param name="size">サイズ</param>
/// <param name="scrollRate">スクロール倍率</param>
void DrawTextureBG(Texture texture, Vector2 centerPos, Vector2 size, float scrollRate) {

	Vector2 vertexW[kVertexNum] = { 0, 0 };
	Vector2i vertexS[kVertexNum] = { 0, 0 };

	SetVertex(centerPos, size, vertexW);

	for (int i = 0; i < kVertexNum; i++) {
		vertexS[i] = ConvertPosWToSForBG(vertexW[i], scrollRate);
	}

	Novice::DrawQuad(
		vertexS[0].x, vertexS[0].y,
		vertexS[1].x, vertexS[1].y,
		vertexS[2].x, vertexS[2].y,
		vertexS[3].x, vertexS[3].y,
		texture.refSize.x * texture.animNum, 0, texture.refSize.x, texture.refSize.y,
		texture.tHandle, texture.color
	);
}

/// <summary>
/// UI(HUD)の描画関数
/// </summary>
/// <param name="texture">テクスチャデータ</param>
/// <param name="centerPos">中心座標</param>
/// <param name="size">サイズ</param>
void DrawTextureUI(Texture texture, Vector2 centerPos, Vector2 size) {
	Vector2 vertexW[kVertexNum] = { 0, 0 };
	Vector2i vertexS[kVertexNum] = { 0, 0 };

	SetVertex(centerPos, size, vertexW);

	for (int i = 0; i < kVertexNum; i++) {
		vertexS[i] = ConvertPosWToSUI(vertexW[i]);
	}

	Novice::DrawQuad(
		vertexS[0].x, vertexS[0].y,
		vertexS[1].x, vertexS[1].y,
		vertexS[2].x, vertexS[2].y,
		vertexS[3].x, vertexS[3].y,
		texture.refSize.x * texture.animNum, 0, texture.refSize.x, texture.refSize.y,
		texture.tHandle, texture.color
	);
}

/// <summary>
/// 回転するUI(HUD)の描画関数
/// </summary>
/// <param name="texture">テクスチャデータ</param>
/// <param name="centerPos">中心座標</param>
/// <param name="size">サイズ</param>
/// <param name="rotateTheta">回転量</param>
void DrawTextureRotateUI(Texture texture, Vector2 centerPos, Vector2 size, float rotateTheta) {
	Vector2 vertexW[kVertexNum] = { 0, 0 };
	Vector2i vertexS[kVertexNum] = { 0, 0 };

	//頂点座標を更新
	SetVertexRotate(centerPos, size, vertexW, rotateTheta);

	for (int i = 0; i < kVertexNum; i++) {
		vertexS[i] = ConvertPosWToSUI(vertexW[i]);
	}

	Novice::DrawQuad(
		vertexS[0].x, vertexS[0].y,
		vertexS[1].x, vertexS[1].y,
		vertexS[2].x, vertexS[2].y,
		vertexS[3].x, vertexS[3].y,
		texture.refSize.x * texture.animNum, 0, texture.refSize.x, texture.refSize.y,
		texture.tHandle, texture.color
	);
}

/// <summary>
/// ゲージの描画処理(Obj)
/// </summary>
/// <param name="texture">テクスチャデータ</param>
/// <param name="centerPos">中心座標</param>
/// <param name="size">サイズ</param>
/// <param name="rate">ゲージの割合</param>
void DrawGaugeAsObj(Texture texture, Vector2 centerPos, Vector2 size, float rate) {
	//割合に補正を掛ける
	rate = std::clamp(rate, 0.0f, 1.0f);
	float gaugeWidth = size.x * rate;
	int gaugeRefWidth = ToInt(texture.refSize.x * rate);

	Vector2 vertexW[kVertexNum] = { 0, 0 };
	Vector2i vertexS[kVertexNum] = { 0, 0 };

	SetVertex(centerPos, size, vertexW);

	for (int i = 0; i < kVertexNum; i++) {
		//右側の頂点をrateに合わせてx座標更新
		if (i == 1) {
			vertexW[i].x = vertexW[0].x + gaugeWidth;
		}
		else if (i == 3) {
			vertexW[i].x = vertexW[2].x + gaugeWidth;
		}

		vertexS[i] = ConvertPosWToS(vertexW[i]);
	}

	Novice::DrawQuad(
		vertexS[0].x, vertexS[0].y,
		vertexS[1].x, vertexS[1].y,
		vertexS[2].x, vertexS[2].y,
		vertexS[3].x, vertexS[3].y,
		texture.refSize.x * texture.animNum, 0, gaugeRefWidth, texture.refSize.y,
		texture.tHandle, texture.color
	);
}

/// <summary>
/// ゲージの描画処理(UI)
/// </summary>
/// <param name="texture">テクスチャデータ</param>
/// <param name="centerPos">中心座標</param>
/// <param name="size">サイズ</param>
/// <param name="rate">ゲージの割合</param>
void DrawGaugeAsUI(Texture texture, Vector2 centerPos, Vector2 size, float rate) {
	//割合に補正を掛ける
	rate = std::clamp(rate, 0.0f, 1.0f);
	float gaugeWidth = size.x * rate;
	int gaugeRefWidth = ToInt(texture.refSize.x * rate);

	Vector2 vertexW[kVertexNum] = { 0, 0 };
	Vector2i vertexS[kVertexNum] = { 0, 0 };

	SetVertex(centerPos, size, vertexW);

	for (int i = 0; i < kVertexNum; i++) {
		//右側の頂点をrateに合わせてx座標更新
		if (i == 1) {
			vertexW[i].x = vertexW[0].x + gaugeWidth;
		}
		else if (i == 3) {
			vertexW[i].x = vertexW[2].x + gaugeWidth;
		}

		vertexS[i] = ConvertPosWToSUI(vertexW[i]);
	}

	Novice::DrawQuad(
		vertexS[0].x, vertexS[0].y,
		vertexS[1].x, vertexS[1].y,
		vertexS[2].x, vertexS[2].y,
		vertexS[3].x, vertexS[3].y,
		texture.refSize.x * texture.animNum, 0, gaugeRefWidth, texture.refSize.y,
		texture.tHandle, texture.color
	);
}

/// <summary>
/// 縦ゲージの描画処理(UI)
/// </summary>
/// <param name="texture">テクスチャデータ</param>
/// <param name="centerPos">中心座標</param>
/// <param name="size">サイズ</param>
/// <param name="rate">ゲージの割合</param>
void DrawGaugeYAsUI(Texture texture, Vector2 centerPos, Vector2 size, float rate) {
	//割合に補正を掛ける
	rate = std::clamp(rate, 0.0f, 1.0f);
	float gaugeHeight = size.y * rate;
	int gaugeRefHeight = ToInt(texture.refSize.y * rate);

	Vector2 vertexW[kVertexNum] = { 0, 0 };
	Vector2i vertexS[kVertexNum] = { 0, 0 };

	SetVertex(centerPos, size, vertexW);

	// 上側の頂点を rate に合わせて y 座標更新
	// vertexW[0] = 左上, vertexW[1] = 右上
	vertexW[0].y = vertexW[2].y + gaugeHeight;  // 左上
	vertexW[1].y = vertexW[3].y + gaugeHeight;  // 右上

	for (int i = 0; i < kVertexNum; i++) {
		vertexS[i] = ConvertPosWToSUI(vertexW[i]);
	}

  	Novice::DrawQuad(
		vertexS[2].x, vertexS[2].y,
		vertexS[3].x, vertexS[3].y,
		vertexS[0].x, vertexS[0].y,
		vertexS[1].x, vertexS[1].y,
		texture.refSize.x * texture.animNum, 0, texture.refSize.x, gaugeRefHeight,
		texture.tHandle, texture.color
	);
}

/// <summary>
/// アニメーション変更処理(通常)
/// </summary>
/// <param name="texture">テクスチャデータ</param>
void UpdateAnimation(Texture& texture) {
	texture.animChangeTimer.count++;
	if (texture.animChangeTimer.count >= texture.animChangeTimer.time) {
		texture.animChangeTimer.count = 0;

		if (texture.animNum < texture.animLimit) {
			texture.animNum++;
		}
		else {
			texture.animNum = 0;
		}

	}
}