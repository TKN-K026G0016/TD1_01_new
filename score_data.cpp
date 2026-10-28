#include "score_data.h"

#include <fstream>
#include <sstream>
#include <string>
#include <limits>

#pragma region データ
//現在のスコア
int nowScore = 0;
//ハイスコア
int highScore = 0;

static constexpr int kFirstHighScore = 100000;
#pragma endregion

///
static void LoadHighScore(void) {
	FILE* fp;
	fopen_s(&fp, "score.dat", "r");

	if (fp != nullptr) {
		(void)fscanf_s(fp, "%d", &highScore);
		fclose(fp);
	}
}

static void SaveHighScore(void) {
	FILE* fp;
	fopen_s(&fp, "score.dat", "w");

	if (fp != nullptr) {
		fprintf_s(fp, "%d", highScore);
		fclose(fp);
	}
}

void InitScoreData(void) {
	LoadHighScore();
}

void UpdateScoreData(void) {

}

int GetNowScore(void) {
	return nowScore;
}

int GetHighScore(void) {
	return highScore;
}

void AddScore(int point) {
	if (point < 0) return;

	//int型の最大値
	constexpr int kMaxScore = std::numeric_limits<int>::max();

	//カンストしていたら
	
	if (nowScore > kMaxScore - point) {
		nowScore = kMaxScore;
	}
	//カンストしていなければ
	else {
		nowScore += point;
	}

}

void ResetNowScore(void) {
	nowScore = 0;
}

void UpdateHighScore(void) {
	if (nowScore <= highScore) return;

	highScore = nowScore;
	SaveHighScore();
}

void ResetHighScore(void) {
	highScore = kFirstHighScore;
	SaveHighScore();
}