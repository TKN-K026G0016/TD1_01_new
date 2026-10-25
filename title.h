#pragma once

void InitTitle();
void UpdateTitle();
void DrawTitle();

enum class ModeTitle {
	//コントローラ推奨画面
	Recommend,
	MovePlayer,
	TriggerLightUp,
	WaitInput,
};

/// <summary>
/// TitleのRecommend起動
/// </summary>
/// <param name=""></param>
void TriggerTitleRecommend(void);

/// <summary>
/// 現在のタイトルの状態を取得
/// </summary>
/// <returns>nowMode</returns>
ModeTitle GetNowModeTitle(void);