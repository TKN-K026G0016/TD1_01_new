#pragma once

void InitScoreData(void);
void UpdateScoreData(void);

/// <summary>
/// 現在のスコア
/// </summary>
/// <returns>nowScore</returns>
int GetNowScore(void);

/// <summary>
/// ハイスコアの取得
/// </summary>
/// <returns>highScore</returns>
int GetHighScore(void);

/// <summary>
/// スコアの加算処理
/// </summary>
/// <param name="point">取得スコア</param>
void AddScore(int point);

/// <summary>
/// 現在のスコアリセット
/// </summary>
void ResetNowScore(void);

/// <summary>
/// ハイスコア更新処理
/// </summary>
void UpdateHighScore(void);

void ResetHighScore(void);