#pragma once

struct Timer {
	int time;
	int count;
};

struct TimeData {
	int min;
	int sec;
	int frame;
};

/// <summary>
/// 指定TimeDataのリセット処理
/// </summary>
/// <param name="data"></param>
void ResetTimeData(TimeData& data);

/// <summary>
/// TimeDataの変更処理(毎フレーム呼び出し)
/// </summary>
/// <param name="data">変更したいtimeData</param>
void CountTimeData(TimeData& data);

/// <summary>
/// TimeDataをintに変換
/// </summary>
/// <param name="data"></param>
/// <returns></returns>
int ConvertTimeDataToInt(const TimeData& data);