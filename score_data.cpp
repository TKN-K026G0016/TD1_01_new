#include "score_data.h"

#include <fstream>
#include <sstream>
#include <string>

#pragma region データ
//現在のスコア
int nowScore = 0;
//ハイスコア
int highScore = 0;

static constexpr int kFirstHighScore = 1000000;
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
	nowScore += point;
}

void ResetNowScore(void) {
	nowScore = 0;
}

void UpdateHighScore(void) {
	highScore = nowScore;
	SaveHighScore();
}

void ResetHighScore(void) {
	highScore = kFirstHighScore;
	SaveHighScore();
}