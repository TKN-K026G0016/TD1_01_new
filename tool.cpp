#include "tool.h"
#include "vector2.h"
#include "matrix.h"
#include "common.h"
#include "stage.h"

#include <Novice.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <algorithm>
#include <stdlib.h>
#include <time.h>

void InitTool(void) {
	srand(ToInt(time(nullptr)));
}


int GetRand(int min, int max) {
	return (rand() % (max - min + 1)) + min;
};


void SetVertex(Vector2 pos, Vector2 size, Vector2 vertex[kVertexNum]) {
	for (int i = 0; i < kVertexNum; i++) {
		switch (i) {
		case 0:
			vertex[i] = { pos.x - size.x / 2, pos.y + size.y / 2 };
			break;

		case 1:
			vertex[i] = { pos.x + size.x / 2, pos.y + size.y / 2 };
			break;

		case 2:
			vertex[i] = { pos.x - size.x / 2, pos.y - size.y / 2 };
			break;

		case 3:
			vertex[i] = { pos.x + size.x / 2, pos.y - size.y / 2 };
			break;
		}
	}
}


void SetVertexRotate(Vector2 pos, Vector2 size, Vector2 vertex[kVertexNum], float rotateTheta) {
	Matrix2x2 rotateMatrix = MakeRotateMatrix(rotateTheta);

	for (int i = 0; i < kVertexNum; i++) {
		//ローカル座標(中心座標が原点の際)
		Vector2 local = { 0, 0 };
		switch (i) {
		case 0:
			local = { -size.x / 2.0f, size.y / 2.0f };
			break;
		case 1:
			local = { size.x / 2.0f, size.y / 2.0f };
			break;
		case 2:
			local = { -size.x / 2.0f, -size.y / 2.0f };
			break;
		case 3:
			local = { size.x / 2.0f, -size.y / 2.0f };
			break;
		}

		//ローカル座標が回転後の座標
		Vector2 rotatedVertex = MultplyVectorVSMatrix(local, rotateMatrix);
		//ワールド座標に戻して、変更
		vertex[i] = { pos.x + rotatedVertex.x, pos.y + rotatedVertex.y };
	}
}

Vector2i ConvertPosWToS(Vector2 posW) {

	Vector2 posS = { 0, 0 };
	Vector2 cameraPos = GetCameraPos();
	// まず通常のスクリーン座標へ変換
	posS.x = (posW.x - cameraPos.x) + kWindowCenter.x;
	posS.y = -(posW.y - cameraPos.y) + kWindowCenter.y;

	// ウィンドウ中心
	float centerX = kWindowCenter.x;
	float centerY = kWindowCenter.y;

	// 中心基準で拡縮
	posS.x = (posS.x - centerX) * GetCameraZoom() + centerX;
	posS.y = (posS.y - centerY) * GetCameraZoom() + centerY;

	return { ToInt(posS.x), ToInt(posS.y) };
}

Vector2i ConvertPosWToSUI(Vector2 posW) {

	Vector2 posS = { 0, 0 };
	// まず通常のスクリーン座標へ変換
	posS.x = (posW.x + kDelayBetweenWToS.x);
	posS.y = (-posW.y + kDelayBetweenWToS.y);

	return { ToInt(posS.x), ToInt(posS.y) };
}

Vector2i ConvertPosWToSForBG(Vector2 posW, float scrollRate) {
	Vector2 posS;
	Vector2 cameraPos = GetCameraPos();

	// まず通常のスクリーン座標へ変換
	posS.x = (posW.x - cameraPos.x) + kWindowCenter.x * scrollRate;
	posS.y = -(posW.y - cameraPos.y) + kWindowCenter.y * scrollRate;

	// ウィンドウ中心
	float centerX = kWindowCenter.x;
	float centerY = kWindowCenter.y;

	// 中心基準で拡縮
	posS.x = (posS.x - centerX) * GetCameraZoom() + centerX;
	posS.y = (posS.y - centerY) * GetCameraZoom() + centerY;

	return { ToInt(posS.x), ToInt(posS.y) };
}

bool CheckCollisionCircleVSCircle(Vector2 circle1Pos, float radius1, Vector2 circle2Pos, float radius2) {
	float disX = circle1Pos.x - circle2Pos.x;
	float disY = circle1Pos.y - circle2Pos.y;
	float disTotal = sqrtf(disX * disX + disY * disY);

	float disRad = radius1 + radius2;

	if (disTotal <= disRad) {
		return true;
	} else {
		return false;
	}

}

bool CheckCollisionBoxVSCircle(Vector2 circlePos, float circleRadius, Vector2 boxVertex0, Vector2 boxVertex3) {
	float closestPosX = std::clamp(circlePos.x, boxVertex0.x, boxVertex3.x);
	float closestPosY = std::clamp(circlePos.y, boxVertex3.y, boxVertex0.y);

	float disX = circlePos.x - closestPosX;
	float disY = circlePos.y - closestPosY;
	float disTotal = sqrtf(disX * disX + disY * disY);

	if (disTotal <= circleRadius) {
		return true;
	} else {
		return false;
	}
}

/// <summary>
/// 点と線分の最短距離を求める
/// </summary>
/// <param name="p">点の座標</param>
/// <param name="a">線分の始点</param>
/// <param name="b">線分の終点</param>
/// <returns></returns>
float DistancePointToSegment(Vector2 p, Vector2 a, Vector2 b) {
	//A→Bのベクトル
	Vector2 vectorAtoB = { b.x - a.x, b.y - a.y };
	//A→Pのベクトル
	Vector2 vectorAtoP = { p.x - a.x, p.y - a.y };

	float abLenSq = vectorAtoB.x * vectorAtoB.x + vectorAtoB.y * vectorAtoB.y;
	float t = (vectorAtoP.x * vectorAtoB.x + vectorAtoP.y * vectorAtoB.y) / abLenSq;

	t = std::clamp(t, 0.0f, 1.0f);

	Vector2 closest = { a.x + vectorAtoB.x * t, a.y + vectorAtoB.y * t };

	float dx = p.x - closest.x;
	float dy = p.y - closest.y;

	return sqrtf(dx * dx + dy * dy);
}

bool CheckCollisionOBBvsCircle(Vector2 circlePos, float circleRadius, Vector2 vertex[4]) {

	// 4辺の線分距離判定
	for (int i = 0; i < 4; i++) {
		Vector2 a = vertex[i];
		Vector2 b = vertex[(i + 1) % 4];

		float dist = DistancePointToSegment(circlePos, a, b);
		if (dist <= circleRadius) {
			return true;
		}
	}

	return false;
}



/// <summary>
/// 線形補正での移動
/// </summary>
/// <param name="startPos">startPos</param>
/// <param name="endPos">endPos</param>
/// <param name="t">媒介変数(この関数の前に更新しておく)</param>
/// <param name="mode">動き方</param>
/// <returns>更新後のPos</returns>
Vector2 MoveByEasing(Vector2 startPos, Vector2 endPos, float t, EasingMode mode) {
	float easedT = 0.0f;

	switch (mode) {
	case EasingMode::Linear:
	{
		easedT = t;

		if (easedT > 1.0f) {
			easedT = 1.0f;
		}

		break;
	}
	case  EasingMode::EaseIn:
	{
		easedT = t * t;

		if (easedT > 1.0f) {
			easedT = 1.0f;
		}

		break;
	}
	case  EasingMode::EaseOut:
	{
		easedT = 1.0f - powf(1.0f - t, 3.0f);

		if (easedT > 1.0f) {
			easedT = 1.0f;
		}

		break;
	}
	case  EasingMode::EaseInOut:
	{
		easedT = -(cosf(static_cast<float>(M_PI) * t) - 1.0f) / 2.0f;

		if (easedT > 1.0f) {
			easedT = 1.0f;
		}

		break;
	}
	case  EasingMode::EaseInBack:
	{
		float c1 = kOvershootAmount;
		float c3 = c1 + 1;

		easedT = c3 * t * t * t - c1 * t * t;

		if (easedT > 1.0f) {
			easedT = 1.0f;
		}

		break;
	}
	case  EasingMode::EaseOutBack:
	{
		float c1 = kOvershootAmount;
		float c3 = c1 + 1;

		easedT = 1 + c3 * static_cast<float>(pow(t - 1, 3)) + c1 * static_cast<float>(pow(t - 1, 2));

		break;
	}
	}



	return { (1.0f - easedT) * startPos.x + easedT * endPos.x,(1.0f - easedT) * startPos.y + easedT * endPos.y };
}

/// <summary>
/// 線形補正での数値(int)の更新
/// </summary>
/// <param name="startPos">最初の数値</param>
/// <param name="endPos">最終的な数値</param>
/// <param name="t">媒介変数(この関数の前に更新しておく)</param>
/// <param name="mode">動き方</param>
/// <returns>更新後の数値</returns>
int UpdateValueByEasing(int startNum, int endNum, float t, EasingMode mode) {
	float easedT = 0.0f;

	switch (mode) {
	case EasingMode::Linear:
	{
		easedT = t;

		if (easedT > 1.0f) {
			easedT = 1.0f;
		}

		break;
	}
	case  EasingMode::EaseIn:
	{
		easedT = t * t;

		if (easedT > 1.0f) {
			easedT = 1.0f;
		}

		break;
	}
	case  EasingMode::EaseOut:
	{
		easedT = 1.0f - powf(1.0f - t, 3.0f);

		if (easedT > 1.0f) {
			easedT = 1.0f;
		}

		break;
	}
	case  EasingMode::EaseInOut:
	{
		easedT = -(cosf(static_cast<float>(M_PI) * t) - 1.0f) / 2.0f;

		if (easedT > 1.0f) {
			easedT = 1.0f;
		}

		break;
	}
	case  EasingMode::EaseInBack:
	{
		float c1 = kOvershootAmount;
		float c3 = c1 + 1;

		easedT = c3 * t * t * t - c1 * t * t;

		if (easedT > 1.0f) {
			easedT = 1.0f;
		}

		break;
	}
	case  EasingMode::EaseOutBack:
	{
		float c1 = kOvershootAmount;
		float c3 = c1 + 1;

		easedT = 1 + c3 * static_cast<float>(pow(t - 1, 3)) + c1 * static_cast<float>(pow(t - 1, 2));

		break;
	}
	}


	return static_cast<int>((1.0f - easedT) * startNum + easedT * endNum);
}

/// <summary>
/// 線形補正での数値(float)の更新
/// </summary>
/// <param name="startPos">最初の数値</param>
/// <param name="endPos">最終的な数値</param>
/// <param name="t">媒介変数(この関数の前に更新しておく)</param>
/// <param name="mode">動き方</param>
/// <returns>更新後の数値</returns>
float UpdateValueByEasing(float startNum, float endNum, float t, EasingMode mode) {
	float easedT = 0.0f;

	switch (mode) {
	case EasingMode::Linear:
	{
		easedT = t;

		if (easedT > 1.0f) {
			easedT = 1.0f;
		}

		break;
	}
	case  EasingMode::EaseIn:
	{
		easedT = t * t;

		if (easedT > 1.0f) {
			easedT = 1.0f;
		}

		break;
	}
	case  EasingMode::EaseOut:
	{
		easedT = 1.0f - powf(1.0f - t, 3.0f);

		if (easedT > 1.0f) {
			easedT = 1.0f;
		}

		break;
	}
	case  EasingMode::EaseInOut:
	{
		easedT = -(cosf(static_cast<float>(M_PI) * t) - 1.0f) / 2.0f;

		if (easedT > 1.0f) {
			easedT = 1.0f;
		}

		break;
	}
	case  EasingMode::EaseInBack:
	{
		float c1 = kOvershootAmount;
		float c3 = c1 + 1;

		easedT = c3 * t * t * t - c1 * t * t;

		if (easedT > 1.0f) {
			easedT = 1.0f;
		}

		break;
	}
	case  EasingMode::EaseOutBack:
	{
		float c1 = kOvershootAmount;
		float c3 = c1 + 1;

		easedT = 1 + c3 * static_cast<float>(pow(t - 1, 3)) + c1 * static_cast<float>(pow(t - 1, 2));

		break;
	}
	}

	return (1.0f - easedT) * startNum + easedT * endNum;
}

float CreateEasedT(float t, EasingMode mode) {
	float easedT = 0.0f;

	switch (mode) {
	case EasingMode::Linear:
	{
		easedT = t;

		if (easedT > 1.0f) {
			easedT = 1.0f;
		}

		break;
	}
	case  EasingMode::EaseIn:
	{
		easedT = t * t;

		if (easedT > 1.0f) {
			easedT = 1.0f;
		}

		break;
	}
	case  EasingMode::EaseOut:
	{
		easedT = 1.0f - powf(1.0f - t, 3.0f);

		if (easedT > 1.0f) {
			easedT = 1.0f;
		}

		break;
	}
	case  EasingMode::EaseInOut:
	{
		easedT = -(cosf(static_cast<float>(M_PI) * t) - 1.0f) / 2.0f;

		if (easedT > 1.0f) {
			easedT = 1.0f;
		}

		break;
	}
	case  EasingMode::EaseInBack:
	{
		float c1 = kOvershootAmount;
		float c3 = c1 + 1;

		easedT = c3 * t * t * t - c1 * t * t;

		if (easedT > 1.0f) {
			easedT = 1.0f;
		}

		break;
	}
	case  EasingMode::EaseOutBack:
	{
		float c1 = kOvershootAmount;
		float c3 = c1 + 1;

		easedT = 1 + c3 * static_cast<float>(pow(t - 1, 3)) + c1 * static_cast<float>(pow(t - 1, 2));

		break;
	}
	}

	return easedT;
}

unsigned int UpdateColorByEasing(unsigned int startColor, unsigned int endColor, float t) {
	int sr = (startColor >> 24) & 0xFF;
	int sg = (startColor >> 16) & 0xFF;
	int sb = (startColor >> 8) & 0xFF;
	int sa = (startColor) & 0xFF;

	int er = (endColor >> 24) & 0xFF;
	int eg = (endColor >> 16) & 0xFF;
	int eb = (endColor >> 8) & 0xFF;
	int ea = (endColor) & 0xFF;

	int r = static_cast<int>(sr + (er - sr) * t);
	int g = static_cast<int>(sg + (eg - sg) * t);
	int b = static_cast<int>(sb + (eb - sb) * t);
	int a = static_cast<int>(sa + (ea - sa) * t);

	return (r << 24) | (g << 16) | (b << 8) | a;
}

/// <summary>
/// 矩形の当たり判定描画
/// </summary>
/// <param name="hitVertex0">左上の頂点座標</param>
/// <param name="hitVertex1">右下の頂点座標</param>
void DrawHitBox(Vector2 hitVertex0, Vector2 hitVertex1) {
	Vector2i hitBoxVertexS[2];
	hitBoxVertexS[0] = ConvertPosWToS(hitVertex0);
	hitBoxVertexS[1] = ConvertPosWToS(hitVertex1);

	Novice::DrawLine
	(
		hitBoxVertexS[0].x, hitBoxVertexS[0].y,
		hitBoxVertexS[1].x, hitBoxVertexS[0].y, RED
	);
	Novice::DrawLine
	(
		hitBoxVertexS[0].x, hitBoxVertexS[0].y,
		hitBoxVertexS[0].x, hitBoxVertexS[1].y, RED
	);
	Novice::DrawLine
	(
		hitBoxVertexS[0].x, hitBoxVertexS[1].y,
		hitBoxVertexS[1].x, hitBoxVertexS[1].y, RED
	);
	Novice::DrawLine
	(
		hitBoxVertexS[1].x, hitBoxVertexS[0].y,
		hitBoxVertexS[1].x, hitBoxVertexS[1].y, RED
	);
}

void DrawHitEllipse(Vector2 centerPos, float radius, unsigned int color, bool solidSwitch) {
	Vector2i posS = ConvertPosWToS(centerPos);
	int viewRadius = ToInt(radius);

	unsigned int viewColor = (color & 0xFFFFFF00) | kDebugHitColor;

	if (solidSwitch) {
		Novice::DrawEllipse(
			posS.x, posS.y, viewRadius, viewRadius, 0.0f, viewColor, kFillModeSolid
		);
	} else {
		Novice::DrawEllipse(
	posS.x, posS.y, viewRadius, viewRadius, 0.0f, viewColor, kFillModeWireFrame
		);
	}

}

/// <summary>
/// 指定の桁の数値を取得
/// </summary>
/// <param name="targetNum">目的の数値</param>
/// <param name="targetDigit">取りたい桁(右端は0)</param>
/// <returns>取りたい桁の数値</returns>
int GetValueAtDigit(int targetNum, int targetDigit) {
	int digit;

	digit = targetNum / static_cast<int>(pow(10, targetDigit));
	digit %= 10;

	return digit;
}

/// <summary>
/// rotateMatrixの作成
/// </summary>
/// <param name="theta">回転量</param>
/// <returns>rotateMatrix</returns>
Matrix2x2 MakeRotateMatrix(float theta) {
	Matrix2x2 rotateMatrix;
	rotateMatrix.m[0][0] = cosf(theta);
	rotateMatrix.m[0][1] = sinf(theta);
	rotateMatrix.m[1][0] = -sinf(theta);
	rotateMatrix.m[1][1] = cosf(theta);

	return rotateMatrix;
}

/// <summary>
/// ベクトルとマトリックスの掛け算
/// </summary>
/// <param name="vector">ベクトル</param>
/// <param name="matrix">マトリックス</param>
/// <returns>ベクトル</returns>
Vector2 MultplyVectorVSMatrix(Vector2 vector, Matrix2x2 matrix) {
	Vector2 result;
	result.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0];
	result.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1];
	return result;
}