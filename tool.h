#pragma once
#include "vector2.h"
#include "matrix.h"

//頂点の数
constexpr int kVertexNum = 4;
//当たり判定の透明度
constexpr unsigned int kDebugHitColor = 0x55;

template <typename T>
//static_cast<int>()の代わり
constexpr int ToInt(T value) {
	return static_cast<int>(value);
}

template <typename T>
//static_cast<float>()の代わり
constexpr float ToFloat(T value) {
	return static_cast<float>(value);
}

void InitTool(void);

/// <summary>
/// ランダムなintを取得
/// </summary>
/// <param name="min">最小値</param>
/// <param name="max">最大値</param>
/// <returns></returns>
int GetRand(int min, int max);

/// <summary>
/// ワールド座標をスクリーン座標に変換
/// </summary>
/// <param name="posW">ワールド座標</param>
/// <returns>スクリーン座標</returns>
Vector2i ConvertPosWToS(Vector2 posW);

/// <summary>
/// ワールド座標をスクリーン座標に変換(UI)
/// </summary>
/// <param name="posW">ワールド座標</param>
/// <returns>スクリーン座標</returns>
Vector2i ConvertPosWToSUI(Vector2 posW);

/// <summary>
/// ワールド座標からスクリーン座標への変換(背景用)
/// </summary>
/// <param name="posW">ワールド座標</param>
/// <param name="delay">スクロールとの倍率</param>
/// <returns>スクリーン座標</returns>
Vector2i ConvertPosWToSForBG(Vector2 posW, float scrollRate);

/// <summary>
/// ワールド座標からスクリーン座標への変換(HUD用)
/// </summary>
/// <param name="posW">ワールド座標</param>
/// <returns>スクリーン座標</returns>
Vector2 ConvertPosWToSForUI(Vector2 posW);

/// <summary>
/// 頂点座標の更新
/// </summary>
/// <param name="pos">中心座標</param>
/// <param name="size">サイズ</param>
/// <param name="vertex">頂点座標</param>
void SetVertex(Vector2 pos, Vector2 size, Vector2 vertex[kVertexNum]);

/// <summary>
/// 回転する頂点座標の更新
/// </summary>
/// <param name="pos">中心座標</param>
/// <param name="size">サイズ</param>
/// <param name="vertex">頂点座標</param>
/// <param name="rotateTheta">回転量</param>
void SetVertexRotate(Vector2 pos, Vector2 size, Vector2 vertex[kVertexNum], float rotateTheta);

/// <summary>
/// CircleVSCircleの当たり判定
/// </summary>
/// <param name="circle1Pos">円1の座標</param>
/// <param name="radius1">円1の半径</param>
/// <param name="circle2Pos">円2の座標</param>
/// <param name="radius2">円2の半径</param>
/// <returns>接触しているか</returns>
bool CheckCollisionCircleVSCircle(Vector2 circle1Pos, float radius1, Vector2 circle2Pos, float radius2);

/// <summary>
/// 矩形と円の当たり判定
/// </summary>
/// <param name="circlePos">円の座標</param>
/// <param name="circleRadius">円の半径</param>
/// <param name="boxVertex0">矩形の頂点座標(左上)</param>
/// <param name="boxVertex3">矩形の頂点座標(右下)</param>
/// <returns>衝突しているか</returns>
bool CheckCollisionBoxVSCircle(Vector2 circlePos, float circleRadius, Vector2 boxVertex0, Vector2 boxVertex3);

/// <summary>
/// 矩形と円の当たり判定(回転矩形にも対応)
/// </summary>
/// <param name="circlePos">円の中心座標</param>
/// <param name="circleRadius">円の半径</param>
/// <param name="vertex">矩形の頂点座標</param>
/// <returns>接触しているか</returns>
bool CheckCollisionOBBvsCircle(Vector2 circlePos, float circleRadius, Vector2 vertex[4]);

enum class EasingMode {
	//一定の速度
	Linear,
	//前半に加速
	EaseIn,
	//後半に減速
	EaseOut,
	EaseInOut,
};

Vector2 MoveByEasing(Vector2 startPos, Vector2 endPos, float t, EasingMode mode);

int UpdateValueByEasing(int startNum, int endNum, float t, EasingMode mode);

float UpdateValueByEasing(float startNum, float endNum, float t, EasingMode mode);

void DrawHitBox(Vector2 hitVertex0, Vector2 hitVertex1);

/// <summary>
/// 円の当たり判定表示
/// </summary>
/// <param name="centerPos">中心座標</param>
/// <param name="radius">半径</param>
/// <param name="color">色</param>
/// <param name="solidSwitch">塗りつぶしスイッチ</param>
void DrawHitEllipse(Vector2 centerPos, float radius, unsigned int color, bool solidSwitch);

/// <summary>
/// 指定の桁の数値を取得
/// </summary>
/// <param name="targetNum">目的の数値</param>
/// <param name="targetDigit">取りたい桁(右端は0)</param>
/// <returns>取りたい桁の数値</returns>
int GetValueAtDigit(int targetNum, int targetDigit);

Matrix2x2 MakeRotateMatrix(float theta);

Vector2 MultplyVectorVSMatrix(Vector2 vector, Matrix2x2 matrix);
